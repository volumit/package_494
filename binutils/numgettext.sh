#! /bin/bash

POTFILES=$1
SRCDIR=$2

TMPFILE="nr.tmp"
TMP2="nr2.tmp"
DICT="nr.dic"
POT="nr.pot"
nl='
'

if [ -e $TMPFILE ]; then
 ls $TMPFILE
fi

# Collect all function calls and assignments that possibly
# will be shown as warning or error.
for FILE in `cat $POTFILES`
do
  if [ -e $SRCDIR/$FILE ]; then
    grep -Pzo "error[^;/]*[^N]_\(\".*[^\\\]\"\)" $SRCDIR/$FILE >> $TMPFILE

    grep -Pzo "warn[^;/]*[^N]_\(\".*[^\\\]\"\)" $SRCDIR/$FILE >> $TMPFILE

    grep -Pzo '[^N]_\(\"((%[a-zA-Z]:?)+ )?(warning|error): ([^\"]|\\"|\"[^)])+\"\)' $SRCDIR/$FILE >> $TMPFILE

    grep -Pzo "as_bad[^;/]*[^N]_\(\".*[^\\\]\"\)" $SRCDIR/$FILE >> $TMPFILE

    grep -Pzo "fatal[^;/]*[^N]_\(\".*[^\\\]\"\)" $SRCDIR/$FILE >> $TMPFILE
  fi;
done;

# Join broken lines
sed -i ':a;N;$!ba;s/\\\n//g' $TMPFILE
sed -i ':a;N;$!ba;s/\"\n\s\+\"//g' $TMPFILE

# Filter the lines that contains gettext function call.
grep -e "_(" $TMPFILE > $TMP2
mv $TMP2 $TMPFILE

# Strip the unneeded part around the text.
sed -i 's/.*_(\"\(\(\\\"\|[^"]\)*\)\")\?.*/\1/g' $TMPFILE

# Add exceptions that can not be found with grep.
if [ -e $SRCDIR/po/dictionary ]; then
  cat $SRCDIR/po/dictionary >> $TMPFILE
fi;

# Remove multiplicated lines.
awk '!x[$0]++' $TMPFILE > $DICT

# Change \` to `
sed -i 's/\\`/`/g' $DICT

# Duplicate \ characters they would be solved othervise.
sed -i 's/\\/\\\\/g' $DICT

# Put the whole lines in quotes to keep whitespaces.
sed -i 's/^\(.*\)$/"\1"/g' $DICT

# Remove lines that contains %I format sequences.
# It is needed because .mo files which contain this kind of
# sequences have the major revision 1, that libintl can't handle.
sed -i 's/^.*%I.*$//g' $DICT

# Generate po format.
while read LINE
do
        if [[ -n $LINE ]]
        then
                echo $nl >> $POT
                echo "#: translation_for_numbering:1" >> $POT
                echo "#, c-format" >> $POT
                echo "msgid $LINE" >> $POT
                echo "msgstr \"\"" >> $POT
        fi
done < $DICT

# Duplicate backslash at \' and \%.
sed -i "s/\([^\]\)\([\]\)\(['%]\)/\1\2\2\3/g" $POT

#rm $DICT $TMPFILE

