/* TriCore-specific support for 32-bit ELF.
   Copyright (C) 1998-2011 Free Software Foundation, Inc.
   Contributed by Michael Schumacher (mike@hightec-rt.com).
   Extended by Horst Lehser (Horst.Lehser@hightec-rt.com).

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "ctype.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "elf-bfd.h"
#include "elf/tricore.h"

#include "elf32-htc.c"
/* The full name of the default instruction set architecture.  */

#define DEFAULT_ISA "TriCore:V1.2"		/* Name of default architecture to print.  */

/* The full name of the dynamic interpreter; put in the .interp section.  */

#define ELF_DYNAMIC_INTERPRETER "/lib/ld-tricore.so.1"

/* The number of reserved entries at the beginning of the PLT.  */

#define PLT_RESERVED_SLOTS 2

/* The size (in bytes) of a PLT entry.  */

#define PLT_ENTRY_SIZE 12

/* Section flag for PCP sections.  */

#define PCP_SEG SEC_PCP

/* Section flags for dynamic relocation sections.  */

#define RELGOTSECFLAGS	(SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS \
			 | SEC_IN_MEMORY | SEC_LINKER_CREATED | SEC_READONLY)
#define DYNOBJSECFLAGS  (SEC_HAS_CONTENTS | SEC_IN_MEMORY \
			 | SEC_LINKER_CREATED | SEC_READONLY)


extern void tricore_elf32_define_sda_section(struct bfd_link_info *, bfd *,
                                    const char *,const char *,const char *);

/* Enumeration to specify the special section.  */
typedef enum elf_linker_section_enum
{
  LINKER_SECTION_UNKNOWN,       /* not used */
  LINKER_SECTION_GOT,           /* .got section for global offset po
inters */
  LINKER_SECTION_PLT,           /* .plt section for generated proced
ure stubs */
  LINKER_SECTION_SDATA,         /* .sdata/.sbss section for PowerPC
*/
  LINKER_SECTION_SDATA2,        /* .sdata2/.sbss2 section for PowerP
C */
  LINKER_SECTION_MAX            /* # of linker sections */
} elf_linker_section_enum_t;


/* Sections created by the linker.  */
typedef struct elf_linker_section
{
  char *name;               /* name of the section */
  char *bss_name;           /* name of a related .bss section */
  char *sym_name;           /* name of symbol to reference this section */
  asection *section;            /* pointer to the section */
  asection *bss_section;        /* pointer to the bss section associated with this */
  struct elf_link_hash_entry *sym_hash; /* pointer to the created symbol hash value */
  bfd_vma sym_offset;           /* offset of symbol from beginning of section */
  unsigned int alignment;       /* alignment for the section */
  flagword flags;           /* flags to use to create the section */
} elf_linker_section_t;

/* Describe "short addressable" memory areas (SDAs, Small Data Areas).  */

typedef struct _sda_t
{
  /* Name of this SDA's short addressable data section.  */
  const char *data_section_name;

  /* Name of this SDA's short addressable BSS section.  */
  const char *bss_section_name;

  /* Name of the symbol that contains the base address of this SDA.  */
  const char *sda_symbol_name;

  /* Pointers to the BFD representations of the above data/BSS sections.  */
  asection *data_section;
  asection *bss_section;

  /* The base address of this SDA; usually points to the middle of a 64k
     memory area to provide full access via a 16-bit signed offset; this
     can, however, be overridden by the user (via "--defsym" on the command
     line, or in the linker script with an assignment statement such as
     "_SMALL_DATA_ = ABSOLUTE (.);" in output section ".sbss" or ".sdata").  */
  bfd_vma gp_value;

  /* The number of the address register that contains the base address
     of this SDA.  This is 0 for .sdata/.sbss (or 12 if this is a dynamic
     executable, because in this case the SDA follows immediately after the
     GOT, and both are accessed via the GOT pointer), 1 for .sdata2/.sbss2,
     8 for .sdata3/.sbss3, and 9 for .sdata4/.sbss4.  */
  int areg;

  /* True if this SDA has been specified as an output section in the
     linker script; it suffices if either the data or BSS section of
     this SDA has been specified (e.g., just ".sbss2", but not ".sdata2"
     for SDA1, or just ".sdata", but not ".sbss" for SDA0).  */
  bfd_boolean valid;
  
  /* pointer to the next SDA description */
  struct _sda_t *next;
} sda_t;

/* We allow up to four independent SDAs in executables.  For instance,
   if you need 128k of initialized short addressable data, and 128k of
   uninitialized short addressable data, you could specify .sdata, .sdata2,
   .sbss3, and .sbss4 as output sections in your linker script.  Note,
   however, that according to the EABI only the first SDA must be supported,
   while support for the second SDA (called "literal section") is optional.
   The other two SDAs are GNU extensions and can only be used in standalone
   applications, or if an underlying OS doesn't use %a8 and %a9 for its own
   purposes.  Also note that shared objects may only use the first SDA,
   which will be addressed via the GOT pointer (%a12), so it can't exceed
   32k, and may only use it for static variables.  That's because if a
   program references a global variable defined in a shared object, the
   linker reserves space for it in the program's ".dynbss" section and emits
   a COPY reloc that will be resolved by the dynamic linker.  If, however,
   the variable would be defined in the SDA of a SO, then this would lead
   to different accesses to this variable, as the program expects it to live
   in its ".dynbss" section, while the SO was compiled to access it in its
   SDA -- clearly a situation that must be avoided.  */

#define NR_SDAS 4

static sda_t sda4 = 
  { ".sdata4",
    ".sbss4",
    "_SMALL_DATA4_",
    (asection *) NULL,
    (asection *) NULL,
    0,
    9,
    FALSE,
     NULL
  };
static sda_t sda3 =
  { ".sdata3",
    ".sbss3",
    "_SMALL_DATA3_",
    (asection *) NULL,
    (asection *) NULL,
    0,
    8,
    FALSE,
    &sda4
  };
static sda_t sda2 =
  { ".sdata2",
    ".sbss2",
    "_SMALL_DATA2_",
    (asection *) NULL,
    (asection *) NULL,
    0,
    1,
    FALSE,
    &sda3
  };
static sda_t sda0 = 
  { ".sdata",
    ".sbss",
    "_SMALL_DATA_",
    (asection *) NULL,
    (asection *) NULL,
    0,
    0,
    FALSE,
    &sda2
  };

static sda_t *small_data_areas = &sda0;
static sda_t *small_data_last = &sda4;

/* If the user requested an extended map file, we might need to keep a list
   of global (and possibly static) symbols.  */

typedef struct _symbol_t
{
  /* Name of symbol/variable.  */
  const char *name;

  /* Memory location of this variable, or value if it's an absolute symbol.  */
  bfd_vma address;

  /* Alignment of this variable (in output section).  */
  int align;

  /* Name of memory region this variable lives in.  */
  char *region_name;

  /* True if this is a bit variable.  */
  bfd_boolean is_bit;

  /* Bit position if this is a bit variable.  */
  int bitpos;

  /* True if this is a static variable.  */
  bfd_boolean is_static;

  /* Size of this variable.  */
  bfd_vma size;

  /* Pointer to the section in which this symbol is defined.  */
  asection *section;

  /* Name of module in which this symbol is defined.  */
  const char *module_name;
} symbol_t;

/* Symbols to be listed are stored in a dynamically allocated array.  */


/* This describes memory regions defined by the user; must be kept in
   sync with ld/emultempl/tricoreelf.em.  */

typedef struct _memreg
{
  /* Name of region.  */
  char *name;

  /* Start of region.  */
  bfd_vma start;

  /* Length of region.  */
  bfd_size_type length;

  /* Number of allocated (used) bytes.  */
  bfd_size_type used;
} memreg_t;

/* This array describes TriCore relocations.  */

