#
# Linker script template for TriCore ELF objects and executables
# (embedded version).
#
# Copyright (C) 1998-2005 Free Software Foundation, Inc.
# Contributed by Michael Schumacher (mike@hightec-rt.com).
#
# When adding sections, do note that the names of some sections are used
# when specifying the start address of the next.
#


CTOR=".ctors        ${CONSTRUCTING-0} : FLAGS(al)
  {
    ${CONSTRUCTING+${CTOR_START}}
    __CTOR_LIST__ = . ;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */

    KEEP (*crtbegin.o(.ctors))

    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */

    KEEP (*(EXCLUDE_FILE (*crtend.o $OTHER_EXCLUDE_FILES) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    LONG(0) ;
    __CTOR_END__ = . ;
	. = ALIGN(8);
    ${CONSTRUCTING+${CTOR_END}}
  } > CODE_MEM"
DTOR=".dtors        ${CONSTRUCTING-0} :  FLAGS(al)
  {
    ${CONSTRUCTING+${DTOR_START}}
    __DTOR_LIST__ = . ;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o $OTHER_EXCLUDE_FILES) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    LONG(0) ;
    __DTOR_END__ = . ;
	. = ALIGN(8);
    ${CONSTRUCTING+${DTOR_END}}
  } > CODE_MEM"

test -z "$ENTRY" && ENTRY=_start
if [ -z "$MACHINE" ]; then OUTPUT_ARCH=${ARCH}; else OUTPUT_ARCH=${ARCH}:${MACHINE}; fi
test "$LD_FLAG" = "N" && DATA_ADDR=.

# if this is for an embedded system, don't add SIZEOF_HEADERS.
if [ -z "$EMBEDDED" ]; then
   test -z "${TEXT_BASE_ADDRESS}" && TEXT_BASE_ADDRESS="${TEXT_START_ADDR} + SIZEOF_HEADERS"
else
   test -z "${TEXT_BASE_ADDRESS}" && TEXT_BASE_ADDRESS="${TEXT_START_ADDR}"
fi
Q=\"

cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${OUTPUT_ARCH})
${RELOCATING+ENTRY(${ENTRY})}
${RELOCATING+${EXECUTABLE_SYMBOLS}}

EOF


cat <<EOF
${RELOCATING+
/*
 * Define the sizes of the user and system stacks.
 */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 4K ;
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 20K ;
__CSA_SIZE = DEFINED(__CSA_SIZE) ? __CSA_SIZE : 4K ;

/*
 * The heap is the space which is used by malloc() and various other 
 * functions. Below you can define the minimum amount of memory that 
 * the heap should provide.
 */
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 8K ;

}
EOF


