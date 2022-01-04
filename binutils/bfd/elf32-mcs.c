/* RB GTM MCS support for 32-bit ELF
   Copyright 2011  Free Software Foundation, Inc.
   Contributed by Horst Lehser  <Horst.Lehser@hightec-rt.com>

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/mcs.h"

#if !defined(HTC_TRICORE) && !defined(HTC_PPCVLE)
#include "elf32-htc.c"
#endif

static bfd_reloc_status_type mcs_elf_wxlb_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static reloc_howto_type mcs_elf_howto_table [] =
{
  /* This reloc does nothing.  */
  HOWTO (R_MCS_NONE,               /* type */
         0,                         /* rightshift */
         2,                         /* size (0 = byte, 1 = short, 2 = long) */
         32,                        /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_NONE",             /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0,                         /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* An 4 bit absolute relocation.  */
  HOWTO (R_MCS_4,                  /* type */
         0,                         /* rightshift */
         0,                         /* size (0 = byte, 1 = short, 2 = long) */
         4,                         /* bitsize */
         FALSE,                     /* pc_relative */
         20,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_4",                /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xf00000,                  /* dst_mask */
         FALSE),                    /* pcrel_offset */


  /* An 5 bit absolute relocation.  */
  HOWTO (R_MCS_5,                  /* type */
         0,                         /* rightshift */
         0,                         /* size (0 = byte, 1 = short, 2 = long) */
         5,                         /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_5",                /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0x0000001f,                /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* An 8 bit absolute relocation.  */
  HOWTO (R_MCS_8,                  /* type */
         0,                         /* rightshift */
         0,                         /* size (0 = byte, 1 = short, 2 = long) */
         8,                         /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_8",                /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xff,                      /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* An 9 bit absolute relocation.  */
  HOWTO (R_MCS_9,                  /* type */
         0,                         /* rightshift */
         1,                         /* size (0 = byte, 1 = short, 2 = long) */
         9,                         /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_9",                /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0x000001ff,                /* dst_mask */
         FALSE),                    /* pcrel_offset */


  /* A 14 bit absolute relocation.  */
  HOWTO (R_MCS_14,                 /* type */
         0,                         /* rightshift */
         1,                         /* size (0 = byte, 1 = short, 2 = long) */
         14,                        /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_14",               /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0x3ffc,                    /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_MCS_16,                 /* type */
         0,                         /* rightshift */
         1,                         /* size (0 = byte, 1 = short, 2 = long) */
         16,                        /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_16",               /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xffff,                    /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* A 24 bit absolute relocation.  */
  HOWTO (R_MCS_24,                 /* type */
         0,                         /* rightshift */
         2,                         /* size (0 = byte, 1 = short, 2 = long) */
         24,                        /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_24",               /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xffffff,                /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_MCS_32,                 /* type */
         0,                         /* rightshift */
         2,                         /* size (0 = byte, 1 = short, 2 = long) */
         32,                        /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         bfd_elf_generic_reloc,     /* special_function */
         "R_MCS_32",               /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xffffffff,                /* dst_mask */
         FALSE),                    /* pcrel_offset */

  /* An 5 bit absolute relocation of operand b.  */
  HOWTO (R_MCS_WXLB,                  /* type */
         0,                         /* rightshift */
         2,                         /* size (0 = byte, 1 = short, 2 = long) */
         32,                         /* bitsize */
         FALSE,                     /* pc_relative */
         0,                         /* bitpos */
         complain_overflow_bitfield,/* complain_on_overflow */
         mcs_elf_wxlb_reloc,     /* special_function */
         "R_MCS_WXLB",               /* name */
         FALSE,                     /* partial_inplace */
         0,                         /* src_mask */
         0xf01000,                  /* dst_mask */
         FALSE),                    /* pcrel_offset */
};

/* Map BFD reloc types to GTM MCS ELF reloc types. */

struct mcs_reloc_map
{
    bfd_reloc_code_real_type bfd_reloc_val;
    unsigned char elf_reloc_val;
};

static const struct mcs_reloc_map mcs_reloc_map[] =
{
  { BFD_RELOC_NONE,         R_MCS_NONE },
  { BFD_RELOC_MCS_WXLB,     R_MCS_WXLB },
  { BFD_RELOC_MCS_4,        R_MCS_4 },
  { BFD_RELOC_8,            R_MCS_8 },
  { BFD_RELOC_MCS_14,       R_MCS_14 },
  { BFD_RELOC_16,           R_MCS_16 },
  { BFD_RELOC_24,           R_MCS_24 },
  { BFD_RELOC_MCS_24,       R_MCS_24 },
  { BFD_RELOC_32,           R_MCS_32 },
};

static reloc_howto_type *
mcs_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                        bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0; i < sizeof (mcs_reloc_map) / sizeof (mcs_reloc_map[0]); i++)
    if (mcs_reloc_map[i].bfd_reloc_val == code)
      return &mcs_elf_howto_table[mcs_reloc_map[i].elf_reloc_val];
  return NULL;
}

static reloc_howto_type *
mcs_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (mcs_elf_howto_table) / sizeof (mcs_elf_howto_table[0]);
       i++)
    if (mcs_elf_howto_table[i].name != NULL
	&& strcasecmp (mcs_elf_howto_table[i].name, r_name) == 0)
      return &mcs_elf_howto_table[i];

  return NULL;
}

/* Set the howto pointer for an Lattice Mico32 ELF reloc.  */

static void
mcs_info_to_howto_rela (bfd *abfd ATTRIBUTE_UNUSED,
                         arelent *cache_ptr,
                         Elf_Internal_Rela *dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_MCS_max);
  cache_ptr->howto = &mcs_elf_howto_table[r_type];
}

/* Handle the R_PPC_ADDR16_HA and R_PPC_REL16_HA relocs.  */

static bfd_reloc_status_type
mcs_elf_wxlb_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			 arelent *reloc_entry,
			 asymbol *symbol,
			 void *data ATTRIBUTE_UNUSED,
			 asection *input_section,
			 bfd *output_bfd,
			 char **error_message ATTRIBUTE_UNUSED)
{
  bfd_vma relocation;

  if (output_bfd != NULL)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_overflow;

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;
  if (reloc_entry->howto->pc_relative)
    relocation -= reloc_entry->address;

  if (relocation >= 24)
    return bfd_reloc_overflow;
 reloc_entry->addend = ((relocation & 0xf) << 20)  | ((relocation & 0x10) << 8);  

  return bfd_reloc_continue;
}
#define ELF_ARCH                bfd_arch_mcs
#define ELF_MACHINE_CODE        EM_GTM_MCS
#define ELF_MAXPAGESIZE		0x4000
#undef  TARGET_LITTLE_SYM
#define TARGET_LITTLE_SYM      bfd_elf32_mcs_vec
#undef  TARGET_LITTLE_NAME
#define TARGET_LITTLE_NAME     "elf32-mcs"

#define bfd_elf32_bfd_reloc_type_lookup         mcs_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup         mcs_reloc_name_lookup
#define elf_info_to_howto                       mcs_info_to_howto_rela


#include "elf32-target.h"
