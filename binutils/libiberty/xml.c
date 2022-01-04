/* HTC XML support for mapfile and objectdump.
   Copyright (C) 1998-2011 Free Software Foundation, Inc.
   Contributed by Tamas Szirbucz (Tamas.Szirbucz@hightec-rt.com).

This file is an extension of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

#ifdef HTC_SUPPORT
#include "xml.h"
#include "libiberty.h"
#include <string.h>
#include <stdlib.h>

static char*
escape_path (char* path)
{
  int num_of_spaces = 0;
  unsigned int i;
  int escaped_length = 0;
  char *escaped;

  for (i = 0; i < strlen (path); ++i)
    {
      if (path[i] == ' ')
        ++ num_of_spaces;
    }

  escaped = (char*) xcalloc (num_of_spaces * 3 + strlen(path) + 1, sizeof (char));

  for (i = 0; i <= strlen(path); ++i)
    {
      if (path[i] == ' ')
        {
          sprintf(escaped + escaped_length, "%%20");
          escaped_length += 3;
        }
      else
        {
          escaped[escaped_length++] = path[i];
        }
    }

  return escaped;
}

char* get_xsd_root (char* binpath, const char* toolbindir)
{
      /* Set the share/xml/ subdir as path of the xsd. */
      char *bindir = make_relative_prefix (binpath, "/", "/");
      char *tooldir = xstrdup (toolbindir);
      char *tooldirend;
      char *bindirend;
      char xsdpath[12];

      char separator = bindir[strlen (bindir) - 1];

      sprintf (xsdpath, "%cshare%cxml%c", separator, separator, separator);

      tooldir[strlen (tooldir) - strlen ("/bin")] = '\0';
      bindir[strlen (bindir) - strlen ("/bin/")] = '\0';

      tooldirend = tooldir + strlen (tooldir) - 1;
      bindirend = bindir + strlen (bindir) - 1;

      while ((*tooldirend != '/') && 
             (*tooldirend != '\\'))
        {
          --tooldirend;
          --bindirend;
        }


      if (!strncmp (tooldirend + 1, bindirend + 1, strlen(tooldirend)))
        *bindirend = '\0';

      return escape_path (concat ("file://", bindir, xsdpath, (char*) 0));
}

static char*
escape (const char *in)
{
  char *out;
  const char *c;
  int add_length = 0;
  int pos = 0;
  
  c = in;
  while (*c)
    {
      switch (*c)
        {
          case '&':
            add_length += 4;
            break;
          case '<':
          case '>':
            add_length += 3;
            break;
          case '"':
          case '\'':
            add_length += 5;
            break;
          default:
            break;
        }

      ++c;
    }

  out = (char*) xcalloc (strlen (in) + (add_length) + 1, sizeof (char));
  
  c = in;
  while (*c)
    {
      switch (*c)
        {
          case '&':
            strcpy ((out + pos), "&amp;");
            pos += 5;
            break;
          case '<':
            strcpy ((out + pos), "&lt;");
            pos += 4;
            break;
          case '>':
            strcpy ((out + pos), "&gt;");
            pos += 4;
            break;
          case '"':
            strcpy ((out + pos), "&quot;");
            pos += 6;
            break;
          case '\'':
            strcpy ((out + pos), "&apos;");
            pos += 6;
            break;
          default:
            out[pos++] = *c;
        }

      ++c;
    }

    return out;
}

void add_xml_attribute (p_xml_element xml, const char* name, const char* value)
{
  p_xml_attribute attribute = (p_xml_attribute) xcalloc (1, sizeof(struct xml_attribute));

  attribute->name = name;

  attribute->value = escape(value);

  attribute->next = NULL;

  if (xml->last_attribute == NULL)
    {
      xml->attributes = attribute;
    }
  else
    {
      xml->last_attribute->next = attribute;
    }

  xml->last_attribute = attribute;
}

void add_xml_attribute_hex (p_xml_element xml, const char* name, unsigned long value)
{
  char *buffer = (char*) xcalloc (11, sizeof (char));
  sprintf (buffer, "%08lx", value);
  add_xml_attribute (xml, name, buffer);
  free (buffer);
}

void add_xml_attribute_dec (p_xml_element xml, const char* name, unsigned long value)
{
  char *buffer = (char*) xcalloc (20, sizeof (char));
  sprintf (buffer, "%lu", value);
  add_xml_attribute (xml, name, buffer);
  free (buffer);
}

void add_xml_attribute_sdec (p_xml_element xml, const char* name, long value)
{
  char *buffer = (char*) xcalloc (20, sizeof (char));
  sprintf (buffer, "%ld", value);
  add_xml_attribute (xml, name, buffer);
  free (buffer);
}

p_xml_element create_xml_element (const char* name)
{
  p_xml_element xml = (p_xml_element) xcalloc (1, sizeof(struct xml_element));

  xml->name = name;
  xml->value = NULL;
  xml->attributes = NULL;
  xml->last_attribute = NULL;
  xml->children = NULL;
  xml->last_child = NULL;
  xml->next = NULL;
  return xml;
}

void add_xml_child (p_xml_element parent, p_xml_element child)
{
  if (parent->last_child == NULL)
    {
      parent->children = child;
    }
  else
    {
      parent->last_child->next = child;
    }

  parent->last_child = child;
}

void add_xml_value (p_xml_element element, const char* value)
{
  element->value = escape (value);
}


void print_xml (FILE* xml_file, p_xml_element element, const char* align)
{
  p_xml_attribute attr = element->attributes;
  p_xml_element child = element->children;
  char *new_align;

  fprintf (xml_file, "%s<%s", align, element->name);

  while (attr != NULL)
    {
      fprintf (xml_file, " %s=\"%s\"", attr->name, attr->value);
      attr = attr->next;
    }

  if (child == NULL && element->value == NULL)
    {
      fprintf (xml_file, "/>\n");
    }
  else
    {
      fprintf (xml_file, ((element->children == NULL) && (element->attributes == NULL)) ? ">" : ">\n");

      new_align = concat (align, "  ", (char*) 0);

      if (element->value != NULL)
        {
          if ((element->children == NULL) && (element->attributes == NULL))
            fprintf(xml_file, "%s", element->value);
          else
            fprintf(xml_file, "%s%s\n", new_align, element->value);
        }

      while (child != NULL)
        {
          print_xml (xml_file, child, new_align);
          child = child->next;
        }

      if ((element->children == NULL) && (element->attributes == NULL))
        fprintf (xml_file, "</%s>\n", element->name);
      else
        fprintf (xml_file, "%s</%s>\n", align, element->name);

      /* Separate the main tags */
      if (!strcmp(align, "  "))
        fprintf (xml_file, "\n\n");
    }
}

void print_xml_root (FILE *xml_file, 
                     p_xml_element root, 
                     const char *version)
{
  fprintf(xml_file, "<?xml version=\"%s\"?>\n",
          version);
  print_xml(xml_file, root, "");
}

p_xml_element get_xml_child (p_xml_element parent, const char* name)
{
  p_xml_element child;

  if (parent->children == NULL)
    return (p_xml_element) NULL;

  child = parent->children;

  while (strcmp (child->name, name))
    {
      child = child->next;
      if (child == NULL)
        return NULL;
    }

  return child;
}
#endif /*HTC_SUPPORT*/
