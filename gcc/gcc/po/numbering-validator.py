#!/usr/bin/python2

import copy
import os
import re
import sys


class Message(object):
    def __init__(self, msgstr_lines):
        self._msgstr_lines = msgstr_lines

    def is_skippable(self):
        msgstr_lines = [line for line in self._msgstr_lines if line != '""']
        if not msgstr_lines:
            return True
        for line in msgstr_lines:
            if re.match("^\"Project-Id-Version:\s.*\"$", line):
                return True
        return False

    def is_valid(self):
        return bool(self.get_number())

    def get_number(self):
        for line in self._msgstr_lines:
            if line == '""':
                continue
            number = re.match(".*\[(\d+)\].*", line)
            if number:
                return number.group(1)
        return None

    def get_raw_message(self):
        return "\n".join(self._msgstr_lines)

    def get_message(self):
        message = []
        for line in self._msgstr_lines:
            if line == '""':
                continue
            message.append(line.strip('"'))
        return " ".join(message)

    def get_unnumbered_message(self):
        message = []
        for line in self._msgstr_lines:
            if line == '""':
                continue
            line = re.sub("^\"([^[]*)(?:\[\d+\]\s)?(.*)\"$", r"\1\2", line)
            message.append(line)
        return " ".join(message)


class PoParser(object):
    def __init__(self, po_file):
        self._po_file = po_file

    def get_next_msgstr(self):
        msgstr_lines = []
        # Forward to the next msgstr
        while True:
            line = self._poptr.readline()
            # End of file
            if not line:
                return []
            message = re.match("^msgstr\s(\".*\")$", line)
            if message:
                msgstr_lines.append(message.group(1))
                break

        # The first line is stored. Storing lines until the next entry.
        while True:
            line = self._poptr.readline()
            # msgstr ends with a new line
            if re.match("^[\r\n\t ]+$", line) or not line:
                break
            msgstr_lines.append(line.strip("\n"))

        return msgstr_lines

    def parse(self):
        messages = []
        self._poptr = open(self._po_file, "r")
        while True:
            msgstr_lines = self.get_next_msgstr()
            if not msgstr_lines:
                break
            messages.append(Message(msgstr_lines))
        self._poptr.close()
        return messages


class MessageCollection(object):
    def __init__(self, messages):
        self._messages = messages
        self._skipped = self._filter_skippable()
        self._invalid = self._filter_invalid()
        self._duplicate_map = self._filter_duplicate()
        self._message_map = self._create_message_map()

    def _filter_skippable(self):
        skipped = []
        unskipped = []
        for m in self._messages:
            if m.is_skippable():
                skipped.append(m)
            else:
                unskipped.append(m)
        self._messages = unskipped
        return skipped

    def _filter_invalid(self):
        invalid = []
        valid = []
        for m in self._messages:
            if m.is_valid():
                valid.append(m)
            else:
                invalid.append(m)
        self._messages = valid
        return invalid

    def _filter_duplicate(self):
        uniq_map = {}
        duplicate_map = {}
        for m in self._messages:
            number = m.get_number()
            message = m.get_message()
            if number in uniq_map.keys():
                if not number in duplicate_map.keys():
                    duplicate_map[number] = [uniq_map[number]]
                duplicate_map[number].append(m)
            else:
                uniq_map[number] = m
        for number in duplicate_map:
            uniq_map.pop(number)
        self._messages = uniq_map.values()
        return duplicate_map

    def _create_message_map(self):
        message_map = {}
        for m in self._messages:
            number = m.get_number()
            message_map[number] = m
        return message_map

    def get_message_map(self):
        return self._message_map

    def get_skipped_list(self):
        return self._skipped

    def get_invalid_list(self):
        return self._invalid

    def get_duplicate_map(self):
        return self._duplicate_map

    def count_duplicates(self):
        sum = 0
        for duplicate_list in self._duplicate_map.values():
            sum += len(duplicate_list)
        return sum


