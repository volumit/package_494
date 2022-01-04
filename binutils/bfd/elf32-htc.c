/* HTC specific support for 32-bit ELF.
   Copyright (C) 1998-2011 Free Software Foundation, Inc.
   Contributed by Horst Lehser (Horst.Lehser@hightec-rt.com).

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

#if 0
#include "bfd.h"
#include "sysdep.h"
#include "ctype.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "elf-bfd.h"
#include "elf/common.h"
#endif
#include "elf32-htc.h"
#include "libiberty.h"
#include "xml.h"
#include "xml_defines.h"


/* The initial part of htc_user_section_struct has to be identical with
   fat_user_section_struct from ld.h  */
typedef struct htc_user_section_struct {
  void *head;
  void *tail;
  unsigned long count;
  const char *region_name;
  const flagword  region_flags;
} htc_section_userdata_type;
/* mapping core specific memory region to alternate global addresses */
typedef struct _memmap {
  struct _memmap *next;
  unsigned int core;
  bfd_vma       origin;
  bfd_size_type length;
  bfd_vma       alt_origin;
} memmap_t;
static memmap_t *htc_region_map = NULL;

typedef struct _corealias {
  const char *name;
  const char *alias;
  unsigned int number;
} corealias_t;

#define NUM_OF_CORES    8

static corealias_t corealias[NUM_OF_CORES] = 
{
    { "GLOBAL", NULL, 0 },
    { "CPU0", NULL, 1 },
    { "CPU1", NULL, 2 },
    { "CPU2", NULL, 3 },
    { "CPU3", NULL, 4 },
    { "CPU4", NULL, 5 },
    { "CPU5", NULL, 6 },
    { "CPU6", NULL, 7 }
};

/* the number of the core to link 
   default is core 0 */
unsigned int htc_elf32_core_number = 0; 

/* Forward declarations.  */
static memmap_t *htc_elf32_memory_map_lookup(bfd_vma addr,unsigned int core);
void htc_elf32_add_memory_map(unsigned int core,bfd_vma origin,bfd_size_type length, bfd_vma altorigin);
unsigned int htc_elf32_get_core_number(void);
void htc_elf32_set_core_number(unsigned int core);
unsigned int htc_elf32_get_core_number_from_name(const char *name);
void htc_elf32_set_core_alias (const char *alias, const char *name);
const char * htc_elf32_get_core_name(void);
int htc_elf32_check_core_name(const char *name);
const char * htc_elf32_get_core_name_from_id(unsigned int coreid);

/* Convert TriCore specific section flags to BFD's equivalent.  */

#ifdef SHF_CORE_NUMBER_MASK
static bfd_boolean
htc_elf32_section_flags (flagword *flags ATTRIBUTE_UNUSED, const Elf_Internal_Shdr *hdr ATTRIBUTE_UNUSED)
{
#ifdef HTC_TRICORE
  if (hdr->sh_flags & SHF_ABSOLUTE_DATA)
    *flags |= SEC_ABSOLUTE_DATA;
#endif
  if (hdr->sh_flags & SHF_SMALL_DATA)
    *flags |= SEC_SMALL_DATA;

  if (hdr->sh_flags & SHF_CORE_NUMBER_MASK)
    {
      *flags &= ~SEC_CORE_NUMBER_MASK;
      *flags |= (SHF_CORE_NUMBER_GET(hdr->sh_flags)
                  << SEC_CORE_NUMBER_SHIFT);
    }
  return TRUE;
}  

/* Same as above, but vice-versa.  */

static bfd_boolean
htc_elf32_fake_sections (bfd *abfd ATTRIBUTE_UNUSED, Elf_Internal_Shdr *hdr ATTRIBUTE_UNUSED, asection *sec ATTRIBUTE_UNUSED)
{
  if ((sec->flags & (SEC_DEBUGGING | SEC_EXCLUDE))
          || !(sec->flags & (SEC_CODE | SEC_DATA | SEC_ALLOC)))
    {
      return TRUE;
    }
  if (htc_elf32_core_number)
    {
      if (sec->flags & SEC_CORE_NUMBER_MASK)
        {
          if (htc_elf32_core_number !=
              SEC_CORE_NUMBER_GET(sec->flags))
            {
                const char *c1_name = htc_elf32_get_core_name_from_id(SEC_CORE_NUMBER_GET(sec->flags));
                const char *c2_name = htc_elf32_get_core_name_from_id(htc_elf32_core_number);
                if (c1_name == NULL)
                  c1_name = "unknown";
                if (c2_name == NULL)
                  c2_name = "unknown";
                 
                (*_bfd_error_handler) (_("%s cannot link input section '%s' of core %s into output section %s of core %s."),
                                       bfd_get_filename (abfd),
	                               sec->name,
                                       c1_name,
				       hdr->bfd_section->name,
                                       c2_name);
            }
        }
      if (!(sec->flags & (SEC_DEBUGGING | SEC_EXCLUDE))
          && (sec->flags & (SEC_CODE | SEC_DATA | SEC_ALLOC)))
        {
          hdr->sh_flags &= ~SHF_CORE_NUMBER_MASK;
          hdr->sh_flags |= SHF_CORE_NUMBER(htc_elf32_core_number);
        }
    }
  else if (sec->flags & SEC_CORE_NUMBER_MASK)
    {
      hdr->sh_flags &= ~SHF_CORE_NUMBER_MASK;
      hdr->sh_flags |=
       SHF_CORE_NUMBER(SEC_CORE_NUMBER_GET(sec->flags));

    }
#ifdef HTC_TRICORE
  if (sec->flags & SEC_ABSOLUTE_DATA)
    hdr->sh_flags |= SHF_ABSOLUTE_DATA;
#endif
  if (sec->flags & SEC_SMALL_DATA)
    hdr->sh_flags |= SHF_SMALL_DATA;
     
  return TRUE;
}