static reloc_howto_type tricore_elf32_howto_table[] =
{
  /* No relocation (ignored).  */
  HOWTO (R_TRICORE_NONE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_NONE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32 bit PC-relative relocation.  */
  HOWTO (R_TRICORE_32REL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_32REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 32 bit absolute relocation.  */
  HOWTO (R_TRICORE_32ABS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_32ABS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* relB 25 bit PC-relative relocation.  */
  HOWTO (R_TRICORE_24REL,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 25,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_24REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffff00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* absB 24 bit absolute address relocation.  */
  HOWTO (R_TRICORE_24ABS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_24ABS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* bolC 16 bit small data section relocation.  */
  HOWTO (R_TRICORE_16SM,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_16SM",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* RLC High 16 bits of symbol value, adjusted.  */
  HOWTO (R_TRICORE_HIADJ,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_HIADJ",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* RLC Low 16 bits of symbol value.  */
  HOWTO (R_TRICORE_LO,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_LO",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* BOL Low 16 bits of symbol value.  */
  HOWTO (R_TRICORE_LO2,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_LO2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* ABS 18 bit absolute address relocation.  */
  HOWTO (R_TRICORE_18ABS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 18,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_18ABS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf3fff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* BO 10 bit relative small data relocation.  */
  HOWTO (R_TRICORE_10SM,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_10SM",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf03f0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* BR 15 bit PC-relative relocation.  */
  HOWTO (R_TRICORE_15REL,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_15REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x7fff0000,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* RLC High 16 bits of symbol value.  */
  HOWTO (R_TRICORE_HI,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_HI",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit signed constant.  */
  HOWTO (R_TRICORE_16CONST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_16CONST",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rcC2 9 bit unsigned constant.  */
  HOWTO (R_TRICORE_9ZCONST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 9,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_9ZCONST",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x001ff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* RcC 9 bit signed constant.  */
  HOWTO (R_TRICORE_9SCONST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 9,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_9SCONST",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x001ff000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* sbD 9 bit PC-relative displacement.  */
  HOWTO (R_TRICORE_8REL,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 9,			/* bitsize */
	 TRUE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_8REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* scC 8 bit unsigned constant.  */
  HOWTO (R_TRICORE_8CONST,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_8CONST",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* BO 10 bit data offset.  */
  HOWTO (R_TRICORE_10OFF,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_10OFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf03f0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* bolC 16 bit data offset.  */
  HOWTO (R_TRICORE_16OFF,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_16OFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 8 bit absolute data relocation.  */
  HOWTO (R_TRICORE_8ABS,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_8ABS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 16 bit absolute data relocation.  */
  HOWTO (R_TRICORE_16ABS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_16ABS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* absBb 1 bit relocation.  */
  HOWTO (R_TRICORE_1BIT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 1,			/* bitsize */
	 FALSE,			/* pc_relative */
	 11,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_1BIT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x00000800,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* absBp 3 bit bit position.  */
  HOWTO (R_TRICORE_3POS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 3,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_3POS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x00000700,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* bitP1 5 bit bit position.  */
  HOWTO (R_TRICORE_5POS,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_5POS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x001f0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* PCP HI relocation.  */
  HOWTO (R_TRICORE_PCPHI,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCPHI",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* PCP LO relocation.  */
  HOWTO (R_TRICORE_PCPLO,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCPLO",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* PCP PAGE relocation.  */
  HOWTO (R_TRICORE_PCPPAGE,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCPPAGE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* PCP OFF relocation.  */
  HOWTO (R_TRICORE_PCPOFF,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCPOFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x003f,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* PCP TEXT relocation.  */
  HOWTO (R_TRICORE_PCPTEXT,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCPTEXT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* bitP2 5 bit bit position.  */
  HOWTO (R_TRICORE_5POS2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 23,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_5POS2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f800000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* brcC 4 bit signed offset.  */
  HOWTO (R_TRICORE_BRCC,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_BRCC",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000f000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* brcC2 4 bit unsigned offset.  */
  HOWTO (R_TRICORE_BRCZ,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_BRCZ",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000f000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* brnN 5 bit bit position.  */
  HOWTO (R_TRICORE_BRNN,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_BRNN",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000f080,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rrN 2 bit unsigned constant.  */
  HOWTO (R_TRICORE_RRN,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 2,			/* bitsize */
	 FALSE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_RRN",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x00030000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* sbcC 4 bit signed constant.  */
  HOWTO (R_TRICORE_4CONST,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4CONST",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* sbcD/sbrD 5 bit PC-relative, zero-extended displacement.  */
  HOWTO (R_TRICORE_4REL,	/* type */
	 1,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 TRUE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* sbrD 5 bit PC-relative, one-extended displacement.  */
  HOWTO (R_TRICORE_4REL2,	/* type */
	 1,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 TRUE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4REL2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* sbrN 5 bit bit position.  */
  HOWTO (R_TRICORE_5POS3,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_5POS3",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf080,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* slroO 4 bit zero-extended offset.  */
  HOWTO (R_TRICORE_4OFF,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4OFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* slroO2 5 bit zero-extended offset.  */
  HOWTO (R_TRICORE_4OFF2,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4OFF2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* slroO4 6 bit zero-extended offset.  */
  HOWTO (R_TRICORE_4OFF4,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4OFF4",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* sroO 4 bit zero-extended offset.  */
  HOWTO (R_TRICORE_42OFF,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_42OFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* sroO2 5 bit zero-extended offset.  */
  HOWTO (R_TRICORE_42OFF2,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_42OFF2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* slroO4 6 bit zero-extended offset.  */
  HOWTO (R_TRICORE_42OFF4,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_42OFF4",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* srrsN 2 bit zero-extended constant.  */
  HOWTO (R_TRICORE_2OFF,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 2,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_2OFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x00c0,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* scC 8 bit zero-extended offset.  */
  HOWTO (R_TRICORE_8CONST2,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_8CONST2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff00,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* sbrnN 4 bit zero-extended constant.  */
  HOWTO (R_TRICORE_4POS,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_4POS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit small data section relocation.  */
  HOWTO (R_TRICORE_16SM2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_16SM2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* sbcD/sbrD 6 bit PC-relative, zero-extended displacement.  */
  HOWTO (R_TRICORE_5REL,	/* type */
	 1,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 TRUE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_5REL",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* Special reloc for optimizing virtual tables.  */
  HOWTO (R_TRICORE_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GNU_VTENTRY",/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* Special reloc for optimizing virtual tables.  */
  HOWTO (R_TRICORE_GNU_VTINHERIT,/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GNU_VTINHERIT",/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),  		/* pcrel_offset */

  /* 16 bit PC-relative relocation.  */
  HOWTO (R_TRICORE_PCREL16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCREL16",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 8 bit PC-relative relocation.  */
  HOWTO (R_TRICORE_PCREL8,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_PCREL8",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xff,			/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* rlcC 16 bit GOT symbol entry.  */
  HOWTO (R_TRICORE_GOT,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOT symbol entry.  */
  HOWTO (R_TRICORE_GOT2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOT2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTHI symbol entry.  */
  HOWTO (R_TRICORE_GOTHI,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTHI",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit GOTLO symbol entry.  */
  HOWTO (R_TRICORE_GOTLO,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTLO",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOTLO symbol entry.  */
  HOWTO (R_TRICORE_GOTLO2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTLO2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTUP symbol entry.  */
  HOWTO (R_TRICORE_GOTUP,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTUP",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit GOTOFF symbol entry.  */
  HOWTO (R_TRICORE_GOTOFF,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFF",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOTOFF symbol entry.  */
  HOWTO (R_TRICORE_GOTOFF2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFF2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTOFFHI symbol entry.  */
  HOWTO (R_TRICORE_GOTOFFHI,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFFHI",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit GOTOFFLO symbol entry.  */
  HOWTO (R_TRICORE_GOTOFFLO,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFFLO",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOTOFFLO symbol entry.  */
  HOWTO (R_TRICORE_GOTOFFLO2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFFLO2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTOFFUP symbol entry.  */
  HOWTO (R_TRICORE_GOTOFFUP,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTOFFUP",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit GOTPC symbol entry.  */
  HOWTO (R_TRICORE_GOTPC,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPC",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOTPC symbol entry.  */
  HOWTO (R_TRICORE_GOTPC2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPC2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTPCHI symbol entry.  */
  HOWTO (R_TRICORE_GOTPCHI,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPCHI",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* rlcC 16 bit GOTPCLO symbol entry.  */
  HOWTO (R_TRICORE_GOTPCLO,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPCLO",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* bolC 16 bit GOTPCLO symbol entry.  */
  HOWTO (R_TRICORE_GOTPCLO2,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPCLO2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* rlcC 16 bit GOTPCUP symbol entry.  */
  HOWTO (R_TRICORE_GOTPCUP,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GOTPCUP",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0ffff000,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* relB PLT entry.  */
  HOWTO (R_TRICORE_PLT,		/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 25,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_PLT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffff00,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* COPY.  */
  HOWTO (R_TRICORE_COPY,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_COPY",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE), 		/* pcrel_offset */

  /* GLOB_DAT.  */
  HOWTO (R_TRICORE_GLOB_DAT,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_GLOB_DAT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE), 		/* pcrel_offset */

  /* JMP_SLOT.  */
  HOWTO (R_TRICORE_JMP_SLOT,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_JMP_SLOT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE), 		/* pcrel_offset */

  /* RELATIVE.  */
  HOWTO (R_TRICORE_RELATIVE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_RELATIVE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE),	 		/* pcrel_offset */

  /* BITPOS.  */
  HOWTO (R_TRICORE_BITPOS,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_BITPOS",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE)	 		/* pcrel_offset */
  ,
  /* SMALL DATA Baseregister operand 2.  */
  HOWTO (R_TRICORE_SBREG_S2,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 12,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_SBREG_S2",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf000,	/* dst_mask */
	 FALSE), 		/* pcrel_offset */

  /* SMALL DATA Baseregister operand 1.  */
  HOWTO (R_TRICORE_SBREG_S1,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 8,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_SBREG_S1",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0f00,	/* dst_mask */
	 FALSE),	 		/* pcrel_offset */

  /* SMALL DATA Baseregister destination.  */
  HOWTO (R_TRICORE_SBREG_D,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 4,			/* bitsize */
	 FALSE,			/* pc_relative */
	 28,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TRICORE_SBREG_D",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf0000000,/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* ABS 32 bit absolute address relocation low 14-bit zero.  */
  HOWTO (R_TRICORE_18ABS_14,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 18,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_TRICORE_18ABS_14",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0xf3fff000,		/* dst_mask */
	 FALSE) 		/* pcrel_offset */

};

/* Describe the mapping between BFD and TriCore relocs.  */

struct elf_reloc_map {
  bfd_reloc_code_real_type bfd_reloc_val;
  enum elf32_tricore_reloc_type tricore_val;
};

static CONST struct elf_reloc_map tricore_reloc_map[] =
{
  {BFD_RELOC_NONE,              R_TRICORE_NONE},
  {BFD_RELOC_TRICORE_32REL,     R_TRICORE_32REL},
  {BFD_RELOC_TRICORE_32ABS,     R_TRICORE_32ABS},
  {BFD_RELOC_TRICORE_24REL,     R_TRICORE_24REL},
  {BFD_RELOC_TRICORE_24ABS,     R_TRICORE_24ABS},
  {BFD_RELOC_TRICORE_16SM,      R_TRICORE_16SM},
  {BFD_RELOC_TRICORE_HIADJ,     R_TRICORE_HIADJ},
  {BFD_RELOC_TRICORE_LO,        R_TRICORE_LO},
  {BFD_RELOC_TRICORE_LO2,       R_TRICORE_LO2},
  {BFD_RELOC_TRICORE_18ABS,     R_TRICORE_18ABS},
  {BFD_RELOC_TRICORE_10SM,      R_TRICORE_10SM},
  {BFD_RELOC_TRICORE_15REL,     R_TRICORE_15REL},
  {BFD_RELOC_TRICORE_HI,        R_TRICORE_HI},
  {BFD_RELOC_TRICORE_16CONST,   R_TRICORE_16CONST},
  {BFD_RELOC_TRICORE_9ZCONST,   R_TRICORE_9ZCONST},
  {BFD_RELOC_TRICORE_9SCONST,   R_TRICORE_9SCONST},
  {BFD_RELOC_TRICORE_8REL,      R_TRICORE_8REL},
  {BFD_RELOC_TRICORE_8CONST,    R_TRICORE_8CONST},
  {BFD_RELOC_TRICORE_10OFF,     R_TRICORE_10OFF},
  {BFD_RELOC_TRICORE_16OFF,     R_TRICORE_16OFF},
  {BFD_RELOC_TRICORE_8ABS,      R_TRICORE_8ABS},
  {BFD_RELOC_TRICORE_16ABS,     R_TRICORE_16ABS},
  {BFD_RELOC_TRICORE_1BIT,      R_TRICORE_1BIT},
  {BFD_RELOC_TRICORE_3POS,      R_TRICORE_3POS},
  {BFD_RELOC_TRICORE_5POS,      R_TRICORE_5POS},
  {BFD_RELOC_TRICORE_PCPHI,     R_TRICORE_PCPHI},
  {BFD_RELOC_TRICORE_PCPLO,     R_TRICORE_PCPLO},
  {BFD_RELOC_TRICORE_PCPPAGE,   R_TRICORE_PCPPAGE},
  {BFD_RELOC_TRICORE_PCPOFF,    R_TRICORE_PCPOFF},
  {BFD_RELOC_TRICORE_PCPTEXT,   R_TRICORE_PCPTEXT},
  {BFD_RELOC_TRICORE_5POS2,     R_TRICORE_5POS2},
  {BFD_RELOC_TRICORE_BRCC,      R_TRICORE_BRCC},
  {BFD_RELOC_TRICORE_BRCZ,      R_TRICORE_BRCZ},
  {BFD_RELOC_TRICORE_BRNN,      R_TRICORE_BRNN},
  {BFD_RELOC_TRICORE_RRN,       R_TRICORE_RRN},
  {BFD_RELOC_TRICORE_4CONST,    R_TRICORE_4CONST},
  {BFD_RELOC_TRICORE_4REL,      R_TRICORE_4REL},
  {BFD_RELOC_TRICORE_4REL2,     R_TRICORE_4REL2},
  {BFD_RELOC_TRICORE_5POS3,     R_TRICORE_5POS3},
  {BFD_RELOC_TRICORE_4OFF,      R_TRICORE_4OFF},
  {BFD_RELOC_TRICORE_4OFF2,     R_TRICORE_4OFF2},
  {BFD_RELOC_TRICORE_4OFF4,     R_TRICORE_4OFF4},
  {BFD_RELOC_TRICORE_42OFF,     R_TRICORE_42OFF},
  {BFD_RELOC_TRICORE_42OFF2,    R_TRICORE_42OFF2},
  {BFD_RELOC_TRICORE_42OFF4,    R_TRICORE_42OFF4},
  {BFD_RELOC_TRICORE_2OFF,      R_TRICORE_2OFF},
  {BFD_RELOC_TRICORE_8CONST2,   R_TRICORE_8CONST2},
  {BFD_RELOC_TRICORE_4POS,      R_TRICORE_4POS},
  {BFD_RELOC_TRICORE_16SM2,     R_TRICORE_16SM2},
  {BFD_RELOC_TRICORE_5REL,      R_TRICORE_5REL},
  {BFD_RELOC_VTABLE_ENTRY,      R_TRICORE_GNU_VTENTRY},
  {BFD_RELOC_VTABLE_INHERIT,    R_TRICORE_GNU_VTINHERIT},
  {BFD_RELOC_TRICORE_PCREL16,	R_TRICORE_PCREL16},
  {BFD_RELOC_TRICORE_PCREL8,	R_TRICORE_PCREL8},
  {BFD_RELOC_TRICORE_GOT,       R_TRICORE_GOT},
  {BFD_RELOC_TRICORE_GOT2,      R_TRICORE_GOT2},
  {BFD_RELOC_TRICORE_GOTHI,     R_TRICORE_GOTHI},
  {BFD_RELOC_TRICORE_GOTLO,     R_TRICORE_GOTLO},
  {BFD_RELOC_TRICORE_GOTLO2,    R_TRICORE_GOTLO2},
  {BFD_RELOC_TRICORE_GOTUP,     R_TRICORE_GOTUP},
  {BFD_RELOC_TRICORE_GOTOFF,    R_TRICORE_GOTOFF},
  {BFD_RELOC_TRICORE_GOTOFF2,   R_TRICORE_GOTOFF2},
  {BFD_RELOC_TRICORE_GOTOFFHI,  R_TRICORE_GOTOFFHI},
  {BFD_RELOC_TRICORE_GOTOFFLO,  R_TRICORE_GOTOFFLO},
  {BFD_RELOC_TRICORE_GOTOFFLO2, R_TRICORE_GOTOFFLO2},
  {BFD_RELOC_TRICORE_GOTOFFUP,  R_TRICORE_GOTOFFUP},
  {BFD_RELOC_TRICORE_GOTPC,     R_TRICORE_GOTPC},
  {BFD_RELOC_TRICORE_GOTPC2,    R_TRICORE_GOTPC2},
  {BFD_RELOC_TRICORE_GOTPCHI,   R_TRICORE_GOTPCHI},
  {BFD_RELOC_TRICORE_GOTPCLO,   R_TRICORE_GOTPCLO},
  {BFD_RELOC_TRICORE_GOTPCLO2,  R_TRICORE_GOTPCLO2},
  {BFD_RELOC_TRICORE_GOTPCUP,   R_TRICORE_GOTPCUP},
  {BFD_RELOC_TRICORE_PLT,       R_TRICORE_PLT},
  {BFD_RELOC_TRICORE_COPY,      R_TRICORE_COPY},
  {BFD_RELOC_TRICORE_GLOB_DAT,  R_TRICORE_GLOB_DAT},
  {BFD_RELOC_TRICORE_JMP_SLOT,  R_TRICORE_JMP_SLOT},
  {BFD_RELOC_TRICORE_RELATIVE,  R_TRICORE_RELATIVE},
  {BFD_RELOC_TRICORE_BITPOS,    R_TRICORE_BITPOS},
  {BFD_RELOC_TRICORE_SBREG_S2, R_TRICORE_SBREG_S2},
  {BFD_RELOC_TRICORE_SBREG_S1, R_TRICORE_SBREG_S1},
  {BFD_RELOC_TRICORE_SBREG_D,  R_TRICORE_SBREG_D},
  {BFD_RELOC_TRICORE_18ABS_14,  R_TRICORE_18ABS_14}
};

static int nr_maps = sizeof tricore_reloc_map / sizeof tricore_reloc_map[0];

/* the number of the core to link 
   default is core 0 */
unsigned int tricore_elf32_core_number = 0; 

/* True if we should compress bit objects during the relaxation pass.  */

bfd_boolean tricore_elf32_relax_bdata = FALSE;

/* True if we should relax call and jump instructions whose target
   addresses are out of reach.  */

bfd_boolean tricore_elf32_relax_24rel = FALSE;

/* True if we should output diagnostic messages when relaxing sections.  */

bfd_boolean tricore_elf32_debug_relax = FALSE;

/* If >= 0, describes the address mapping scheme for PCP sections.  */

int tricore_elf32_pcpmap = -1;

/* True if PCP address mappings should be printed (for debugging only).  */

bfd_boolean tricore_elf32_debug_pcpmap = FALSE;

/* True if small data accesses should be checked for non-small accesses.  */

bfd_boolean tricore_elf32_check_sdata = FALSE;

/* the core architecture of the executable set in the eflags of the ELF header*/
unsigned long tricore_core_arch = EF_EABI_TRICORE_V1_2;

/* Forward declarations.  */

static void tricore_elf32_add_call_symbol(const char *name);
static bfd_boolean tricore_elf32_call_symbol_hash_create(bfd *abfd);

static reloc_howto_type *tricore_elf_reloc_name_lookup 
	 PARAMS ((bfd *abfd ATTRIBUTE_UNUSED, const char *r_name));

static reloc_howto_type *tricore_elf32_reloc_type_lookup
     PARAMS ((bfd *, bfd_reloc_code_real_type));

static void tricore_elf32_info_to_howto
     PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));

static void tricore_elf32_final_sda_bases
     PARAMS ((bfd *, struct bfd_link_info *,bfd_boolean));

static bfd_reloc_status_type tricore_elf32_final_sda_base
     PARAMS ((struct bfd_link_info *,asection *, bfd_vma *, int *,bfd_boolean));

static bfd_boolean tricore_elf32_merge_private_bfd_data PARAMS ((bfd *, bfd *));

static bfd_boolean tricore_elf32_copy_private_bfd_data PARAMS ((bfd *, bfd *));

const bfd_target *tricore_elf32_object_p PARAMS ((bfd *));

static bfd_boolean tricore_elf32_fake_sections
     PARAMS ((bfd *, Elf_Internal_Shdr *, asection *));

static bfd_boolean tricore_elf32_set_private_flags PARAMS ((bfd *, flagword));

static bfd_boolean tricore_elf32_section_flags
     PARAMS ((flagword *, const Elf_Internal_Shdr *));

static bfd_boolean tricore_elf32_final_gp
     PARAMS ((bfd *, struct bfd_link_info *));

static bfd_boolean tricore_elf32_relocate_section
     PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
              Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

static void tricore_elf32_set_arch_mach
     PARAMS ((bfd *, enum bfd_architecture));

static unsigned long tricore_elf32_get_bitpos
     PARAMS ((bfd *, struct bfd_link_info *, Elf_Internal_Rela *,
             Elf_Internal_Shdr *, Elf_Internal_Sym *,
             struct elf_link_hash_entry **, asection *, bfd_boolean *));

static bfd_boolean tricore_elf32_adjust_bit_relocs
     PARAMS ((bfd *, struct bfd_link_info *, unsigned long,
             bfd_vma, bfd_vma, int, unsigned int));

static bfd_boolean tricore_elf32_relax_section
     PARAMS ((bfd *, asection *, struct bfd_link_info *, bfd_boolean *));

void tricore_elf32_list_bit_objects PARAMS ((struct bfd_link_info *,FILE *));

static enum elf_reloc_type_class tricore_elf32_reloc_type_class
     PARAMS ((const Elf_Internal_Rela *));


static reloc_howto_type *
tricore_elf_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
               const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (tricore_elf32_howto_table) / sizeof (tricore_elf32_howto_table[0]);
       i++)
    if (tricore_elf32_howto_table[i].name != NULL
    && strcasecmp (tricore_elf32_howto_table[i].name, r_name) == 0)
      return &tricore_elf32_howto_table[i];

  return NULL;
}


/* Given a BFD reloc type CODE, return the corresponding howto structure.  */

static reloc_howto_type *
tricore_elf32_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  int i;

  if (code == BFD_RELOC_8)
    code = BFD_RELOC_TRICORE_8ABS;
  else if (code == BFD_RELOC_16)
    code = BFD_RELOC_TRICORE_16ABS;
  else if (code == BFD_RELOC_32)
    code = BFD_RELOC_TRICORE_32ABS;
  else if (code == BFD_RELOC_32_PCREL)
    code = BFD_RELOC_TRICORE_32REL;
  else if (code == BFD_RELOC_16_PCREL)
    code = BFD_RELOC_TRICORE_PCREL16;
  else if (code == BFD_RELOC_8_PCREL)
    code = BFD_RELOC_TRICORE_PCREL8;
		   
  for (i = 0; i < nr_maps; ++i)
    if (tricore_reloc_map[i].bfd_reloc_val == code)
      return &tricore_elf32_howto_table[tricore_reloc_map[i].tricore_val];

  bfd_set_error (bfd_error_bad_value);
  return (reloc_howto_type *) 0;
}

/* Set CACHE_PTR->howto to the howto entry for the relocation DST.  */

static void
tricore_elf32_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  BFD_ASSERT (ELF32_R_TYPE (dst->r_info) < (unsigned int) R_TRICORE_max);
  cache_ptr->howto = &tricore_elf32_howto_table[ELF32_R_TYPE (dst->r_info)];
}

/* Return the class a relocation belongs to.  */

static enum elf_reloc_type_class
tricore_elf32_reloc_type_class (rela)
     const Elf_Internal_Rela *rela;
{
  switch ((int) ELF32_R_TYPE (rela->r_info))
    {
    case R_TRICORE_RELATIVE:
      return reloc_class_relative;

    case R_TRICORE_JMP_SLOT:
      return reloc_class_plt;

    case R_TRICORE_COPY:
      return reloc_class_copy;

    default:
      return reloc_class_normal;
    }
}

/* Create a special linker section (".sdata"/".sbss", or ".zdata"/".zbss",
   depending on the value of WHICH).  */
static bfd_boolean
tricore_elf32_add_symbol_hook (bfd *abfd ATTRIBUTE_UNUSED,
                            struct bfd_link_info *info ATTRIBUTE_UNUSED,
                            Elf_Internal_Sym *sym ATTRIBUTE_UNUSED,
                            const char **namep ATTRIBUTE_UNUSED,
                            flagword *flagsp ATTRIBUTE_UNUSED,
                            asection **secp ATTRIBUTE_UNUSED,
                            bfd_vma *valp ATTRIBUTE_UNUSED)
{
  if (strncmp(*namep,"__caller.",9) == 0 
      || strncmp(*namep,"__callee.",9) == 0)
   tricore_elf32_add_call_symbol(*namep);
#if 0
  /* the _SMALL_DATA_ pointer and the special sections .sdata*/.sbss*
   will be created in tricore_elf32_final_sda_bases if necessary */
  if (! info->relocatable
      && strcmp (*namep, "_SMALL_DATA_") == 0
      && bfd_get_flavour(info->output_bfd) == bfd_target_elf_flavour)
    {
      /* This is simpler than using _bfd_elf_create_linker_section
         (our needs are simpler than ppc's needs).  Also
         _bfd_elf_create_linker_section currently has a bug where if a .sdata
         section already exists a new one is created that follows it which
         screws of _SDA_BASE_ address calcs because output_offset != 0.  */
      struct elf_link_hash_entry *h;
      struct bfd_link_hash_entry *bh;
      asection *s = bfd_get_section_by_name (abfd, ".sdata");

      /* The following code was cobbled from elf32-ppc.c and elflink.c.  */

      if (s == NULL)
        {
          flagword flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
                            | SEC_IN_MEMORY | SEC_LINKER_CREATED);
          s = bfd_make_section_anyway (abfd, ".sdata");
          if (s == NULL)
            return FALSE;
          bfd_set_section_flags (abfd, s, flags);
          bfd_set_section_alignment (abfd, s, 2);
        }
      h = (struct elf_link_hash_entry *)
       bfd_link_hash_lookup (info->hash, "_SMALL_DATA_", FALSE, FALSE, FALSE);

      bh = (struct bfd_link_hash_entry *)h;
      if ((h == NULL || h->root.type == bfd_link_hash_undefined)
          && !(_bfd_generic_link_add_one_symbol (info,
                                                 abfd,
                                                 "_SMALL_DATA_",
                                                 BSF_GLOBAL,
                                                 s,
                                                 (bfd_vma) 32768,
                                                 (const char *) NULL,
                                                 FALSE,
                                                 get_elf_backend_data (abfd)->collect,
                                                 (struct bfd_link_hash_entry **) &bh)))
        return FALSE;
      h = (struct elf_link_hash_entry *)bh;
      h->type = STT_OBJECT;
    }
#endif
  return TRUE;
}




/* This is called once from tricore_elf32_relocate_section, before any
   relocation has been performed.  We need to determine the final offset
   for the GOT pointer, which is currently zero, meaning that the symbol
   "_GLOBAL_OFFSET_TABLE_" will point to the beginning of the ".got"
   section.  There are, however, two cases in which this is undesirable:

      1. If the GOT contains more than 8192 entries, single TriCore
	 instructions can't address the excessive entries with their
	 16-bit signed offset.  Of course, that's only a problem
	 when there are modules compiled with "-fpic".

      2. In a shared object, the GOT pointer is also used to address
         variables in SDA0, so the combined size of the ".got", ".sbss"
	 and ".sdata" sections must not exceed 32k (well, of course the
	 combined size of these sections can be 64k, but not if the
	 GOT pointer offset is zero).

   To address these problems, we use the following algorithm to determine
   the final GOT offset:

      1. If the combined size of the ".got", ".sbss" and ".sdata"
	 sections is <= 32k, we'll keep the zero offset.

      2. If the GOT contains more than 8192 entries, we'll
	 set the offset to 0x8000, unless doing that would
	 render any SDA entries unaccessible.

      3. In all other cases, we'll set the offset to the size
	 of the ".got" section minus 4 (because of the _DYNAMIC
	 entry at _GLOBAL_OFFSET_TABLE_[0]).
	 
   In any case, if either ".sdata" or ".sbss" is non-empty, we're adjusting
   the symbol "_SMALL_DATA_" to have the same value as "_GLOBAL_OFFSET_TABLE_",
   as both the GOT and the SDA are addressed via the same register (%a12).
   Note that the algorithm described above won't guarantee that all GOT
   and SDA entries are reachable using a 16-bit offset -- it's just
   increasing the probability for this to happen.  */

static bfd_boolean
tricore_elf32_final_gp (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  asection *sgot;

  sgot = bfd_get_section_by_name (output_bfd, ".got");
  if (sgot && (sgot->rawsize > 0))
    {
      struct elf_link_hash_entry *h;
      bfd_vma gp, sda;
      asection *sdata = NULL, *sbss = NULL;
      long got_size = sgot->rawsize, sda_size = 0;
      long gp_offset;

      if (info->shared)
        {
	  sdata = bfd_get_section_by_name (output_bfd, ".sdata");
          sbss = bfd_get_section_by_name (output_bfd, ".sbss");
          if (sdata != NULL)
            {
              if (sdata->size != 0)
	        sda_size += sdata->size;
	      else
	        sda_size += sdata->rawsize;
	    }

          if (sbss != NULL)
            {
              if (sbss->size != 0)
	        sda_size += sbss->size;
	      else
	        sda_size += sbss->rawsize;
	    }

          if (sda_size > (0x8000 - 4))
            {
	      (*_bfd_error_handler) (_("%s: Too many SDA entries (%ld bytes)"),
	  			     bfd_get_filename(output_bfd),
				     sda_size);
	      return FALSE;
	    }
        }

      if ((got_size + sda_size) <= 0x8000)
        gp_offset = 0;
      else if ((got_size > 0x8000)
	       && ((sda_size + got_size - 0x8000) <= 0x8000))
	gp_offset = 0x8000;
      else
        gp_offset = got_size - 4;

      if (gp_offset != 0)
	elf_hash_table (info)->hgot->root.u.def.value = gp_offset;

      /* If there's any data in ".sdata"/".sbss", set the value of
         _SMALL_DATA_ to that of the GOT pointer.  */
      if (((sdata != NULL) && (sdata->rawsize > 0))
          || ((sbss != NULL) && (sbss->rawsize > 0)))
        {
	  h = (struct elf_link_hash_entry *)
	       bfd_link_hash_lookup (info->hash, "_SMALL_DATA_",
	  			     FALSE, FALSE, FALSE);
	  if (h == NULL)
	    {
	      /* This can't possibly happen, as we're always creating the
	         ".sdata"/".sbss" output sections and the "_SMALL_DATA_"
		 symbol in tricore_elf32_check_relocs.  */
	      (*_bfd_error_handler)
	       (_("%s: SDA entries, but _SMALL_DATA_ undefined"),
	        bfd_get_filename(output_bfd));
	      return FALSE;
	    }
	  gp = gp_offset + sgot->output_section->vma + sgot->output_offset;
	  sdata = h->root.u.def.section;
	  sda = sdata->output_section->vma + sdata->output_offset;
	  h->root.u.def.value = gp - sda;
	}
    }

  return TRUE;
}

static void
tricore_elf32_generate_sdas(int coreid, int sdatanr, bfd *output_bfd,
     struct bfd_link_info *info, bfd_boolean create_gp)
{
  sda_t *sda, *tpsda;
  asection *data, *bss;
  struct bfd_link_hash_entry *h;
  char *pdata,*pbss,*psym;
  const char * corename;

  corename = htc_elf32_get_core_name_from_id(coreid);

  if ((corename == NULL) || !strcmp(corename,"GLOBAL"))
    return;

  sda = bfd_malloc(sizeof(sda_t)*4);
  /* allocate buffers for the different names */
  pdata = bfd_malloc(strlen(".") + strlen(corename) + strlen(".sdata0") + 1); 
  pbss = bfd_malloc(strlen(".") + strlen(corename) + strlen(".sbss0") + 1); 
  psym = bfd_malloc(strlen("_SMALL_DATA0__") + strlen(corename) + strlen("_") + 1); 
  if (sda == NULL || pdata == NULL || pbss == NULL || psym == NULL)
    {
      (*_bfd_error_handler) (_("fatal: virtual memory exhausted"));
      return;
    }
  if (sdatanr == 1)
    {
      sprintf(pdata,".%s.sdata",corename);
      sprintf(pbss,".%s.sbss",corename);
      sprintf(psym,"_SMALL_DATA__%s_",corename);
    }
  else
    {
      sprintf(pdata,".%s.sdata%d",corename,sdatanr);
      sprintf(pbss,".%s.sbss%d",corename,sdatanr);
      sprintf(psym,"_SMALL_DATA%d__%s_",sdatanr,corename);
    }
  /* 
   * if we have alread a description for this core return 
   */
  for (tpsda = small_data_areas; tpsda != NULL; tpsda = tpsda->next)
    {
      if (!strcmp(tpsda->data_section_name, pdata))
        {
          free(sda);
          free(pdata);
          free(pbss);
          free(psym);
          return;
        }
    }
  sda->data_section_name = pdata;
  sda->bss_section_name = pbss;
  sda->sda_symbol_name = psym;
  data = sda->data_section = bfd_get_section_by_name (output_bfd, sda->data_section_name);
  bss = sda->bss_section = bfd_get_section_by_name (output_bfd, sda->bss_section_name);
  switch (sdatanr)
    {
    default:
    case 1:
      sda->areg = 0;
      break;
    case 2:
      sda->areg = 1;
      break;
    case 3:
      sda->areg = 8;
      break;
    case 4:
      sda->areg = 9;
      break;
    }
  if (data || bss)
    {
      if (!data)
        data = bss;
      h = bfd_link_hash_lookup (info->hash, sda->sda_symbol_name,
      				TRUE, FALSE, TRUE);
      if (h->type != bfd_link_hash_defined)
        {
          h->u.def.value = 32768;
	  h->u.def.section = data;
	  h->type = bfd_link_hash_defined;
	}
      if (create_gp)
        {
          sda->gp_value = (h->u.def.value
		       + h->u.def.section->output_section->vma
		       + h->u.def.section->output_offset
			);
          sda->valid = TRUE;
        }
      else
        {
          sda->valid = FALSE;
          sda->gp_value = 0;
        }
    }
  sda->next = NULL;
  small_data_last->next = sda;
  small_data_last = sda;
  return;
}

void tricore_elf32_define_sda_section(struct bfd_link_info *info, 
                                      bfd * output_bfd,
                                      const char *sec_name,
                                      const char *base_sym,
                                      const char *reg_name)
{
  sda_t *sda, *tpsda;
  asection *data;
  struct bfd_link_hash_entry *h;

  for (tpsda = small_data_areas; tpsda != NULL; tpsda = tpsda->next)
    {
      if (!strcmp(tpsda->data_section_name, sec_name))
        {
          (*_bfd_error_handler) (_("warning: Section %s is already defined as small data section"),sec_name);
          return;
        }
    }
        
  sda = bfd_malloc(sizeof(sda_t)*4);
  if (sda == NULL)
    {
      (*_bfd_error_handler) (_("fatal: virtual memory exhausted"));
      return;
    }
  memset((void *)sda,0,sizeof(*sda));

  sda->areg = atoi(reg_name+1);
  sda->data_section_name = sec_name;
  sda->bss_section_name = NULL;
  sda->sda_symbol_name = base_sym;
  sda->next = NULL;
  small_data_last->next = sda;
  small_data_last = sda;

  if (output_bfd)
    {
      data = sda->data_section = bfd_get_section_by_name (output_bfd, sda->data_section_name);

      h = bfd_link_hash_lookup (info->hash, sda->sda_symbol_name,
                                TRUE, FALSE, TRUE);
      if (h->type != bfd_link_hash_defined)
        {
          h->u.def.value = 32768;
          h->u.def.section = data;
          h->type = bfd_link_hash_defined;
        }
    }
//    (*_bfd_error_handler) (_("warning: definition sda sections not yet supported"));
}

/* This is called once from tricore_elf32_relocate_section, before any
   relocation has been performed.  We need to determine the final values
   for the various small data area base pointers, so that the 1[06]SM
   relocations (if any) can be handled correctly.  Of course, this is
   only necessary for a final link.  Note, however, that if this is a
   final dynamic link, ".sdata"/".sbss" will be adressed via %a12 instead
   of %a0, and the base pointer value for this particular area has
   already been computed by tricore_elf32_final_gp above.  */

static void
tricore_elf32_final_sda_bases (output_bfd, info,create_gp)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd_boolean create_gp;
{
  sda_t *sda;
  asection *data, *bss;
  struct bfd_link_hash_entry *h;

  if (info->relocatable)
    return;

  for (sda = small_data_areas; sda != NULL; sda = sda->next)
    {
      if (sda->data_section_name)
        data = sda->data_section =
         bfd_get_section_by_name (output_bfd, sda->data_section_name);
      else
        data = sda->data_section = NULL;
      if (sda->bss_section_name)
        bss = sda->bss_section =
         bfd_get_section_by_name (output_bfd, sda->bss_section_name);
      else 
        bss = sda->bss_section = NULL;
      if (!data && !bss)
        continue;

      if (!data)
        data = bss;
      h = bfd_link_hash_lookup (info->hash, sda->sda_symbol_name,
                                TRUE, FALSE, TRUE);
      if ((sda == small_data_areas) && info->shared)
        {
          sda->areg = 12;
          BFD_ASSERT (h->type == bfd_link_hash_defined);
        }
      else if (h->type != bfd_link_hash_defined)
        {
          h->u.def.value = 32768;
          h->u.def.section = data;
          h->type = bfd_link_hash_defined;
        }
      /* HL 2007-05-07 F0210 I think we only have to add vma without ouput_offset 
         because the output_offset is the offset of the input section .sdata to the beginning
         of the output section .sdata
         */
#if 0
      /* adjust the value to the beginning of the output section */
      h->u.def.value -= h->u.def.section->output_offset;
#endif
      if (create_gp)
        {
          sda->gp_value = (h->u.def.value
                           + h->u.def.section->output_section->vma
#if 1
                           + h->u.def.section->output_offset
#endif
                          );
          sda->valid = TRUE;
        }
    }
  /* walk through all output section and look for sdata and generate the base
  symbols */
  for (data = output_bfd->sections; data != NULL; data = data->next)
    {
      //bfd_vma sda_base = 0;
      //int     sda_reg = 0;
      //tricore_elf32_final_sda_base(info,data,&sda_base,&sda_reg,create_gp);

      if (strstr(data->name,".sdata") || strstr(data->name,".sbss"))
        {
          /* build base symbol and calculate GP value */
          unsigned int  coreid;

          coreid = SEC_CORE_NUMBER_GET(data->flags);

          if (coreid <= 0)
            {
              /* should be a standard section handled above */
              continue;
            }
          /* we have to link for multicore generate the small data section
             descriptions for this core */
          tricore_elf32_generate_sdas(coreid,1,output_bfd,info,create_gp);
          tricore_elf32_generate_sdas(coreid,2,output_bfd,info,create_gp);
          tricore_elf32_generate_sdas(coreid,3,output_bfd,info,create_gp);
          tricore_elf32_generate_sdas(coreid,4,output_bfd,info,create_gp);
        }

#if 0
          basesym_name = bfd_malloc (strlen ("_SMALL_DATA_") + strlen ("_CORE") + 3);
          if (basesym_name == NULL)
            {
              (*_bfd_error_handler) (_("fatal: virtual memory exhausted"));
              return;
            }
          /* determine the base reg */
          cp = strstr(data->name,".sdata");
          if (cp == 0)
            {
              cp = strstr(data->name,".sbss");
              cp += strlen(".sbss");
            }
          else
            cp += strlen(".sdata");
          switch (*cp)
            {
            case '.':
            case 0:
              sda_reg = 0;
              break;
            case '2':
              sda_reg = 1;
              break;
            case '3':
              sda_reg = 8;
              break;
            case '4':
              sda_reg = 9;
              break;
            default:
              (*_bfd_error_handler)(_("wrong .sdata section name %s"),data->name);
              return;
            }
          if (*cp != 0)
            sprintf(basesym_name,"%s%c%s%d","_SMALL_DATA",*cp,"_CORE",coreid);
          else
            sprintf(basesym_name,"%s%s%d","_SMALL_DATA","_CORE",coreid);

          /* generate base symbol name entry */
          h = bfd_link_hash_lookup (info->hash, basesym_name, TRUE, FALSE, TRUE);
          if (h->type != bfd_link_hash_defined)
            {
              h->u.def.value = 32768;
              h->u.def.section = data;
              h->type = bfd_link_hash_defined;
            }
          h->u.def.value -= h->u.def.section->output_offset;
          if (create_gp)
            {
              sda_base = (h->u.def.value
                           + h->u.def.section->output_section->vma
                           + h->u.def.section->output_offset);
            }
          /* create sda_t entry in sda list */
          sda = bfd_malloc(sizeof(sda_t));
          if (sda == NULL)
            {
              (*_bfd_error_handler) (_("fatal: virtual memory exhausted"));
              return;
            }
          printf("SDA: found small data section %s of core %d basesymbol %s\n",
                 data->name,coreid,basesym_name);
#endif
    }
}

/* This is called by tricore_elf32_relocate_section for each 1[06]SM
   relocation.  Check if the target of the relocation is within a known
   and defined output section OSEC (i.e., .s{data,bss}{,2,3,4}), and if
   so, return bfd_reloc_ok and the respective SDA base address and base
   address register in *SDA_BASE and *SDA_REG.  Otherwise, return
   bfd_reloc_dangerous to indicate an error.  */

static bfd_reloc_status_type
tricore_elf32_final_sda_base (info ,osec, sda_base, sda_reg,create_gp )
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     asection *osec;
     bfd_vma *sda_base;
     int *sda_reg;
     bfd_boolean create_gp ATTRIBUTE_UNUSED;
{
  sda_t *sda;

  for (sda = small_data_areas; sda != NULL; sda = sda->next)
    {
      if (!sda->valid)
        continue;

      if ((sda->data_section == osec) || (sda->bss_section == osec))
        {
	  *sda_base = sda->gp_value;
	  *sda_reg = sda->areg;
	  return bfd_reloc_ok;
	}
    }
  return bfd_reloc_dangerous;
}

/* Relocate a TriCore ELF section.  This function is responsible for
   adjusting the section contents as necessary, and, if generating a
   relocateable output file, adjusting the reloc addend as necessary.
   This function does not have to worry about setting the reloc
   address or the reloc symbol index.  LOCAL_SYMS is a pointer to
   the swapped-in local symbols.  LOCAL_SECTIONS is an array giving
   the section in the input file corresponding to the st_shndx field
   of each local symbol.  The global hash table entry for the global
   symbols can be found via elf_sym_hashes (input_bfd).  When generating
   relocateable output, this function must handle STB_LOCAL/STT_SECTION
   symbols specially: the output symbol is going to be the section
   symbol corresponding to the output section, which means that the
   addend must be adjusted accordingly.  */

static bfd_boolean
tricore_elf32_relocate_section (output_bfd, info, input_bfd, input_section,
			        contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (input_bfd), *h;
  unsigned long r_symndx, insn;
  int sda_reg, rel_abs, len32 = 0, r_type;
  const char *sym_name, *sec_name, *errmsg = NULL;
  static bfd_boolean final_got = FALSE;
  static bfd_boolean final_sda_bases = FALSE;
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  Elf_Internal_Sym *sym;
  Elf_Internal_Rela *rel, *relend;
  bfd *dynobj = elf_hash_table (info)->dynobj;
  bfd_vma got_base = 0, got_ptr = 0, sda_base, addend, offset, relocation;
  bfd_reloc_status_type r;
  bfd_byte *byte_ptr = NULL;
  asection *sreloc = NULL, *sec;
  reloc_howto_type *howto;
  bfd_boolean bitpos_seen = FALSE;
  bfd_boolean do_pcpmap = FALSE;
  bfd_boolean ret = TRUE; /* Assume success.  */

#define CHECK_DISPLACEMENT(off,min,max)					\
  if (off & 1)								\
    {									\
      errmsg = _("Displacement is not even");				\
      break;								\
    }									\
  else if (((int) (off) < (min)) || ((int) (off) > (max)))		\
    {									\
      errmsg = _("Displacement overflow");				\
      break;								\
    }

  if (!final_got && !info->relocatable)
    {
      /* Determine the final values for "_GLOBAL_OFFSET_TABLE_" and the
         defined SDA symbols; also, if a map file is to be generated,
         show the addresses assigned to bit objects.  */
      final_got = TRUE;
      if (!tricore_elf32_final_gp (output_bfd, info))
        return FALSE;

      if (final_sda_bases == FALSE)
        {
          tricore_elf32_final_sda_bases (output_bfd, info,TRUE);
          final_sda_bases = TRUE;
        }
    }

  /* Initialize various variables needed for a final link.  */
  sreloc = NULL;
  if (elf_hash_table (info)->hgot == NULL)
    got_base = got_ptr = 0;
  else
    {
      got_base = elf_hash_table (info)->hgot->root.u.def.value;
      got_ptr = (got_base
                 + (elf_hash_table (info)
                    ->hgot->root.u.def.section->output_section->vma)
                 + (elf_hash_table (info)
                    ->hgot->root.u.def.section->output_offset));
    }

  /* See if we need to perform PCP address mappings.  */
  if ((tricore_elf32_pcpmap >= 0)
      && ((input_section->flags & PCP_SEG)
          || !strcmp (input_section->name, ".pcp_c_ptr_init")
          || !strncmp (input_section->name, ".pcp_c_ptr_init.", 16)))
    do_pcpmap = TRUE;


  /* Walk through all relocs of this input section.  */
  relend = relocs + input_section->reloc_count;
  for (rel = relocs; rel < relend; rel++)
    {
      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);

      if ((r_type < 0)
          || (r_type < R_TRICORE_NONE)
	  || (r_type >= R_TRICORE_max))
	{
	  (*_bfd_error_handler) (_("%s: unknown relocation type %d"),
				 bfd_get_filename (input_bfd), r_type);
	  bfd_set_error (bfd_error_bad_value);
	  ret = FALSE;
	  bitpos_seen = FALSE;
	  continue;
	}
      else if (r_type == R_TRICORE_NONE)
        {
	  /* This relocation type is typically caused by the TriCore-specific
	     relaxation pass, either because it has changed R_TRICORE_BITPOS
	     relocations against local bit variables to refer to the actual
	     symbol representing their respective bit offset, or because it
	     was found that a call instruction requires a trampoline to reach
	     its target (in which case the call instruction is modified to
	     directly branch to the trampoline, so the original relocation
	     against the target needs to be nullified, while the instructions
	     within the trampoline will add their own relocation entries).  */
	  bitpos_seen = FALSE;
          continue;
	}

      /* This is a final link.  All kinds of strange things may happen...  */

      if ((r_type == R_TRICORE_GNU_VTENTRY)
	  || (r_type == R_TRICORE_GNU_VTINHERIT))
	{
	  /* Handled by GC (if enabled via "--gc-sections", that is).  */
          continue;
	}

      if (r_type == R_TRICORE_BITPOS)
        {
	  /* The next relocation will be against a bit object; remember that
	     its bit position should be used rather than its address.  */
	  bitpos_seen = TRUE;
	  continue;
	}

      howto = tricore_elf32_howto_table + r_type;
      addend = rel->r_addend;
      offset = rel->r_offset;
      r = bfd_reloc_ok;
      h = NULL;
      sym = NULL;
      sec = NULL;
      insn = rel_abs = 0;
      errmsg = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
          bfd_vma reloc_alt;
	  /* Relocation against a local symbol.  */
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  sym_name = "<local symbol>";
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
          /* look for an alternate address */
          reloc_alt = htc_elf32_get_alternate_address (relocation,
                                                        sym->st_other,
                                                        sec->output_section,
                                                        input_section->output_section);
          if ((reloc_alt != relocation) && 0)
            fprintf(stderr,"LOC alternate address 0x%x <-> 0x%x (from %s -> %s at 0x%x)\n"
                    , (unsigned int)relocation, (unsigned int)reloc_alt,
                    sec->output_section->name,input_section->output_section->name,
                    (unsigned int)rel->r_offset);
          relocation = reloc_alt;
	}
      else
	{
          bfd_vma reloc_alt;
	  /* Relocation against an external symbol.  */
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while ((h->root.type == bfd_link_hash_indirect)
		 || (h->root.type == bfd_link_hash_warning))
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  sym_name = h->root.root.string;

	  /* Can this relocation be resolved immediately?  */

	  if ((h->root.type == bfd_link_hash_defined)
	      || (h->root.type == bfd_link_hash_defweak))
	    {
	      sec = h->root.u.def.section;
	      if (sec->output_section == NULL)
		{
		  /* Set a flag that will be cleared later if we find a
		     relocation value for this symbol.  output_section
		     is typically NULL for symbols satisfied by a shared
		     library.  */
		  relocation = 0;
		}  
	      else if (howto->pc_relative && (sec == bfd_abs_section_ptr))
		{
		  rel_abs = 1;
		  relocation = (h->root.u.def.value
			        + sec->output_section->vma);
		}
	      else
		relocation = (h->root.u.def.value
			      + sec->output_section->vma
			      + sec->output_offset);

              /* look for an alternate address */
              reloc_alt = htc_elf32_get_alternate_address (relocation,
                                                            h->other,
                                                            sec->output_section,
                                                            input_section->output_section);
          if ((reloc_alt != relocation) && 0)
            fprintf(stderr,"EXT alternate address 0x%x <-> 0x%x (from %s -> %s at 0x%x)\n"
                    , (unsigned int)relocation, (unsigned int)reloc_alt,
                    sec->output_section->name,input_section->output_section->name,
                    (unsigned int)rel->r_offset);
          relocation = reloc_alt;
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
          else if (info->shared
                   && ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)
	    relocation = 0;
	  else if (!info->relocatable)
	    {
              if (!((*info->callbacks->undefined_symbol)
                    (info, h->root.root.string, input_bfd,
                     input_section, rel->r_offset,
                     (!info->shared 
                      || ELF_ST_VISIBILITY (h->other)))))
                return FALSE;

	      ret = FALSE;
	      bitpos_seen = FALSE;
	      continue;
	    }
        }
      if (sec != NULL && elf_discarded_section (sec))
	{
	  /* For relocs against symbols from removed linkonce sections,
	     or sections discarded by a linker script, we just want the
	     section contents zeroed.  Avoid any special processing.  */
	  howto = NULL;
	  if (r_type < R_TRICORE_max)
	    howto = tricore_elf32_howto_table + r_type;
	  _bfd_clear_contents (howto, input_bfd, contents + rel->r_offset);
	  rel->r_info = 0;
	  rel->r_addend = 0;
	  continue;
	}
      if (info->relocatable)
        continue;

      /* The linker's optimization passes may have deleted entries in
         the ".eh_frame" (ELF only) and ".stab" sections; as a result,
	 we have to skip relocations that are no longer valid, and to
	 compute and use the (potentially) new offsets of entries that
	 couldn't be eliminated during said optimization passes.  */
      {
	bfd_vma n_offset;

	n_offset = _bfd_elf_section_offset (output_bfd, info,
	      				    input_section, offset);
	if ((n_offset == (bfd_vma) -1)  /* eh_frame entry deleted.  */
	    || (n_offset == (bfd_vma) -2))  /* stab entry deleted.  */
	  continue;

	offset = n_offset;
      }

      /* Sanity check the address.  Note that if the relaxation pass has
         been enabled, additional instructions and relocs for these may
	 have been created, so we must compare the offset against the
	 section's cooked size.  This is okay even if relaxation hasn't
	 been enabled, or if it hasn't increased the size, because in the
	 former case, the linker will have set the cooked size to the raw
	 size in lang_size_sections_sections_1, while in the latter case
	 this is done by tricore_elf32_relax_section itself.  Likewise, if
	 relaxing has decreased the section size, the cooked size will be
	 smaller than the raw size, so if there are any relocations against
	 locations beyond the cooked size, this indicates an error.  */
      if (offset > input_section->size)
	{
	  r = bfd_reloc_outofrange;
	  goto check_reloc;
	}

      /* If PC-relative, adjust the relocation value appropriately.  */
      if (howto->pc_relative && !rel_abs)
        {
          relocation -= (input_section->output_section->vma
                         + input_section->output_offset);
          if (howto->pcrel_offset)
            relocation -= offset;
        }

      /* Add the r_addend field.  */
      relocation += addend;

      /* Special case: the previous relocation was R_TRICORE_BITPOS,
         which means that the relocation value is not the symbol's
	 address, but its bit position.  */
      if (bitpos_seen)
        {
	  relocation = tricore_elf32_get_bitpos (input_bfd, info, rel,
	  					 symtab_hdr,
						 local_syms, sym_hashes,
						 input_section, &ret);
	  bitpos_seen = FALSE;
	}
      else if (do_pcpmap && sec
      	       && ((sec->flags & SEC_DATA) || !(sec->flags & SEC_LOAD)))
	{
          /* Perform PCP address mapping based on the relocation value
	     and the specified mapping scheme (there's currently only
	     one of them, so we don't need to check the actual value of
	     tricore_elf32_pcpmap).  Note that we don't care about the
	     symbol or section the relocation value was derived from;
	     this is because even knowing the type of a symbol or a
	     section won't help in deciding automatically if a relocation
	     value should be mapped or not (e.g., if we would exempt
	     absolute symbols from being mapped, this would fail if the
	     symbol would hold the absolute address of a variable that
	     is located within a mappable memory region).  This sort of
	     hardware design really, really sucks big time!  */
          if ((relocation >= 0xc0000000) && (relocation < 0xd4100000))
	    {
	      bfd_vma poffset = 0;

	      if (relocation < 0xc0400000)
	        poffset = 0x28000000;
	      else if (relocation >= 0xd0000000)
	        {
		  if (relocation < 0xd0100000)
		    poffset = 0x18400000;
		  else if (relocation >= 0xd4000000)
		    poffset = 0x14500000;
		}

	      if (poffset)
	        {
		  if (tricore_elf32_debug_pcpmap)
		    {
		      printf ("PCPMAP: %s(%s+%ld): 0x%08lx [",
			      bfd_get_filename(input_section->owner),
			      input_section->name,
			      offset,
			      relocation);
		      if (h)
			printf ("%s", sym_name);
		      else
			printf ("%s", sec->name);
		      if (addend)
			printf ("+%ld", addend);
		      printf ("] += 0x%08lx (= 0x%08lx)\n",
		      	      poffset, relocation + poffset);
		    }
		  relocation += poffset;
		}
	    }
	}
      else if (tricore_elf32_check_sdata && (h != NULL) && (sec != NULL)
      	       && !(input_section->output_section->flags & SEC_DEBUGGING))
        {
	  /* Check whether the referenced data lives in a small data area,
	     but is accessed as "normal" data (usually meaning that this
	     variable wasn't declared "small" in some module that accesses
	     it, but .debug sections will also reference small data vars
	     via ".word varname", leading to non-small data relocs; however,
	     we ignore these special cases, as they aren't meaningful).  */
	  switch (r_type)
	    {
	    case R_TRICORE_16SM:
	    case R_TRICORE_10SM:
	    case R_TRICORE_16SM2:
	    case R_TRICORE_SBREG_S2:
            case R_TRICORE_SBREG_S1:
            case R_TRICORE_SBREG_D:
	    	/* Ignore small data relocs; if they're wrong for whatever
		   reason, we'll soon find out in the code below.  */
	        break;

	    default:
	  	sec_name = sec->name;
	  	if (!strcmp (sec_name, ".sdata")
	            || !strcmp (sec_name, ".sbss")
	      	    || !strncmp (sec_name, ".sdata.", 7)
	      	    || !strncmp (sec_name, ".sbss.", 6))
		  {
		    const char *name;
		    char *msg;

		    /* Non-small data reloc against a global variable living in
		       some SDA; issue a warning.  Oops: the linker's default
		       warning callback won't output the name of the affected
		       variable; our remedy is to dynamically build the
		       warning message, including the variable's name.  */
#define NONSMALLMSG \
  _("warning: non-small data access to small data variable \"%s\"")
		    name = h->root.root.string;
		    /* Ignore accesses to each SDA's base symbol.  */
		    if (!strncmp (name, "_SMALL_DATA", 11)
		        && (!strcmp (name + 11, "_")
		            || !strcmp (name + 11, "2_")
		            || !strcmp (name + 11, "3_")
		            || !strcmp (name + 11, "4_")))
		      break;

		    msg = bfd_malloc (strlen (NONSMALLMSG) + strlen (name) + 1);
		    if (msg == NULL)
		      {
		        (*_bfd_error_handler)
			 (_("fatal: virtual memory exhausted"));

			return FALSE;
		      }
		    sprintf (msg, NONSMALLMSG, name);
#undef NONSMALLMSG
	            (void) ((*info->callbacks->warning)
		            (info, msg, name, input_bfd, input_section,
			    offset));
		    free (msg);
		  }
		break;
	    }
	}

      /* Now apply the fixup.  Handle simple data relocs first.  */
      byte_ptr = (bfd_byte *) contents + offset;

      switch (r_type)
        {
	case R_TRICORE_8ABS:
	case R_TRICORE_PCREL8:
	  bfd_put_8 (input_bfd, relocation, byte_ptr);
	  continue;

	case R_TRICORE_16ABS:
	case R_TRICORE_PCREL16:
	  bfd_put_16 (input_bfd, relocation, byte_ptr);
	  continue;

	case R_TRICORE_32ABS:
	case R_TRICORE_32REL:
	  if (info->shared
	      && (r_symndx != 0)
	      && (input_section->flags & SEC_ALLOC)
	      && ((r_type != R_TRICORE_32REL)
	          || ((h != NULL)
		      && (h->dynindx != -1)
		      && (!info->symbolic))))
	    {
	      Elf_Internal_Rela outrel;
	      bfd_boolean skip = FALSE, relocate = FALSE;

	      if (sreloc == NULL)
	        {
		  const char *name;

		  name = (bfd_elf_string_from_elf_section
		  	  (input_bfd,
			   elf_elfheader (input_bfd)->e_shstrndx,
			   elf_section_data (input_section)->rel_hdr.sh_name));
		  if (name == NULL)
		    return FALSE;

		  BFD_ASSERT (!strncmp (name, ".rela", 5)
		  	      && !strcmp (name + 5, bfd_get_section_name
			      			    (input_bfd,
					  	     input_section)));
		  sreloc = bfd_get_section_by_name (dynobj, name);
		  BFD_ASSERT (sreloc != NULL);
		}

	      outrel.r_offset =
	        _bfd_elf_section_offset (output_bfd, info, input_section,
					 rel->r_offset);
	      if (outrel.r_offset == (bfd_vma) -1)
	        skip = TRUE;  /* eh_frame entry deleted.  */
	      else if (outrel.r_offset == (bfd_vma) -2)
	        skip = relocate = TRUE;  /* stab entry deleted.  */
	      outrel.r_offset += (input_section->output_section->vma
	      			  + input_section->output_offset);
	      outrel.r_addend = rel->r_addend;

	      if (skip)
	        memset (&outrel, 0, sizeof (outrel));
	      else if (r_type == R_TRICORE_32REL)
	        {
		  BFD_ASSERT ((h != NULL) && (h->dynindx != -1));
		  outrel.r_info = ELF32_R_INFO (h->dynindx, R_TRICORE_32REL);
		}
	      else
	        {
		  /* h->dynindx may be -1 if this symbol was marked to
		     become local.  */
		  if ((h == NULL)
		      || ((info->symbolic || (h->dynindx == -1))))
		    {
		      relocate = TRUE;
		      outrel.r_info = ELF32_R_INFO (0, R_TRICORE_RELATIVE);
		    }
		  else
		    {
		      BFD_ASSERT (h->dynindx != -1);
		      outrel.r_info = ELF32_R_INFO
		      		       (h->dynindx, R_TRICORE_32ABS);
		    }
		}
	      bfd_elf32_swap_reloca_out (output_bfd, &outrel,
	      				 (bfd_byte *)(((Elf32_External_Rela *)
					   sreloc->contents)
					  + sreloc->reloc_count));
	      ++sreloc->reloc_count;
	      
	      /* If this reloc is against an external symbol, we do
	         not want to fiddle with the addend.  Otherwise, we
		 need to include the symbol value so that it becomes
		 an addend for the dynamic reloc.  */
	      if (!relocate)
	        continue;
	    }
	  bfd_put_32 (input_bfd, relocation, byte_ptr);
	  continue;
	}

      /* It's a relocation against an instruction.  */

      /* Handle PCP relocs.  */
      if ((r_type >= R_TRICORE_PCPHI) && (r_type <= R_TRICORE_PCPTEXT))
	{
          switch (r_type)
            {
	    case R_TRICORE_PCPHI:
	      len32 = 0;
	      insn = ((relocation >> 16) & 0xffff);
	      goto check_reloc;

	    case R_TRICORE_PCPLO:
	      len32 = 0;
	      insn = (relocation & 0xffff);
	      goto check_reloc;

	    case R_TRICORE_PCPPAGE:
	      len32 = 0;
	      /* Sanity check: the target address of this reloc must
	         belong to a PCP data section.  */
	      if (sec && (!(sec->flags & PCP_SEG) || !(sec->flags & SEC_DATA)))
	        {
		  errmsg = _("PRAM target address not within a PCP "
		  	     "data section");
		  goto check_reloc;
		}
	      /* Ideally, the target address should be aligned to a
	         256-byte-boundary, so that PCP code can access the
		 64 words starting at the target address using the 6-bit
		 unsigned offset in "*.PI" instructions.  However, I
		 think we should allow a user to organize its data as
		 he sees fit, so we're just enforcing the minimum
		 requirement (target address must be word-aligned),
		 but issue a warning if the target address isn't
		 properly aligned, as this can potentially lead to
		 incorrect PRAM accesses for non-zero offsets; the user
		 may turn off this warning by defining the global symbol
		 "NOPCPWARNING" with a non-zero value.  */
	      if (relocation & 0x3)
	        {
		  errmsg = _("PRAM target address is not word-aligned");
		  goto check_reloc;
		}
	      else if (relocation & 0xff)
	        {
	          struct bfd_link_hash_entry *w;

                  w = bfd_link_hash_lookup (info->hash, "NOPCPWARNING",
	  			            FALSE, FALSE, FALSE);
                  if ((w == (struct bfd_link_hash_entry *) NULL)
	              || ((w->type == bfd_link_hash_defined)
	                  && (w->u.def.value == 0)))
            	    {
		      const char *name, *msg;

		      if (h != NULL)
		        name = h->root.root.string;
		      else
		        {
		          name = (bfd_elf_string_from_elf_section
			          (input_bfd, symtab_hdr->sh_link,
				  sym->st_name));
		          if (name == NULL || *name == '\0')
			    name = bfd_section_name (input_bfd, sec);
		        }
		      msg = _("PRAM target address should be aligned "
		              "to a 256-byte boundary");
	              (void) ((*info->callbacks->warning)
		              (info, msg, name, input_bfd, input_section,
			      offset));
		    }
		}
              insn = bfd_get_16 (input_bfd, byte_ptr);
              insn |= (relocation & 0xff00);
	      goto check_reloc;

	    case R_TRICORE_PCPOFF:
	      len32 = 0;
	      /* Sanity check: the target address of this reloc must
	         belong to a PCP data section.  */
	      if (sec && (!(sec->flags & PCP_SEG) || !(sec->flags & SEC_DATA)))
	        {
		  errmsg = _("PRAM target address not within a PCP "
		  	     "data section");
		  goto check_reloc;
		}
	      if (relocation & 0x3)
	        {
		  errmsg = _("PRAM target address is not word-aligned");
		  goto check_reloc;
		}
              insn = bfd_get_16 (input_bfd, byte_ptr);
	      insn |= ((relocation >> 2) & 0x3f);	
	      goto check_reloc;

	    case R_TRICORE_PCPTEXT:
	      len32 = 0;
	      /* Sanity check: the target address of this reloc must
	         belong to a PCP text section.  */
	      if (sec && (!(sec->flags & PCP_SEG) || !(sec->flags & SEC_CODE)))
	        {
		  errmsg = _("PCODE target address not within a PCP "
		  	     "text section");
		  goto check_reloc;
		}
	      if (relocation & 0x1)
	        {
		  errmsg = _("PCODE target address is not even");
		  goto check_reloc;
		}
	      insn = ((relocation >> 1) & 0xffff);	
	      goto check_reloc;

	    default:
	      break;
	    }
	}

      /* Handle TriCore relocs.  */
      len32 = (*byte_ptr & 1);
      if (len32)
        insn = bfd_get_32 (input_bfd, byte_ptr);
      else
	insn = bfd_get_16 (input_bfd, byte_ptr);


      switch (r_type)
        {
	case R_TRICORE_24REL:
	  if (relocation & 1)
	    {
	      errmsg = _("24-bit PC-relative displacement is not even");
	      break;
	    }
	  else if (((int) (relocation) < (-16777216)) ||
		   ((int) (relocation) > (16777214)))
	    {
	      /* The target address is out of range; check if it
	         is reachable using absolute addressing mode.  */
	      bfd_vma target_address = relocation;

	      if (!rel_abs)
	        {
		  /* The target symbol isn't in the absolute section,
		     so we need to undo the relative offset.  */
	          target_address += input_section->output_section->vma
	      		          + input_section->output_offset;
                  if (howto->pcrel_offset)
                    target_address += offset;
		}

	      if (!(target_address & 0x0fe00000))
	        {
		  /* Okay, it's reachable using absolute addressing mode;
		     turn [f]call into [f]calla, or j[l] into j[l]a.  */
		  insn |= 0x80;
		  target_address >>= 1;
		  target_address |= ((target_address & 0x78000000) >> 7);
		  insn |= ((target_address & 0xffff) << 16);
		  insn |= ((target_address & 0xff0000) >> 8);
		}
	      else
	        {
		  /* We need additional instructions to reach the target.
		     If the target is a global symbol, then enabling the
		     linker's relaxing pass will do the trick.  Local
		     symbols, however, require the user to take action.  */
		  if (tricore_elf32_relax_24rel)
		    {
		      errmsg = _("24-bit PC-relative displacement overflow: "
		      		 "if the target of this call or jump insn "
				 "is a static function or procedure, just "
				 "turn it into a global one; otherwise try "
				 "to move excessive code from the affected "
				 "control statement's (e.g., if/else/switch) "
				 "body to separate new functions/procedures");
		    }
		  else
		    {
		      errmsg = _("24-bit PC-relative displacement overflow; "
		  	         "re-run linker with -relax or --relax-24rel");
		    }
		}

	      break;
	    }
	  else
	    {
	      /* Target address w/in +/- 16 MB.  */
	      relocation >>= 1;
	      insn |= ((relocation & 0xffff) << 16);
	      insn |= ((relocation & 0xff0000) >> 8);
	    }
	  break;

	case R_TRICORE_24ABS:
	  if (relocation & 0x0fe00001)
	    {
	      errmsg = _("Illegal 24-bit absolute address");
	      break;
	    }
          relocation >>= 1;
          relocation |= ((relocation & 0x78000000) >> 7);
          insn |= ((relocation & 0xffff) << 16);
          insn |= ((relocation & 0xff0000) >> 8);
          break;
	  
	case R_TRICORE_18ABS:
	  if (relocation & 0x0fffc000)
	    {
	      errmsg = _("Illegal 18-bit absolute address");
	      break;
	    }
	  insn |= ((relocation & 0x3f) << 16);
	  insn |= ((relocation & 0x3c0) << 22);
	  insn |= ((relocation & 0x3c00) << 12);
	  insn |= ((relocation & 0xf0000000) >> 16);
	  break;

        case R_TRICORE_18ABS_14:
          if (relocation & 0x00003fff)
            {
              errmsg = _("Illegal 18-bit absolute address (low 14-bit not zero)");
              break;
            }
          relocation = relocation >> 14;
          insn |= ((relocation &    0x3f) << 16);
          insn |= ((relocation &   0x3c0) << 22);
          insn |= ((relocation &  0x3c00) << 12);
          insn |= ((relocation & 0x3c000) >> 2);
          break;

	case R_TRICORE_HI:
	  insn |= (((relocation >> 16) & 0xffff) << 12);
	  break;

	case R_TRICORE_HIADJ:
	  insn |= ((((relocation + 0x8000) >> 16) & 0xffff) << 12);
	  break;

	case R_TRICORE_LO:
	  insn |= ((relocation & 0xffff) << 12);
	  break;

	case R_TRICORE_LO2:
	  insn |= ((relocation & 0x3f) << 16);
	  insn |= ((relocation & 0x3c0) << 22);
	  insn |= ((relocation & 0xfc00) << 12);
	  break;

	case R_TRICORE_16SM:
	case R_TRICORE_10SM:
	case R_TRICORE_16SM2:
	case R_TRICORE_SBREG_S2:
	case R_TRICORE_SBREG_S1:
	case R_TRICORE_SBREG_D:
          BFD_ASSERT (sec);
          sec_name = sec->name;
          if (!strstr(sec_name,".sdata") && !strstr(sec_name,".sbss") &&
              !strstr(sec_name,".srodata"))
	    {
	      errmsg = _("Small data relocation against an object not in a "
	      	         "named small data section (i.e., .s{data,bss}{,.*}); "
			 "see \"List-of-errors-and-warnings.pdf\" [chapter "
			 "\"Linker\"] for details");
	      break;
	    }
	  r = tricore_elf32_final_sda_base (info,sec->output_section,
	  				    &sda_base, &sda_reg,TRUE);
	  if (r != bfd_reloc_ok)
	    {
	      errmsg = _("Undefined or illegal small data output section");
	      break;
	    }
	  if (r_type == R_TRICORE_SBREG_S2) 
		{
			insn = (insn & 0xffff0fff) | (sda_reg << 12);
			break;
		} 
	  else if (r_type == R_TRICORE_SBREG_S1) 
		{
			insn = (insn & 0xfffff0ff) | (sda_reg << 8);
			break;
		} 
	  else if (r_type == R_TRICORE_SBREG_D) 
		{
			insn = (insn & 0x0fffffff) | (sda_reg << 28);
			break;
		} 
	  relocation -= sda_base;
	  if ((r_type == R_TRICORE_16SM) || (r_type == R_TRICORE_16SM2))
	    {
	      if (((int) relocation < -32768) || ((int) relocation > 32767))
	      {
	        errmsg = _("16-bit signed SDA-relative offset overflow");
	        break;
	      }
	      if (r_type == R_TRICORE_16SM)
	        insn |= ((relocation & 0xfc00) << 12);
	      else
	        {
		  insn |= ((relocation & 0xffff) << 12);
		  break;
		}
	    }
	  else
	    {
	      if (((int) relocation < -512) || ((int) relocation > 511))
	      {
	        errmsg = _("10-bit signed SDA-relative offset overflow");
	        break;
	      }
	    }
	  insn |= ((relocation & 0x3f) << 16);
	  insn |= ((relocation & 0x3c0) << 22);
	  /* Insert the address register number for the given SDA,
	     except for 16SM2 (which only requests the SDA offset as a
	     constant and won't access a variable directly).  */
	  if ((sda_reg != 0) && (r_type != R_TRICORE_16SM2))
	    insn = (insn & 0xffff0fff) | (sda_reg << 12);
	  break;

	case R_TRICORE_16CONST:
	  if (((int) relocation < -32768) || ((int) relocation > 32767))
	    {
	      errmsg = _("16-bit signed value overflow");
	      break;
	    }
	  insn |= ((relocation & 0xffff) << 12);
	  break;

	case R_TRICORE_15REL:
	  CHECK_DISPLACEMENT (relocation, -32768, 32766);
	  insn |= (((relocation >> 1) & 0x7fff) << 16);
	  break;

	case R_TRICORE_9SCONST:
	  if (((int) relocation < -256) || ((int) relocation > 255))
	    {
	      errmsg = _("9-bit signed value overflow");
	      break;
	    }
	  insn |= ((relocation & 0x1ff) << 12);
	  break;

	case R_TRICORE_9ZCONST:
	  if (relocation & ~511)
	    {
	      errmsg = _("9-bit unsigned value overflow");
	      break;
	    }
	  insn |= (relocation << 12);
	  break;

	case R_TRICORE_8REL:
	  CHECK_DISPLACEMENT (relocation, -256, 254);
	  relocation >>= 1;
	  insn |= ((relocation & 0xff) << 8);
	  break;

	case R_TRICORE_8CONST:
	  if (relocation & ~255)
	    {
	      errmsg = _("8-bit unsigned value overflow");
	      break;
	    }
	  insn |= (relocation << 8);
	  break;

	case R_TRICORE_10OFF:
	  if (((int) relocation < -512) || ((int) relocation > 511))
	    {
	      errmsg = _("10-bit signed offset overflow");
	      break;
	    }
	  insn |= ((relocation & 0x3f) << 16);
	  insn |= ((relocation & 0x3c0) << 12);
	  break;

	case R_TRICORE_16OFF:
	  if (((int) relocation < -32768) || ((int) relocation > 32767))
	    {
	      errmsg = _("16-bit signed offset overflow");
	      break;
	    }
	  insn |= ((relocation & 0x3f) << 16);
	  insn |= ((relocation & 0x3c0) << 22);
	  insn |= ((relocation & 0xfc00) << 12);
	  break;

	case R_TRICORE_1BIT:
	  if (relocation & ~1)
	    {
	      errmsg = _("Invalid bit value");
	      break;
	    }
	  insn |= (relocation << 11);
	  break;

	case R_TRICORE_3POS:
	  if (relocation & ~7)
	    {
	      errmsg = _("Invalid 3-bit bit position");
	      break;
	    }
	  insn |= (relocation << 8);
	  break;

	case R_TRICORE_5POS:
	  if (relocation & ~31)
	    {
	      errmsg = _("Invalid 5-bit bit position");
	      break;
	    }
	  insn |= (relocation << 16);
	  break;

	case R_TRICORE_5POS2:
	  if (relocation & ~31)
	    {
	      errmsg = _("Invalid 5-bit bit position");
	      break;
	    }
	  insn |= (relocation << 23);
	  break;

	case R_TRICORE_BRCC:
	  if (((int) relocation < -8) || ((int) relocation > 7))
	    {
	      errmsg = _("4-bit signed value overflow");
	      break;
	    }
	  insn |= ((relocation & 0xf) << 12);
	  break;

	case R_TRICORE_BRCZ:
	  if (relocation & ~15)
	    {
	      errmsg = _("4-bit unsigned value overflow");
	      break;
	    }
	  insn |= (relocation << 12);
	  break;

	case R_TRICORE_BRNN:
	  if (relocation & ~31)
	    {
	      errmsg = _("Invalid 5-bit bit position");
	      break;
	    }
	  insn |= ((relocation & 0xf) << 12);
	  insn |= ((relocation & 0x10) << 3);
	  break;

	case R_TRICORE_RRN:
	  if (relocation & ~3)
	    {
	      errmsg = _("2-bit unsigned value overflow");
	      break;
	    }
	  insn |= (relocation << 16);
	  break;

	case R_TRICORE_4CONST:
	  if (((int) relocation < -8) || ((int) relocation > 7))
	    {
	      errmsg = _("4-bit signed value overflow");
	      break;
	    }
	  insn |= ((relocation & 0xf) << 12);
	  break;

	case R_TRICORE_4REL:
	  CHECK_DISPLACEMENT (relocation, 0, 30);
	  relocation >>= 1;
	  insn |= ((relocation & 0xf) << 8);
	  break;

	case R_TRICORE_4REL2:
	  CHECK_DISPLACEMENT (relocation, -32, -2);
	  relocation >>= 1;
	  insn |= ((relocation & 0xf) << 8);
	  break;

	case R_TRICORE_5REL:
	  CHECK_DISPLACEMENT (relocation, 0, 62);
	  relocation >>= 1;
	  insn &= ~0x0080;
	  insn |= ((relocation & 0xf) << 8);
	  insn |= ((relocation & 0x10) << 3);
	  break;

	case R_TRICORE_5POS3:
	  if (relocation & ~31)
	    {
	      errmsg = _("Invalid 5-bit bit position");
	      break;
	    }
	  insn |= ((relocation & 0xf) << 12);
	  insn |= ((relocation & 0x10) << 3);
	  break;

	case R_TRICORE_4OFF:
	  if (relocation & ~15)
	    {
	      errmsg = _("4-bit unsigned offset overflow");
	      break;
	    }
	  insn |= (relocation << 12);
	  break;

	case R_TRICORE_4OFF2:
	  if (relocation & ~31)
	    {
	      errmsg = _("5-bit unsigned offset overflow");
	      break;
	    }
	  else if (relocation & 1)
	    {
	      errmsg = _("5-bit unsigned offset is not even");
	      break;
	    }
	  insn |= (relocation << 11);
	  break;

	case R_TRICORE_4OFF4:
	  if (relocation & ~63)
	    {
	      errmsg = _("6-bit unsigned offset overflow");
	      break;
	    }
	  else if (relocation & 3)
	    {
	      errmsg = _("6-bit unsigned offset is not a multiple of 4");
	      break;
	    }
	  insn |= (relocation << 10);
	  break;

	case R_TRICORE_42OFF:
	  if (relocation & ~15)
	    {
	      errmsg = _("4-bit unsigned offset overflow");
	      break;
	    }
	  insn |= (relocation << 8);
	  break;

	case R_TRICORE_42OFF2:
	  if (relocation & ~31)
	    {
	      errmsg = _("5-bit unsigned offset overflow");
	      break;
	    }
	  else if (relocation & 1)
	    {
	      errmsg = _("5-bit unsigned offset is not even");
	      break;
	    }
	  insn |= (relocation << 7);
	  break;

	case R_TRICORE_42OFF4:
	  if (relocation & ~63)
	    {
	      errmsg = _("6-bit unsigned offset overflow");
	      break;
	    }
	  else if (relocation & 3)
	    {
	      errmsg = _("6-bit unsigned offset is not a multiple of 4");
	      break;
	    }
	  insn |= (relocation << 6);
	  break;

	case R_TRICORE_2OFF:
	  if (relocation & 3)
	    {
	      errmsg = _("2-bit unsigned value overflow");
	      break;
	    }
	  insn |= (relocation << 6);
	  break;

	case R_TRICORE_8CONST2:
	  if (relocation & ~1023)
	    {
	      errmsg = _("10-bit unsigned offset overflow");
	      break;
	    }
	  else if (relocation & 3)
	    {
	      errmsg = _("10-bit unsigned offset is not a multiple of 4");
	      break;
	    }
	  insn |= (relocation << 6);
	  break;

	case R_TRICORE_4POS:
	  if (relocation & ~15)
	    {
	      errmsg = _("Invalid 4-bit bit position");
	      break;
	    }
	  insn |= ((relocation & 0xf) << 12);
	  break;

	default:
	  errmsg = _("Internal error: unimplemented relocation type");
	  break;
	}


check_reloc:
      if ((r == bfd_reloc_ok) && (errmsg == NULL))
        {
	  /* No error occurred; just write back the relocated insn.  */
	  if (len32)
  	    bfd_put_32 (input_bfd, insn, byte_ptr);
	  else
  	    bfd_put_16 (input_bfd, insn, byte_ptr);
	}
      else
	{
	  /* Some error occured.  Gripe.  */
	  const char *name;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  if (errmsg != NULL)
	    {
	      ret = FALSE;
	      (*_bfd_error_handler)
	       ("%s; symbol name = %s (defined in section %s of file %s), "
	        "addend = %ld, input file = %s, input section = %s, "
		"relocation offset = 0x%08lx (VMA = 0x%08lx), "
		"relocation value = 0x%08lx (%ld), output section = %s",
	        errmsg, name, sec->name,
	        sec->owner ? bfd_get_filename (sec->owner) : "<unknown>",
		addend,
	        bfd_get_filename (input_bfd),
	        bfd_get_section_name (input_bfd, input_section),
	        offset, offset + input_section->output_section->vma
	              	       + input_section->output_offset,
	        relocation, relocation,
	        input_section->output_section->name);
	    }
	  else
	    {
	      switch (r)
	        {
	        case bfd_reloc_overflow:
	          if (!((*info->callbacks->reloc_overflow)
		        (info, (h ? &h->root: NULL), name, howto->name, (bfd_vma) 0,
		        input_bfd, input_section, offset)))
		    return FALSE;
		  ret = FALSE;
	          break;

	        case bfd_reloc_undefined:
	          if (!((*info->callbacks->undefined_symbol)
		        (info, name, input_bfd, input_section, offset, TRUE)))
		    return FALSE;
	          break;

	        case bfd_reloc_outofrange:
	          errmsg = _("Out of range error");
	          goto common_error;

	        case bfd_reloc_notsupported:
	          errmsg = _("Unsupported relocation error");
	          goto common_error;

	        case bfd_reloc_dangerous:
	          errmsg = _("Dangerous error");
	          goto common_error;

	        default:
	          errmsg = _("Internal error: unknown error");
common_error:
		  ret = FALSE;
	          if (!((*info->callbacks->warning)
		        (info, errmsg, name, input_bfd, input_section, offset)))
		    return FALSE;
	          break;
	        }
	    }
	}
    }

  return ret;

#undef CHECK_DISPLACEMENT
}

/* Check whether it's okay to merge objects IBFD and OBFD.  */
static char *tric_arch_name(unsigned long arch)
{
	switch (arch) {
	case EF_EABI_TRICORE_V1_1:
		return ("TC_V1.1");
	case EF_EABI_TRICORE_V1_2:
		return ("TC_V1.2");
	case EF_EABI_TRICORE_V1_3:
		return ("TC_V1.3");
	case EF_EABI_TRICORE_V1_3_1:
		return ("TC_V1.3.1");
	case EF_EABI_TRICORE_V1_6:
		return ("TC_V1.6");
	case EF_EABI_TRICORE_V1_6_1:
		return ("TC_V1.6.1");
	case EF_EABI_TRICORE_V1_6_2:
		return ("TC_V1.6.2");
	default:
		return ("unknown architecture");
	}
}

/* convert old eflags to EABI conforming eflags */
unsigned long 
tricore_elf32_convert_eflags(unsigned long eflags)
{
  unsigned long new_flags = 0;
  int i;

  if (eflags & 0xffff0000)
    new_flags = eflags;
  else
    for (i = 0; i < 16; i++)
      {
        if (eflags & (1 << i))
          new_flags |= 1 << (31-i);
      }
  return new_flags;
}

static bfd_boolean
tricore_elf32_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  bfd_boolean error = FALSE;
  //static unsigned long linkmask = 0;
  unsigned long mask;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return TRUE;

  if (((bfd_get_arch (ibfd) != bfd_arch_tricore) && (bfd_get_arch (ibfd) != bfd_arch_mcs))
      ||
      (bfd_get_arch (obfd) != bfd_arch_tricore))
    {
      error = TRUE;
      (*_bfd_error_handler)
       (_("%s and/or %s don't use the TriCore architecture."),
        bfd_get_filename (ibfd), bfd_get_filename (obfd));
    }
  else
    {
      unsigned long old_isa, new_isa;

      mask = tricore_elf32_convert_eflags(elf_elfheader(ibfd)->e_flags);
      new_isa = mask & EF_EABI_TRICORE_CORE_MASK;
      old_isa = tricore_core_arch & EF_EABI_TRICORE_CORE_MASK;
      switch (tricore_core_arch & EF_EABI_TRICORE_CORE_MASK) {
      case EF_EABI_TRICORE_V1_1:
        if (new_isa != EF_EABI_TRICORE_V1_1)
          {
            error = TRUE;
          }
        break;
      case EF_EABI_TRICORE_V1_2:
      case EF_EABI_TRICORE_V1_3:
        switch (new_isa) {
        case EF_EABI_TRICORE_V1_1:
        case EF_EABI_TRICORE_V1_3_1:
        case EF_EABI_TRICORE_V1_6:
        case EF_EABI_TRICORE_V1_6_1:
        case EF_EABI_TRICORE_V1_6_2:
          error = TRUE;
          break;
        }
        break;
      case EF_EABI_TRICORE_V1_3_1:
        switch (new_isa) {
        case EF_EABI_TRICORE_V1_1:
        case EF_EABI_TRICORE_V1_6:
        case EF_EABI_TRICORE_V1_6_1:
        case EF_EABI_TRICORE_V1_6_2:
          error = TRUE;
          break;
        }
        break;
      case EF_EABI_TRICORE_V1_6:
        switch (new_isa) {
        case EF_EABI_TRICORE_V1_1:
        case EF_EABI_TRICORE_V1_6_1:
        case EF_EABI_TRICORE_V1_6_2:
          error = TRUE;
          break;
        }
        break;
      case EF_EABI_TRICORE_V1_6_1:
        switch (new_isa) {
        case EF_EABI_TRICORE_V1_1:
        case EF_EABI_TRICORE_V1_6_2:
          error = TRUE;
          break;
        }
        break;
      case EF_EABI_TRICORE_V1_6_2:
        switch (new_isa) {
        case EF_EABI_TRICORE_V1_1:
          error = TRUE;
          break;
        }
        break;
      }
      if (error == TRUE)
        {
          (*_bfd_error_handler) (_("%s uses an incompatible TriCore core architecture %s (should be %s)."),
                                 bfd_get_filename (ibfd),
                                 tric_arch_name(new_isa),
                                 tric_arch_name(old_isa));
        }
      else
        {
          elf_elfheader (obfd)->e_flags = tricore_core_arch;
        }
    }

  if (error)
    {
      bfd_set_error (bfd_error_wrong_format);
      return FALSE;
    }

  return TRUE;
}

/* Copy e_flags from IBFD to OBFD.  */

static bfd_boolean
tricore_elf32_copy_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  bfd_boolean error = FALSE;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return TRUE;

  if (bfd_get_arch (ibfd) != bfd_arch_tricore)
    {
      error = TRUE;
      (*_bfd_error_handler)
       (_("%s doesn't use the TriCore architecture."), bfd_get_filename (ibfd));
    }
  else
    elf_elfheader (obfd)->e_flags = elf_elfheader (ibfd)->e_flags;

  if (error)
    {
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  return TRUE;
}

/* Set the correct machine number (i.e., the ID for the instruction set
   architecture) for a TriCore ELF file.  */

static void
tricore_elf32_set_arch_mach (abfd, arch)
     bfd *abfd;
     enum bfd_architecture arch;
{
  bfd_arch_info_type *ap, *def_ap;
  unsigned long mach;

  if (arch != bfd_arch_tricore)
    return; /* Case already handled by bfd_default_set_arch_mach.  */

  mach = tricore_elf32_convert_eflags(elf_elfheader (abfd)->e_flags) & EF_EABI_TRICORE_CORE_MASK;

  /* Find the default arch_info.  */
  def_ap = (bfd_arch_info_type *) bfd_scan_arch (DEFAULT_ISA);

  /* Scan all sub-targets of the default architecture until we find
     the one that matches "mach".  If we find a target that is not
     the current default, we're making it the new default.  */
  for (ap = def_ap; ap != NULL; ap = (bfd_arch_info_type *) ap->next)
    if (ap->mach == mach)
      {
	abfd->arch_info = ap;
	return;
      }

  abfd->arch_info = &bfd_default_arch_struct;
  bfd_set_error (bfd_error_bad_value);
}

/* This hack is needed because it's not possible to redefine the
   function bfd_default_set_arch_mach.  Since we need to set the
   correct instruction set architecture, we're redefining
   bfd_elf32_object_p below (but calling it here to do the real work)
   and then we're calling tricore_elf32_set_arch_mach to set the
   correct ISA.  */

const bfd_target *
tricore_elf32_object_p (abfd)
     bfd *abfd;
{
  const bfd_target *bt;
  const struct elf_backend_data *ebd;
  extern const bfd_target *elf_object_p PARAMS ((bfd *));

  if ((bt = bfd_elf32_object_p (abfd)) != NULL)
    {
      ebd = abfd->xvec->backend_data;
      tricore_elf32_set_arch_mach (abfd, ebd->arch);
    }

  return bt;
}

/* Convert TriCore specific section flags to BFD's equivalent.  */

static bfd_boolean
tricore_elf32_section_flags (flags, hdr)
     flagword *flags;
     const Elf_Internal_Shdr *hdr;
{
  if (hdr->sh_flags & SHF_TRICORE_PCP)
    *flags |= SEC_PCP;

  return htc_elf32_section_flags(flags,hdr);
}  

/* Same as above, but vice-versa.  */

static bfd_boolean
tricore_elf32_fake_sections (abfd, hdr, sec)
     bfd *abfd ATTRIBUTE_UNUSED;
     Elf_Internal_Shdr *hdr;
     asection *sec;
{
  if (sec->flags & SEC_PCP)
    hdr->sh_flags |= SHF_TRICORE_PCP;
  
  return htc_elf32_fake_sections(abfd,hdr,sec);
}

/* This callback function is currently unused, but the possibilities we
   could apply to a BFD are pretty much unlimited...  ;-)  */

static bfd_boolean
tricore_elf32_set_private_flags (abfd, flags)
     bfd *abfd ATTRIBUTE_UNUSED;
     flagword flags ATTRIBUTE_UNUSED;
{
  return TRUE;
}

/* Given a relocation entry REL in input section IS (part of ABFD) that
   references a bit object, return the bit position of said object within
   the referenced byte, and reset *OK on failure.  INFO points to the
   link info structure, SYMTAB_HDR points to ABFD's symbol table header,
   ISYMBUF points to ABFD's internal symbols, and HASHES points to ABFD's
   symbol hashes.  */

static unsigned long
tricore_elf32_get_bitpos (abfd, info, rel, symtab_hdr, isymbuf, hashes, is, ok)
     bfd *abfd;
     struct bfd_link_info *info;
     Elf_Internal_Rela *rel;
     Elf_Internal_Shdr *symtab_hdr;
     Elf_Internal_Sym *isymbuf;
     struct elf_link_hash_entry **hashes;
     asection *is;
     bfd_boolean *ok;
{
  unsigned long symidx = ELF32_R_SYM (rel->r_info);
  asection *bsec;
  struct elf_link_hash_entry *h, *h2;
  size_t len;
  const char *sname = NULL;
  char *pname;

  if (symidx < symtab_hdr->sh_info)
    {
      Elf_Internal_Sym *sym, *sym2;
      const char *sname2;
      asection *boffs;
      unsigned long boffs_shndx;

      if (symidx == SHN_UNDEF)
        {
	  /* This is a classic case of "Can't happen!": it is normally
	     not possible to produce a relocation against a local symbol
	     that is undefined, because the assembler (well, at least
	     GNU as) marks undefined symbols automatically as global,
	     and other assemblers requiring an explicit global/external
	     declaration would certainly gripe if references to local
	     symbols cannot be resolved.  So how can it be that we still
	     need to deal with references to undefined local symbols?
	     Part of the answer is how the GNU linker handles "garbage
	     collection" (enabled with "--gc-sections"), a method to
	     automatically remove any unneded sections.  A section is
	     considered unneeded if the linker script doesn't provide
	     an explicit "KEEP" statement for it, and if there are no
	     symbols within that section that are referenced from any
	     other section.  If both conditions are met, the linker can
	     safely remove this section from the output BFD.  However,
	     when considering whether there are any references to a
	     given symbol, the linker ignores relocations coming from
	     debug sections -- which makes sense, because debug sections
	     don't really contribute to a program's code and data, but
	     are merely relevant to a debugger.  It thus can happen that
	     a debug section references a symbol which is defined in an
	     otherwise unneeded section, and the GNU linker handles this
	     by completely zeroing such relocation entries in case they
	     are referencing global symbols, or by setting a referenced
	     local symbol's symbol index to zero.  This means that for
	     relocations against such "deleted" global symbols the
	     relocation type is 0 (i.e., BFD_RELOC_NONE, which is also
	     mapped to R_TRICORE_NONE), so tricore_elf32_relocate_section
	     will simply ignore it, and for relocations against "deleted"
	     local symbols, the symbol's section index will point to the
	     "undefined section" (SHN_UNDEF) --  a valid input section
	     to _bfd_elf_rela_local_sym (tricore_elf32_relocate_section
	     calls this function whenever it needs to resolve references
	     to local symbols).  Now, if a reference to a deleted symbol
	     is either ignored or properly handled, where's the problem,
	     then?  Well, the problem is that for relocations against
	     "deleted" local symbols only their *symbol index* will be
	     changed, while their original *relocation type* information
	     will remain intact, so the relocation function won't simply
	     ignore such relocs.  But while the "usual" processing of
	     relocations against deleted symbols causes no grief at all
	     (see comment above regarding _bfd_elf_rela_local_sym), this
	     TriCore port of the GNU tools supports the allocation of
	     "single-bit" objects, and so tricore_elf32_relocate_section
	     must call this current function to determine a bit object's
	     position (or "bit offset") within its containing byte.  So,
	     without checking for deleted local bit objects right here
	     (by means of the introductory SHN_UNDEF comparison), the
	     code below could potentially cause illegal memory accesses,
	     because "bfd_section_from_elf_index (abfd, 0)" returns NULL,
	     unless "abfd" actually has an "undefined section" attached
	     to it -- which is a rather unlikely event.  For more details,
	     take a look at elf_link_input_bfd and the various "garbage
	     collect" (gc) functions, all defined in elflink.h.  */

	  return 0;
	}

      /* Local bit symbol; either we haven't relaxed bit variables,
         or a bit section contains only a single local bit variable.
	 In both cases the bit offset is zero, so we don't have to
	 compute it.  However, we're doing some sanity checking, albeit
	 not too fancy: since relocs against local labels are usually
	 adjusted (i.e., turned into relocs against section+offset),
	 the reloc references the section symbol, not the bit symbol,
	 so we skip some tests in this case (of course, we could search
	 for the actual bit symbol, but it isn't worth the effort, as
	 making sure that the reloc is against a bit section, and that
	 the ".boffs" section exists in the input BFD (and has a size
	 of zero), already provides a great deal of assurance).  */
      sym = isymbuf + symidx;
      bsec = bfd_section_from_elf_index (abfd, sym->st_shndx);
      if (sym->st_name != 0)
	sname = (bfd_elf_string_from_elf_section
	    	 (abfd, symtab_hdr->sh_link, sym->st_name));
      if (sname == NULL)
        sname = "<unknown local>";

      /* Make sure the symbol lives in a bit section.  */
      if (strcmp (bsec->name, ".bdata") && strncmp (bsec->name, ".bdata.", 7)
          && strcmp (bsec->name, ".bbss") && strncmp (bsec->name, ".bbss.", 6))
	{
error_no_bit_section:
	  (*_bfd_error_handler)
	   (_("%s(%s+%ld): bit variable \"%s\" defined in non-bit "
	      "section \"%s\""),
	    bfd_get_filename(abfd), is->name, rel->r_offset,
	    sname, bsec->name);
	  *ok = FALSE;
	  return 0;
	}

      /* Make sure the input BFD has a section ".boffs" with size 0.  */
      boffs = bfd_get_section_by_name (abfd, ".boffs");
      if (boffs == NULL)
        {
          (*_bfd_error_handler) (_("%s: missing section \".boffs\""),
      			         bfd_get_filename(abfd));
          *ok = FALSE;
          return 0;
        }
      if (boffs->rawsize != 0)
        {
          (*_bfd_error_handler) (_("%s: section \".boffs\" has non-zero size"),
      			         bfd_get_filename(abfd));
          *ok = FALSE;
          return 0;
        }

      /* Perform the following tests only if the reloc is against
         an actual bit symbol, not just the section symbol.  */
      if (sym->st_name != 0)
	{
	  /* Make sure the symbol represents a 1-byte object and is
	     associated with a ".pos" symbol in section ".boffs".  */
	  if ((symidx == (symtab_hdr->sh_info - 1))
	      || (ELF_ST_TYPE (sym->st_info) != STT_OBJECT)
	      || (sym->st_size != 1))
	    {
error_no_bit:
	      (*_bfd_error_handler)
	       (_("%s(%s+%ld): symbol \"%s\" doesn't represent a bit object"),
	        bfd_get_filename(abfd), is->name, rel->r_offset, sname);
	      *ok = FALSE;
	      return 0;
	    }

	  boffs_shndx = _bfd_elf_section_from_bfd_section (abfd, boffs);
	  len = strlen (sname);
	  sym2 = isymbuf + symidx + 1;
	  if ((sym2->st_name == 0)
	      || (sym2->st_shndx != boffs_shndx)
	      || (sym2->st_size != 0)
	      || (sym2->st_value != 0)
	      || (ELF_ST_TYPE (sym2->st_info) != STT_NOTYPE)
	      || ((sname2 = (bfd_elf_string_from_elf_section
			     (abfd, symtab_hdr->sh_link, sym2->st_name)))
		   == NULL)
	      || (strlen (sname2) != (len + 4))
	      || strncmp (sname2, sname, len)
	      || strcmp (sname2 + len, ".pos"))
	    goto error_no_bit;
	}

      return 0;
    }

  /* Global bit symbol; the bit offset is the value of the corresponding
     ".pos" symbol, which must be defined in some ".boffs" section.  */
  h = hashes[symidx - symtab_hdr->sh_info];
  while ((h->root.type == bfd_link_hash_indirect)
         || (h->root.type == bfd_link_hash_warning))
    h = (struct elf_link_hash_entry *) h->root.u.i.link;
  if ((h->root.type != bfd_link_hash_defined)
      && (h->root.type != bfd_link_hash_defweak))
    {
      /* Symbol is undefined; will be catched by the normal relocation
         process in tricore_elf32_relocate_section.  */
      *ok = FALSE;
      return 0;
    }
  bsec = h->root.u.def.section;
  sname = h->root.root.string;

  /* Make sure the symbol lives in a bit section.  */
  if (strcmp (bsec->name, ".bdata") && strncmp (bsec->name, ".bdata.", 7)
      && strcmp (bsec->name, ".bbss") && strncmp (bsec->name, ".bbss.", 6))
    goto error_no_bit_section;

  /* Make sure the symbol represents a 1-byte object and is
     associated with a ".pos" symbol in section ".boffs".  */
  if ((h->type != STT_OBJECT) || (h->size != 1))
    goto error_no_bit;

  len = strlen (sname) + 5;
  if ((pname = bfd_malloc (len)) == NULL)
    {
      *ok = FALSE;
      return 0;
    }
  sprintf (pname, "%s.pos", h->root.root.string);
  h2 = (struct elf_link_hash_entry *)
        bfd_link_hash_lookup (info->hash, pname, FALSE, FALSE, FALSE);
  free (pname);
  if ((h2 == NULL) || (h2->root.type != bfd_link_hash_defined)
      || (h2->type != STT_NOTYPE)
      || (h2->size != 0)
      || strcmp (h2->root.u.def.section->name, ".boffs"))
    goto error_no_bit;

  return h2->root.u.def.value;
}

/* Search for the index of a global symbol in it's defining object file.  */

static unsigned long
global_sym_index (bfd *abfd, struct elf_link_hash_entry *h)
{
  struct elf_link_hash_entry **p;

  BFD_ASSERT (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak);

  for (p = elf_sym_hashes (abfd); *p != h; ++p)
    continue;

  return p - elf_sym_hashes (abfd) + elf_tdata (abfd)->symtab_hdr.sh_info;
}


/* Access to internal relocations, section contents and symbols.  */

/* During relaxation, we need to modify relocations, section contents,
   and symbol definitions, and we need to keep the original values from
   being reloaded from the input files, i.e., we need to "pin" the
   modified values in memory.  We also want to continue to observe the
   setting of the "keep-memory" flag.  The following functions wrap the
   standard BFD functions to take care of this for us.  */

static Elf_Internal_Rela *
retrieve_internal_relocs (bfd *abfd, asection *sec, bfd_boolean keep_memory)
{
  Elf_Internal_Rela *internal_relocs;

  if ((sec->flags & SEC_LINKER_CREATED) != 0)
    return NULL;

  internal_relocs = elf_section_data (sec)->relocs;
  if (internal_relocs == NULL)
    internal_relocs = (_bfd_elf_link_read_relocs
		       (abfd, sec, NULL, NULL, keep_memory));
  return internal_relocs;
}


static void
pin_internal_relocs (asection *sec, Elf_Internal_Rela *internal_relocs)
{
  elf_section_data (sec)->relocs = internal_relocs;
}


static void
release_internal_relocs (asection *sec, Elf_Internal_Rela *internal_relocs)
{
  if (internal_relocs
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);
}


static bfd_byte *
retrieve_contents (bfd *abfd, asection *sec, bfd_boolean keep_memory)
{
  bfd_byte *contents;
  bfd_size_type sec_size;

  sec_size = bfd_get_section_limit (abfd, sec);
  contents = elf_section_data (sec)->this_hdr.contents;
  
  if (contents == NULL && sec_size != 0)
    {
      if (!bfd_malloc_and_get_section (abfd, sec, &contents))
	{
	  if (contents)
	    free (contents);
	  return NULL;
	}
      if (keep_memory) 
	elf_section_data (sec)->this_hdr.contents = contents;
    }
  return contents;
}


static void
pin_contents (asection *sec, bfd_byte *contents)
{
  elf_section_data (sec)->this_hdr.contents = contents;
}


static void
release_contents (asection *sec, bfd_byte *contents)
{
  if (contents && elf_section_data (sec)->this_hdr.contents != contents)
    free (contents);
}


static Elf_Internal_Sym *
retrieve_local_syms (bfd *input_bfd)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Sym *isymbuf;
  size_t locsymcount;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  locsymcount = symtab_hdr->sh_info;

  isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
  if (isymbuf == NULL && locsymcount != 0)
    isymbuf = bfd_elf_get_elf_syms (input_bfd, symtab_hdr, locsymcount, 0,
				    NULL, NULL, NULL);

  /* Save the symbols for this input file so they won't be read again.  */
  if (isymbuf && isymbuf != (Elf_Internal_Sym *) symtab_hdr->contents)
    symtab_hdr->contents = (unsigned char *) isymbuf;

  return isymbuf;
}


/* This is called once by tricore_elf32_relocate_section, and only if
   the linker was directed to create a map file.  We walk through all
   input BFDs (in link order) and print out all allocated bit objects
   (note that garbage collection ("--gc-sections") and bit relaxation
   ("-relax"|"--relax-bdata") have already been performed formerly in
   case they were requested by the user).  */

void
tricore_elf32_list_bit_objects (info, out)
     struct bfd_link_info *info;
     FILE *out;
{
  bfd *ibfd;
  asection *bdata, *boffs;
  bfd_boolean header_printed = FALSE;
  Elf_Internal_Shdr *symtab_hdr, *strtab_hdr;
  Elf_Internal_Sym *isymbuf;
  unsigned char *strtab;
  unsigned int symcount;
  unsigned int boffs_shndx;
  struct elf_link_hash_entry **beg_hashes, **end_hashes, **sym_hashes;

  for (ibfd = info->input_bfds; ibfd; ibfd = ibfd->link_next)
    {
      /* If this input BFD doesn't have a ".boffs" section, it also
         won't have any bit sections, so we can safely skip it.  */
      if (((boffs = bfd_get_section_by_name (ibfd, ".boffs")) == NULL)
          || (bfd_get_section_flags (ibfd, boffs) & SEC_EXCLUDE)
	  || (boffs->rawsize != 0))
	continue;

      boffs_shndx = _bfd_elf_section_from_bfd_section (ibfd, boffs);
      isymbuf = NULL;
      strtab = NULL;
      symtab_hdr = &elf_tdata (ibfd)->symtab_hdr;
      if (symtab_hdr->sh_info != 0)
        {
          isymbuf = retrieve_local_syms(ibfd);
	  if (isymbuf == NULL)
	    continue;  /* This error will be catched elsewhere.  */

	  strtab_hdr = &elf_tdata (ibfd)->strtab_hdr;
	  strtab = strtab_hdr->contents;
	  if (strtab == NULL)
	    {
              (void)bfd_elf_string_from_elf_section(ibfd, symtab_hdr->sh_link,1);
	    //  bfd_elf_get_str_section (ibfd, symtab_hdr->sh_link);
	      strtab = strtab_hdr->contents;
	    }
	  BFD_ASSERT (strtab);
	}

      /* Iterate over the sections of this BFD; skip non-bit sections.  */
      for (bdata = ibfd->sections; bdata; bdata = bdata->next)
	{
          if ((bfd_get_section_flags (ibfd, bdata) & SEC_EXCLUDE)
	      || (bdata->rawsize == 0)
	      || (strcmp (bdata->name, ".bdata")
	          && strncmp (bdata->name, ".bdata.", 7)
		  && strcmp (bdata->name, ".bbss")
		  && strncmp (bdata->name, ".bbss.", 6)))
	    continue;

	  if (!header_printed)
	    {
	      fprintf (out, "\n%s\n",
	  	       _("Allocating bit symbols (l/g = local or global "
		         "scope)"));
	      fprintf (out, "%s\n\n",
	  	       _("Bit symbol          bit address  l/g  file"));
	      header_printed = TRUE;
	    }

	  /* Walk through all local symbols in this section.  */
          if (isymbuf != NULL)
	    {
	      Elf_Internal_Sym *isym, *isymend = isymbuf + symtab_hdr->sh_info;
              const char *this_file = NULL, *this_base = NULL;
	      unsigned int bdata_shndx;

	      bdata_shndx = _bfd_elf_section_from_bfd_section (ibfd, bdata);
	      for (isym = isymbuf; isym < isymend; ++isym)
	        {
	          if (ELF_ST_TYPE (isym->st_info) == STT_FILE)
	            {
	              this_file = (const char *)(strtab + isym->st_name);
		      this_base = strchr (this_file, '.');
		      continue;
		    }

	          if ((isym->st_shndx == bdata_shndx)
	              && (ELF_ST_TYPE (isym->st_info) == STT_OBJECT))
		    {
		      Elf_Internal_Sym *isym2;
		      char *aname;
		      size_t len;

		      aname = (char *)strtab + isym->st_name;
		      len = strlen (aname);
		      if (isym->st_size != 1)
		        {
		          (*_bfd_error_handler)
		           (_("%s: bit symbol \"%s\" has size != 1 (%ld)"),
		            bfd_get_filename(ibfd), aname, isym->st_size);
		          bfd_set_error (bfd_error_bad_value);
		          continue;
		        }
		      for (isym2 = isym + 1; isym2 < isymend; ++isym2)
		        {
			  const char *aname2;

		          /* Bail out if this symbol indicates the beginning
		             of a new file, as this means that the ".pos"
			     symbol is missing in the current file (shouldn't
			     really happen, but just in case).  */
	                  if (ELF_ST_TYPE (isym2->st_info) == STT_FILE)
		            {
			      isym2 = isymend;
			      break;
			    }

			  aname2 = (char *)(strtab + isym2->st_name);
		          if ((isym2->st_shndx == boffs_shndx)
			      && (ELF_ST_TYPE (isym2->st_info) == STT_NOTYPE)
			      && (strlen (aname2) == (len + 4))
			      && !strncmp (aname2, aname, len)
			      && !strcmp (aname2 + len, ".pos"))
			    {
			      fprintf (out, "%-19s 0x%08lx.%ld  l   %s",
			  	       aname,
				       (bdata->output_section->vma
				        + bdata->output_offset
				        + isym->st_value),
				       isym2->st_value,
				       bfd_get_filename(ibfd));

			      /* If this is an input object that is the result
			         of a relocateable link run, also print the
				 name of the original file that defined this
				 bit object.  */
			      if (this_file
			          && this_base
			          && strncmp (this_file, ibfd->filename,
			      		      this_base - this_file))
			        fprintf (out, " <%s>", this_file);
			      fprintf (out, "\n");
			      break;
			    }
		        }
		      if (isym2 == isymend)
		        {
#ifndef FATAL_BITVAR_ERRORS
		          (*_bfd_error_handler)
		           (_("%s: warning: missing or invalid local bit "
			      "position "
		              "symbol \"%s.pos\" in section \".boffs\""),
			    bfd_get_filename(ibfd), aname);
#else
		          (*_bfd_error_handler)
		           (_("%s: missing or invalid local bit position "
		              "symbol \"%s.pos\" in section \".boffs\""),
			    bfd_get_filename(ibfd), aname);
		          bfd_set_error (bfd_error_bad_value);
#endif
		        }
		    }
	        }
	    }

	  /* Walk through all global symbols in this section.  */
          symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
      		      - symtab_hdr->sh_info);
          beg_hashes = elf_sym_hashes (ibfd);
          end_hashes = elf_sym_hashes (ibfd) + symcount;
          for (sym_hashes = beg_hashes; sym_hashes < end_hashes; ++sym_hashes)
	    {
	      struct elf_link_hash_entry *sym = *sym_hashes;

	      if ((sym->root.u.def.section == bdata)
	          && (sym->type == STT_OBJECT)
	          && ((sym->root.type == bfd_link_hash_defined)
	              || (sym->root.type == bfd_link_hash_defweak)))
	        {
	          struct elf_link_hash_entry *sym2;
	          const char *aname = sym->root.root.string;
	          char *pname;
	          size_t len;

	          if (sym->size != 1)
	            {
		      (*_bfd_error_handler)
		       (_("%s: bit symbol \"%s\" has size != 1 (%ld)"),
		        bfd_get_filename(ibfd), aname, sym->size);
		      bfd_set_error (bfd_error_bad_value);
	              continue;
	            }

	          /* Lookup the global symbol "aname.pos", which should be
	             defined in section ".boffs" and have type STT_NOTYPE.  */
	          len = strlen (aname) + 5;
	          if ((pname = bfd_malloc (len)) == NULL)
	            return;

		  sprintf (pname, "%s.pos", aname);
	          sym2 = (struct elf_link_hash_entry *)
	      	          bfd_link_hash_lookup (info->hash, pname,
	      				        FALSE, FALSE, FALSE);
	          if ((sym2 == NULL)
		      || (sym2->root.u.def.section != boffs)
	              || (sym2->type != STT_NOTYPE))
		    {
		      (*_bfd_error_handler)
		       (_("%s: missing or invalid bit position symbol \"%s\""),
		        bfd_get_filename(ibfd), pname);
		      bfd_set_error (bfd_error_bad_value);
		      free (pname);
		      continue;
		    }

	          free (pname);
	          fprintf (out, "%-19s 0x%08lx.%ld  g   %s\n",
		           aname,
		           (bdata->output_section->vma
			    + bdata->output_offset
			    + sym->root.u.def.value),
		           sym2->root.u.def.value,
		           bfd_get_filename(ibfd));
	        }
	    }
        }
    }
}

/* Adjust all relocs contained in input object ABFD that reference the
   given bit object: SECIDX is the index of a bit section, OLD is the
   former offset of the bit address within this section, NEW is the
   new offset, BPOS is the new bit position (0..7), BIDX is the symbol
   index of the symbol representing that bit offset (i.e., "name.pos" in
   section ".boffs"), and INFO is a pointer to the link info structure.  */

static bfd_boolean
tricore_elf32_adjust_bit_relocs (abfd, info, secidx, old, new, bpos, bidx)
     bfd *abfd;
     struct bfd_link_info *info;
     unsigned long secidx;
     bfd_vma old, new;
     int bpos;
     unsigned int bidx;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs;
  Elf_Internal_Rela *irel, *irelend;
  Elf_Internal_Rela *lastrel = NULL;
  Elf_Internal_Sym *isymbuf, *sym;
  unsigned long symidx;
  asection *sec;
  int r_type;
  bfd_boolean bitoff_seen = FALSE;
  static bfd_boolean initialized = FALSE;
  static char **reloc_adjusted;

  /* If nothing changes, return immediately.  */
  if ((old == new) && (bpos == 0))
    return TRUE;

  /* We must remember which relocations we've already changed, as we have
     to avoid modifying the same relocation multiple times.  Why?  Well,
     consider the following case: first we change a relocation from, say,
     .bdata+42 to .bdata+3 (note that it doesn't matter whether we're also
     (or merely) changing the bit position); then another, yet unmodified
     relocation references .bdata+3, which should now, after relaxation,
     be changed into a relocation against .bdata+7.  Without remembering
     which relocs we've already changed, we would again modify the already
     modified relocation against .bdata+3 (which was originally against
     .bdata+42) -- ouch!  Unfortunately, this ain't too obvious...  */
  if (!initialized)
    {
      int i, max = -1;
      bfd *ibfd;
      asection *section;

      /* Find the highest section ID of all input sections.  */
      for (ibfd = info->input_bfds; ibfd; ibfd = ibfd->link_next)
        for (section = ibfd->sections; section; section = section->next)
	  if (section->id > max)
	    max = section->id;

      ++max;
      if ((reloc_adjusted = bfd_malloc (max * sizeof (char *))) == NULL)
        return FALSE;

      for (i = 0; i < max; ++i)
	reloc_adjusted[i] = NULL;

      initialized = TRUE;
    }

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  isymbuf = retrieve_local_syms(abfd);

  for (sec = abfd->sections; sec != NULL; sec = sec->next)
    {
      if (((sec->flags & SEC_RELOC) == 0)
          || (sec->reloc_count == 0))
	continue;

      internal_relocs = retrieve_internal_relocs(abfd, sec,
                                                 info->keep_memory);
      if (internal_relocs == NULL)
        return FALSE;

      if (!reloc_adjusted[sec->id])
        {
	  if ((reloc_adjusted[sec->id] = bfd_malloc (sec->reloc_count)) == NULL)
	    return FALSE;

	  memset (reloc_adjusted[sec->id], 0, sec->reloc_count);
	}

      irelend = internal_relocs + sec->reloc_count;
      for (irel = internal_relocs; irel < irelend; irel++)
        {
	  if (irel->r_addend != old)
	    {
	      bitoff_seen = FALSE;
	      continue;
	    }

	  r_type = ELF32_R_TYPE (irel->r_info);
	  if ((r_type == R_TRICORE_NONE)
	      || reloc_adjusted[sec->id][irel - internal_relocs])
	    {
	      /* Don't touch an already modified reloc; see comment above.  */
	      bitoff_seen = FALSE;
	      continue;
	    }
          if ((r_type < 0)
              || (r_type < R_TRICORE_NONE)
	      || (r_type >= R_TRICORE_max))
	    {
	      (*_bfd_error_handler) (_("%s: unknown relocation type %d"),
				     sec->name, r_type);
	      bfd_set_error (bfd_error_bad_value);
	      return FALSE;
	    }
	  if (r_type == R_TRICORE_BITPOS)
	    {
	      bitoff_seen = TRUE;
	      lastrel = irel;
	      continue;
	    }

	  symidx = ELF32_R_SYM (irel->r_info);
	  if (symidx >= symtab_hdr->sh_info)
	    {
	      /* Relocation against a global symbol.  Can be safely
	         ignored, because even if bitoff_seen is TRUE and
		 this symbol turns out to not represent a bit symbol,
		 this will be catched either by the relaxation pass
		 or later by tricore_elf32_relocate_section when it
		 calls tricore_elf32_get_bitpos.  */
	      bitoff_seen = FALSE;
	      continue;
	    }

	  sym = isymbuf + symidx;
	  if ((sym->st_shndx != secidx)
	      || (ELF_ST_TYPE (sym->st_info) != STT_SECTION))
	    {
	      /* This symbol doesn't belong to the given bit section,
	         or the relocation is not against the section symbol.  */
	      bitoff_seen = FALSE;
	      continue;
	    }

	  if (tricore_elf32_debug_relax)
	    {
	      asection *bsec;

	      bsec = bfd_section_from_elf_index (abfd, sym->st_shndx);
	      printf ("    %s+%ld: symbol at %s+%ld, changing ",
	              sec->name, irel->r_offset, bsec->name, irel->r_addend);
	    }

	  /* Remember that this reloc is going to be changed; see
	     comment above for details about why this is necessary.  */
	  reloc_adjusted[sec->id][irel - internal_relocs] = 1;
	  if (bitoff_seen)
	    {
	      if (tricore_elf32_debug_relax)
	        printf ("bpos from 0 to %d\n", bpos);

	      irel->r_info = ((irel->r_info & 0xff) | bidx << 8);
	      irel->r_addend = 0;
	      lastrel->r_info = R_TRICORE_NONE;
	      bitoff_seen = FALSE;
	    }
	  else
	    {
	      if (tricore_elf32_debug_relax)
	        printf ("addr from %ld to %ld\n", old, new);

	      irel->r_addend = new;
	    }
	}
    }

  return TRUE;
}



/* Relax a TriCore ELF section.  We use this function to take care of
   PC-relative call and jump instructions that cannot reach their target
   addresses (i.e., the target address is more than +/-16 MB away from
   the referencing call/jump instruction, and it is also not within the
   first 16 KB of a 256 MB TriCore memory segment).  In that case, we
   build a "trampoline" at the end of the text section that loads the
   target address into address register %a2 and then jumps indirectly
   through this register; the original call/jump instruction is modified
   to branch to the trampoline.  To save memory, we're keeping track of
   all target addresses for which we've already built a trampoline; all
   call/jump instructions referencing the same target address will also
   use the same trampoline.
   
   Another thing we're doing here is to compress sections containing
   bit objects.  Such bit objects initially occupy one byte each and are
   accessed via their absolute address and bit position zero; after we've
   relaxed them, up to eight bits share a single byte, and their bit
   position can be 0..7, so we need to adjust all relocations and symbols
   (in section ".boffs") dealing with bit objects.  */

static bfd_boolean
tricore_elf32_relax_section (
     bfd *abfd,
     asection *sec,
     struct bfd_link_info *info,
     bfd_boolean *again)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs = NULL;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  Elf_Internal_Sym *isymbuf = NULL;
  static bfd_boolean initialized = FALSE;
  static bfd_boolean *rela_malloced = NULL;
  static struct _rela
  {
    bfd_vma offset;
    bfd_vma addend;
    unsigned long symoff;
    int size;
  } **rela_p = NULL;
  static int *num_rela = NULL, *max_rela = NULL;
  int now_rela = 0, idx = sec->id;
  bfd_size_type sec_size;


  if (tricore_elf32_debug_relax)
    printf ("Relaxing %s(%s) [secid = %d], raw = %ld, cooked = %ld\n",
  	    bfd_get_filename (abfd), sec->name, sec->id,
	    sec->rawsize, sec->size);

  /* Assume nothing changes.  */
  *again = FALSE;

  sec_size = bfd_get_section_limit(abfd,sec);
  contents = retrieve_contents (abfd, sec, info->keep_memory);
  if (contents == NULL && sec_size != 0)
    {
      return FALSE;
    }

  /* If this is the first time we've been called for this section,
     initialize its sizes.  */
  if (sec->size == 0)
    sec->size = sec_size;
  if (sec->rawsize == 0)
    sec->rawsize = sec_size;


  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  /* If requested, compress bit sections (.b{data,bss}{,.*}).  */
  if (tricore_elf32_relax_bdata
      && !info->relocatable
      && (sec_size > 0)                         /* the section contains
                                                   something */
      && (sec->rawsize == sec->size)            /* is not relaxed */
      && (!strcmp (sec->name, ".bdata")
          || !strncmp (sec->name, ".bdata.", 7)
	  || !strcmp (sec->name, ".bbss")
	  || !strncmp (sec->name, ".bbss.", 6)))
    {
      bfd_vma baddr = 0;
      int bpos = -1;
      asection *boffs;
      Elf_Internal_Shdr *strtab_hdr = NULL;
      unsigned char *strtab = NULL;
      unsigned int sec_shndx, boffs_shndx;
      unsigned int symcount;
      bfd_byte *orig_contents = NULL;
      struct elf_link_hash_entry **beg_hashes;
      struct elf_link_hash_entry **end_hashes;
      struct elf_link_hash_entry **sym_hashes;
      bfd_boolean is_bdata = (sec->name[2] == 'd');

      boffs = bfd_get_section_by_name (abfd, ".boffs");
      if (boffs == NULL)
        {
	  (*_bfd_error_handler) (_("%s: missing section \".boffs\""),
	  			 bfd_get_filename (abfd));
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}
      if (boffs->rawsize != 0)
        {
	  (*_bfd_error_handler) (_("%s: section \".boffs\" has non-zero size"),
	  			 bfd_get_filename (abfd));
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}
      sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);
      boffs_shndx = _bfd_elf_section_from_bfd_section (abfd, boffs);

      if (is_bdata)
	{
          /* Make a copy of the original data to avoid overwriting
             them while packing the bit objects.  */
          orig_contents = bfd_malloc (sec->rawsize);
          if (orig_contents == NULL)
            goto error_return;

          memcpy (orig_contents, contents, sec->rawsize);
	}

      /* Read this BFD's local symbols.  */
      if (symtab_hdr->sh_info != 0)
        {
	  isymbuf = retrieve_local_syms(abfd);
	  if (isymbuf == NULL)
	    goto error_return;

	  /* Cache local syms, as we're going to change their values.  */
          symtab_hdr->contents = (unsigned char *) isymbuf;

	  strtab_hdr = &elf_tdata (abfd)->strtab_hdr;
	  strtab = strtab_hdr->contents;
	  if (strtab == NULL)
	    {
              (void)bfd_elf_string_from_elf_section(abfd, symtab_hdr->sh_link,1);
	      // bfd_elf_get_str_section (abfd, symtab_hdr->sh_link);
	      strtab = strtab_hdr->contents;
	    }
	  BFD_ASSERT (strtab);
	}

      /* Walk through all local symbols in this section.  */
      if (isymbuf != NULL)
        {
	  Elf_Internal_Sym *isym, *isymend = isymbuf + symtab_hdr->sh_info;

	  for (isym = isymbuf; isym < isymend; isym++)
	    {
	      if ((isym->st_shndx == sec_shndx)
	          && (ELF_ST_TYPE (isym->st_info) == STT_OBJECT))
		{
		  Elf_Internal_Sym *isym2;
		  char *aname, *pname;
		  size_t len;

		  aname = (char *)(strtab + isym->st_name);
		  len = strlen (aname);
		  if (isym->st_size != 1)
		    {
		      /* This symbol no bit.  Gripe.  */
	              (*_bfd_error_handler)
		       (_("%s: bit symbol \"%s\" has size != 1 (%ld)"),
	  		bfd_get_filename (abfd), aname, isym->st_size);
		      bfd_set_error (bfd_error_bad_value);
		      goto error_return;
		    }

		  if (tricore_elf32_debug_relax)
		    {
		      printf ("  * %s (local), old addr = %ld",
		  	      aname, isym->st_value);
		      fflush (stdout);
		    }

		  /* Usually (in case of tricore-as, at least), we expect the
		     next local symbol after a local bit symbol to be a
		     local bit position symbol attached to section ".boffs",
		     but nevertheless we're allowing said bit position
		     symbol to be defined anywhere between the local bit
		     symbol and the end of this object file's local syms.  */
		  for (isym2 = isym + 1; isym2 < isymend; isym2++)
		    {
	      	      if (ELF_ST_TYPE (isym2->st_info) == STT_FILE)
		        {
			  /* No bit position symbol defined; bail out.  */
			  isym2 = isymend;
			  break;
			}

		      if ((isym2->st_shndx == boffs_shndx)
		          && (ELF_ST_TYPE (isym2->st_info) == STT_NOTYPE)
			  && (strlen ((char *)(strtab + isym2->st_name)) == (len + 4))
			  && !strncmp (aname, (char *)(strtab + isym2->st_name), len)
			  && !strcmp ((char *)(strtab + isym2->st_name + len), ".pos"))
			{
			  unsigned char byte = '\0';

			  pname = (char *)(strtab + isym2->st_name);
			  if (isym2->st_value != 0)
			    {
			      if (tricore_elf32_debug_relax)
			        printf ("\n");

	                      (*_bfd_error_handler)
		               (_("%s: bit position symbol \"%s\" has "
			          "non-zero value %ld"),
	  		        bfd_get_filename (abfd), pname,
				isym2->st_value);
		              bfd_set_error (bfd_error_bad_value);
			      goto error_return;
			    }

			  /* Determine next free bit address.  */
			  if (++bpos == 8)
			    {
			      bpos = 0;
			      ++baddr;
			    }

			  if (is_bdata)
			    {
			      /* Copy the bit object's original value
			         to its new bit address.  */
			      byte =
			        (orig_contents[isym->st_value] & 1) << bpos;
			      contents[baddr] &= ~(1 << bpos);
			      contents[baddr] |= byte;
			    }

			  if (tricore_elf32_debug_relax)
			    {
			      printf (".0, new addr = %ld.%d", baddr, bpos);
			      if (is_bdata)
			        printf (", val = 0x%02x", byte);
			      printf ("\n");
			    }

			  /* Adjust relocations against the old bit object.  */
                          if (!tricore_elf32_adjust_bit_relocs
			       (abfd, info, sec_shndx, isym->st_value, baddr,
			        bpos, isym2 - isymbuf))
			    return FALSE;

			  /* Finally, set the new values.  */
			  isym->st_value = baddr;
			  isym2->st_value = bpos;
			  break;
			}
		    }
		  if (isym2 == isymend)
		    {
		      if (tricore_elf32_debug_relax)
		        printf ("\n");

	              (*_bfd_error_handler)
		       (_("%s: missing or invalid local bit position "
		          "symbol \"%s.pos\" in section \".boffs\""),
	  		bfd_get_filename (abfd), aname);
		      bfd_set_error (bfd_error_bad_value);
		      goto error_return;
		    }
		}
	    }
	}

      /* Walk through all global symbols in this section.  */
      symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
      		  - symtab_hdr->sh_info);
      beg_hashes = elf_sym_hashes (abfd);
      end_hashes = elf_sym_hashes (abfd) + symcount;
      for (sym_hashes = beg_hashes; sym_hashes < end_hashes; sym_hashes++)
        {
	  struct elf_link_hash_entry *sym = *sym_hashes;

	  if ((sym->root.u.def.section == sec)
	      && (sym->type == STT_OBJECT)
	      && ((sym->root.type == bfd_link_hash_defined)
	          || (sym->root.type == bfd_link_hash_defweak)))
	    {
	      struct elf_link_hash_entry *sym2;
	      const char *aname = sym->root.root.string;
	      char *pname;
	      size_t len;

	      if (sym->size != 1)
		{
		  /* This symbol no bit.  Gripe.  */
	          (*_bfd_error_handler)
		   (_("%s: bit symbol \"%s\" has size != 1 (%ld)"),
	  	    bfd_get_filename (abfd), aname, sym->size);
		  bfd_set_error (bfd_error_bad_value);
		  goto error_return;
		}

	      if (tricore_elf32_debug_relax)
	        {
		  printf ("  * %s (global), old addr = %ld",
	                  aname, sym->root.u.def.value);
	          fflush (stdout);
		}

	      len = strlen (aname) + 5;
	      if ((pname = bfd_malloc (len)) == NULL)
	        goto error_return;

	      sprintf (pname, "%s.pos", aname);
	      sym2 = (struct elf_link_hash_entry *)
	      	      bfd_link_hash_lookup (info->hash, pname,
		      			    FALSE, FALSE, FALSE);
	      if ((sym2 == NULL)
		  || (sym2->type != STT_NOTYPE)
		  || (sym2->root.u.def.value != 0)
	          || strcmp (sym2->root.u.def.section->name, ".boffs"))
		{
	          if (tricore_elf32_debug_relax)
		    printf ("\n");

		  (*_bfd_error_handler)
		   (_("%s: missing or invalid global bit position "
		      "symbol \"%s\""),
		    bfd_get_filename (abfd), pname);
		  bfd_set_error (bfd_error_bad_value);
		  free (pname);
		  goto error_return;
		}
	      else
		{
		  unsigned char byte = '\0';

		  free (pname);

		  /* Determine next free bit address.  */
		  if (++bpos == 8)
		    {
		      bpos = 0;
		      ++baddr;
		    }
		  if (is_bdata)
		    {
		      /* Copy the bit object's original value
		         to its new bit address.  */
		      byte = (orig_contents[sym->root.u.def.value] & 1) << bpos;
		      contents[baddr] &= ~(1 << bpos);
		      contents[baddr] |= byte;
		    }

	          if (tricore_elf32_debug_relax)
		    {
		      printf (".0, new addr = %ld.%d", baddr, bpos);
		      if (is_bdata)
		        printf (", val = 0x%02x", byte);
		      printf ("\n");
		    }

                  /* get the sym index into the elf sym_hashes */
                  
		  /* This catches cases in which the assembler (or other
		     binutil) has transformed relocations against "local
		     globals" (i.e., global symbols defined in the same
		     module as the instructions referencing them) into
		     relocations against section+offset, as it is usually
		     only done with relocs against local symbols.  */
                  if (!(tricore_elf32_adjust_bit_relocs
			(abfd, info, sec_shndx,
			 sym->root.u.def.value, baddr, bpos,
			 global_sym_index(abfd,sym2))))
		    return FALSE;

		  /* Finally, set the new values.  */
		  sym->root.u.def.value = baddr;
		  sym2->root.u.def.value = bpos;
		}
	    }
	}

      if (sec->rawsize == 0)
        sec->rawsize = sec->size;
      sec->size = baddr + 1;
      if (is_bdata && (orig_contents != NULL))
        free (orig_contents);
      if (sec->rawsize != sec->size)
        *again = TRUE;
      goto ok_return;
    }

  /* If requested, check whether all call and jump instructions can reach
     their respective target addresses.  We don't have to do anything for a
     relocateable link, or if this section doesn't contain code or relocs.  */
  if (!tricore_elf32_relax_24rel
      || info->relocatable
      || ((sec->flags & SEC_CODE) == 0)
      || ((sec->flags & SEC_RELOC) == 0)
      || (sec->reloc_count == 0))
    return TRUE;

  if (!initialized)
    {
      int i, max = -1;
      bfd *ibfd;
      asection *section;

      /* Find the highest section ID of all input sections.  */
      for (ibfd = info->input_bfds; ibfd; ibfd = ibfd->link_next)
        for (section = ibfd->sections; section; section = section->next)
	  if (section->id > max)
	    max = section->id;

      ++max;
      if (((rela_malloced = bfd_malloc (max * sizeof (bfd_boolean))) == NULL)
          || ((rela_p = bfd_malloc (max * sizeof (struct _rela *))) == NULL)
          || ((num_rela = bfd_malloc (max * sizeof (int))) == NULL)
          || ((max_rela = bfd_malloc (max * sizeof (int))) == NULL))
        return FALSE;

      for (i = 0; i < max; ++i)
        {
	  rela_malloced[i] = FALSE;
	  rela_p[i] = NULL;
	  num_rela[i] = -1;
	  max_rela[i] = 12;
	}

      initialized = TRUE;
    }

  /* Get a copy of the native relocations.  */
  internal_relocs = retrieve_internal_relocs(abfd,sec,info->keep_memory);

  if (internal_relocs == NULL)
    goto error_return;

  /* Walk through the relocs looking for call/jump targets.  */
  irelend = internal_relocs + sec->reloc_count;
  for (irel = internal_relocs; irel < irelend; irel++)
    {
      struct elf_link_hash_entry *h;
      const char *sym_name;
      bfd_vma pc, sym_val, use_offset;
      long diff, r_type;
      unsigned long r_index, insn;
      int add_bytes, doff = 0;

      r_type = ELF32_R_TYPE (irel->r_info);
      if ((r_type < 0)
       	  || (r_type < R_TRICORE_NONE)
  	  || (r_type >= R_TRICORE_max))
	goto error_return;

      if (r_type != R_TRICORE_24REL)
	continue;

      r_index = ELF32_R_SYM (irel->r_info);
      if (r_index < symtab_hdr->sh_info)
        {
          asection *bsec;
	  Elf_Internal_Sym *sym;

	  /* This is a call/jump instruction to a local label or function.
	     The GNU/TriCore assembler will usually already have resolved
	     such local branches, so we won't see any relocs for them here,
	     but maybe this input object has been generated by another
	     vendor's assembler.  Anyway, we must assume that targets of
	     local branches can always be reached directly, so there's no
	     need for us to bother.
             Since section symbols are also marked as local symbols
             we have to check here explicitly for section symbols because 
             we can have reloction to section relative addresses, e.g
             call (.mysec + 0x10)
             */

	  if (isymbuf == NULL)
            {
              isymbuf = retrieve_local_syms(abfd);
	      if (isymbuf == NULL)
                continue;
            }
          sym = isymbuf + r_index;

	  if (ELF_ST_TYPE (sym->st_info) != STT_SECTION)
            continue;

          /* Symbol is a section name:
             Get the value of the symbol referred to by the reloc.  */
          bsec = bfd_section_from_elf_index (abfd, sym->st_shndx);
          sym_name = bsec->name;
          sym_val = sym->st_value 
           + bsec->output_section->vma
           + bsec->output_offset
           + irel->r_addend;
	}
      else
        {
          /* Get the value of the symbol referred to by the reloc.  */
          h = elf_sym_hashes (abfd)[r_index - symtab_hdr->sh_info];
          BFD_ASSERT (h != NULL);
          while ((h->root.type == bfd_link_hash_indirect)
                 || (h->root.type == bfd_link_hash_warning))
            h = (struct elf_link_hash_entry *) h->root.u.i.link;
          if ((h->root.type != bfd_link_hash_defined)
              && (h->root.type != bfd_link_hash_defweak))
            {
              /* This appears to be a reference to an undefined symbol.  Just
                 ignore it; it'll be caught by the regular reloc processing.  */
              continue;
            }
          sym_name = h->root.root.string;
          sym_val = h->root.u.def.value
           + h->root.u.def.section->output_section->vma
           + h->root.u.def.section->output_offset
           + irel->r_addend;
        }
      pc = irel->r_offset + sec->output_section->vma + sec->output_offset;

      /* If the target address can be reached directly or absolutely, or if
         the PC or target address are odd, we don't need to do anything, as
	 all of this will be handled by tricore_elf32_relocate_section.  */
      diff = sym_val - pc;
      if (((diff >= -16777216) && (diff <= 16777214))
          || !(sym_val & 0x0fe00000)
	  || (sym_val & 1) || (pc & 1))
	continue;

      /* Get the instruction.  */
      insn = bfd_get_32 (abfd, contents + irel->r_offset);
      if (((insn & 0xff) == 0x6d) || ((insn & 0xff) == 0x61))
        {
	  struct bfd_link_hash_entry *h;

          h = bfd_link_hash_lookup (info->hash, "CPUBUG14",
	  			    FALSE, FALSE, FALSE);
          if ((h != (struct bfd_link_hash_entry *) NULL)
	      && (h->type == bfd_link_hash_defined)
	      && (h->u.def.value != 0))
            add_bytes = 20;
	  else
	    add_bytes = 12;
	}
      else if (((insn & 0xff) == 0x1d) || ((insn & 0xff) == 0x5d))
        add_bytes = 12;
      else
        {
	  (*_bfd_error_handler) (_("%s: Unknown call/jump insn at offset %ld"),
	  			 bfd_get_filename (abfd), irel->r_offset);
	  goto error_return;
	}

      /* See if we already created a stub for this symbol.  */
      use_offset = sec->size;
      if (rela_p[idx] != NULL)
        {
	  int i;

	  for (i = 0; i <= num_rela[idx]; ++i)
	    if ((rela_p[idx][i].symoff == r_index)
	        && (rela_p[idx][i].addend == irel->r_addend)
		&& (rela_p[idx][i].size == add_bytes))
	      {
		/* Use the existing stub.  */
		use_offset = rela_p[idx][i].offset;
      		diff = use_offset - irel->r_offset;
		if (add_bytes == 20)
      		  {
		    diff -= 8;
		    use_offset -= 8;
		  }
	        if (diff > 16777214)
		  goto error_return;

		diff >>= 1;
                insn |= ((diff & 0xffff) << 16);
                insn |= ((diff & 0xff0000) >> 8);
                bfd_put_32 (abfd, insn, contents + irel->r_offset);
      		irel->r_info = R_TRICORE_NONE;
		add_bytes = 0;
		break;
	      }
	}

      if (tricore_elf32_debug_relax)
	{
	  printf ("  # 0x%08lx[+%ld]: \"%s %s",
	  	  pc, irel->r_offset,
		  ((insn & 0xff) == 0x6d) ? "call"
		   : ((insn & 0xff) == 0x61) ? "fcall"
		      : ((insn & 0xff) == 0x1d) ? "j" : "jl",
      	          sym_name);
	  if (irel->r_addend != 0)
	    printf ("+%ld", irel->r_addend);
	  printf ("\" @0x%08lx[+%ld,%d]->0x%08lx\n", 
		  use_offset + sec->output_section->vma + sec->output_offset,
		  use_offset, add_bytes, sym_val);
	}

      if (add_bytes == 0)
        continue;
      else if ((sec->size - irel->r_offset) > 16777214)
        goto error_return;

      /* Make room for the additional instructions.  */
      contents = bfd_realloc (contents, sec->size + add_bytes);
      if (contents == NULL)
        goto error_return;

      /* Read this BFD's local symbols if we haven't done so already.  */
      if ((isymbuf == NULL) && (symtab_hdr->sh_info != 0))
        {
	  isymbuf = retrieve_local_syms(abfd);
	  if (isymbuf == NULL)
	    goto error_return;
	}

      /* For simplicity of coding, we are going to modify the section
         contents, the section relocs, and the BFD symbol table.  We
	 must tell the rest of the code not to free up this information.  */
      pin_internal_relocs(sec, internal_relocs);
      pin_contents(sec , contents);

      /* Modify the call or jump insn to branch to the beginning of
         the additional code.  */
      diff = (sec->size - irel->r_offset) >> 1;
      insn |= ((diff & 0xffff) << 16);
      insn |= ((diff & 0xff0000) >> 8);
      bfd_put_32 (abfd, insn, contents + irel->r_offset);

      /* Nullify the insn's reloc.  */
      irel->r_info = R_TRICORE_NONE;

      /* Emit the additonal code.  */
      if (add_bytes == 20)
        {
	  /* This is called by a [f]call instruction, and we need a
	     "dsync; nop; nop" sequence as a workaround for a hw bug.  */
	  bfd_put_32 (abfd, 0x0480000d, contents + sec->size);
	  bfd_put_32 (abfd, 0x00000000, contents + sec->size + 4);
	  doff = 8;
	}
      /* This is "movh.a %a2,hi:tgt; lea %a2,[%a2]lo:tgt; nop; ji %a2".  */
      bfd_put_32 (abfd, 0x20000091, contents + sec->size + doff);
      bfd_put_32 (abfd, 0x000022d9, contents + sec->size + doff + 4);
      bfd_put_32 (abfd, 0x02dc0000, contents + sec->size + doff + 8);

      /* Remember the new relocs.  */
      if (num_rela[idx] == -1)
        {
	  if ((rela_p[idx] = (struct _rela *)
	        bfd_malloc (max_rela[idx] * sizeof (struct _rela))) == NULL)
	    goto error_return;

	  num_rela[idx] = 0;
	}
      else if (++num_rela[idx] == max_rela[idx])
        {
	  max_rela[idx] *= 2;
	  if ((rela_p[idx] = (struct _rela *)
	  	bfd_realloc (rela_p[idx],
			     max_rela[idx] * sizeof (struct _rela))) == NULL)
	    goto error_return;
	}
      ++now_rela;
      rela_p[idx][num_rela[idx]].offset = sec->size + doff;
      rela_p[idx][num_rela[idx]].addend = irel->r_addend;
      rela_p[idx][num_rela[idx]].symoff = r_index;
      rela_p[idx][num_rela[idx]].size = add_bytes;

      /* Set the new section size.  */
      sec->size += add_bytes;

      /* We've changed something.  */
      *again = TRUE;
    }

  /* Add new relocs, if required.  */
  if (now_rela > 0)
    {
      int i;

      if (info->keep_memory && !rela_malloced[idx])
        {
	  if ((irel = bfd_malloc ((sec->reloc_count + 2 * now_rela)
	  			    * sizeof (Elf_Internal_Rela))) == NULL)
	    goto error_return;

	  /* In this case, the memory for the relocs has been
	     bfd_alloc()ed, so we can't use bfd_realloc() to make
	     room for the additional relocs, and we can also not
	     free(internal_relocs).  */

	  memcpy (irel, internal_relocs,
	  	  sec->reloc_count * sizeof (Elf_Internal_Rela));
	  internal_relocs = irel;
	  rela_malloced[idx] = TRUE;
	}
      else if ((internal_relocs =
                  bfd_realloc (internal_relocs,
	    		       (sec->reloc_count + 2 * now_rela)
			         * sizeof (Elf_Internal_Rela))) == NULL)
	goto error_return;

      pin_internal_relocs(sec, internal_relocs);
      irel = internal_relocs + sec->reloc_count;
      for (i = num_rela[idx] - now_rela + 1; i <= num_rela[idx]; ++i, ++irel)
        {
	  irel->r_addend = rela_p[idx][i].addend;
	  irel->r_offset = rela_p[idx][i].offset;
	  irel->r_info = ELF32_R_INFO (rela_p[idx][i].symoff, R_TRICORE_HIADJ);
	  ++irel;
	  irel->r_addend = rela_p[idx][i].addend;
	  irel->r_offset = rela_p[idx][i].offset + 4;
	  irel->r_info = ELF32_R_INFO (rela_p[idx][i].symoff, R_TRICORE_LO2);
	}

      /* Set the new relocation count.  */
      sec->reloc_count += 2 * now_rela;
    }

  /* Free or cache the memory we've allocated for relocs, local symbols,
     etc.  */
ok_return:

  release_internal_relocs(sec,internal_relocs);
  release_contents(sec,contents);

  return TRUE;

error_return:

  release_internal_relocs(sec,internal_relocs);
  release_contents(sec,contents);

  return FALSE;
}

static const struct bfd_elf_special_section tricore_elf32_special_sections[] =
{
  { STRING_COMMA_LEN (".rodata"),    -2, SHT_PROGBITS, SHF_ALLOC  },
  { STRING_COMMA_LEN (".zrodata"),   -2, SHT_PROGBITS, SHF_ALLOC + SHF_ABSOLUTE_DATA },
  { STRING_COMMA_LEN (".zdata"),     -2, SHT_PROGBITS, SHF_ALLOC  + SHF_WRITE + SHF_ABSOLUTE_DATA},
  { STRING_COMMA_LEN (".zbss"),      -2, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE + SHF_ABSOLUTE_DATA},
  { STRING_COMMA_LEN (".clear_sec"), -2, SHT_PROGBITS, SHF_ALLOC  },
  { STRING_COMMA_LEN (".copy_sec"),  -2, SHT_PROGBITS, SHF_ALLOC  },
  { STRING_COMMA_LEN (".stack"),     -2, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE },
  { STRING_COMMA_LEN (".ustack"),    -2, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE },
  { STRING_COMMA_LEN (".istack"),    -2, SHT_NOBITS, SHF_ALLOC + SHF_WRITE },
  { STRING_COMMA_LEN (".heap"),      -2, SHT_NOBITS, SHF_ALLOC + SHF_WRITE  },
  { STRING_COMMA_LEN (".csa"),        0, SHT_NOBITS, SHF_ALLOC + SHF_WRITE  },
  { STRING_COMMA_LEN (".sdata.rodata"),0, SHT_PROGBITS, SHF_ALLOC + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sdata"),      0, SHT_PROGBITS, SHF_ALLOC + SHF_WRITE + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sbss"),       0, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sdata2"),     0, SHT_PROGBITS, SHF_ALLOC  + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sbss2"),      0, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sdata3"),     0, SHT_PROGBITS, SHF_ALLOC + SHF_WRITE },
  { STRING_COMMA_LEN (".sbss3"),      0, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".sdata4"),     0, SHT_PROGBITS, SHF_ALLOC + SHF_WRITE },
  { STRING_COMMA_LEN (".sbss4"),      0, SHT_NOBITS, SHF_ALLOC  + SHF_WRITE + SHF_SMALL_DATA},
  { STRING_COMMA_LEN (".boffs"),      0, SHT_NOBITS, 0 },
  { NULL,                    0,       0,         0   , 0 }
};

static const struct bfd_elf_special_section *
tricore_elf32_get_sec_type_attr (bfd *abfd ATTRIBUTE_UNUSED, asection *sec)
{
  const struct bfd_elf_special_section *ssect;

  /* See if this is one of the special sections.  */
  if (sec->name == NULL)
    return NULL;

  ssect = _bfd_elf_get_special_section (sec->name, tricore_elf32_special_sections,
					sec->use_rela_p);
  if (ssect != NULL)
    {
      return ssect;
    }

  return _bfd_elf_get_sec_type_attr (abfd, sec);
}


/* create different hash tables for imported and exported symbols */
static struct bfd_link_hash_table *
tricore_elf32_link_hash_table_create(bfd *abfd)
{

    if (!htc_elf32_link_hash_table_create(abfd))
      return NULL;
    if (!tricore_elf32_call_symbol_hash_create(abfd))
      return NULL;
    return _bfd_elf_link_hash_table_create(abfd);
}

/* handle __caller.<func>/__callee.<func> symbols
   we create an own hash for all __caller. symbols and one
   for all __callee. symbols
   At the end of the linking phase, we check if for each 
   __caller. symbols a corrensponding __callee. symbol exists.
   They may only differ in the prefix __caller./__callee.
   */
/* define a hash table to hold the caller/callee symbols */
struct tricore_elf_callee_hash_table
{
  struct elf_link_hash_table elf;
  const char *func_name;
  const char *func_model;
  const char *func_return;
  const char *func_param;
};
#define tricore_elf_hash_entry(ent) ((struct tricore_elf_callee_hash_table *) (ent))

/* some known positions in the symbol strings 
   0123456789
   __callee.name.type.return.parameter
   __caller.name.type.return.parameter
   */
#define FUNC_NAME 9
#define CALLER    7


static struct tricore_elf_callee_hash_table caller_hash;
static struct tricore_elf_callee_hash_table callee_hash;

/* create a new hash entry for the caller/callee symbol */
static struct bfd_hash_entry *
tricore_elf32_callee_hash_new (struct bfd_hash_entry *entry,
                               struct bfd_hash_table *table,
                               const char *name)
{
  if (entry == NULL)
    {
      entry = bfd_hash_allocate (table, sizeof(struct tricore_elf_callee_hash_table));
      if (entry == NULL)
        return entry;
    }
  entry = _bfd_elf_link_hash_newfunc (entry, table, name);
  if (entry == NULL)
    return entry;

  tricore_elf_hash_entry(entry)->func_model = NULL;
  tricore_elf_hash_entry(entry)->func_return = NULL;
  tricore_elf_hash_entry(entry)->func_param = NULL;
  return entry;
}
static bfd_boolean
tricore_elf32_call_symbol_hash_create (bfd *abfd)
{
    if (!_bfd_elf_link_hash_table_init (&caller_hash.elf, abfd, tricore_elf32_callee_hash_new,
                sizeof(struct tricore_elf_callee_hash_table)))
        return FALSE;
    if (!_bfd_elf_link_hash_table_init (&callee_hash.elf, abfd, tricore_elf32_callee_hash_new,
                sizeof(struct tricore_elf_callee_hash_table)))
        return FALSE;
    return TRUE;
}

/* called from tricore_elf32_add_symbol_hook to add name to the hash table */
static void
tricore_elf32_add_call_symbol (const char *name)
{
  struct elf_link_hash_entry *h;
  struct elf_link_hash_table *hash;
  char *func_name;
  char *dot = NULL;
  char *dot2;

  if (strncmp(name,"__caller.",9) == 0)
    {
      /* hash the full name for caller */
      func_name = (char *)name;
      hash = &caller_hash.elf;
    }
  else if (strncmp(name,"__callee.",9) == 0)
    {
      /* hash the stripped name for callee */
      dot = strchr (&name[FUNC_NAME], '.');
      func_name = xstrndup (name, dot - name);
      hash = &callee_hash.elf;
    }
  else
    return;

  h  = elf_link_hash_lookup (hash, func_name,TRUE,FALSE,FALSE);
  if (h == NULL)
    return ;
  h->root.type = bfd_link_hash_defined;
  h->root.u.def.section = bfd_abs_section_ptr;
  h->root.u.def.value = 0;
  h->type = STT_FUNC;
  h->size = 0;

  /* split the symbol name into 
     __callee/r.<name>
     <type>
     <return_type>
     <parameter_types> */
  if (dot == NULL)
    {
      dot = strchr (&name[FUNC_NAME],'.');
      func_name = xstrndup (name, dot - name);
    }
  tricore_elf_hash_entry(h)->func_name = func_name;
  dot2 = strchr (dot+1, '.');
  tricore_elf_hash_entry(h)->func_model = xstrndup (dot+1, dot2 - dot - 1);
  dot = dot2;
  dot2 = strchr (dot+1, '.');
  tricore_elf_hash_entry(h)->func_return = xstrndup (dot+1, dot2 - dot - 1);
  dot = dot2;
  tricore_elf_hash_entry(h)->func_param = xstrdup (dot+1);

}

/* set to FALSE if a callee does not match the caller */
static bfd_boolean tricore_callee_caller;

/* callback too traverse through all caller symbols
   and lookup the corresponding callee symbol */
static bfd_boolean
tricore_elf32_check_caller_symbol (struct bfd_link_hash_entry *entry , PTR parm ATTRIBUTE_UNUSED)
{
  struct elf_link_hash_entry *hr = (struct elf_link_hash_entry *) entry;
  struct elf_link_hash_entry *he;
  struct tricore_elf_callee_hash_table *callee_entry;
  struct tricore_elf_callee_hash_table *caller_entry;
  char *callee;
  char *dot;
  bfd_boolean stack_model;

  caller_entry = (struct tricore_elf_callee_hash_table *)entry;
  callee = xstrdup(hr->root.root.string);
  callee[CALLER] = 'e';
  dot = strchr(&callee[FUNC_NAME],'.');
  *dot = 0;

  he  = elf_link_hash_lookup(&callee_hash.elf,callee ,FALSE,FALSE,FALSE);
  if (he == NULL)
    {
      /* check for callee with return type 'void' */
      (*_bfd_error_handler) (_("warning: callee symbol %s for %s not found"),
                             callee, hr->root.root.string);
      free(callee);
      return TRUE;
    }
  callee_entry = (struct tricore_elf_callee_hash_table *)he;

  stack_model = (caller_entry->func_model[0] == 'S' && callee_entry->func_model[0] == 'S');

  if (strcmp(callee_entry->func_model,caller_entry->func_model))
    {
      (*_bfd_error_handler) (_("error: callee %s.%s.%s.%s and caller %s use a different model"),
                             callee,
                             callee_entry->func_model,
                             callee_entry->func_return,
                             callee_entry->func_param,
                             hr->root.root.string);
      tricore_callee_caller = FALSE;
    }
  if (strcmp(callee_entry->func_return,caller_entry->func_return)
      && (caller_entry->func_return[0] != 'v'))
    {
      (*_bfd_error_handler) (_("%s: callee %s.%s.%s.%s and caller %s have different return types"),
                             (stack_model)? "warning" : "error",
                             callee,
                             callee_entry->func_model,
                             callee_entry->func_return,
                             callee_entry->func_param,
                             hr->root.root.string);
      if (!stack_model)
        tricore_callee_caller = FALSE;
    }
  if (strcmp(callee_entry->func_param,caller_entry->func_param))
    {
      (*_bfd_error_handler) (_("%s: callee %s.%s.%s.%s and caller %s have different parameter types"),
                             (stack_model)? "warning" : "error",
                             callee,
                             callee_entry->func_model,
                             callee_entry->func_return,
                             callee_entry->func_param,
                             hr->root.root.string);
      if (!stack_model)
        tricore_callee_caller = FALSE;
    }

  free(callee);
  return TRUE;
}

/* check for all callee/caller pairs, traverse through all caller symbols */
static bfd_boolean
tricore_elf32_check_caller_callee (bfd *abfd, struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
  tricore_callee_caller = TRUE;

  bfd_link_hash_traverse ((struct bfd_link_hash_table *)&caller_hash.elf, tricore_elf32_check_caller_symbol, (PTR) abfd);

  return tricore_callee_caller;
}
static bfd_boolean
tricore_elf32_final_link (bfd *abfd, struct bfd_link_info *info)
{
  /* Invoke the regular ELF backend linker to do all the work.  */
  if (!bfd_elf_final_link (abfd, info))
    return FALSE;
  /* do the callee/caller checking */
  return tricore_elf32_check_caller_callee(abfd,info);
}





#undef  elf_backend_special_sections
#define elf_backend_special_sections 		tricore_elf32_special_sections
/* Now #define all necessary stuff to describe this target.  */
#define bfd_elf32_bfd_link_hash_table_create tricore_elf32_link_hash_table_create


// #define USE_RELA			1
#define ELF_ARCH			bfd_arch_tricore
#define ELF_MACHINE_CODE		EM_TRICORE
#define ELF_MAXPAGESIZE			0x4000
#define TARGET_LITTLE_SYM		bfd_elf32_tricore_vec
#define TARGET_LITTLE_NAME		"elf32-tricore"
#define bfd_elf32_bfd_relax_section	tricore_elf32_relax_section
#define bfd_elf32_bfd_reloc_type_lookup	tricore_elf32_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup	tricore_elf_reloc_name_lookup
#define bfd_elf32_object_p		tricore_elf32_object_p
#define bfd_elf32_bfd_merge_private_bfd_data \
					tricore_elf32_merge_private_bfd_data
#define bfd_elf32_bfd_copy_private_bfd_data \
					tricore_elf32_copy_private_bfd_data
#define bfd_elf32_bfd_set_private_flags tricore_elf32_set_private_flags
#define elf_info_to_howto		tricore_elf32_info_to_howto
#define elf_info_to_howto_rel		0
#define elf_backend_get_sec_type_attr	tricore_elf32_get_sec_type_attr
#define elf_backend_section_flags	tricore_elf32_section_flags
#define elf_backend_relocate_section	tricore_elf32_relocate_section
#define elf_backend_fake_sections	tricore_elf32_fake_sections

#define elf_backend_reloc_type_class    tricore_elf32_reloc_type_class
#define elf_backend_add_symbol_hook     tricore_elf32_add_symbol_hook
#define elf_backend_merge_symbol_attribute htc_elf32_merge_symbol_attribute
#define elf_backend_match_sections_by_type htc_elf32_match_sections_by_type

#define elf_backend_can_gc_sections	1
#define elf_backend_can_refcount	1
#define elf_backend_rela_normal	        1


#define bfd_elf32_bfd_final_link		tricore_elf32_final_link


#include "elf32-target.h"