class Validator(object):
    THRESHOLD = 0.75

    def __init__(self, message_collection_ref, message_collection_new):
        self._message_map_ref = copy.deepcopy(message_collection_ref.get_message_map())
        self._message_map_new = copy.deepcopy(message_collection_new.get_message_map())
        self._matches = self._filter_matches()
        self._changes = self._filter_changes()
        self._removed = self._filter_removed_messages(message_collection_new)
        self._new = self._filter_new_messages(message_collection_ref)
        self._possible_changes = self._filter_possible_changes()

    def _filter_matches(self):
        matches = []
        for number, message in self._message_map_new.items():
            if number in self._message_map_ref.keys():
                if message.get_message() == self._message_map_ref[number].get_message():
                    matches.append(message)
                    self._message_map_ref.pop(number)
                    self._message_map_new.pop(number)
        return matches

    def _filter_changes(self):
        changes = []
        for number, message in self._message_map_new.items():
            if number in self._message_map_ref.keys():
                if message.get_message() != self._message_map_ref[number].get_message():
                    changes.append((self._message_map_ref[number], message))
                    self._message_map_ref.pop(number)
                    self._message_map_new.pop(number)
        return changes

    def _filter_removed_messages(self, message_collection_new):
        removed = []
        for number, message in self._message_map_ref.items():
            if not number in message_collection_new.get_duplicate_map().keys():
                removed.append(message)

            # Remove it even if this is an duplicate. Already counted in duplicate map.
            self._message_map_ref.pop(number)

        return removed

    def _filter_new_messages(self, message_collection_ref):
        new = []
        for number, message in self._message_map_new.items():
            if not number in message_collection_ref.get_duplicate_map().keys():
                new.append(message)
            else:
                # This is not a new message just stored in the ref's duplicate map.
                # TODO(pvarga): Possible match, investigate this!
                pass
            self._message_map_new.pop(number)

        return new

    def _filter_possible_changes(self):
        changes = []
        for ref_message in self._removed[:]:
            best_ratio = 0
            for new_message in self._new:
                ref_string = ref_message.get_unnumbered_message()
                new_string = new_message.get_unnumbered_message()
                diff = self._levenshtein(ref_string, new_string)
                ratio = 1 - float(diff) / max(len(ref_string), len(new_string))

                if ratio >= self.THRESHOLD and ratio > best_ratio:
                    best_ratio = ratio
                    most_similar_message = new_message

            # If there is no similar message skip it
            if best_ratio == 0:
                continue

            changes.append((ref_message, most_similar_message))
            self._removed.remove(ref_message)
            self._new.remove(most_similar_message)

        return changes

    def _levenshtein(self, s1, s2):
        if len(s1) < len(s2):
            return self._levenshtein(s2, s1)

        # len(s1) >= len(s2)
        if len(s2) == 0:
            return len(s1)

        previous_row = range(len(s2) + 1)
        for i, c1 in enumerate(s1):
            current_row = [i + 1]
            for j, c2 in enumerate(s2):
                insertions = previous_row[j + 1] + 1 # j+1 instead of j since previous_row and current_row are one character longer
                deletions = current_row[j] + 1       # than s2
                substitutions = previous_row[j] + (c1 != c2)
                current_row.append(min(insertions, deletions, substitutions))
            previous_row = current_row

        return previous_row[-1]

    def get_matches(self):
        return self._matches

    def get_changes(self):
        return self._changes

    def get_removed_messages(self):
        return self._removed

    def get_new_messages(self):
        return self._new

    def get_possible_changes(self):
        return self._possible_changes

    # Returns list of messages that have unknown problem: probably duplicate
    def get_unknown_messages(self):
        unknown = []
        unknown.extend(self._message_map_ref.values())
        unknown.extend(self._message_map_new.values())
        return unknown


