/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     NAME = 259,
     LNAME = 260,
     FLAGS = 261,
     EXPORT_K = 262,
     FUNCTION_K = 263,
     OBJECT_K = 264,
     CORE_K = 265,
     COREID_K = 266,
     CORESYM_K = 267,
     COREALIAS_K = 268,
     REGION_MAP_K = 269,
     SDA_SECTION_K = 270,
     AS_K = 271,
     OREQ = 272,
     ANDEQ = 273,
     RSHIFTEQ = 274,
     LSHIFTEQ = 275,
     DIVEQ = 276,
     MULTEQ = 277,
     MINUSEQ = 278,
     PLUSEQ = 279,
     OROR = 280,
     ANDAND = 281,
     NE = 282,
     EQ = 283,
     GE = 284,
     LE = 285,
     RSHIFT = 286,
     LSHIFT = 287,
     UNARY = 288,
     END = 289,
     ALIGN_K = 290,
     BLOCK = 291,
     BIND = 292,
     QUAD = 293,
     SQUAD = 294,
     LONG = 295,
     SHORT = 296,
     BYTE = 297,
     SECTIONS = 298,
     PHDRS = 299,
     INSERT_K = 300,
     AFTER = 301,
     BEFORE = 302,
     DATA_SEGMENT_ALIGN = 303,
     DATA_SEGMENT_RELRO_END = 304,
     DATA_SEGMENT_END = 305,
     SORT_BY_NAME = 306,
     SORT_BY_ALIGNMENT = 307,
     SIZEOF_HEADERS = 308,
     OUTPUT_FORMAT = 309,
     FORCE_COMMON_ALLOCATION = 310,
     OUTPUT_ARCH = 311,
     INHIBIT_COMMON_ALLOCATION = 312,
     SEGMENT_START = 313,
     INCLUDE = 314,
     MEMORY = 315,
     REGION_ALIAS = 316,
     NOLOAD = 317,
     DSECT = 318,
     COPY = 319,
     INFO = 320,
     OVERLAY = 321,
     DEFINED = 322,
     TARGET_K = 323,
     SEARCH_DIR = 324,
     MAP = 325,
     ENTRY = 326,
     NEXT = 327,
     SIZEOF = 328,
     ALIGNOF = 329,
     ADDR = 330,
     LOADADDR = 331,
     MAX_K = 332,
     MIN_K = 333,
     STARTUP = 334,
     HLL = 335,
     SYSLIB = 336,
     FLOAT = 337,
     NOFLOAT = 338,
     NOCROSSREFS = 339,
     ORIGIN = 340,
     FILL = 341,
     LENGTH = 342,
     CREATE_OBJECT_SYMBOLS = 343,
     INPUT = 344,
     GROUP = 345,
     OUTPUT = 346,
     CONSTRUCTORS = 347,
     ALIGNMOD = 348,
     AT = 349,
     SUBALIGN = 350,
     PROVIDE = 351,
     PROVIDE_HIDDEN = 352,
     AS_NEEDED = 353,
     REGION_MIRROR = 354,
     CHIP = 355,
     LIST = 356,
     SECT = 357,
     ABSOLUTE = 358,
     LOAD = 359,
     NEWLINE = 360,
     ENDWORD = 361,
     ORDER = 362,
     NAMEWORD = 363,
     ASSERT_K = 364,
     FORMAT = 365,
     PUBLIC = 366,
     DEFSYMEND = 367,
     BASE = 368,
     ALIAS = 369,
     TRUNCATE = 370,
     REL = 371,
     INPUT_SCRIPT = 372,
     INPUT_MRI_SCRIPT = 373,
     INPUT_DEFSYM = 374,
     CASE = 375,
     EXTERN = 376,
     START = 377,
     VERS_TAG = 378,
     VERS_IDENTIFIER = 379,
     GLOBAL = 380,
     LOCAL = 381,
     VERSIONK = 382,
     INPUT_VERSION_SCRIPT = 383,
     KEEP = 384,
     ONLY_IF_RO = 385,
     ONLY_IF_RW = 386,
     SPECIAL = 387,
     EXCLUDE_FILE = 388,
     CONSTANT = 389,
     INPUT_DYNAMIC_LIST = 390
   };