static bfd_boolean
htc_elf32_match_sections_by_type(bfd *abfd ATTRIBUTE_UNUSED, const asection *asec ATTRIBUTE_UNUSED,
                                 bfd *bbfd ATTRIBUTE_UNUSED, const asection *bsec ATTRIBUTE_UNUSED)
{
  if ((asec->flags & SEC_CORE_NUMBER_MASK) || (bsec->flags &
                                               SEC_CORE_NUMBER_MASK))
    {
      if ((asec->flags ^ bsec->flags) & SEC_CORE_NUMBER_MASK)
        {
          /* section are for different cores */
          return FALSE;
        }
    }
  return TRUE;
}
#endif


#ifdef STT_HTC_EXPORT_FUNC
static bfd_boolean htc_elf32_export_add_sym(struct bfd_link_hash_entry *,PTR);
/* hash tables for exported symbols */
static struct elf_link_hash_table export_hash;

#endif
void htc_elf32_do_export_symbols(struct bfd_link_info *info);
void htc_elf32_add_exported_symbol
   (const char *name, bfd_boolean type, bfd_size_type size, const char *export_name);

/* export Symbols all marked symbols into  */
bfd_boolean htc_elf32_export_symbols = FALSE;

struct htc_export_hash_entry
{
  struct bfd_link_hash_entry root;
  const char * export_name;
};


/* add an exported symbol to hash table of exported symbols */
void
htc_elf32_add_exported_symbol
   (const char *name ATTRIBUTE_UNUSED, bfd_boolean type ATTRIBUTE_UNUSED, bfd_size_type size ATTRIBUTE_UNUSED, const char *export_name ATTRIBUTE_UNUSED)
{ 
#ifdef STT_HTC_EXPORT_FUNC
  int obj_type = (type == TRUE)? STT_HTC_EXPORT_FUNC: STT_HTC_EXPORT_OBJECT;
  struct elf_link_hash_entry *h;
  h  = elf_link_hash_lookup(&export_hash, name,TRUE,FALSE,FALSE);
  if (h == NULL)
    return ;
  h->root.type = bfd_link_hash_defined;
  h->root.u.def.section = bfd_abs_section_ptr;
  h->root.u.def.value = 0;
  h->type = obj_type;
  h->size = size;
  h->u.weakdef = (struct elf_link_hash_entry *)export_name;

  htc_elf32_export_symbols = TRUE;
#endif
}
void
htc_elf32_do_export_symbols(struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
#ifdef STT_HTC_EXPORT_FUNC
  if (htc_elf32_export_symbols)
    {
      /* Add export symbols.  */
      bfd_link_hash_traverse (info->hash, htc_elf32_export_add_sym, (PTR) info);
    }
#endif
}

unsigned int
htc_elf32_get_flag_core_number(int other ATTRIBUTE_UNUSED)
{
#ifdef STT_HTC_EXPORT_FUNC
  return ELF_STO_READ_CORE_NUMBER(other);
#else
  return 0;
#endif
}

#ifdef STT_HTC_EXPORT_FUNC
/* create hash table for exported symbols */
static bfd_boolean
htc_elf32_link_hash_table_create(bfd *abfd)
{

    if (!_bfd_elf_link_hash_table_init (&export_hash, abfd, _bfd_elf_link_hash_newfunc,
                sizeof(struct elf_link_hash_entry)))
        return FALSE;
    return TRUE;
}


/*
 * mark all not exported symbols as local and set visibility to hidden
 * mark symbol with core number
 */