def main():
    if len(sys.argv) != 3:
        print("Invalid arguments!")
        print("Usage: %s [reference.po] [new.po]" % sys.argv[0])
        exit(1)

    root_dir = os.path.abspath(os.path.dirname(__file__))
    po_ref = os.path.join(root_dir, sys.argv[1])
    po_new = os.path.join(root_dir, sys.argv[2])

    if not os.path.exists(po_ref):
        print("Reference po file doesn't exist: %s" % po_ref)
        exit(1)

    if not os.path.exists(po_new):
        print("New po file doesn't exist: %s" % po_new)
        exit(1)

    messages_ref = PoParser(po_ref).parse()
    message_collection_ref = MessageCollection(messages_ref)

    messages_new = PoParser(po_new).parse()
    message_collection_new = MessageCollection(messages_new)


    validator = Validator(message_collection_ref, message_collection_new)

    print("Changed Messages (%d):" % len(validator.get_changes()))
    for (orig, new) in validator.get_changes():
        print("\t[%s] %s  ==>  %s" % (orig.get_number(), orig.get_unnumbered_message(), new.get_unnumbered_message()))

    print("")
    print("Possibly Changed Messages (%d):" % len(validator.get_possible_changes()))
    for (orig, new) in validator.get_possible_changes():
        print("\t%s  ==>  %s" % (orig.get_message(), new.get_message()))

    print("")
    print("Removed Messages (%d):" % len(validator.get_removed_messages()))
    for message in validator.get_removed_messages():
        print("\t[%s] %s" % (message.get_number(), message.get_unnumbered_message()))

    print("")
    print("New Messages (%d):" % len(validator.get_new_messages()))
    for message in validator.get_new_messages():
        print("\t[%s] %s" % (message.get_number(), message.get_unnumbered_message()))

    print("")
    print("Invalid Messages (%d):" % len(message_collection_new.get_invalid_list()))
    for message in message_collection_new.get_invalid_list():
        print("\t[%s] %s" % (message.get_number(), message.get_unnumbered_message()))

    print("")
    print("Duplicated Messages (%d):" % message_collection_new.count_duplicates())
    for messages in message_collection_new.get_duplicate_map().values():
        for message in messages:
            print("\t[%s] %s" % (message.get_number(), message.get_unnumbered_message()))

    print("")
    print("Unknown Problems (%d):" % len(validator.get_unknown_messages()))
    for message in validator.get_unknown_messages():
        print("\t[%s] %s" % (message.get_number(), message.get_unnumbered_message()))

    print("\n")
    print("Parser Stats:")
    format_string = "\t{0:<10} {1:^10} {2:^10}"
    print(format_string.format("Message", "Reference", "New"))
    print("\t" + "-"*30)
    print(format_string.format("Skipped", len(message_collection_ref.get_skipped_list()), len(message_collection_new.get_skipped_list())))
    print(format_string.format("Invalid", len(message_collection_ref.get_invalid_list()), len(message_collection_new.get_invalid_list())))
    print(format_string.format("Duplicate",
            "%d(%d)" % (message_collection_ref.count_duplicates(), len(message_collection_ref.get_duplicate_map().values())), 
            "%d(%d)" % (message_collection_new.count_duplicates(), len(message_collection_new.get_duplicate_map().values()))))
    print(format_string.format("Valid", len(message_collection_ref.get_message_map().keys()), len(message_collection_new.get_message_map().keys())))
    print("\t" + "-"*30)
    print(format_string.format("Total", len(messages_ref), len(messages_new)))


    print("\n")
    print("Summary:")
    format_string = "\t{0:<16} {1:>10}"
    print(format_string.format("Message", "Count"))
    print("\t" + "-"*30)
    print(format_string.format("Match", len(validator.get_matches())))
    print(format_string.format("Change", len(validator.get_changes())))
    print(format_string.format("Possible Change", len(validator.get_possible_changes())))
    print(format_string.format("New", len(validator.get_new_messages())))
    print(format_string.format("Duplicate", message_collection_new.count_duplicates()))
    print(format_string.format("Invalid", len(message_collection_new.get_invalid_list())))
    print(format_string.format("Problem", len(validator.get_unknown_messages())))
    print("\t" + "-"*30)
    print(format_string.format("Total", len(messages_new) - len(message_collection_new.get_skipped_list())))


if __name__ == "__main__":
    main()