#endif
/* Tokens.  */
#define INT 258
#define NAME 259
#define LNAME 260
#define FLAGS 261
#define EXPORT_K 262
#define FUNCTION_K 263
#define OBJECT_K 264
#define CORE_K 265
#define COREID_K 266
#define CORESYM_K 267
#define COREALIAS_K 268
#define REGION_MAP_K 269
#define SDA_SECTION_K 270
#define AS_K 271
#define OREQ 272
#define ANDEQ 273
#define RSHIFTEQ 274
#define LSHIFTEQ 275
#define DIVEQ 276
#define MULTEQ 277
#define MINUSEQ 278
#define PLUSEQ 279
#define OROR 280
#define ANDAND 281
#define NE 282
#define EQ 283
#define GE 284
#define LE 285
#define RSHIFT 286
#define LSHIFT 287
#define UNARY 288
#define END 289
#define ALIGN_K 290
#define BLOCK 291
#define BIND 292
#define QUAD 293
#define SQUAD 294
#define LONG 295
#define SHORT 296
#define BYTE 297
#define SECTIONS 298
#define PHDRS 299
#define INSERT_K 300
#define AFTER 301
#define BEFORE 302
#define DATA_SEGMENT_ALIGN 303
#define DATA_SEGMENT_RELRO_END 304
#define DATA_SEGMENT_END 305
#define SORT_BY_NAME 306
#define SORT_BY_ALIGNMENT 307
#define SIZEOF_HEADERS 308
#define OUTPUT_FORMAT 309
#define FORCE_COMMON_ALLOCATION 310
#define OUTPUT_ARCH 311
#define INHIBIT_COMMON_ALLOCATION 312
#define SEGMENT_START 313
#define INCLUDE 314
#define MEMORY 315
#define REGION_ALIAS 316
#define NOLOAD 317
#define DSECT 318
#define COPY 319
#define INFO 320
#define OVERLAY 321
#define DEFINED 322
#define TARGET_K 323
#define SEARCH_DIR 324
#define MAP 325
#define ENTRY 326
#define NEXT 327
#define SIZEOF 328
#define ALIGNOF 329
#define ADDR 330
#define LOADADDR 331
#define MAX_K 332
#define MIN_K 333
#define STARTUP 334
#define HLL 335
#define SYSLIB 336
#define FLOAT 337
#define NOFLOAT 338
#define NOCROSSREFS 339
#define ORIGIN 340
#define FILL 341
#define LENGTH 342
#define CREATE_OBJECT_SYMBOLS 343
#define INPUT 344
#define GROUP 345
#define OUTPUT 346
#define CONSTRUCTORS 347
#define ALIGNMOD 348
#define AT 349
#define SUBALIGN 350
#define PROVIDE 351
#define PROVIDE_HIDDEN 352
#define AS_NEEDED 353
#define REGION_MIRROR 354
#define CHIP 355
#define LIST 356
#define SECT 357
#define ABSOLUTE 358
#define LOAD 359
#define NEWLINE 360
#define ENDWORD 361
#define ORDER 362
#define NAMEWORD 363
#define ASSERT_K 364
#define FORMAT 365
#define PUBLIC 366
#define DEFSYMEND 367
#define BASE 368
#define ALIAS 369
#define TRUNCATE 370
#define REL 371
#define INPUT_SCRIPT 372
#define INPUT_MRI_SCRIPT 373
#define INPUT_DEFSYM 374
#define CASE 375
#define EXTERN 376
#define START 377
#define VERS_TAG 378
#define VERS_IDENTIFIER 379
#define GLOBAL 380
#define LOCAL 381
#define VERSIONK 382
#define INPUT_VERSION_SCRIPT 383
#define KEEP 384
#define ONLY_IF_RO 385
#define ONLY_IF_RW 386
#define SPECIAL 387
#define EXCLUDE_FILE 388
#define CONSTANT 389
#define INPUT_DYNAMIC_LIST 390




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 64 "ldgram.y"

  bfd_vma integer;
  struct big_int
    {
      bfd_vma integer;
      char *str;
    } bigint;
  fill_type *fill;
  char *name;
  const char *cname;
  struct wildcard_spec wildcard;
  struct wildcard_list *wildcard_list;
  struct name_list *name_list;
  int token;
  union etree_union *etree;
  struct phdr_info
    {
      bfd_boolean filehdr;
      bfd_boolean phdrs;
      union etree_union *at;
      union etree_union *flags;
    } phdr;
  struct lang_nocrossref *nocrossref;
  struct lang_output_section_phdr_list *section_phdr;
  struct bfd_elf_version_deps *deflist;
  struct bfd_elf_version_expr *versyms;
  struct bfd_elf_version_tree *versnode;



/* Line 2068 of yacc.c  */
#line 351 "/home/horst/work/hightec-development-platform/toolchain/binutils-2.20/ld/ldgram.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


