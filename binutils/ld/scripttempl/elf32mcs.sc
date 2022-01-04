# Linker Script for RB GTM MCS.

# The next line should be uncommented if it is desired to link
# without libstart.o and directly enter main.

# ENTRY=_main

test -z "$ENTRY" && ENTRY=_start
cat <<EOF

/* Example Linker Script for linking NS CRX elf32 files. */

OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
${RELOCATING+ENTRY(${ENTRY})}

/* Define memory regions.  */
MEMORY
{
        ram         : ORIGIN = 0x0,         LENGTH = 0x4000
}

SECTIONS
{
  .init :
  { 
    __INIT_START = .; 
    KEEP (*(.init))
    __INIT_END = .; 
  } > ram

  .fini :
  { 
    __FINI_START = .; 
    KEEP (*(.fini))
    __FINI_END = .; 
  } > ram

  .text : 
  {
    __TEXT_START = .;
    *(.text) *(.text.*) 
    __TEXT_END = .;
  } > ram

  .rodata :
  {
    __RDATA_START = .;
    *(.rodata_4) *(.rodata_2) *(.rodata_1) *(.rodata.*)
    __RDATA_END = .;
  } > ram

  .data :
  {
    __DATA_START = .;
    *(.data_4) *(.data_2) *(.data_1) *(.data) *(.data.*) 
    __DATA_END = .;
  } > ram 

  .bss (NOLOAD) :
  {
    __BSS_START = .;
    *(.bss_4) *(.bss_2) *(.bss_1) *(.bss) *(COMMON) *(.bss.*) 
    __BSS_END = .;
  } > ram

/* You may change the sizes of the following sections to fit the actual
   size your program requires.

  .stack (NOLOAD) :
  {
    . = ALIGN(4);
    . += 0x400;
    __STACK_START = .;
  } > ram

  .comment        0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  ${OTHER_SECTIONS}
}

EOF