cat <<EOF
SECTIONS
{
  ${RELOCATING+
  /*
   * The startup code should be placed where the CPU expects it after a reset,
   * so we try to locate it first, no matter where it appears in the list of
   * objects and libraries (note: because the wildcard pattern doesn't match
   * directories, we'll try to find crt0.o in various (sub)directories).
   */
  .startup :
  {
    KEEP (*(.startup_code))
	. = ALIGN(8);    
  "}" ${RELOCATING+> CODE_MEM =${NOP-0}}
  }
  ${RELOCATING+
  /*
   * Allocate space for absolute addressable sections; this requires that
   * ${Q}ZDATA_MEM${Q} starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
  }
  
  ${RELOCATING-
   CORE_SEC(.zrodata) 0: FLAGS(arzl)
  {
    *(.zrodata)
    *(.zrodata.*)
  "}" 
  }
 
  ${RELOCATING-
  CORE_SEC(.bbss) 0 : FLAGS(awz)
  {
	  *(.bbss)
	  *(.bbss.*)
  "}"
  }

 CORE_SEC(.zbss) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(azw)}
  {
    ${RELOCATING+CORE_SYM(ZBSS_BASE) = . ;}
    *(.zbss)
    ${RELOCATING+*(.zbss.*)}
    ${RELOCATING+*(.gnu.linkonce.zb.*)}
	${RELOCATING+*(.bbss)}
    ${RELOCATING+*(.bbss.*)}
    ${RELOCATING+CORE_SYM(ZBSS_END) = . ;}
  } ${RELOCATING+> ZDATA_MEM}

  ${RELOCATING-
  CORE_SEC(.bbata) 0 :
  {
	  *(.bbata)
	  *(.bbata.*)
  "}"
  }
  
  CORE_SEC(.zdata) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awzl)}
  {
    ${RELOCATING+CORE_SYM(ZDATA_BASE) = . ;}
   *(.zdata)
	${RELOCATING+*(.zdata.*)}
    ${RELOCATING+*(.gnu.linkonce.z.*)}
    ${RELOCATING+*(.bdata)}
    ${RELOCATING+*(.bdata.*)}
    ${RELOCATING+CORE_SYM(ZDATA_END) = . ;}
  } ${RELOCATING+> ZDATA_MEM AT> CODE_MEM}

  CORE_SEC(.zrodata) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(arzl)}
  {
 	${RELOCATING+*(.zrodata)}
	${RELOCATING+*(.zrodata.*)}
    ${RELOCATING+*(.gnu.linkonce.zr.*)}
  } ${RELOCATING+> ZDATA_MEM AT> CODE_MEM}

  ${RELOCATING+
	  
  /*
   * Allocate trap and interrupt vector tables.
   */
  CORE_SEC(.traptab)  : ALIGN(8)
  {
    *(.traptab)
  "}" > CODE_MEM

  CORE_SEC(.inttab)  : ALIGN(8)
  {
    *(.inttab)
  "}" > CODE_MEM
  }

  
  CORE_SEC(.init) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(axl)}
  {
      /* we have to keep all .init/.fini section 
         because the linker should not delete this
         sections with --gc-sections
       */
	KEEP( *(SORT(.init*)) )

	KEEP( *(SORT(.fini*)) )
  } ${RELOCATING+> CODE_MEM =${NOP-0}}

  
  
  ${RELOCATING+
  /*
   * Allocate .text and other read-only sections.
   */
  }
  CORE_SEC(.text) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(axl) } 
  {
    ${RELOCATING+${TEXT_START_SYMBOLS}}
    *(.text)
    ${RELOCATING+
	*(.text.*)
    *(.pcp_c_ptr_init)
    *(.pcp_c_ptr_init.*)
    *(.gnu.linkonce.t.*)
    /*
     * .gnu.warning sections are handled specially by elf32.em.
     */
    *(.gnu.warning)
	}
  } ${RELOCATING+> CODE_MEM =${NOP-0}}


  CORE_SEC(.rodata) ${RELOCATING- 0 } : ${RELOCATING+ALIGN(8) FLAGS(arl) }
  {
    *(.rodata)
    ${RELOCATING+
	*(.rodata.*)
	*(.gnu.linkonce.r.*)
    . = ALIGN(8);
    }
  } ${RELOCATING+> CODE_MEM}

  ${RELOCATING+
  CORE_SEC(.clear_sec) : ALIGN(8) FLAGS(arl)
  {
    /*
     * Create the clear and copy tables that tell the startup code
     * which memory areas to clear and to copy, respectively.
     */
    PROVIDE(CORE_SYM(__clear_table) = .) ;
    LONG(0 + ADDR(CORE_SEC(.bss)));     LONG(SIZEOF(CORE_SEC(.bss)));
    LONG(0 + ADDR(CORE_SEC(.sbss)));    LONG(SIZEOF(CORE_SEC(.sbss)));
    LONG(0 + ADDR(CORE_SEC(.zbss)));    LONG(SIZEOF(CORE_SEC(.zbss)));
    LONG(__PCP_CODE_RAM_BEGIN);    LONG(__PCP_CODE_RAM_SIZE);
    LONG(-1);                 LONG(-1);
  "}" > CODE_MEM
  CORE_SEC(.copy_sec) : ALIGN(8) FLAGS(arl)
  {

    PROVIDE(CORE_SYM(__copy_table) = .) ;
    LONG(LOADADDR(CORE_SEC(.data)));    LONG(0 + ADDR(CORE_SEC(.data))); LONG(SIZEOF(CORE_SEC(.data)));
    LONG(LOADADDR(CORE_SEC(.sdata)));   LONG(0 + ADDR(CORE_SEC(.sdata)));LONG(SIZEOF(CORE_SEC(.sdata)));
    LONG(LOADADDR(CORE_SEC(.zdata)));   LONG(0 + ADDR(CORE_SEC(.zdata)));LONG(SIZEOF(CORE_SEC(.zdata)));
    LONG(LOADADDR(CORE_SEC(.zrodata))); LONG(0 + ADDR(CORE_SEC(.zrodata)));LONG(SIZEOF(CORE_SEC(.zrodata)));
    LONG(LOADADDR(.pcpdata)); LONG(0 + ADDR(.pcpdata)); LONG(SIZEOF(.pcpdata));
    LONG(LOADADDR(.pcptext)); LONG(0 + ADDR(.pcptext));LONG(SIZEOF(.pcptext));
    LONG(-1);                 LONG(-1);                  LONG(-1);
  "}" > CODE_MEM
  }

  CORE_SEC(.sdata2) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(arsl) }
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SMALL_DATA2_) = 0x8000);}
    *(.sdata.rodata)
    ${RELOCATING+*(.sdata.rodata.*)}
    *(.srodata)
    ${RELOCATING+*(.srodata.*)}
    ${RELOCATING+*(.sdata2)}
    ${RELOCATING+*(.sdata2.*)}
    ${RELOCATING+*(.gnu.linkonce.sr.*)}
  } ${RELOCATING+> CODE_MEM}


  ${RELOCATING+
  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the ${Q}strip${Q} and ${Q}objcopy${Q} utilities to remove
   * the .eh_frame section from the executable.
   */
  }
  CORE_SEC(.eh_frame) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8)}
  {
    ${RELOCATING+*(.gcc_except_table)}
    ${RELOCATING+__EH_FRAME_BEGIN__ = . ;}
    ${RELOCATING+KEEP (*(.eh_frame))}
    ${RELOCATING-*(.eh_frame)}
    ${RELOCATING+__EH_FRAME_END__ = . ;}
  } ${RELOCATING+> CODE_MEM}

  .jcr          ${RELOCATING-0} : { KEEP (*(.jcr)) } ${RELOCATING+> CODE_MEM}

  ${RELOCATING+${CTOR}}
  ${RELOCATING+${DTOR}}

  ${RELOCATING+
  /*
   * We"'"re done now with the text part of the executable.  The
   * following sections are special in that their initial code or
   * data (if any) must also be stored in said text part of an
   * executable, but they ${Q}live${Q} at completely different addresses
   * at runtime -- usually in RAM areas.  NOTE: This is not really
   * necessary if you use a special program loader (e.g., a debugger)
   * to load a complete executable consisting of code, data, BSS, etc.
   * into the RAM of some target hardware or a simulator, but it *is*
   * necessary if you want to burn your application into non-volatile
   * memories such as EPROM or FLASH.
   */
  }

  .pcptext${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(axpl) }
  {
    ${RELOCATING+CORE_SYM(PCODE_BASE) = . ;}
    KEEP(*(.pcptext))
    ${RELOCATING+KEEP(*(.pcptext.*))}
    ${RELOCATING+. = ALIGN(8) ;}
   ${RELOCATING+CORE_SYM(PCODE_END) = . ;}
  } ${RELOCATING+> PCP_CODE AT> CODE_MEM}

  .pcpdata${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awpl)}
  {
    ${RELOCATING+CORE_SYM(PRAM_BASE) = . ;}
    KEEP(*(.pcpdata))
    ${RELOCATING+KEEP(*(.pcpdata.*))}
    ${RELOCATING+. = ALIGN(8) ;}
    ${RELOCATING+CORE_SYM(PRAM_END) = . ;}
  } ${RELOCATING+> PCP_DATA AT> CODE_MEM}

  CORE_SEC(.data)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl) }
  {
    ${RELOCATING+CORE_SYM(DATA_BASE) = . ;}
    *(.data)
    ${RELOCATING+*(.data.*)}
    ${RELOCATING+*(.gnu.linkonce.d.*)}
    ${CONSTRUCTING+SORT(CONSTRUCTORS)}
    ${RELOCATING+CORE_SYM(DATA_END) = . ;}
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  CORE_SEC(.sdata) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl) }
  {
	${RELOCATING+CORE_SYM(SDATA_BASE) = . ;}
    ${RELOCATING+PROVIDE(CORE_SYM(__sdata_start) = .);}
    ${RELOCATING+PROVIDE(CORE_SYM(_SMALL_DATA_) = 0x8000);}
    *(.sdata)
    ${RELOCATING+*(.sdata.*)}
    ${RELOCATING+*(.gnu.linkonce.s.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws) }
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss_start) = .);}
    *(.sbss)
    ${RELOCATING+*(.sbss.*)}
    ${RELOCATING+*(.gnu.linkonce.sb.*)}
  } ${RELOCATING+> SDATA_MEM }

  ${RELOCATING+
  /*
   * Allocate space for BSS sections.
   */
  }
  CORE_SEC(.bss) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+CORE_SYM(BSS_BASE) = . ;}
    ${RELOCATING+${OTHER_BSS_SYMBOLS}}
    *(.bss)
    ${RELOCATING+
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8) ;
    }
  } ${RELOCATING+> BSS_MEM}
  CORE_SEC(.ustack) : ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+
    . +=  __USTACK_SIZE ;
    . = ALIGN(8) ;
    CORE_SYM(__USTACK) =  . ;
    }
  } ${RELOCATING+> BSS_MEM}

  CORE_SEC(.istack) : ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+
    . +=  __ISTACK_SIZE ;
    . = ALIGN(8) ;
    CORE_SYM(__ISTACK) =  . ;
    }
  } ${RELOCATING+> BSS_MEM}
  CORE_SEC(.heap) : ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+
    CORE_SYM(__HEAP) = . ;
    . +=  __HEAP_SIZE ;
    . = ALIGN(8) ;
    CORE_SYM(__HEAP_END) =  . ;
    }
  } ${RELOCATING+> BSS_MEM}


  ${RELOCATING+
  /* define the CSA Memory area as an own section
   * this section will be allocated into the internal RAM
   * after the absolute addressable sections .zdata/.zbss
   * and allocate all internal memory not occupied by .zdata/.zbss
  */
  CORE_SEC(.csa) : ALIGN(64) FLAGS(aw)
  {
    CORE_SYM(__CSA_BEGIN) = . ;
    . +=  __CSA_SIZE ;
    CORE_SYM(__CSA_END) = .;
  "}" > CSA_MEM
  CORE_SYM(__CSA_SIZE) = CORE_SYM(__CSA_END) - CORE_SYM(__CSA_BEGIN);
  }



  ${RELOCATING+CORE_SYM(_end) = CORE_SYM(__HEAP_END) ;}
  ${RELOCATING+PROVIDE(CORE_SYM(end) = CORE_SYM(_end)) ;}

  ${RELOCATING+

  /* Make sure CSA, stack and heap addresses are properly aligned.  */
  _. = ASSERT ((CORE_SYM(__CSA_BEGIN) & 0x3f) == 0 , ${Q}illegal CSA start address${Q}) ;
  _. = ASSERT ((CORE_SYM(__CSA_SIZE) & 0x3f) == 0 , ${Q}illegal CSA size${Q}) ;
  _. = ASSERT ((CORE_SYM(__ISTACK) & 7) == 0 , ${Q}ISTACK not doubleword aligned${Q}) ;
  _. = ASSERT ((CORE_SYM(__USTACK) & 7) == 0 , ${Q}USTACK not doubleword aligned${Q}) ;
  _. = ASSERT ((CORE_SYM(__HEAP_END) & 7) == 0 , ${Q}HEAP not doubleword aligned${Q}) ;

  /* Define a default symbol for address 0.  */
  NULL = DEFINED (NULL) ? NULL : 0 ;
  }

  /*
   * DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the
   * beginning of the section, so we begin them at 0.
   */

  /*
   * DWARF 1
   */
  .comment         0 : { *(.comment) }
  .debug           0 : { *(.debug) }
  .line            0 : { *(.line) }

  /*
   * GNU DWARF 1 extensions
   */
  .debug_srcinfo   0 : { *(.debug_srcinfo) }
  .debug_sfnames   0 : { *(.debug_sfnames) }

  /*
   * DWARF 1.1 and DWARF 2
   */
  .debug_aranges   0 : { *(.debug_aranges) }
  .debug_pubnames  0 : { *(.debug_pubnames) }

  /*
   * DWARF 2
   */
  .debug_info      0 : { *(.debug_info) }
  .debug_abbrev    0 : { *(.debug_abbrev) }
  .debug_line      0 : { *(.debug_line) }
  .debug_frame     0 : { *(.debug_frame) }
  .debug_str       0 : { *(.debug_str) }
  .debug_loc       0 : { *(.debug_loc) }
  .debug_macinfo   0 : { *(.debug_macinfo) }
  .debug_ranges    0 : { *(.debug_ranges) }

   /*
    * DWARF 2 control flow extension
    */
  .debug_control_flow 0 : { *(.debug_control_flow) }

  /*
   * SGI/MIPS DWARF 2 extensions
   */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  ${RELOCATING+
  /*
   * Optional sections that may only appear when relocating.
   */
  ${OTHER_RELOCATING_SECTIONS}
  }

  /*
   * Optional sections that may appear regardless of relocating.
   */
  ${OTHER_SECTIONS}
}
EOF