static bfd_boolean
htc_elf32_export_add_sym(struct bfd_link_hash_entry *entry ATTRIBUTE_UNUSED, PTR parm ATTRIBUTE_UNUSED)
{
  struct elf_link_hash_entry *h = (struct elf_link_hash_entry *) entry;
  struct elf_link_hash_entry *he;
  struct bfd_link_info *info = (struct bfd_link_info *)parm;

  if (( (h->root.type == bfd_link_hash_defined)
         ||  (h->root.type == bfd_link_hash_defweak)))
  {
        he  = elf_link_hash_lookup(&export_hash, h->root.root.string,FALSE,FALSE,FALSE);
        if (he == NULL || (he->type == h->type))
          {
            if (!h->unique_global)
              {
                h->forced_local = 1;
                h->hidden = 1;
                h->other = (h->other & ~ELF_ST_VISIBILITY (-1)) | STV_HIDDEN;
              }
          }
        else if (he->u.weakdef)
          {
            struct elf_link_hash_entry *newh;
            const char * newname;
            newname = (const char *)(he->u.weakdef);
            newh  = (struct elf_link_hash_entry *)bfd_link_hash_lookup(info->hash, newname,TRUE,FALSE,FALSE);
            h->forced_local = 1;
            h->hidden = 1;
            newh->other = h->other;
            h->other = (h->other & ~ELF_ST_VISIBILITY (-1)) | STV_HIDDEN;
            newh->other = ( newh->other & ~ELF_STO_WRITE_CORE_NUMBER(-1)) | ELF_STO_WRITE_CORE_NUMBER(htc_elf32_core_number);
            newh->root.u.def = h->root.u.def;
	    newh->root.type = bfd_link_hash_defined;
            newh->type = h->type;
            newh->size = h->size;
            newh->unique_global = 1;
          }
        h->other = (h->other & ~ELF_STO_WRITE_CORE_NUMBER(-1)) | ELF_STO_WRITE_CORE_NUMBER(htc_elf32_core_number);

  }
  return TRUE;
}

static void
htc_elf32_merge_symbol_attribute(struct elf_link_hash_entry *h ATTRIBUTE_UNUSED,
                                      const Elf_Internal_Sym *isym ATTRIBUTE_UNUSED,
                                      bfd_boolean definition ATTRIBUTE_UNUSED,
                                      bfd_boolean dynamic ATTRIBUTE_UNUSED)
{
  if ((isym->st_other & ~ELF_ST_VISIBILITY (-1)) != 0)
    {
      unsigned char other;

      other = (definition ? isym->st_other : h->other);
      other &= ~ELF_ST_VISIBILITY (-1);
      h->other = other | ELF_ST_VISIBILITY (h->other);
    }
  h->other |= ELF_STO_WRITE_CORE_NUMBER(ELF_STO_READ_CORE_NUMBER(isym->st_other));
}
#endif

/* lookup for a memory map returns NULL if no mapping exists */
static memmap_t *
htc_elf32_memory_map_lookup(bfd_vma addr,unsigned int core)
{
  memmap_t *p;

  p = htc_region_map;

  while (p)
    {
      if (((p->origin <= addr) && (addr < (p->origin + p->length))) && (p->core == core))
        return p;
      p = p->next;
    }
  return p;
}
#ifdef SHF_CORE_NUMBER_MASK
/* get an alternate address */
static bfd_vma
htc_elf32_get_alternate_address(bfd_vma addr , unsigned int other, asection *sym_section, asection *osec)
{
  memmap_t      *map;
  unsigned int core;
  htc_section_userdata_type *ud = NULL;
  flagword  sym_core = ELF_STO_READ_CORE_NUMBER(other);

  if (sym_section)
        ud = bfd_get_section_userdata(NULL,sym_section);

  if (ud)
    sym_core = SHF_CORE_NUMBER_GET(ud->region_flags);
        
  if (sym_core == SHF_CORE_NUMBER_GET(osec->flags))
    return addr;

  core = sym_core;

  map = htc_elf32_memory_map_lookup(addr,core);
  if (map != 0)
    {
      return (addr - map->origin) + map->alt_origin; 
    }
  return addr;
}

#endif
const char * htc_elf32_get_memory_region_from_section(asection *osec);
const char *
htc_elf32_get_memory_region_from_section(asection *osec)
{
  htc_section_userdata_type *ud;
  ud = bfd_get_section_userdata(NULL,osec);
  if (ud)
    return ud->region_name;
  else
    return "NOREGION";
}
/* get the actual core number linker for */
unsigned int
htc_elf32_get_core_number(void)
{
  return htc_elf32_core_number;
}

void
htc_elf32_set_core_number(unsigned int core)
{
  if (core > 7)
    {
      (*_bfd_error_handler)(_("error: invalid core number %d\n"),core);
      return;
    }
  htc_elf32_core_number = core;
}

