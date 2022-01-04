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

#ifndef XML_H
#ifdef HTC_SUPPORT
#define XML_H

#include <stdio.h>

typedef struct xml_attribute
{
  const char* name;
  char* value;
  struct xml_attribute* next;
} *p_xml_attribute;

typedef struct xml_element
{
  const char* name;
  char* value;
  p_xml_attribute attributes;
  p_xml_attribute last_attribute;
  struct xml_element* children;
  struct xml_element* last_child;
  struct xml_element* next;
} *p_xml_element;

p_xml_element xml_root;

char* get_xsd_root (char*, const char*);

void add_xml_attribute (p_xml_element, const char*, const char*);

void add_xml_attribute_hex (p_xml_element, const char*, unsigned long);

void add_xml_attribute_dec (p_xml_element, const char*, unsigned long);

void add_xml_attribute_sdec (p_xml_element, const char*, long);

p_xml_element create_xml_element (const char*);

void add_xml_child (p_xml_element, p_xml_element);

void add_xml_value (p_xml_element, const char*);

void print_xml (FILE*, p_xml_element, const char*);

void print_xml_root (FILE*, p_xml_element, const char*);

p_xml_element get_xml_child (p_xml_element, const char*);

#endif /* HTC_SUPPORT */
#endif /* XML_H */
