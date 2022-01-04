/* extended map support for elf format
   Copyright  2011 Free Software Foundation, Inc.
   Contributed by Horst Lehser (Horst.Lehser@hightec-rt.com).

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "xml.h"

typedef struct _symbol_t
{
  /* Name of symbol/variable.  */
  const char *name;

  /* Memory location of this variable, or value if it's an absolute symbol.  */
  bfd_vma address;

  /* Alignment of this variable (in output section).  */
  int align;

  /* Name of memory region this variable lives in.  */
  const char *region_name;

  /* True if this is a static variable.  */
  bfd_boolean is_static;

  /* Size of this variable.  */
  bfd_vma size;

  /* Pointer to the section in which this symbol is defined.  */
  asection *section;

  /* Name of module in which this symbol is defined.  */
  const char *module_name;

  bfd_boolean is_exported;

  bfd_boolean is_unique;

  bfd_boolean hidden;

  int type;

} symbol_t;


/* Structure to contain the symbol list and additional information 
   for formating. */
typedef struct {
  symbol_t *symbol_list;
  size_t maxlen_size;
  size_t maxlen_symname;
  size_t maxlen_memreg;
  size_t maxlen_osecname;
  size_t maxlen_isecname;
  size_t maxlen_modname;
  bfd_boolean bit_seen;
} extmap_info;

typedef extmap_info* p_extmap_info;

/* Symbol list order */
typedef enum {ORDER_BY_ADDRESS, ORDER_BY_NAME} symbol_list_order;

/* Pointer to a function that prints the linker version to a file;
   only valid if elf32_extmap_enabled.  */
void (*elf32_extmap_ld_version) (FILE *);

/* Pointer to a function that returns the name of memory region
   to a memory address. */
const char * (*elf32_addr_to_memory_region) (bfd_vma);

/* Collect additional information to the map file.  INFO is the link
   info that contains pointers to all input sections and the hash table.  */
extern void elf32_collect_extmap_info (struct bfd_link_info*, p_extmap_info, const char *(*)(bfd_vma));

/* Output additional information to the map file. */
extern void elf32_print_extmap_info (FILE*, p_extmap_info, symbol_list_order);

/* Output header and linker information. */
extern void elf32_print_extmap_header (FILE*, const char*, void (*)(FILE *));

/* Output footer of extended map listing. */
extern void elf32_print_extmap_footer (FILE*);

/* Generate XML data from extmap information. */
extern void elf32_xml_extmap (p_xml_element, p_extmap_info);

/* parse the argument string for extmap */
extern char *elf32_parse_extmap_args (char *args, const char *prog_name);

/* get memory region name from output section */
extern const char * htc_elf32_get_memory_region_from_section(asection *osec);


