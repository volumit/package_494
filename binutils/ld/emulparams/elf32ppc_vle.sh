# If you change this file, please also look at files which source this one:
# elf32lppcnto.sh elf32lppc.sh elf32ppclinux.sh elf32ppcnto.sh 
# elf32ppcsim.sh

. ${srcdir}/emulparams/elf32ppccommon.sh
# Yes, we want duplicate .got and .plt sections.  The linker chooses the
# appropriate one magically in ppc_after_open
SCRIPT_NAME=elf32ppc_vle
# TEMPLATE_NAME=ppc32elf
DATA_GOT=
SDATA_GOT=
SEPARATE_GOTPLT=0
BSS_PLT=
GOT=".got          ${RELOCATING-0} : SPECIAL { *(.got) }"
GOTPLT=".plt          ${RELOCATING-0} : SPECIAL { *(.plt) }"
PLT=".plt          ${RELOCATING-0} : SPECIAL { *(.plt) }
  .iplt         ${RELOCATING-0} : { *(.iplt) }"
OTHER_TEXT_SECTIONS="*(.glink)"
EXTRA_EM_FILE=ppc32elf-vle
if grep -q 'ld_elf32_spu_emulation' ldemul-list.h; then
# crt1.o defines data_start and __data_start.  Keep them first.
# Next put all the .data.spehandle sections, with a trailing zero word.
  DATA_START_SYMBOLS="${RELOCATING+*crt1.o(.data .data.* .gnu.linkonce.d.*)
    PROVIDE (__spe_handle = .);
    *(.data.spehandle)
    . += 4 * (DEFINED (__spe_handle) || . != 0);}"
fi
OTHER_SECTIONS=".version_info    0 : { *(.version_info) }"
