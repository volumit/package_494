#
# Linker script template for PPC VLE ELF objects and executables
# (embedded version).
#
# Copyright (C) 2010 Free Software Foundation, Inc.
# Contributed by Horst Lehser (Horst.Lehser@hightec-rt.com).
#
# When adding sections, do note that the names of some sections are used
# when specifying the start address of the next.
#


CTOR="CORE_SEC(.ctors)        ${CONSTRUCTING-0} : FLAGS(al)
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
DTOR="CORE_SEC(.dtors)        ${CONSTRUCTING-0} : FLAGS(al)
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

test -z "$ATTRS_SECTIONS" && ATTRS_SECTIONS=".gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }"
test -z "$OTHER_SECTIONS" && OTHER_SECTIONS=".PPC.EMB.apuinfo 0 : { KEEP (*(.PPC.EMB.apuinfo)) }"

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
${RELOCATING+

/*
 * You may need to change the values to match
 * your actual hardware setup and pass the modified script to the
 * linker using its ${Q}-dT${Q} option. Alternativly you can pass a 
 * text file with the option ${Q}-T${Q} includeing the following symbol 
 * definition and a MEMORY section to the linker to override the default 
 * definitions
 * (look at memory.x in the compiler specific directories for an example)
 */

/* the external RAM description 
__INT_CODE_RAM_BEGIN = DEFINED(__INT_CODE_RAM_BEGIN)? __INT_CODE_RAM_BEGIN : 0x00000000;
__INT_CODE_RAM_SIZE  = DEFINED(__INT_CODE_RAM_SIZE)? __INT_CODE_RAM_SIZE : 1512K ;
__INT_DATA_RAM_BEGIN = DEFINED(__INT_DATA_RAM_BEGIN)? __INT_DATA_RAM_BEGIN : 0x40000000 + 64;
__INT_DATA_RAM_SIZE  = DEFINED(__INT_DATA_RAM_SIZE)? __INT_DATA_RAM_SIZE : 64;
*/


MEMORY
{
  code_mem (rx):	org = 0x00000000, len = 1512K  /* internal flash */
  data_mem (w!x):	org = 0x40000000, len = 64K     /* internal ram */
"}"

}
EOF

if test "$RELOCATING"; then
  # Can't use ${RELOCATING+blah "blah" blah} for this,
  # because bash 2.x will lose the doublequotes.
  cat <<EOF
 /* map all region to existing memory areas */
REGION_ALIAS("CODE_MEM", code_mem)
REGION_ALIAS("SDATA2_MEM", code_mem) 
REGION_ALIAS("DATA_MEM", data_mem)
REGION_ALIAS("SDATA_MEM", data_mem)
REGION_ALIAS("BSS_MEM", data_mem)
EOF
fi



cat <<EOF
${RELOCATING+
/*
 * Define the sizes of the user and system stacks.
 */
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 4K ;

/*
 * The heap is the memory between the top of the user stack and
 * __RAM_END (as defined above); programs can dynamically allocate
 * space in this area using malloc() and various other functions.
 * Below you can define the minimum amount of memory that the heap
 * should provide.
 */
__HEAP_MIN = DEFINED (__HEAP_MIN) ? __HEAP_MIN : 4K ;
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 4K ;

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
  CORE_SEC(.startup) :
  {
    KEEP (*(.startup_code))
    KEEP (*(.startup))
	. = ALIGN(8);    
  "}" > CODE_MEM =${NOP-0}
  }
  
  ${RELOCATING+
	  
  /*
   * Allocate trap and interrupt vector tables.
   */
  CORE_SEC(.traptable)  : ALIGN(16)
  {
    *(.traptab)
    *(.traptable)
  "}" > CODE_MEM

  CORE_SEC(.inttable)  : ALIGN(2048)
  {
    *(.inttab)
    *(.inttable)
  "}" > CODE_MEM
  }

  CORE_SEC(.init_vle) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8)}
  {
      /* we have to keep all .init/.fini section 
         because the linker should not delete this
         sections with --gc-sections
       */
	KEEP( *(SORT(.init_vle)) )

	KEEP( *(SORT(.fini_vle)) )
  } ${RELOCATING+> CODE_MEM =${NOP-0}}
  
  CORE_SEC(.init) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8)}
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
  CORE_SEC(.text_vle) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(axv) } 
  {
    ${RELOCATING+${TEXT_VLE_START_SYMBOLS}}
    *(.text_vle)
    ${RELOCATING+
	*(.text_vle.*)
    *(.gnu.linkonce.t_vle.*)
    /*
     * .gnu.warning sections are handled specially by elf32.em.
     */
    *(.gnu.warning)
	}
  } ${RELOCATING+> CODE_MEM =${NOP-0}}

  CORE_SEC(.text) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(ax) } 
  {
    ${RELOCATING+${TEXT_START_SYMBOLS}}
    *(.text)
    ${RELOCATING+
	*(.text.*)
    *(.gnu.linkonce.t.*)
    /*
     * .gnu.warning sections are handled specially by elf32.em.
     */
    *(.gnu.warning)
	}
  } ${RELOCATING+> CODE_MEM =${NOP-0}}

  CORE_SEC(.rodata) ${RELOCATING- 0 } : ${RELOCATING+ALIGN(8)}
  {
    *(.rodata)
    ${RELOCATING+
	*(.rodata.*)
	*(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.toc)
    . = ALIGN(8);
    }
  } ${RELOCATING+> CODE_MEM}

  CORE_SEC(.jcr) ${RELOCATING- 0 } : ${RELOCATING+ALIGN(8)}
  {
    *(.jcr)
  } ${RELOCATING+> CODE_MEM}

   
   ${RELOCATING+
  /*
   * Create the clear and copy tables that tell the startup code
   * which memory areas to clear and to copy, respectively.
   */
  CORE_SEC(.rom_copy_info) : ALIGN(8)
  {
    PROVIDE(CORE_SYM(__rom_copy_info) = . ) ;
    LONG(LOADADDR(CORE_SEC(.data))) ; LONG(0 + ADDR(CORE_SEC(.data))) ; LONG(SIZEOF(CORE_SEC(.data)));
    LONG(LOADADDR(CORE_SEC(.sdata))) ; LONG(0 + ADDR(CORE_SEC(.sdata))) ; LONG(SIZEOF(CORE_SEC(.sdata)));
    LONG(LOADADDR(CORE_SEC(.sdata14))) ; LONG(0 + ADDR(CORE_SEC(.sdata14))) ; LONG(SIZEOF(CORE_SEC(.sdata14)));
    LONG(LOADADDR(CORE_SEC(.sdata15))) ; LONG(0 + ADDR(CORE_SEC(.sdata15))) ; LONG(SIZEOF(CORE_SEC(.sdata15)));
    LONG(LOADADDR(CORE_SEC(.sdata16))) ; LONG(0 + ADDR(CORE_SEC(.sdata16))) ; LONG(SIZEOF(CORE_SEC(.sdata16)));
    LONG(LOADADDR(CORE_SEC(.sdata17))) ; LONG(0 + ADDR(CORE_SEC(.sdata17))) ; LONG(SIZEOF(CORE_SEC(.sdata17)));
    LONG(0) ; LONG(0) ; LONG(0) ;
  "}" > CODE_MEM

  CORE_SEC(.bss_init_info)  : ALIGN(8)
  {
    PROVIDE(CORE_SYM(__bss_init_info) = . ) ;
    LONG(0 + ADDR(CORE_SEC(.bss))); LONG(SIZEOF(CORE_SEC(.bss)));
    LONG(0 + ADDR(CORE_SEC(.sbss))); LONG(SIZEOF(CORE_SEC(.sbss)));
    LONG(0 + ADDR(CORE_SEC(.sbss2))); LONG(SIZEOF(CORE_SEC(.sbss2)));
    LONG(0 + ADDR(CORE_SEC(.sbss14))); LONG(SIZEOF(CORE_SEC(.sbss14)));
    LONG(0 + ADDR(CORE_SEC(.sbss15))); LONG(SIZEOF(CORE_SEC(.sbss15)));
    LONG(0 + ADDR(CORE_SEC(.sbss16))); LONG(SIZEOF(CORE_SEC(.sbss16)));
    LONG(0 + ADDR(CORE_SEC(.sbss17))); LONG(SIZEOF(CORE_SEC(.sbss17)));
    LONG(0) ; LONG(0) ; LONG(0) ;
  "}" > CODE_MEM

  }


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
  CORE_SEC(.eh_frame) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) }
  {
    ${RELOCATING+*(.gcc_except_table)}
    ${RELOCATING+CORE_SYM(__EH_FRAME_BEGIN__) = . ;}
    ${RELOCATING+KEEP (*(.eh_frame))}
    ${RELOCATING-*(.eh_frame)}
    ${RELOCATING+CORE_SYM(__EH_FRAME_END__) = . ;}
  } ${RELOCATING+> CODE_MEM}

  ${RELOCATING+${CTOR}}
  ${RELOCATING+${DTOR}}

  ${OTHER_RELRO_SECTIONS}
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
  CORE_SEC(.got)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl)}
  {
    ${RELOCATING+CORE_SYM(GOT_BASE) = . ;}
    *(.got)
    *(.got.*)
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  CORE_SEC(.got1)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl)}
  {
    ${RELOCATING+CORE_SYM(GOT1_BASE) = . ;}
    *(.got1)
    *(.got1.*)
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  CORE_SEC(.got2)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl)}
  {
    ${RELOCATING+CORE_SYM(GOT2_BASE) = . ;}
    *(.got2)
    *(.got2.*)
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  CORE_SEC(.fixup)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl)}
  {
    ${RELOCATING+CORE_SYM(FIXUP_BASE) = . ;}
    *(.fixup)
    *(.fixup.*)
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  ${RELOCATING+
  /*
   * allocating the small addressable data areas
   * .sdata2 (using base register r2) includes also the 
   * readonly small addressable parts
   * by default we assume that .sdata2 contains only readonly data
   * and that .sbss2 is not used
   * if you use .sdata2 for RW data and .sbss2, you have to allocate
   * .sdata2 into RAM like .sdata
   * and add an entry into the .rom_copy_info table
   */
   }
  CORE_SEC(.sdata2) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(asl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA2_BASE_) = 0x8000);}
    *(.sdata.rodata)
    *(.sdata.rodata.*)
    *(.srodata)
    *(.srodata.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.PPC.EMB.sdata2*)
  } ${RELOCATING+> SDATA2_MEM}

  CORE_SEC(.sbss2) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss2_start) = .);}
    *(.sbss2)
    ${RELOCATING+*(.sbss2.*)}
    ${RELOCATING+*(.PPC.EMB.sbss2*)}
  } ${RELOCATING+> SDATA2_MEM }

  CORE_SEC(.sdata) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA_BASE_) = 0x8000);}
    *(.sdata)
    ${RELOCATING+*(.sdata.*)}
    ${RELOCATING+*(.gnu.linkonce.s.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss_start) = .);}
    *(.sbss)
    ${RELOCATING+*(.sbss.*)}
    ${RELOCATING+*(.gnu.linkonce.sb.*)}
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss_end) = .);}
  } ${RELOCATING+> SDATA_MEM }

  CORE_SEC(.sdata14) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA14_BASE_) = 0x8000);}
    *(.sdata14)
    ${RELOCATING+*(.sdata14.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss14) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss14_start) = .);}
    *(.sbss14)
    ${RELOCATING+*(.sbss14.*)}
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss14_end) = .);}
  } ${RELOCATING+> SDATA_MEM }

  CORE_SEC(.sdata15) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA15_BASE_) = 0x8000);}
    *(.sdata15)
    ${RELOCATING+*(.sdata15.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss15) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss15_start) = .);}
    *(.sbss15)
    ${RELOCATING+*(.sbss15.*)}
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss15_end) = .);}
  } ${RELOCATING+> SDATA_MEM }

  CORE_SEC(.sdata16) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA16_BASE_) = 0x8000);}
    *(.sdata16)
    ${RELOCATING+*(.sdata16.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss16) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss16_start) = .);}
    *(.sbss16)
    ${RELOCATING+*(.sbss16.*)}
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss16_end) = .);}
  } ${RELOCATING+> SDATA_MEM }

  CORE_SEC(.sdata17) ${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awsl)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(_SDA17_BASE_) = 0x8000);}
    *(.sdata17)
    ${RELOCATING+*(.sdata17.*)}
  } ${RELOCATING+> SDATA_MEM  AT> CODE_MEM}

  CORE_SEC(.sbss17) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aws)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss17_start) = .);}
    *(.sbss17)
    ${RELOCATING+*(.sbss17.*)}
    ${RELOCATING+PROVIDE(CORE_SYM(__sbss17_end) = .);}
  } ${RELOCATING+> SDATA_MEM }

  CORE_SEC(.data)${RELOCATING- 0 }: ${RELOCATING+ALIGN(8) FLAGS(awl)}
  {
    ${RELOCATING+CORE_SYM(DATA_BASE) = . ;}
    *(.data)
    ${RELOCATING+*(.data.*)}
    ${RELOCATING+*(.gnu.linkonce.d.*)}
    ${CONSTRUCTING+SORT(CONSTRUCTORS)}
    ${RELOCATING+CORE_SYM(DATA_END) = . ;}
  } ${RELOCATING+> DATA_MEM AT> CODE_MEM}

  ${RELOCATING+
  /*
   * Allocate space for BSS sections.
   */
  }
  CORE_SEC(.bss) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+PROVIDE(CORE_SYM(__bss_start) = .);}
    ${RELOCATING+CORE_SYM(BSS_BASE) = . ;}
    ${RELOCATING+${OTHER_BSS_SYMBOLS}}
    *(.bss)
    ${RELOCATING+
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    ${RELOCATING+PROVIDE(CORE_SYM(__bss_end) = .);}
    }
  } ${RELOCATING+> BSS_MEM}

  ${RELOCATING+
  /*
   * Allocate space for stack and heap
   * we allocate this at the end and use all available memory
   * in the BSS_MEM for stack end heap
   */
  }
  CORE_SEC(.stack) ${RELOCATING-0 }: ${RELOCATING+ALIGN(8) FLAGS(aw)}
  {
    ${RELOCATING+
    . = ALIGN(8) ;
    CORE_SYM(__HEAP) = . ;
    end = . ;
    _end = . ;              /* sbrk will allocate from here */
                            /* the heap may run into the stack */
    . += __HEAP_SIZE ;
    CORE_SYM(__HEAP_END) = . ;
    CORE_SYM(_stack_end) = . ;
    . += __USTACK_SIZE ;

    /* use the rest of RAM for stack */
    . = (ORIGIN(BSS_MEM) + LENGTH(BSS_MEM) - 8) - ABSOLUTE(_end);

    CORE_SYM(__stack) = . ;
    CORE_SYM(_stack_addr) = . ;
    CORE_SYM(__USTACK) = . ;
    . += 8 ;
    }
  } ${RELOCATING+> BSS_MEM}

  ${RELOCATING+

  /* Make sure stack and heap addresses are properly aligned.  */
  _. = ASSERT ((CORE_SYM(__USTACK) & 7) == 0 , ${Q}USTACK not doubleword aligned${Q}) ;
  _. = ASSERT ((CORE_SYM(__HEAP_END) & 7) == 0 , ${Q}HEAP not doubleword aligned${Q}) ;

  /* Make sure enough memory is available for stacks and heap.  */
  _. = ASSERT ((CORE_SYM(__HEAP_END) - CORE_SYM(__HEAP)) >= __HEAP_MIN ,
               ${Q}not enough memory for HEAP${Q}) ;
  _. = ASSERT (((ORIGIN(BSS_MEM) + LENGTH(BSS_MEM)) >= CORE_SYM(_stack_addr)), ${Q}stack begins outside of available memory${Q}) ;

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
EOF

cat <<EOF
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }

  .comment       0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info${RELOCATING+ .gnu.linkonce.wi.*}) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

   /* DWARF 2 control flow extension */
  .debug_control_flow 0 : { *(.debug_control_flow) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }

  ${TINY_DATA_SECTION}
  ${TINY_BSS_SECTION}

  ${STACK_ADDR+${STACK}}
  ${ATTRS_SECTIONS}
  ${OTHER_SECTIONS}
  ${RELOCATING+${OTHER_SYMBOLS}}
  ${RELOCATING+${DISCARDED}}
}
EOF
