/* HTC XML support for mapfile and objectdump.
   Copyright (C) 1998-2011 Free Software Foundation, Inc.
   Contributed by Tamas Szirbucz (Tamas.Szirbucz@hightec-rt.com).

This file is part of the libiberty library.
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
#ifndef XML_DEFINES_H
#define XML_DEFINES_H

#define XML_V_OBJDUMP_XSD "objdump.xsd"

#define XML_E_ARCHIVE "Archive"
#define XML_A_NAME "name"

#define XML_E_FILE "File"

#define XML_E_OBJECT_DUMP "ObjectDump"
#define XML_A_XMLNS "xmlns"
#define XML_A_XMLNS_XSI "xmlns:xsi"
#define XML_A_XSI_SCHEMA_LOCATION "xsi:schemaLocation"
#define XML_V_HTC_OBJDUMP_NS "http://www.hightec-rt.com/objdump/"
#define XML_V_W3_NS "http://www.w3.org/2001/XMLSchema-instance"


#define XML_A_FORMAT "format"

#define XML_E_PROGRAM_HEADER_LIST "ProgramHeaderList"

#define XML_E_PROGRAM_HEADER "ProgramHeader"
#define XML_A_TYPE "type"
#define XML_A_OFFSET "offset"
#define XML_A_VIRTUAL_ADDRESS "virtual_address"
#define XML_A_PHYSICAL_ADDRESS "physical_address"
#define XML_A_ALIGN "align"
#define XML_A_FILE_SIZE "file_size"
#define XML_A_MEMORY_SIZE "memory_size"

#define XML_A_BITMASK "bitmask"

#define XML_E_VERSION_DEFINITION_LIST "VersionDefinitionList"
#define XML_E_VERSION_DEFINITION "VersionDefinition"
#define XML_A_INDEX "index"
#define XML_A_FLAGS "flags"
#define XML_A_HASH "hash"
#define XML_A_NODENAME "nodename"

#define XML_E_NODENAME "VDNodeName"

#define XML_E_VERSION_REFERENCE_LIST "VersionReferenceList"
#define XML_E_VERSION_REFERENCE "VersionReference"
#define XML_E_REQUIRED_FROM "RequiredFrom"
#define XML_A_FILENAME "filename"
#define XML_A_OTHER "other"

#define XML_E_ARCHIVE_HEADER "ArchiveHeader"
#define XML_A_MODE "mode"
#define XML_A_UID "uid"
#define XML_A_GID "gid"
#define XML_A_SIZE "size"
#define XML_A_TIME "time"

#define XML_E_FILE_HEADER "FileHeader"
#define XML_A_ARCHITECTURE "architecture"
#define XML_A_START_ADDRESS "start_address"

#define XML_E_SECTION_HEADER_LIST "SectionHeaderList"
#define XML_E_SECTION_HEADER "SectionHeader"
#define XML_A_VMA "VMA"
#define XML_A_LMA "LMA"
#define XML_A_FILE_OFFSET "file_offset"

#define XML_E_CORE "Core"

#define XML_E_INSN "Insn"
#define XML_A_BYTECODE "hex"
#define XML_A_ASSEMBLY "asm"

#define XML_E_CONTENT_OF_SECTION "ContentOfSection"
#define XML_E_CONTENT_LINE "ContentLine"
#define XML_E_HEX_DUMP "HexDump"
#define XML_E_TEXT "Text"

#define XML_E_DATA "Data"
#define XML_A_ADDRESS "addr"
#define XML_A_VALUE "value"

#define XML_E_RELOC "Reloc"

#define XML_E_LINE_NUMBER "LineNumber"
#define XML_A_LINE "line"
#define XML_A_SECTION_NAME "section_name"
#define XML_A_ADDEND "addend"

#define XML_E_RELOCS_OF_SECTION "RelocsOfSection"

#define XML_E_SECTION "Section"

#define XML_E_SYMBOL "Symbol"

#define XML_A_SIGN_OF_OFFSET "sign_of_offset"
#define XML_A_KIND "kind"
#define XML_V_FILE "file"
#define XML_V_FUNCTION "function"
#define XML_V_OBJECT "object"

#define XML_E_DISASSEMBLY "Disassembly"

#define XML_E_FUNCTION "Function"

#define XML_E_DYNAMIC_SYMBOL_TABLE "DynamicSymbolTable"
#define XML_E_SYMBOL_TABLE "SymbolTable"
#define XML_A_VERSION "version"

#define XML_E_OTHER_FLAG "OtherFlag"
#define XML_V_FLAG_INTERNAL "INTERNAL"
#define XML_V_FLAG_HIDDEN "HIDDEN"
#define XML_V_FLAG_PROTECTED "PROTECTED"
#define XML_A_VISIBILITY "visibility"
#define XML_V_VISIBLE "visible"
#define XML_V_HIDDEN "hidden"

#define XML_A_SECTION "section"

#define XML_E_EXPORTED_SYMBOLS "ExportedSymbols"

#define XML_E_EXPORTED_SYMBOL "ExportedSymbol"
#define XML_A_EXPORTED_NAME "exported_name"

#define XML_E_DWARF_SECTION "DwarfSection"

#define XML_E_DWARF_ABBREV "DwarfAbbrev"
#define XML_E_DWARF_ARANGES "DwarfAranges"
#define XML_E_DWARF_FRAMES "DwarfFrames"
#define XML_E_DWARF_INFO "DwarfInfo"
#define XML_E_DWARF_LINES "DwarfLines"
#define XML_E_DWARF_MACINFO "DwarfMacinfo"
#define XML_E_DWARF_STR "DwarfStr"
#define XML_E_DWARF_LOC "DwarfLoc"
#define XML_E_DWARF_RANGES "DwarfRanges"

#define XML_E_ABBREV "AbbreviationTable"
#define XML_A_NUMBER "number"
#define XML_A_HAS_CHILDREN "has_children"
#define XML_A_FORM "form"

#define XML_E_ARANGE "Arange"
#define XML_A_OFFSET_INTO_DEBUG_INFO "offset_into_debug_info"
#define XML_A_POINTER_SIZE "pointer_size"
#define XML_A_SEGMENT_SIZE "segment_size"

#define XML_E_COMPILATION_UNIT_INFO "CompilationUnitInfo"
#define XML_E_COMPILATION_UNIT_ARANGE "CompilationUnitArange"
#define XML_E_COMPILATION_UNIT_LINE "CompilationUnitLine"

#define XML_E_DWARF_OP "DwarfOP"

#define XML_E_ZERO_TERMINATOR "ZeroTerminator"

#define XML_E_COMMON_INFORMATION_ENTRY "CommonInformationEntry"
#define XML_E_FRAME_DESCRIPTOR "FrameDescriptor"
#define XML_A_CIE_ID "cie_id"
#define XML_A_AUGMENTATION "augmentation"
#define XML_A_CODE_ALIGNMENT_FACTOR "code_alignment_factor"
#define XML_A_DATA_ALIGNMENT_FACTOR "data_alignment_factor"
#define XML_A_RETURN_ADDRESS_COLUMN "return_address_column"
#define XML_A_AUGMENTATION_DATA "augmentation_data"
#define XML_A_CIE "cie"
#define XML_A_PC_BEGIN "pc_begin"
#define XML_A_PC_END "pc_end"


#define XML_E_INSTRUCTION "Instruction"
#define XML_A_OPERATOR "operator"
#define XML_A_OPERAND1 "operand1"
#define XML_A_OPERAND2 "operand2"
#define XML_A_OPCODE "opcode"
#define XML_A_NEW_ADDRESS "new_address"

#define XML_E_TAG "Tag"
#define XML_A_LEVEL "level"
#define XML_A_ABBREV_NUMBER "abbrev_number"
#define XML_A_ABBREV_OFFSET "abbrev_offset"

#define XML_A_FORM_NAME "form_name"
#define XML_A_REFERENCE "reference"
#define XML_A_INDIRECT_STRING_OFFSET "indirect_string_offset"
#define XML_A_TAG_NAME "tag_name"

#define XML_E_EXPRESSION "Expression"
#define XML_A_WITH_FRAME_BASE "with_frame_base"

#define XML_A_DATA "data"

#define XML_E_BYTE_BLOCK "ByteBlock"

#define XML_E_LINE "Line"
#define XML_A_DIRNAME "dirname"
#define XML_A_LINE_NUMBER "line_number"

#define XML_E_START_FILE "StartFile"
#define XML_A_FILE_NUMBER "file_number"

#define XML_E_END_FILE "EndFile"

#define XML_E_MACRO_DEFINE "MacroDefine"
#define XML_A_MACRO "macro"

#define XML_E_MACRO_UNDEF "MacroUndef"

#define XML_E_VENDOR_EXT "VendorExt"
#define XML_A_CONSTANT "constant"
#define XML_A_STRING "string"

#define XML_E_LOCATION "Location"
#define XML_A_BEGIN "begin"

#define XML_E_RANGE "Range"

#define XML_A_LENGTH "length"
#define XML_A_END "end"

#define XML_E_ATTRIBUTE "Attribute"

#define XML_E_ABBREV_ATTRIBUTE "AbbrevAttribute"

#define XML_A_SCOPE "scope"
#define XML_V_GLOBAL "global"
#define XML_V_LOCAL "local"


#define XML_V_MAP_XSD "map.xsd"
#define XML_V_HTC_MAP_NS "http://www.hightec-rt.com/map/"

#define XML_E_MAP "Map"
#define XML_A_TARGET "target"

#define XML_E_ARCHIVE_LIST "ArchiveList"

#define XML_E_CORE_ID "CoreID"

#define XML_E_OBJECT "Object"
#define XML_A_NAME "name"

#define XML_E_LIBRARY "Library"

#define XML_E_CALL "Call"
#define XML_A_CALLER_LIB "caller_lib"
#define XML_A_CALLER_OBJECT "caller_object"
#define XML_A_CALLED_FUNCTION "called_function"

#define XML_E_DISCARDED_INPUT_SECTIONS "DiscardedInputSections"

#define XML_E_LINKER_INFORMATION "LinkerInformation"
#define XML_A_NAME_OF_LINKER "name_of_linker"
#define XML_A_DATE_OF_RUN "date_of_run"


#define XML_E_LINKER_VERSION "LinkerVersion"
#define XML_A_VERSION_STRING "version_string"
#define XML_A_ARCHITECTURE_NAME "architecture_name"
#define XML_A_BUILD_STRING "build_string"
#define XML_A_TOOL_VERSION "tool_version"

#define XML_E_SYMBOL_LIST "SymbolList"

#define XML_A_START "start"
#define XML_A_MEMORY "memory"
#define XML_A_OUTPUT_SECTION "output_section"
#define XML_A_INPUT_SECTION "input_section"
#define XML_A_INPUT_OBJECT "input_object"

#define XML_E_MEMORY_CONFIGURATION "MemoryConfiguration"

#define XML_E_MEMORY_REGION "MemoryRegion"
#define XML_A_ORIGIN "origin"
#define XML_A_USED "used"
#define XML_A_FREE "free"

#define XML_E_FLAGS "Flags"

#define XML_E_FLAG "Flag"
#define XML_V_ALLOC "ALLOC"
#define XML_V_CODE "CODE"
#define XML_V_READONLY "READONLY"
#define XML_V_DATA "DATA"
#define XML_V_LOAD "LOAD"
#define XML_V_PCP "PCP"

#define XML_E_MEMORY_ALIAS_LIST "MemoryAliasList"

#define XML_E_ALIASES "Aliases"

#define XML_E_ALIAS "Alias"

#define XML_E_MEMORY_REGION_MAPS "MemoryRegionMaps"

#define XML_E_REGION_MAP "RegionMap"
#define XML_A_CORE "core"
#define XML_A_LOCAL_REGION "local_region"
#define XML_A_GLOBAL_REGION "global_region"

#define XML_E_SECTION_LIST "SectionList"

#define XML_E_OUTPUT "Output"

#define XML_E_CONSTRUCTOR_LIST "ConstructorList"
#define XML_A_SORTED "sorted"
#define XML_V_TRUE "true"
#define XML_V_FALSE "false"

#define XML_E_CREATE_OBJECT_SYMBOLS "CreateObjectSymbols"

#define XML_E_TARGET "Target"

#define XML_E_INSERT "Insert"
#define XML_A_IS_BEFORE "is_before"
#define XML_A_WHERE "where"

#define XML_E_GROUP "Group"

#define XML_E_ADDRESS "Address"

#define XML_E_FILL "Fill"
#define XML_A_MASK "mask"

#define XML_E_OUTPUT_SECTION "OutputSection"
#define XML_A_MEMORY_REGION "memory_region"
#define XML_A_LOAD_ADDRESS "load_address"

#define XML_E_WILDCARD "Wildcard"

#define XML_E_PATTERN "Pattern"
#define XML_E_EXCLUDE_FILES "ExcludeFiles"

#define XML_E_ASSIGNMENT "Assignment"
#define XML_A_EXPRESSION "expression"
#define XML_V_UNRESOLVED "*unresolved*"
#define XML_V_UNDEFINED "*undefined*"

#define XML_E_LOADS "Loads"

#define XML_A_RELOC_NAME "reloc_name"

#define XML_E_PADDING "Padding"
#define XML_A_FILL "fill"

#define XML_E_CROSS_REFERENCE_TABLE "CrossReferenceTable"
#define XML_E_CROSS_REFERENCE "CrossReference"
#define XML_A_SYMBOL "symbol"
#define XML_A_ARCHIVE "archive"

#define XML_A_EXPORT_NAME "export_name"

#define XML_E_SECTION_SYMBOL "SectionSymbol"

#endif /* XML_DEFINES_H */
#endif /* HTC_SUPPORT */