unsigned int 
htc_elf32_get_core_number_from_name(const char *name)
{
  int i;
  for (i = 0; i < NUM_OF_CORES; i++)
    {
      if (!strcmp(name,corealias[i].name))
        return corealias[i].number;
      else
        if (corealias[i].alias && !strcmp(corealias[i].alias,name))
          return corealias[i].number;
    }
  return 0;
}
const char *
htc_elf32_get_core_name(void)
{
  if (htc_elf32_core_number == 0)
    return NULL;
  return (corealias[htc_elf32_core_number].alias)? 
   corealias[htc_elf32_core_number].alias :
   corealias[htc_elf32_core_number].name;
}

const char *
htc_elf32_get_core_name_from_id(unsigned int coreid)
{
  if (coreid < NUM_OF_CORES)
    {
      if (corealias[coreid].alias)
        return corealias[coreid].alias;
      else
        return corealias[coreid].name;
    }
  else
    return NULL;
}

int 
htc_elf32_check_core_name(const char *name)
{
  int i;
  for (i = 0; i < NUM_OF_CORES; i++)
    {
      if (corealias[i].alias && !strcmp(name,corealias[i].alias))
        return corealias[i].number;
      if (!strcmp(name,corealias[i].name))
        return corealias[i].number;
    }
  return -1;
}
void
htc_elf32_set_core_alias (const char *alias, const char *name)
{
  int i;

  /* test for existing alias */
  for (i = 0; i < NUM_OF_CORES; i++)
    {
      if (corealias[i].alias && !strcmp(alias,corealias[i].alias))
        {
          (*_bfd_error_handler) (_("warning: Core alias %s already exists for %s\n"), alias,
                 corealias[i].name);
          return;
        }
      if (!strcmp(alias,corealias[i].name))
        {
          (*_bfd_error_handler) (_("warning: Core alias %s is a default core name\n"), alias);
          return;
        }
    }
  for (i = 0; i < NUM_OF_CORES; i++)
    {
      if (!strcmp(name,corealias[i].name))
        {
          if (corealias[i].alias)
            {
              (*_bfd_error_handler)(_("warning: Core alias for %s already defined as %s\n"),
                    name, corealias[i].alias);
              return;
            }
          corealias[i].alias = xstrdup(alias);
          return;
        }
    }
  (*_bfd_error_handler)(_("warning: Core name %s not defined\n"),name);
}


/* add a region map for a core */
void 
htc_elf32_add_memory_map(unsigned int core,bfd_vma origin,bfd_size_type length, bfd_vma altorigin)
{
  memmap_t *p;
  memmap_t *new_p;

  p = htc_elf32_memory_map_lookup(origin,core);
  if (p != 0)
    {
      (*_bfd_error_handler)
       (_("warning: memory region mapping for core %d of region 0x%x already declared to 0x%x\n"), core,p->origin,p->alt_origin);
    }
  new_p = (memmap_t *) bfd_malloc (sizeof (memmap_t));

  new_p->core = core;
  new_p->origin = origin;
  new_p->length = length;
  new_p->alt_origin = altorigin;
  new_p->next = htc_region_map;
  htc_region_map = new_p;
}

/* print out the region maps */
void print_region_maps(FILE *);
void xml_region_maps (p_xml_element);

void print_region_maps(FILE *out)
{
  if (htc_region_map)
    {
      memmap_t *p;
      const char *cname;

      fprintf(out,"\nMemory region maps:\n\n");
      fprintf(out,"CORE     Local Region   Global Region\n");
      for (p = htc_region_map; p ; p = p->next)
        {
          cname = htc_elf32_get_core_name_from_id(p->core);
          fprintf(out,"%-8s ", (cname)? cname: "UNKNOWN");
          fprintf(out,"0x%08lx  ", p->origin);
          fprintf(out,"   0x%08lx\n", p->alt_origin);
        }
      fprintf(out,"\n");
    }
}

void xml_region_maps (p_xml_element root)
{
  if (htc_region_map)
    {
      memmap_t *p;
      const char *cname;

      p_xml_element mem_reg_maps = create_xml_element (XML_E_MEMORY_REGION_MAPS);

      add_xml_child (root, mem_reg_maps);

      for (p = htc_region_map; p ; p = p->next)
        {
          p_xml_element reg_map = create_xml_element (XML_E_REGION_MAP);
          add_xml_child (mem_reg_maps, reg_map);

          cname = htc_elf32_get_core_name_from_id (p->core);
          add_xml_attribute (reg_map, XML_A_CORE, (cname) ? cname : "UNKNOWN");

          add_xml_attribute_hex (reg_map, XML_A_LOCAL_REGION, p->origin);

          add_xml_attribute_hex (reg_map, XML_A_GLOBAL_REGION, p->alt_origin);
        }
    }
}
