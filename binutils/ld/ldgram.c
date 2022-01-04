/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 24 "ldgram.y"

/*

 */

#define DONTDECLARE_MALLOC

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "ld.h"
#include "ldexp.h"
#include "ldver.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"
#include "ldmisc.h"
#include "ldmain.h"
#include "mri.h"
#include "ldctor.h"
#include "ldlex.h"
#include "libiberty.h"

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

static enum section_type sectype;
static lang_memory_region_type *region;
static flagword os_flags;

bfd_boolean ldgram_had_keep = FALSE;
char *ldgram_vers_current_lang = NULL;

#define ERROR_NAME_MAX 20
static char *error_names[ERROR_NAME_MAX];
static int error_index;
#define PUSH_ERROR(x) if (error_index < ERROR_NAME_MAX) error_names[error_index] = x; error_index++;
#define POP_ERROR()   error_index--;


/* Line 268 of yacc.c  */
#line 113 "/home/horst/work/hightec-development-platform-336/toolchain/binutils-2.20/ld/ldgram.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
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



/* Line 293 of yacc.c  */
#line 450 "/home/horst/work/hightec-development-platform-336/toolchain/binutils-2.20/ld/ldgram.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 462 "/home/horst/work/hightec-development-platform-336/toolchain/binutils-2.20/ld/ldgram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2067

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  159
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  132
/* YYNRULES -- Number of rules.  */
#define YYNRULES  377
/* YYNRULES -- Number of states.  */
#define YYNSTATES  830

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   390

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   157,     2,     2,     2,    45,    32,     2,
      48,   152,    43,    41,   153,    42,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    27,   154,
      35,    17,    36,    26,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   155,     2,   156,    31,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,    30,    68,   158,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    18,    19,    20,    21,    22,    23,    24,    25,
      28,    29,    33,    34,    37,    38,    39,    40,    46,    47,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    20,    25,
      27,    28,    33,    34,    37,    41,    42,    45,    50,    52,
      54,    57,    59,    64,    69,    73,    76,    81,    85,    90,
      95,   100,   105,   110,   113,   116,   119,   124,   129,   132,
     135,   138,   141,   142,   148,   151,   152,   156,   159,   160,
     162,   166,   168,   172,   173,   175,   179,   180,   183,   185,
     188,   192,   193,   196,   199,   200,   202,   204,   206,   208,
     210,   212,   214,   216,   218,   220,   225,   230,   235,   240,
     249,   254,   256,   258,   263,   264,   270,   275,   276,   282,
     287,   292,   296,   300,   304,   310,   314,   320,   327,   338,
     342,   349,   350,   356,   358,   362,   364,   368,   371,   373,
     377,   380,   381,   387,   388,   396,   397,   404,   409,   412,
     415,   418,   419,   424,   427,   428,   436,   445,   447,   449,
     451,   453,   455,   461,   466,   471,   479,   487,   495,   503,
     512,   515,   517,   521,   523,   525,   529,   534,   536,   537,
     543,   546,   548,   550,   552,   557,   559,   564,   569,   570,
     579,   580,   586,   589,   591,   592,   594,   596,   598,   600,
     602,   604,   606,   609,   610,   612,   614,   616,   618,   620,
     622,   624,   626,   628,   630,   634,   638,   645,   652,   654,
     655,   660,   662,   663,   667,   669,   670,   678,   679,   685,
     689,   693,   694,   698,   700,   703,   705,   708,   713,   718,
     722,   726,   728,   733,   737,   738,   740,   742,   743,   746,
     750,   751,   754,   757,   761,   766,   769,   772,   775,   779,
     783,   787,   791,   795,   799,   803,   807,   811,   815,   819,
     823,   827,   831,   835,   839,   845,   849,   853,   858,   860,
     862,   867,   872,   877,   882,   887,   892,   897,   904,   911,
     918,   923,   930,   935,   937,   944,   951,   958,   963,   968,
     972,   973,   978,   979,   984,   985,   990,   991,   993,   995,
     997,   998,  1003,  1004,  1009,  1011,  1012,  1013,  1014,  1015,
    1016,  1037,  1038,  1039,  1040,  1041,  1042,  1061,  1062,  1063,
    1071,  1072,  1078,  1080,  1082,  1084,  1086,  1088,  1092,  1093,
    1096,  1100,  1103,  1110,  1121,  1124,  1126,  1127,  1129,  1132,
    1133,  1134,  1138,  1139,  1140,  1141,  1142,  1154,  1159,  1160,
    1163,  1164,  1165,  1172,  1174,  1175,  1179,  1185,  1191,  1192,
    1196,  1197,  1200,  1202,  1205,  1210,  1213,  1214,  1217,  1218,
    1224,  1226,  1229,  1234,  1240,  1247,  1249,  1252,  1253,  1256,
    1261,  1266,  1275,  1277,  1279,  1283,  1287,  1288,  1298,  1299,
    1307,  1309,  1313,  1315,  1319,  1321,  1325,  1326
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     160,     0,    -1,   133,   177,    -1,   134,   165,    -1,   144,
     279,    -1,   151,   274,    -1,   135,   163,    -1,     4,    -1,
      12,    48,     4,   152,    -1,     4,    -1,    -1,   164,   162,
      17,   233,    -1,    -1,   166,   167,    -1,   167,   168,   121,
      -1,    -1,   116,   233,    -1,   116,   233,   153,   233,    -1,
       4,    -1,   117,    -1,   123,   170,    -1,   122,    -1,   127,
       4,    17,   233,    -1,   127,     4,   153,   233,    -1,   127,
       4,   233,    -1,   126,     4,    -1,   118,     4,   153,   233,
      -1,   118,     4,   233,    -1,   118,     4,    17,   233,    -1,
      49,     4,    17,   233,    -1,    49,     4,   153,   233,    -1,
     109,     4,    17,   233,    -1,   109,     4,   153,   233,    -1,
     119,   172,    -1,   120,   171,    -1,   124,     4,    -1,   130,
       4,   153,     4,    -1,   130,     4,   153,     3,    -1,   129,
     233,    -1,   131,     3,    -1,   136,   173,    -1,   137,   174,
      -1,    -1,    75,   161,   169,   167,    47,    -1,   138,     4,
      -1,    -1,   170,   153,     4,    -1,   170,     4,    -1,    -1,
       4,    -1,   171,   153,     4,    -1,     4,    -1,   172,   153,
       4,    -1,    -1,     4,    -1,   173,   153,     4,    -1,    -1,
     175,   176,    -1,     4,    -1,   176,     4,    -1,   176,   153,
       4,    -1,    -1,   178,   179,    -1,   179,   180,    -1,    -1,
     213,    -1,   189,    -1,   266,    -1,   224,    -1,   225,    -1,
     227,    -1,   229,    -1,   191,    -1,   281,    -1,   154,    -1,
      84,    48,     4,   152,    -1,    85,    48,   161,   152,    -1,
     107,    48,   161,   152,    -1,    70,    48,     4,   152,    -1,
      70,    48,     4,   153,     4,   153,     4,   152,    -1,    72,
      48,     4,   152,    -1,    71,    -1,    73,    -1,   105,    48,
     185,   152,    -1,    -1,   106,   181,    48,   185,   152,    -1,
      86,    48,   161,   152,    -1,    -1,    75,   161,   182,   179,
      47,    -1,   100,    48,   230,   152,    -1,   137,    48,   174,
     152,    -1,    59,    60,     4,    -1,    59,    61,     4,    -1,
       7,     9,     4,    -1,     7,     9,     4,    16,     4,    -1,
       7,     8,     4,    -1,     7,     8,     4,    16,   240,    -1,
      77,    48,     4,   153,     4,   152,    -1,    14,    48,     4,
     153,   231,   153,   231,   153,   231,   152,    -1,    11,    17,
       4,    -1,    13,    48,     4,    17,     4,   152,    -1,    -1,
     115,   183,    48,   184,   152,    -1,     4,    -1,   184,   212,
       4,    -1,     4,    -1,   185,   153,     4,    -1,   185,     4,
      -1,     5,    -1,   185,   153,     5,    -1,   185,     5,    -1,
      -1,   114,    48,   186,   185,   152,    -1,    -1,   185,   153,
     114,    48,   187,   185,   152,    -1,    -1,   185,   114,    48,
     188,   185,   152,    -1,    57,    67,   190,    68,    -1,   190,
     241,    -1,   190,   191,    -1,   190,   193,    -1,    -1,    87,
      48,     4,   152,    -1,   211,   210,    -1,    -1,   125,   192,
      48,   233,   153,     4,   152,    -1,    15,    48,   240,   153,
     162,   153,     4,   152,    -1,     4,    -1,   240,    -1,    43,
      -1,    26,    -1,   194,    -1,   149,    48,   196,   152,   194,
      -1,    65,    48,   194,   152,    -1,    66,    48,   194,   152,
      -1,    65,    48,    66,    48,   194,   152,   152,    -1,    65,
      48,    65,    48,   194,   152,   152,    -1,    66,    48,    65,
      48,   194,   152,   152,    -1,    66,    48,    66,    48,   194,
     152,   152,    -1,    65,    48,   149,    48,   196,   152,   194,
     152,    -1,   196,   194,    -1,   194,    -1,   197,   212,   195,
      -1,   195,    -1,     4,    -1,   155,   197,   156,    -1,   195,
      48,   197,   152,    -1,   198,    -1,    -1,   145,    48,   200,
     198,   152,    -1,   211,   210,    -1,   104,    -1,   154,    -1,
     108,    -1,    65,    48,   108,   152,    -1,   199,    -1,   206,
      48,   231,   152,    -1,   102,    48,   207,   152,    -1,    -1,
     125,   202,    48,   233,   153,     4,   152,   210,    -1,    -1,
      75,   161,   203,   205,    47,    -1,   204,   201,    -1,   201,
      -1,    -1,   204,    -1,    52,    -1,    53,    -1,    54,    -1,
      55,    -1,    56,    -1,   231,    -1,    17,   207,    -1,    -1,
      25,    -1,    24,    -1,    23,    -1,    22,    -1,    21,    -1,
      20,    -1,    19,    -1,    18,    -1,   154,    -1,   153,    -1,
     162,    17,   231,    -1,   162,   209,   231,    -1,   112,    48,
     162,    17,   231,   152,    -1,   113,    48,   162,    17,   231,
     152,    -1,   153,    -1,    -1,    76,    67,   214,    68,    -1,
     215,    -1,    -1,   215,   212,   216,    -1,   216,    -1,    -1,
       4,   217,   221,    27,   219,   212,   220,    -1,    -1,    75,
     161,   218,   214,    47,    -1,   101,    17,   231,    -1,   103,
      17,   231,    -1,    -1,    48,   222,   152,    -1,   223,    -1,
     222,   223,    -1,     4,    -1,   157,     4,    -1,    95,    48,
     161,   152,    -1,    96,    48,   226,   152,    -1,    96,    48,
     152,    -1,   226,   212,   161,    -1,   161,    -1,    97,    48,
     228,   152,    -1,   228,   212,   161,    -1,    -1,    98,    -1,
      99,    -1,    -1,     4,   230,    -1,     4,   153,   230,    -1,
      -1,   232,   233,    -1,    42,   233,    -1,    48,   233,   152,
      -1,    88,    48,   233,   152,    -1,   157,   233,    -1,    41,
     233,    -1,   158,   233,    -1,   233,    43,   233,    -1,   233,
      44,   233,    -1,   233,    45,   233,    -1,   233,    41,   233,
      -1,   233,    42,   233,    -1,   233,    40,   233,    -1,   233,
      39,   233,    -1,   233,    34,   233,    -1,   233,    33,   233,
      -1,   233,    38,   233,    -1,   233,    37,   233,    -1,   233,
      35,   233,    -1,   233,    36,   233,    -1,   233,    32,   233,
      -1,   233,    31,   233,    -1,   233,    30,   233,    -1,   233,
      26,   233,    27,   233,    -1,   233,    29,   233,    -1,   233,
      28,   233,    -1,    83,    48,   162,   152,    -1,     3,    -1,
      69,    -1,    90,    48,   240,   152,    -1,    89,    48,   240,
     152,    -1,    91,    48,   240,   152,    -1,    92,    48,   240,
     152,    -1,   150,    48,     4,   152,    -1,   119,    48,   233,
     152,    -1,    49,    48,   233,   152,    -1,    49,    48,   233,
     153,   233,   152,    -1,    62,    48,   233,   153,   233,   152,
      -1,    63,    48,   233,   153,   233,   152,    -1,    64,    48,
     233,   152,    -1,    74,    48,     4,   153,   233,   152,    -1,
      50,    48,   233,   152,    -1,   162,    -1,    93,    48,   233,
     153,   233,   152,    -1,    94,    48,   233,   153,   233,   152,
      -1,   125,    48,   233,   153,     4,   152,    -1,   101,    48,
       4,   152,    -1,   103,    48,     4,   152,    -1,   110,    36,
     162,    -1,    -1,   110,    48,   233,   152,    -1,    -1,    49,
      48,   233,   152,    -1,    -1,   111,    48,   233,   152,    -1,
      -1,   146,    -1,   147,    -1,   148,    -1,    -1,     6,    48,
       4,   152,    -1,    -1,    10,    48,     4,   152,    -1,     4,
      -1,    -1,    -1,    -1,    -1,    -1,   240,   242,   257,   235,
     236,   237,   243,   238,   239,    67,   244,   205,    68,   245,
     260,   234,   261,   208,   246,   212,    -1,    -1,    -1,    -1,
      -1,    -1,    82,   247,   258,   259,   235,   237,   248,    67,
     249,   262,    68,   250,   260,   234,   261,   208,   251,   212,
      -1,    -1,    -1,   106,   252,   257,   253,    67,   190,    68,
      -1,    -1,    75,   161,   254,   190,    47,    -1,    78,    -1,
      79,    -1,    80,    -1,    81,    -1,    82,    -1,    48,   255,
     152,    -1,    -1,    48,   152,    -1,   233,   256,    27,    -1,
     256,    27,    -1,    51,    48,   233,   152,   256,    27,    -1,
      51,    48,   233,   152,    50,    48,   233,   152,   256,    27,
      -1,   233,    27,    -1,    27,    -1,    -1,   100,    -1,    36,
     162,    -1,    -1,    -1,   261,    27,     4,    -1,    -1,    -1,
      -1,    -1,   262,     4,   263,    67,   205,    68,   264,   261,
     208,   265,   212,    -1,    58,    67,   267,    68,    -1,    -1,
     267,   268,    -1,    -1,    -1,     4,   269,   271,   272,   270,
     154,    -1,   233,    -1,    -1,     4,   273,   272,    -1,     6,
      48,   233,   152,   272,    -1,   110,    48,   233,   152,   272,
      -1,    -1,    48,   233,   152,    -1,    -1,   275,   276,    -1,
     277,    -1,   276,   277,    -1,    67,   278,    68,   154,    -1,
     287,   154,    -1,    -1,   280,   283,    -1,    -1,   282,   143,
      67,   283,    68,    -1,   284,    -1,   283,   284,    -1,    67,
     286,    68,   154,    -1,   139,    67,   286,    68,   154,    -1,
     139,    67,   286,    68,   285,   154,    -1,   139,    -1,   285,
     139,    -1,    -1,   287,   154,    -1,   141,    27,   287,   154,
      -1,   142,    27,   287,   154,    -1,   141,    27,   287,   154,
     142,    27,   287,   154,    -1,   140,    -1,     4,    -1,   287,
     154,   140,    -1,   287,   154,     4,    -1,    -1,   287,   154,
     137,     4,    67,   288,   287,   290,    68,    -1,    -1,   137,
       4,    67,   289,   287,   290,    68,    -1,   141,    -1,   287,
     154,   141,    -1,   142,    -1,   287,   154,   142,    -1,   137,
      -1,   287,   154,   137,    -1,    -1,   154,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   179,   179,   180,   181,   182,   183,   187,   190,   202,
     207,   207,   217,   217,   230,   231,   235,   236,   237,   240,
     243,   244,   245,   247,   249,   251,   253,   255,   257,   259,
     261,   263,   265,   267,   268,   269,   271,   273,   275,   277,
     279,   280,   282,   281,   285,   287,   291,   292,   293,   297,
     299,   303,   305,   310,   311,   312,   317,   317,   322,   324,
     326,   331,   331,   337,   338,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   355,   357,   359,   362,
     364,   366,   368,   370,   372,   371,   375,   378,   377,   381,
     385,   386,   388,   390,   394,   398,   402,   406,   408,   416,
     420,   425,   424,   431,   433,   436,   439,   442,   445,   448,
     451,   455,   454,   459,   458,   463,   462,   469,   473,   474,
     475,   476,   480,   482,   483,   483,   488,   497,   501,   506,
     510,   517,   523,   529,   535,   541,   547,   553,   559,   565,
     574,   583,   594,   603,   614,   622,   626,   633,   635,   634,
     641,   642,   646,   647,   652,   657,   658,   663,   667,   667,
     671,   670,   677,   678,   681,   683,   687,   689,   691,   693,
     695,   700,   707,   709,   713,   715,   717,   719,   721,   723,
     725,   727,   732,   732,   737,   741,   749,   753,   761,   761,
     765,   768,   768,   771,   772,   777,   776,   782,   781,   788,
     796,   804,   805,   809,   810,   814,   816,   821,   826,   827,
     832,   834,   840,   842,   844,   848,   850,   856,   859,   868,
     879,   879,   885,   887,   889,   891,   893,   895,   898,   900,
     902,   904,   906,   908,   910,   912,   914,   916,   918,   920,
     922,   924,   926,   928,   930,   932,   934,   936,   938,   940,
     943,   945,   947,   949,   951,   953,   955,   957,   959,   961,
     963,   965,   974,   976,   978,   980,   982,   984,   986,   992,
     993,   997,   998,  1002,  1003,  1007,  1008,  1012,  1013,  1014,
    1015,  1019,  1020,  1024,  1035,  1040,  1044,  1048,  1054,  1056,
    1040,  1063,  1065,  1067,  1072,  1074,  1062,  1084,  1086,  1084,
    1092,  1091,  1098,  1099,  1100,  1101,  1102,  1106,  1107,  1108,
    1112,  1113,  1118,  1119,  1124,  1125,  1130,  1131,  1136,  1138,
    1143,  1146,  1159,  1163,  1168,  1170,  1161,  1178,  1181,  1183,
    1187,  1188,  1187,  1197,  1242,  1245,  1257,  1263,  1272,  1275,
    1282,  1282,  1294,  1295,  1299,  1303,  1312,  1312,  1326,  1326,
    1336,  1337,  1341,  1345,  1349,  1356,  1360,  1368,  1371,  1375,
    1379,  1383,  1390,  1394,  1398,  1402,  1407,  1406,  1420,  1419,
    1429,  1433,  1437,  1441,  1445,  1449,  1455,  1457
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "NAME", "LNAME", "FLAGS",
  "EXPORT_K", "FUNCTION_K", "OBJECT_K", "CORE_K", "COREID_K", "CORESYM_K",
  "COREALIAS_K", "REGION_MAP_K", "SDA_SECTION_K", "AS_K", "'='", "OREQ",
  "ANDEQ", "RSHIFTEQ", "LSHIFTEQ", "DIVEQ", "MULTEQ", "MINUSEQ", "PLUSEQ",
  "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'", "NE", "EQ", "'<'",
  "'>'", "GE", "LE", "RSHIFT", "LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "UNARY", "END", "'('", "ALIGN_K", "BLOCK", "BIND", "QUAD", "SQUAD",
  "LONG", "SHORT", "BYTE", "SECTIONS", "PHDRS", "INSERT_K", "AFTER",
  "BEFORE", "DATA_SEGMENT_ALIGN", "DATA_SEGMENT_RELRO_END",
  "DATA_SEGMENT_END", "SORT_BY_NAME", "SORT_BY_ALIGNMENT", "'{'", "'}'",
  "SIZEOF_HEADERS", "OUTPUT_FORMAT", "FORCE_COMMON_ALLOCATION",
  "OUTPUT_ARCH", "INHIBIT_COMMON_ALLOCATION", "SEGMENT_START", "INCLUDE",
  "MEMORY", "REGION_ALIAS", "NOLOAD", "DSECT", "COPY", "INFO", "OVERLAY",
  "DEFINED", "TARGET_K", "SEARCH_DIR", "MAP", "ENTRY", "NEXT", "SIZEOF",
  "ALIGNOF", "ADDR", "LOADADDR", "MAX_K", "MIN_K", "STARTUP", "HLL",
  "SYSLIB", "FLOAT", "NOFLOAT", "NOCROSSREFS", "ORIGIN", "FILL", "LENGTH",
  "CREATE_OBJECT_SYMBOLS", "INPUT", "GROUP", "OUTPUT", "CONSTRUCTORS",
  "ALIGNMOD", "AT", "SUBALIGN", "PROVIDE", "PROVIDE_HIDDEN", "AS_NEEDED",
  "REGION_MIRROR", "CHIP", "LIST", "SECT", "ABSOLUTE", "LOAD", "NEWLINE",
  "ENDWORD", "ORDER", "NAMEWORD", "ASSERT_K", "FORMAT", "PUBLIC",
  "DEFSYMEND", "BASE", "ALIAS", "TRUNCATE", "REL", "INPUT_SCRIPT",
  "INPUT_MRI_SCRIPT", "INPUT_DEFSYM", "CASE", "EXTERN", "START",
  "VERS_TAG", "VERS_IDENTIFIER", "GLOBAL", "LOCAL", "VERSIONK",
  "INPUT_VERSION_SCRIPT", "KEEP", "ONLY_IF_RO", "ONLY_IF_RW", "SPECIAL",
  "EXCLUDE_FILE", "CONSTANT", "INPUT_DYNAMIC_LIST", "')'", "','", "';'",
  "'['", "']'", "'!'", "'~'", "$accept", "file", "filename", "sym_name",
  "defsym_expr", "$@1", "mri_script_file", "$@2", "mri_script_lines",
  "mri_script_command", "$@3", "ordernamelist", "mri_load_name_list",
  "mri_abs_name_list", "casesymlist", "extern_name_list", "$@4",
  "extern_name_list_body", "script_file", "$@5", "ifile_list", "ifile_p1",
  "$@6", "$@7", "$@8", "region_mirror_list", "input_list", "@9", "@10",
  "@11", "sections", "sec_or_group_p1", "statement_anywhere", "$@12",
  "statement_sda_section", "wildcard_name", "wildcard_spec",
  "exclude_name_list", "file_NAME_list", "input_section_spec_no_keep",
  "input_section_spec", "$@13", "statement", "$@14", "$@15",
  "statement_list", "statement_list_opt", "length", "fill_exp", "fill_opt",
  "assign_op", "end", "assignment", "opt_comma", "memory",
  "memory_spec_list_opt", "memory_spec_list", "memory_spec", "$@16",
  "$@17", "origin_spec", "length_spec", "attributes_opt",
  "attributes_list", "attributes_string", "startup", "high_level_library",
  "high_level_library_NAME_list", "low_level_library",
  "low_level_library_NAME_list", "floating_point_support",
  "nocrossref_list", "mustbe_exp", "$@18", "exp", "memspec_at_opt",
  "opt_at", "opt_align", "opt_subalign", "sect_constraint", "opt_flags",
  "sec_name", "section", "$@19", "$@20", "$@21", "$@22", "$@23", "$@24",
  "$@25", "$@26", "$@27", "$@28", "$@29", "$@30", "$@31", "type", "atype",
  "opt_exp_with_type", "opt_exp_without_type", "opt_nocrossrefs",
  "memspec_opt", "phdr_opt", "overlay_section", "$@32", "$@33", "$@34",
  "phdrs", "phdr_list", "phdr", "$@35", "$@36", "phdr_type",
  "phdr_qualifiers", "phdr_val", "dynamic_list_file", "$@37",
  "dynamic_list_nodes", "dynamic_list_node", "dynamic_list_tag",
  "version_script_file", "$@38", "version", "$@39", "vers_nodes",
  "vers_node", "verdep", "vers_tag", "vers_defns", "@40", "@41",
  "opt_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    61,   272,   273,
     274,   275,   276,   277,   278,   279,    63,    58,   280,   281,
     124,    94,    38,   282,   283,    60,    62,   284,   285,   286,
     287,    43,    45,    42,    47,    37,   288,   289,    40,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   123,   125,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,    41,    44,    59,    91,    93,    33,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   159,   160,   160,   160,   160,   160,   161,   162,   162,
     164,   163,   166,   165,   167,   167,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   169,   168,   168,   168,   170,   170,   170,   171,
     171,   172,   172,   173,   173,   173,   175,   174,   176,   176,
     176,   178,   177,   179,   179,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   181,   180,   180,   182,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   183,   180,   184,   184,   185,   185,   185,   185,   185,
     185,   186,   185,   187,   185,   188,   185,   189,   190,   190,
     190,   190,   191,   191,   192,   191,   193,   194,   194,   194,
     194,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     196,   196,   197,   197,   198,   198,   198,   199,   200,   199,
     201,   201,   201,   201,   201,   201,   201,   201,   202,   201,
     203,   201,   204,   204,   205,   205,   206,   206,   206,   206,
     206,   207,   208,   208,   209,   209,   209,   209,   209,   209,
     209,   209,   210,   210,   211,   211,   211,   211,   212,   212,
     213,   214,   214,   215,   215,   217,   216,   218,   216,   219,
     220,   221,   221,   222,   222,   223,   223,   224,   225,   225,
     226,   226,   227,   228,   228,   229,   229,   230,   230,   230,
     232,   231,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   234,
     234,   235,   235,   236,   236,   237,   237,   238,   238,   238,
     238,   239,   239,   240,   240,   242,   243,   244,   245,   246,
     241,   247,   248,   249,   250,   251,   241,   252,   253,   241,
     254,   241,   255,   255,   255,   255,   255,   256,   256,   256,
     257,   257,   257,   257,   258,   258,   259,   259,   260,   260,
     261,   261,   262,   263,   264,   265,   262,   266,   267,   267,
     269,   270,   268,   271,   272,   272,   272,   272,   273,   273,
     275,   274,   276,   276,   277,   278,   280,   279,   282,   281,
     283,   283,   284,   284,   284,   285,   285,   286,   286,   286,
     286,   286,   287,   287,   287,   287,   288,   287,   289,   287,
     287,   287,   287,   287,   287,   287,   290,   290
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     1,     4,     1,
       0,     4,     0,     2,     3,     0,     2,     4,     1,     1,
       2,     1,     4,     4,     3,     2,     4,     3,     4,     4,
       4,     4,     4,     2,     2,     2,     4,     4,     2,     2,
       2,     2,     0,     5,     2,     0,     3,     2,     0,     1,
       3,     1,     3,     0,     1,     3,     0,     2,     1,     2,
       3,     0,     2,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     4,     4,     8,
       4,     1,     1,     4,     0,     5,     4,     0,     5,     4,
       4,     3,     3,     3,     5,     3,     5,     6,    10,     3,
       6,     0,     5,     1,     3,     1,     3,     2,     1,     3,
       2,     0,     5,     0,     7,     0,     6,     4,     2,     2,
       2,     0,     4,     2,     0,     7,     8,     1,     1,     1,
       1,     1,     5,     4,     4,     7,     7,     7,     7,     8,
       2,     1,     3,     1,     1,     3,     4,     1,     0,     5,
       2,     1,     1,     1,     4,     1,     4,     4,     0,     8,
       0,     5,     2,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     6,     6,     1,     0,
       4,     1,     0,     3,     1,     0,     7,     0,     5,     3,
       3,     0,     3,     1,     2,     1,     2,     4,     4,     3,
       3,     1,     4,     3,     0,     1,     1,     0,     2,     3,
       0,     2,     2,     3,     4,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     5,     3,     3,     4,     1,     1,
       4,     4,     4,     4,     4,     4,     4,     6,     6,     6,
       4,     6,     4,     1,     6,     6,     6,     4,     4,     3,
       0,     4,     0,     4,     0,     4,     0,     1,     1,     1,
       0,     4,     0,     4,     1,     0,     0,     0,     0,     0,
      20,     0,     0,     0,     0,     0,    18,     0,     0,     7,
       0,     5,     1,     1,     1,     1,     1,     3,     0,     2,
       3,     2,     6,    10,     2,     1,     0,     1,     2,     0,
       0,     3,     0,     0,     0,     0,    11,     4,     0,     2,
       0,     0,     6,     1,     0,     3,     5,     5,     0,     3,
       0,     2,     1,     2,     4,     2,     0,     2,     0,     5,
       1,     2,     4,     5,     6,     1,     2,     0,     2,     4,
       4,     8,     1,     1,     3,     3,     0,     9,     0,     7,
       1,     3,     1,     3,     1,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,    61,    12,    10,   346,   340,     0,     2,    64,     3,
      15,     6,     0,     4,     0,     5,     0,     1,    62,    13,
       9,     0,     0,   357,     0,   347,   350,     0,   341,   342,
       0,     0,     0,     0,     0,     0,     0,     0,    81,     0,
      82,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   215,   216,     0,     0,    84,     0,     0,     0,   101,
     124,     0,    74,     0,    63,    66,    72,     0,    65,    68,
      69,    70,    71,    67,    73,     0,    18,     0,     0,     0,
       0,    19,     0,     0,     0,    21,    48,     0,     0,     0,
       0,     0,     0,    53,    56,     0,     0,     0,     0,   363,
     374,   362,   370,   372,     0,     0,   357,   351,   370,   372,
       0,     0,   343,     0,     0,     0,     0,     0,   121,   328,
       0,     0,     0,     0,     7,    87,   192,     0,     0,     0,
       0,     0,     0,     0,   214,   217,     0,     0,     0,     0,
       0,     0,     0,    56,   220,   181,   180,   179,   178,   177,
     176,   175,   174,   220,   183,   182,   123,     0,     0,    42,
       0,   248,     0,     0,     0,     0,     0,     0,     0,     0,
     249,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   263,    16,     0,
      51,    33,    49,    34,    20,    35,    25,     0,    38,     0,
      39,    54,    40,    41,     0,    44,    14,     0,    11,     0,
       0,     0,     0,   358,     0,     0,   345,    95,    93,    99,
       0,     0,     0,     0,    91,    92,     0,     0,    64,   195,
       0,     0,   189,   194,     0,     0,     0,     0,     0,     0,
     209,   211,   189,   189,   217,     0,   105,   108,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   184,     0,   185,
       0,     0,     0,    15,     0,     0,   226,   222,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   225,   227,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,     0,     0,    47,     0,     0,     0,    24,
       0,     0,    58,    57,     8,   368,     0,     0,   352,   365,
     375,   364,   371,   373,     0,   344,     0,     0,     0,   220,
     284,     0,     0,   117,     0,   291,   297,   119,   120,   285,
     118,   330,   327,   329,    78,     0,    80,   348,   201,   197,
     190,   188,     0,     0,    75,    76,    86,   122,   207,   208,
       0,   212,     0,   217,   218,    89,   111,   107,   110,     0,
      83,     0,     0,    77,   220,   220,   103,   189,     0,    90,
     221,     0,    29,    30,    45,    31,    32,   223,     0,     0,
       0,     0,     0,     0,     0,     0,   284,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   246,
     245,   243,   242,   241,   236,   235,   239,   240,   238,   237,
     234,   233,   231,   232,   228,   229,   230,    17,    28,    26,
      52,    50,    46,    22,    23,    37,    36,    55,    59,     0,
       0,   359,   360,     0,   355,   353,     0,    96,    94,     0,
       0,     0,     0,   300,     0,   308,   308,     0,     0,    88,
       0,     0,   192,   193,     0,   210,   213,   219,     0,   115,
     106,   109,     0,    85,     0,     0,   102,     0,     0,   349,
      43,   256,     0,   262,     0,     0,   260,     0,   247,   224,
     251,   250,   252,   253,     0,     0,   267,   268,   255,     0,
     254,     0,    60,   376,   373,   366,   356,   354,   100,   220,
       0,     0,   121,   315,     0,   316,     0,     0,   308,     0,
     298,   272,   333,   334,     0,   205,     0,     0,   203,     0,
       0,    97,     0,     0,   113,   186,   187,   104,     0,     0,
       0,     0,     0,     0,     0,     0,   244,   377,     0,     0,
       0,     0,   283,     0,     0,   314,   317,   272,   302,   303,
     304,   305,   306,   309,     0,     0,     0,     0,   311,     0,
       0,   274,   338,     0,     0,   331,     0,   206,   202,   204,
       0,   189,   198,   112,     0,     0,   125,   257,   258,   259,
     261,   264,   265,   266,   369,     0,   376,   220,     0,   301,
     276,   307,     0,   310,   121,     0,     0,   276,     0,   334,
       0,     0,     0,    79,   220,     0,   116,     0,   361,     0,
       0,     0,     0,   292,   308,     0,     0,     0,   286,     0,
     335,     0,     0,   332,   199,     0,   196,   114,   367,    98,
       0,     0,     0,     0,     0,   299,   271,     0,   280,   339,
     334,   334,   220,   126,     0,   293,     0,   312,   273,   277,
     278,   279,   282,   336,   337,   200,   275,   322,     0,     0,
       0,     0,   308,     0,   287,   323,   294,     0,     0,   164,
       0,   319,   313,   281,   144,   130,   129,   166,   167,   168,
     169,   170,     0,     0,     0,     0,   151,   153,   158,     0,
       0,   152,     0,   131,     0,   147,   155,   163,   165,     0,
       0,     0,   128,   164,     0,   270,     0,     0,   160,   220,
       0,   148,     0,   127,     0,   143,   189,     0,   162,   288,
     220,   150,     0,   318,     0,   320,     0,     0,     0,     0,
       0,     0,     0,     0,   164,     0,   171,     0,     0,   141,
       0,     0,   145,     0,   189,   319,     0,   324,     0,   173,
       0,     0,   154,     0,   133,     0,     0,   134,     0,   157,
       0,   127,     0,     0,   140,   142,   146,   270,   156,   320,
     269,   220,     0,   295,     0,     0,     0,     0,     0,   161,
       0,   149,   132,   320,   173,   172,   321,   189,     0,     0,
       0,     0,     0,     0,   173,   325,   296,   136,   135,     0,
     137,   138,     0,   289,   189,   139,   159,   189,   326,   290
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,   125,   187,    11,    12,     9,    10,    19,    96,
     263,   194,   193,   191,   202,   203,   204,   323,     7,     8,
      18,    64,   137,   228,   141,   387,   249,   478,   595,   543,
      65,   222,   347,   142,   348,   713,   714,   760,   736,   715,
     716,   758,   717,   730,   754,   718,   719,   720,   755,   793,
     153,   156,    67,   763,    68,   231,   232,   233,   358,   472,
     591,   646,   471,   537,   538,    69,    70,   242,    71,   243,
      72,   245,   756,   258,   268,   745,   581,   617,   633,   672,
     680,   722,   350,   466,   658,   689,   765,   827,   464,   652,
     677,   691,   807,   465,   579,   522,   574,   529,   530,   525,
     567,   725,   769,   681,   690,   789,   824,    73,   223,   353,
     467,   622,   533,   585,   619,    15,    16,    28,    29,   110,
      13,    14,    74,    75,    25,    26,   456,   104,   105,   560,
     450,   558
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -699
static const yytype_int16 yypact[] =
{
     -57,  -699,  -699,  -699,  -699,  -699,    75,  -699,  -699,  -699,
    -699,  -699,   100,  -699,   -26,  -699,    12,  -699,   961,   868,
    -699,    52,   104,   119,    57,   -26,  -699,   125,    12,  -699,
     154,   116,    97,   109,   101,   122,   144,   151,  -699,   185,
    -699,   233,   173,   197,   206,   209,   223,   230,   234,   242,
     247,  -699,  -699,   256,   267,  -699,   272,   281,   282,  -699,
    -699,   294,  -699,   480,  -699,  -699,  -699,    55,  -699,  -699,
    -699,  -699,  -699,  -699,  -699,   200,  -699,   345,   233,   347,
     797,  -699,   361,   362,   363,  -699,  -699,   365,   375,   378,
     797,   379,   386,   388,  -699,   390,   275,   393,   797,  -699,
     394,  -699,   380,   381,   338,   251,   119,  -699,  -699,  -699,
     341,   257,  -699,   406,   408,   410,   413,   415,  -699,  -699,
     416,   419,   420,   423,  -699,  -699,    44,   426,   427,   233,
     233,   430,   233,    23,  -699,   434,    32,   395,   233,   100,
     100,   396,   397,  -699,  -699,  -699,  -699,  -699,  -699,  -699,
    -699,  -699,  -699,  -699,  -699,  -699,  -699,   374,     6,  -699,
      21,  -699,   797,   797,   797,   399,   400,   405,   411,   412,
    -699,   414,   417,   421,   425,   436,   437,   439,   441,   443,
     444,   445,   459,   460,   461,   797,   797,  -699,  1237,   387,
    -699,   289,  -699,   305,    31,  -699,  -699,   515,  2022,   311,
    -699,  -699,   314,  -699,   464,  -699,  -699,   319,  2022,   407,
     125,   125,   309,   132,   446,   328,   132,   495,   497,  -699,
     498,   367,   442,    41,  -699,  -699,   149,   364,  -699,  -699,
     233,   454,   -16,  -699,   370,   373,   376,   382,   383,   391,
    -699,  -699,   156,   158,    42,   401,  -699,  -699,   478,    17,
      32,   409,   513,   514,   529,   797,   418,  -699,   797,  -699,
     -26,   797,   797,  -699,   797,   797,  -699,  -699,  1423,   797,
     797,   797,   797,   797,   534,   100,   797,    70,    70,    70,
      70,   797,   797,   542,   543,   797,   797,   555,  -699,  -699,
     797,   797,   797,   797,   797,   797,   797,   797,   797,   797,
     797,   797,   797,   797,   797,   797,   797,   797,   797,   797,
     797,   797,  2022,   556,   558,  -699,   562,   797,   797,  2022,
     310,   564,  -699,    43,  -699,  -699,   422,   428,  -699,  -699,
     565,  -699,  -699,  -699,   -51,  -699,    70,   567,   569,  -699,
     668,   526,   527,  -699,   233,  -699,  -699,  -699,  -699,  -699,
    -699,  -699,  -699,  -699,  -699,   577,  -699,  1071,   535,  -699,
    -699,  -699,    44,   581,  -699,  -699,  -699,  -699,  -699,  -699,
     233,  -699,   233,   434,  -699,  -699,  -699,  -699,  -699,   538,
    -699,    39,    20,  -699,  -699,  -699,  -699,   166,  1257,  -699,
    2022,     5,  2022,  2022,  1104,  2022,  2022,  -699,  1217,  1443,
    1277,  1297,  1463,   435,   438,  1489,  -699,   440,   449,   450,
     458,  1317,  1363,   465,   467,  1509,  1383,   469,  1982,  1342,
    1546,  1690,  1880,  1865,  1162,  1162,   332,   332,   332,   332,
     208,   208,   107,   107,  -699,  -699,  -699,  2022,  2022,  2022,
    -699,  -699,  -699,  2022,  2022,  -699,  -699,  -699,  -699,   587,
     125,   139,   132,   528,  -699,  -699,   -37,  -699,  -699,   470,
     471,   589,    70,  -699,   633,   725,   725,   797,   472,  -699,
      10,   584,    44,  -699,   474,  -699,  -699,  -699,    32,  -699,
    -699,  -699,   546,  -699,   475,   477,  -699,   592,   608,  -699,
    -699,  -699,   797,  -699,   797,   797,  -699,   797,  -699,  -699,
    -699,  -699,  -699,  -699,   797,   797,  -699,  -699,  -699,   609,
    -699,   797,  -699,   476,   604,  -699,  -699,  -699,  -699,  -699,
     481,   479,  -699,  -699,  2002,   539,   243,   590,  1904,   614,
    -699,   525,  2022,    45,   638,  -699,   639,     9,  -699,   545,
     597,  -699,    25,    32,  -699,  -699,  -699,  -699,   499,  1529,
    1568,  1588,  1614,  1634,  1654,   500,  2022,   132,   579,   125,
     125,   501,  -699,   100,  1882,  -699,  -699,   525,  -699,  -699,
    -699,  -699,  -699,  -699,   503,   797,   -11,   623,  -699,   586,
     618,   607,   619,   621,   622,  -699,   505,  -699,  -699,  -699,
     645,   523,  -699,  -699,    28,    32,  -699,  -699,  -699,  -699,
    -699,  -699,  -699,  -699,  -699,   540,   476,  -699,   547,  -699,
     568,  -699,  1674,  -699,  -699,   797,   632,   568,   797,    45,
     797,   797,   549,  -699,  -699,   596,  -699,    35,   132,   636,
     553,   697,   660,  -699,   108,  1013,  1713,   797,  -699,  1739,
    -699,  1759,  1779,  -699,  -699,   692,  -699,  -699,  -699,  -699,
     561,   797,   643,   666,   688,  -699,  -699,  1799,   141,  -699,
      45,    45,  -699,  -699,  1819,  -699,   797,  -699,  -699,  -699,
    -699,  -699,   711,  -699,  -699,  -699,  -699,  -699,  1840,   670,
     652,    46,   672,   726,  -699,  -699,  -699,   704,   580,   858,
     671,   699,  -699,  -699,   336,  -699,  -699,  -699,  -699,  -699,
    -699,  -699,   685,   691,   233,   693,  -699,  -699,  -699,   695,
     696,  -699,   273,  -699,   698,  -699,  -699,  -699,   858,   677,
     700,    55,  -699,   858,   100,   630,   232,   429,  -699,  -699,
     701,  -699,   260,  -699,   702,  -699,   -55,   273,  -699,  -699,
    -699,  -699,   679,  -699,   715,  -699,   705,   706,   603,   709,
     610,   712,   713,   611,   858,   612,  -699,   797,    16,  -699,
      82,   337,  -699,   273,   175,   699,   613,  -699,   100,   113,
     260,   260,  -699,   260,  -699,   260,   260,  -699,   721,  -699,
    1403,   617,   620,   260,  -699,  -699,  -699,   630,  -699,  -699,
    -699,  -699,   755,  -699,   625,   626,    89,   627,   628,  -699,
     766,  -699,  -699,  -699,   113,  -699,  -699,   523,   629,   634,
     260,   640,   641,   644,   113,  -699,  -699,  -699,  -699,   646,
    -699,  -699,    55,  -699,   523,  -699,  -699,   523,  -699,  -699
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -699,  -699,   -76,   -12,  -699,  -699,  -699,  -699,   508,  -699,
    -699,  -699,  -699,  -699,  -699,   642,  -699,  -699,  -699,  -699,
     554,  -699,  -699,  -699,  -699,  -699,  -243,  -699,  -699,  -699,
    -699,  -494,    -9,  -699,  -699,  -112,  -652,    11,    58,    47,
    -699,  -699,    79,  -699,  -699,  -699,  -665,  -699,    13,  -666,
    -699,  -687,  -628,  -227,  -699,   330,  -699,   448,  -699,  -699,
    -699,  -699,  -699,  -699,   266,  -699,  -699,  -699,  -699,  -699,
    -699,  -226,  -141,  -699,   -79,    19,   240,  -699,   194,  -699,
    -699,  -214,  -699,  -699,  -699,  -699,  -699,  -699,  -699,  -699,
    -699,  -699,  -699,  -699,  -699,  -699,  -699,  -518,   346,  -699,
    -699,    56,  -698,  -699,  -699,  -699,  -699,  -699,  -699,  -699,
    -699,  -699,  -699,  -564,  -699,  -699,  -699,  -699,   792,  -699,
    -699,  -699,  -699,  -699,   563,   -21,  -699,   716,   -10,  -699,
    -699,   218
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -349
static const yytype_int16 yytable[] =
{
      22,   188,   159,   257,   107,   362,    63,   382,   349,    66,
     577,   198,   259,   535,   535,   370,   372,   111,   374,   208,
     781,   377,   378,   261,   377,   378,   341,   124,   564,   377,
     378,  -191,   377,   378,   741,   315,   246,   247,   264,   377,
     378,    23,   695,   480,   481,   351,   244,   448,   229,   582,
     685,   583,  -191,   236,   237,   640,   239,   241,   742,   696,
     735,   721,   251,   407,   408,   409,   410,   568,   569,   570,
     571,   572,    23,   489,   406,    17,     1,     2,     3,    27,
     341,   734,   703,   266,   267,   735,   733,     4,   454,   778,
     721,   804,   341,   733,     5,   721,   673,   674,   361,   341,
      97,   762,   516,   455,    20,   814,   288,   289,   695,   352,
     312,   785,    21,    24,   686,   695,   654,   517,   319,   230,
     635,    98,   457,    99,   106,   696,   721,   252,   253,    99,
     791,   379,   696,   115,   379,   826,   329,   361,   815,   379,
     792,   573,   379,   329,    24,   116,   248,   477,   823,   379,
     306,   307,   308,   482,   359,   584,   576,   117,   653,   262,
     487,   588,   113,   114,   687,   710,   536,   536,   118,   380,
     381,   712,   483,   381,   265,   240,   388,   593,   381,   390,
     626,   381,   392,   393,   316,   395,   396,   647,   381,   119,
     398,   399,   400,   401,   402,   373,   449,   405,   460,   122,
     326,   327,   411,   412,   120,   121,   415,   416,   154,   155,
      63,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   431,   432,   433,   434,   435,   436,
     437,   438,   439,   123,   783,   542,   733,   124,   443,   444,
     126,   810,   341,   484,   485,   127,   161,    20,   521,   304,
     305,   306,   307,   308,   128,    21,   100,   129,   695,   101,
     102,   103,   100,   404,   733,   101,   108,   109,   463,   330,
     341,   130,   331,   332,   333,   696,   330,   733,   131,   331,
     332,   514,   132,   341,   162,   163,   695,   669,   670,   671,
     133,   164,   165,   166,   475,   134,   476,   746,   747,   695,
     594,   354,   355,   696,   135,   167,   168,   169,   369,   361,
     371,   361,   170,   445,   446,   136,   696,   171,   486,   361,
     138,   568,   569,   570,   571,   572,   172,   786,   361,   139,
     140,   173,   174,   175,   176,   177,   178,   179,   734,   703,
     748,   733,   143,   157,   180,    63,   181,   341,    66,   158,
     349,   160,   627,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,   182,   695,   625,   189,   190,   192,   183,   195,
     107,   302,   303,   304,   305,   306,   307,   308,   561,   196,
     696,   749,   197,   199,  -127,   524,   528,   528,   532,   200,
     161,    20,   201,   184,   205,   573,   206,   207,   209,    21,
     185,   186,   746,   747,   310,   213,   212,   210,   211,   215,
     217,   216,   218,   549,   219,   550,   551,   220,   552,   221,
     224,   349,   710,   225,   226,   553,   554,   227,   162,   163,
     234,   235,   556,   733,   238,   164,   165,   166,   244,   341,
     513,   260,   313,   250,   254,   255,   340,   269,   270,   167,
     168,   169,   341,   271,    21,   695,   170,   342,   314,   272,
     273,   171,   274,   328,   320,   275,   630,   321,   322,   276,
     172,   324,   696,   277,   325,   173,   174,   175,   176,   177,
     178,   179,   335,   644,   278,   279,   749,   280,   180,   281,
     181,   282,   283,   284,   751,   752,   612,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   182,   285,   286,   287,
     343,   336,   183,   337,   334,   338,   356,   344,   161,    20,
     339,   675,   360,   363,   345,   364,   376,    21,   365,    47,
     384,   385,   317,   386,   366,   367,   636,   184,   403,   639,
     311,   641,   642,   368,   185,   186,   413,   414,   346,   605,
     606,   608,    63,   375,    57,    58,   162,   163,   657,   417,
     440,   383,   441,   164,   165,   166,   442,    60,   447,   453,
     389,   458,   664,   459,   461,   462,   451,   167,   168,   169,
     816,   468,   452,   470,   170,   474,   479,   678,   497,   171,
     498,   512,   500,   520,   544,   515,   547,   828,   172,   766,
     829,   501,   502,   173,   174,   175,   176,   177,   178,   179,
     503,   539,   548,   555,   750,   753,   180,   506,   181,   507,
     759,   510,   518,    63,   519,   534,   541,   545,   728,   546,
     557,   559,   563,   562,   182,   580,   161,    20,   575,   566,
     183,   578,   586,   587,   592,    21,   590,   604,   784,   750,
     613,   596,   603,   614,   607,   611,   616,   623,   794,   795,
     523,   759,   624,   797,   798,   184,   615,   618,   318,   620,
     621,   802,   185,   186,   162,   163,   361,    63,   780,   632,
     637,   164,   165,   166,   784,    -9,    -9,    -9,    -9,    -9,
      -9,    -9,    -9,    -9,   628,   167,   168,   169,   819,   645,
     631,   650,   170,   643,   648,   649,    63,   171,   651,   662,
     665,    63,   743,   663,   666,   667,   172,   679,   683,   684,
     576,   173,   174,   175,   176,   177,   178,   179,   161,    20,
     688,   692,   693,   726,   180,   724,   181,    21,   723,   727,
     744,   729,    63,   731,   732,   739,   737,   767,   740,   757,
     761,   768,   182,   770,   771,   772,   790,   773,   183,   806,
     775,   776,   774,   777,   779,   788,   162,   163,   799,  -144,
     813,   394,   801,   526,   165,   166,   527,   808,   809,   811,
     812,   817,   357,   184,   796,   256,   818,   167,   168,   169,
     185,   186,   820,   821,   170,   764,   822,   738,   825,   171,
     161,    20,   540,   589,   805,   782,   803,   610,   172,    21,
     473,   638,   531,   173,   174,   175,   176,   177,   178,   179,
     112,   787,   214,   391,   629,     0,   180,     0,   181,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   162,   163,
       0,     0,     0,     0,   182,   164,   165,   166,     0,     0,
     183,     0,     0,     0,     0,     0,     0,     0,     0,   167,
     168,   169,   694,     0,     0,     0,   170,     0,   341,     0,
      21,   171,    76,     0,     0,   184,     0,     0,     0,     0,
     172,     0,   185,   186,   695,   173,   174,   175,   176,   177,
     178,   179,     0,     0,     0,     0,     0,     0,   180,     0,
     181,   696,     0,     0,     0,     0,     0,     0,     0,     0,
     697,   698,   699,   700,   701,     0,   182,    77,     0,     0,
       0,     0,   183,   702,   703,     0,     0,     0,     0,     0,
       0,     0,     0,   704,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    78,     0,     0,     0,   184,     0,     0,
       0,     0,     0,     0,   185,   186,     0,     0,     0,     0,
     705,     0,   706,     0,     0,    20,   707,     0,    30,     0,
      57,    58,    31,    21,    32,    33,     0,    79,     0,     0,
       0,     0,     0,   708,    80,    81,    82,    83,    84,   -45,
      85,    86,    87,     0,    88,    89,     0,    90,    91,    92,
       0,     0,     0,   709,    93,    94,    95,   710,     0,     0,
       0,     0,   711,   712,     0,     0,     0,   340,    34,    35,
      36,     0,     0,   341,     0,    21,     0,     0,   342,     0,
       0,    37,    38,    39,    40,     0,    41,    42,    43,     0,
       0,     0,     0,     0,     0,    44,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,    51,
      52,    53,     0,     0,     0,     0,    54,    55,    56,     0,
       0,     0,     0,    57,    58,    20,    59,     0,    30,     0,
       0,   655,    31,    21,    32,    33,    60,     0,   344,     0,
       0,     0,     0,     0,     0,   345,     0,     0,    61,     0,
      47,     0,     0,     0,  -348,     0,     0,     0,    76,     0,
       0,     0,     0,     0,     0,    62,     0,     0,   469,   346,
       0,     0,     0,     0,     0,    57,    58,     0,    34,    35,
      36,     0,     0,     0,     0,     0,     0,     0,    60,     0,
       0,    37,    38,    39,    40,     0,    41,    42,    43,     0,
       0,   490,     0,    77,     0,    44,    45,    46,    47,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,    51,
      52,    53,     0,     0,     0,     0,    54,    55,    56,    78,
       0,     0,     0,    57,    58,     0,    59,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    60,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,    61,     0,
       0,     0,     0,    79,     0,     0,     0,     0,     0,     0,
      80,    81,    82,    83,    84,    62,    85,    86,    87,     0,
      88,    89,     0,    90,    91,    92,     0,     0,     0,     0,
      93,    94,    95,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   290,     0,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,     0,     0,     0,     0,     0,     0,   491,
     492,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,     0,   290,
     309,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   290,
     488,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   290,
     494,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   290,
     495,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   290,
     504,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   290,
       0,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,     0,
       0,     0,     0,     0,     0,   290,   505,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,   509,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,   800,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   397,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,     0,     0,   290,   493,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   290,   496,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,     0,     0,     0,     0,     0,     0,
     290,   499,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     290,   508,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     290,   597,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     290,     0,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     598,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,     0,     0,     0,   290,
     599,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,     0,
       0,     0,     0,     0,     0,   290,   600,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,   601,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,   602,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,   634,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   290,     0,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   656,   290,     0,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   340,     0,     0,     0,
       0,   659,   341,     0,    21,     0,     0,   342,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   660,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,     0,     0,     0,   609,
     290,   661,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
       0,   668,   576,     0,     0,     0,     0,   344,     0,     0,
       0,     0,     0,     0,   345,     0,     0,     0,     0,    47,
       0,   676,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   346,     0,
       0,     0,   682,     0,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,   290,   511,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   290,   565,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   290,     0,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-699))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      12,    80,    78,   144,    25,   232,    18,   250,   222,    18,
     528,    90,   153,     4,     4,   242,   243,    27,   244,    98,
       4,     4,     5,    17,     4,     5,    10,     4,   522,     4,
       5,    47,     4,     5,   721,     4,     4,     5,    17,     4,
       5,    67,    26,     4,     5,     4,     4,     4,     4,     4,
       4,     6,    68,   129,   130,   619,   132,   133,   723,    43,
     712,   689,   138,   277,   278,   279,   280,    78,    79,    80,
      81,    82,    67,    68,     4,     0,   133,   134,   135,    67,
      10,    65,    66,   162,   163,   737,     4,   144,   139,   754,
     718,   789,    10,     4,   151,   723,   660,   661,   153,    10,
      48,   156,   139,   154,     4,   803,   185,   186,    26,    68,
     189,   763,    12,   139,    68,    26,   634,   154,   197,    75,
     614,    17,   336,     4,    67,    43,   754,   139,   140,     4,
      17,   114,    43,    17,   114,   822,     4,   153,   804,   114,
      27,   152,   114,     4,   139,    48,   114,   373,   814,   114,
      43,    44,    45,   114,   230,   110,    48,    48,    50,   153,
     387,   152,     8,     9,   682,   149,   157,   157,    67,   152,
     153,   155,   152,   153,   153,   152,   255,   152,   153,   258,
     152,   153,   261,   262,   153,   264,   265,   152,   153,    67,
     269,   270,   271,   272,   273,   153,   153,   276,   339,    48,
     210,   211,   281,   282,    60,    61,   285,   286,   153,   154,
     222,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,    48,   152,   478,     4,     4,   317,   318,
      67,   152,    10,   384,   385,    48,     3,     4,   462,    41,
      42,    43,    44,    45,    48,    12,   137,    48,    26,   140,
     141,   142,   137,   275,     4,   140,   141,   142,   344,   137,
      10,    48,   140,   141,   142,    43,   137,     4,    48,   140,
     141,   142,    48,    10,    41,    42,    26,   146,   147,   148,
      48,    48,    49,    50,   370,    48,   372,    65,    66,    26,
     543,   152,   153,    43,    48,    62,    63,    64,   152,   153,
     152,   153,    69,     3,     4,    48,    43,    74,   152,   153,
      48,    78,    79,    80,    81,    82,    83,   152,   153,    48,
      48,    88,    89,    90,    91,    92,    93,    94,    65,    66,
     108,     4,    48,   143,   101,   357,   103,    10,   357,     4,
     564,     4,   595,    17,    18,    19,    20,    21,    22,    23,
      24,    25,   119,    26,   591,     4,     4,     4,   125,     4,
     391,    39,    40,    41,    42,    43,    44,    45,   519,     4,
      43,   149,     4,     4,    48,   464,   465,   466,   467,     3,
       3,     4,     4,   150,     4,   152,   121,     4,     4,    12,
     157,   158,    65,    66,    17,   154,    68,    27,    27,    68,
       4,   154,     4,   492,     4,   494,   495,     4,   497,     4,
       4,   635,   149,     4,     4,   504,   505,     4,    41,    42,
       4,     4,   511,     4,     4,    48,    49,    50,     4,    10,
     450,    67,   153,    48,    48,    48,     4,    48,    48,    62,
      63,    64,    10,    48,    12,    26,    69,    15,   153,    48,
      48,    74,    48,   154,   153,    48,   607,   153,     4,    48,
      83,   152,    43,    48,    67,    88,    89,    90,    91,    92,
      93,    94,   154,   624,    48,    48,   149,    48,   101,    48,
     103,    48,    48,    48,    65,    66,   575,    17,    18,    19,
      20,    21,    22,    23,    24,    25,   119,    48,    48,    48,
      68,    16,   125,    16,    68,    17,   152,    75,     3,     4,
     153,   662,    68,   153,    82,   152,    48,    12,   152,    87,
      17,    17,    17,     4,   152,   152,   615,   150,     4,   618,
     153,   620,   621,   152,   157,   158,     4,     4,   106,   559,
     560,   563,   564,   152,   112,   113,    41,    42,   637,     4,
       4,   152,     4,    48,    49,    50,     4,   125,     4,     4,
     152,     4,   651,     4,    48,    48,   154,    62,    63,    64,
     807,     4,   154,    48,    69,     4,    48,   666,   153,    74,
     152,     4,   152,     4,    48,    67,     4,   824,    83,   740,
     827,   152,   152,    88,    89,    90,    91,    92,    93,    94,
     152,    27,     4,     4,   726,   727,   101,   152,   103,   152,
     732,   152,   152,   635,   153,   153,   152,   152,   704,   152,
     154,    27,   153,   152,   119,   110,     3,     4,    48,   100,
     125,    27,     4,     4,    47,    12,   101,    68,   760,   761,
      27,   152,   152,    67,   153,   152,    49,   152,   770,   771,
      27,   773,    17,   775,   776,   150,    48,    48,   153,    48,
      48,   783,   157,   158,    41,    42,   153,   689,   757,   111,
      48,    48,    49,    50,   796,    17,    18,    19,    20,    21,
      22,    23,    24,    25,   154,    62,    63,    64,   810,   103,
     153,     4,    69,   154,    68,   152,   718,    74,    48,    17,
      67,   723,   724,   152,    48,    27,    83,     6,    48,    67,
      48,    88,    89,    90,    91,    92,    93,    94,     3,     4,
       4,    27,   152,    48,   101,    36,   103,    12,    67,    48,
     110,    48,   754,    48,    48,    68,    48,    68,    48,    48,
      48,    36,   119,    48,    48,   152,   768,    48,   125,     4,
      48,    48,   152,   152,   152,   152,    41,    42,    47,   152,
       4,   263,   152,    48,    49,    50,    51,   152,   152,   152,
     152,   152,   228,   150,   773,   143,   152,    62,    63,    64,
     157,   158,   152,   152,    69,   737,   152,   718,   152,    74,
       3,     4,   472,   537,   791,   758,   787,   567,    83,    12,
     362,   617,   466,    88,    89,    90,    91,    92,    93,    94,
      28,   765,   106,   260,   606,    -1,   101,    -1,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      -1,    -1,    -1,    -1,   119,    48,    49,    50,    -1,    -1,
     125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,
      63,    64,     4,    -1,    -1,    -1,    69,    -1,    10,    -1,
      12,    74,     4,    -1,    -1,   150,    -1,    -1,    -1,    -1,
      83,    -1,   157,   158,    26,    88,    89,    90,    91,    92,
      93,    94,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,
     103,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    53,    54,    55,    56,    -1,   119,    49,    -1,    -1,
      -1,    -1,   125,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    -1,    -1,    -1,   150,    -1,    -1,
      -1,    -1,    -1,    -1,   157,   158,    -1,    -1,    -1,    -1,
     102,    -1,   104,    -1,    -1,     4,   108,    -1,     7,    -1,
     112,   113,    11,    12,    13,    14,    -1,   109,    -1,    -1,
      -1,    -1,    -1,   125,   116,   117,   118,   119,   120,   121,
     122,   123,   124,    -1,   126,   127,    -1,   129,   130,   131,
      -1,    -1,    -1,   145,   136,   137,   138,   149,    -1,    -1,
      -1,    -1,   154,   155,    -1,    -1,    -1,     4,    57,    58,
      59,    -1,    -1,    10,    -1,    12,    -1,    -1,    15,    -1,
      -1,    70,    71,    72,    73,    -1,    75,    76,    77,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,    -1,    -1,    -1,    -1,   105,   106,   107,    -1,
      -1,    -1,    -1,   112,   113,     4,   115,    -1,     7,    -1,
      -1,    68,    11,    12,    13,    14,   125,    -1,    75,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,   137,    -1,
      87,    -1,    -1,    -1,   143,    -1,    -1,    -1,     4,    -1,
      -1,    -1,    -1,    -1,    -1,   154,    -1,    -1,    47,   106,
      -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    57,    58,
      59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    70,    71,    72,    73,    -1,    75,    76,    77,    -1,
      -1,    47,    -1,    49,    -1,    84,    85,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,    -1,    -1,    -1,    -1,   105,   106,   107,    75,
      -1,    -1,    -1,   112,   113,    -1,   115,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   125,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,   137,    -1,
      -1,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,   119,   120,   154,   122,   123,   124,    -1,
     126,   127,    -1,   129,   130,   131,    -1,    -1,    -1,    -1,
     136,   137,   138,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,   152,
     153,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    -1,    26,
     153,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
     153,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
     153,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
     153,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
     153,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    26,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    26,   153,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,   153,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,   153,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,   152,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    26,   152,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    26,   152,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      26,   152,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      26,   152,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      26,   152,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      26,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
     152,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    -1,    -1,    26,
     152,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    26,   152,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,   152,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,   152,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,   152,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,   152,    26,    -1,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,     4,    -1,    -1,    -1,
      -1,   152,    10,    -1,    12,    -1,    -1,    15,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,   152,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    -1,    -1,    -1,    47,
      26,   152,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      -1,   152,    48,    -1,    -1,    -1,    -1,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    87,
      -1,   152,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,    -1,
      -1,    -1,   152,    -1,   112,   113,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   125,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    26,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   133,   134,   135,   144,   151,   160,   177,   178,   165,
     166,   163,   164,   279,   280,   274,   275,     0,   179,   167,
       4,    12,   162,    67,   139,   283,   284,    67,   276,   277,
       7,    11,    13,    14,    57,    58,    59,    70,    71,    72,
      73,    75,    76,    77,    84,    85,    86,    87,    95,    96,
      97,    98,    99,   100,   105,   106,   107,   112,   113,   115,
     125,   137,   154,   162,   180,   189,   191,   211,   213,   224,
     225,   227,   229,   266,   281,   282,     4,    49,    75,   109,
     116,   117,   118,   119,   120,   122,   123,   124,   126,   127,
     129,   130,   131,   136,   137,   138,   168,    48,    17,     4,
     137,   140,   141,   142,   286,   287,    67,   284,   141,   142,
     278,   287,   277,     8,     9,    17,    48,    48,    67,    67,
      60,    61,    48,    48,     4,   161,    67,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,   181,    48,    48,
      48,   183,   192,    48,    17,    18,    19,    20,    21,    22,
      23,    24,    25,   209,   153,   154,   210,   143,     4,   161,
       4,     3,    41,    42,    48,    49,    50,    62,    63,    64,
      69,    74,    83,    88,    89,    90,    91,    92,    93,    94,
     101,   103,   119,   125,   150,   157,   158,   162,   233,     4,
       4,   172,     4,   171,   170,     4,     4,     4,   233,     4,
       3,     4,   173,   174,   175,     4,   121,     4,   233,     4,
      27,    27,    68,   154,   286,    68,   154,     4,     4,     4,
       4,     4,   190,   267,     4,     4,     4,     4,   182,     4,
      75,   214,   215,   216,     4,     4,   161,   161,     4,   161,
     152,   161,   226,   228,     4,   230,     4,     5,   114,   185,
      48,   161,   162,   162,    48,    48,   174,   231,   232,   231,
      67,    17,   153,   169,    17,   153,   233,   233,   233,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,   233,   233,
      26,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,   153,
      17,   153,   233,   153,   153,     4,   153,    17,   153,   233,
     153,   153,     4,   176,   152,    67,   287,   287,   154,     4,
     137,   140,   141,   142,    68,   154,    16,    16,    17,   153,
       4,    10,    15,    68,    75,    82,   106,   191,   193,   240,
     241,     4,    68,   268,   152,   153,   152,   179,   217,   161,
      68,   153,   212,   153,   152,   152,   152,   152,   152,   152,
     212,   152,   212,   153,   230,   152,    48,     4,     5,   114,
     152,   153,   185,   152,    17,    17,     4,   184,   233,   152,
     233,   283,   233,   233,   167,   233,   233,   152,   233,   233,
     233,   233,   233,     4,   162,   233,     4,   240,   240,   240,
     240,   233,   233,     4,     4,   233,   233,     4,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
       4,     4,     4,   233,   233,     3,     4,     4,     4,   153,
     289,   154,   154,     4,   139,   154,   285,   240,     4,     4,
     231,    48,    48,   161,   247,   252,   242,   269,     4,    47,
      48,   221,   218,   216,     4,   161,   161,   230,   186,    48,
       4,     5,   114,   152,   231,   231,   152,   212,   153,    68,
      47,   152,   153,   152,   153,   153,   152,   153,   152,   152,
     152,   152,   152,   152,   153,   153,   152,   152,   152,   153,
     152,    27,     4,   287,   142,    67,   139,   154,   152,   153,
       4,   240,   254,    27,   233,   258,    48,    51,   233,   256,
     257,   257,   233,   271,   153,     4,   157,   222,   223,    27,
     214,   152,   185,   188,    48,   152,   152,     4,     4,   233,
     233,   233,   233,   233,   233,     4,   233,   154,   290,    27,
     288,   231,   152,   153,   190,    27,   100,   259,    78,    79,
      80,    81,    82,   152,   255,    48,    48,   256,    27,   253,
     110,   235,     4,     6,   110,   272,     4,     4,   152,   223,
     101,   219,    47,   152,   185,   187,   152,   152,   152,   152,
     152,   152,   152,   152,    68,   287,   287,   153,   162,    47,
     235,   152,   233,    27,    67,    48,    49,   236,    48,   273,
      48,    48,   270,   152,    17,   212,   152,   185,   154,   290,
     231,   153,   111,   237,   152,   190,   233,    48,   237,   233,
     272,   233,   233,   154,   231,   103,   220,   152,    68,   152,
       4,    48,   248,    50,   256,    68,   152,   233,   243,   152,
     152,   152,    17,   152,   233,    67,    48,    27,   152,   146,
     147,   148,   238,   272,   272,   231,   152,   249,   233,     6,
     239,   262,   152,    48,    67,     4,    68,   256,     4,   244,
     263,   250,    27,   152,     4,    26,    43,    52,    53,    54,
      55,    56,    65,    66,    75,   102,   104,   108,   125,   145,
     149,   154,   155,   194,   195,   198,   199,   201,   204,   205,
     206,   211,   240,    67,    36,   260,    48,    48,   161,    48,
     202,    48,    48,     4,    65,   195,   197,    48,   201,    68,
      48,   210,   205,   162,   110,   234,    65,    66,   108,   149,
     194,    65,    66,   194,   203,   207,   231,    48,   200,   194,
     196,    48,   156,   212,   197,   245,   231,    68,    36,   261,
      48,    48,   152,    48,   152,    48,    48,   152,   205,   152,
     233,     4,   198,   152,   194,   195,   152,   260,   152,   264,
     162,    17,    27,   208,   194,   194,   196,   194,   194,    47,
     153,   152,   194,   234,   261,   207,     4,   251,   152,   152,
     152,   152,   152,     4,   261,   208,   212,   152,   152,   194,
     152,   152,   152,   208,   265,   152,   210,   246,   212,   212
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:

/* Line 1806 of yacc.c  */
#line 191 "ldgram.y"
    {
          const char *core_name;

          core_name = lang_get_core_name();
          if (core_name)
            {
              (yyval.name) = concat((yyvsp[(3) - (4)].name),"_",core_name,"_",NULL); 
            }
          else
            (yyval.name) = (yyvsp[(3) - (4)].name);
        }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 202 "ldgram.y"
    { (yyval.name)=(yyvsp[(1) - (1)].name); }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 207 "ldgram.y"
    { ldlex_defsym(); }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 209 "ldgram.y"
    {
		  ldlex_popstate();
		  lang_add_assignment(exp_assop((yyvsp[(3) - (4)].token),(yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)));
		}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 217 "ldgram.y"
    {
		  ldlex_mri_script ();
		  PUSH_ERROR (_("MRI style script"));
		}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 222 "ldgram.y"
    {
		  ldlex_popstate ();
		  mri_draw_tree ();
		  POP_ERROR ();
		}
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 237 "ldgram.y"
    {
			einfo(_("%P%F: error: unrecognised keyword in MRI style script '%s'\n"),(yyvsp[(1) - (1)].name));
			}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 240 "ldgram.y"
    {
			config.map_filename = "-";
			}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 246 "ldgram.y"
    { mri_public((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree)); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 248 "ldgram.y"
    { mri_public((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree)); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 250 "ldgram.y"
    { mri_public((yyvsp[(2) - (3)].name), (yyvsp[(3) - (3)].etree)); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 252 "ldgram.y"
    { mri_format((yyvsp[(2) - (2)].name)); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 254 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree));}
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 256 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (3)].name), (yyvsp[(3) - (3)].etree));}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 258 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree));}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 260 "ldgram.y"
    { mri_align((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 262 "ldgram.y"
    { mri_align((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 264 "ldgram.y"
    { mri_alignmod((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 266 "ldgram.y"
    { mri_alignmod((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 270 "ldgram.y"
    { mri_name((yyvsp[(2) - (2)].name)); }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 272 "ldgram.y"
    { mri_alias((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].name),0);}
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 274 "ldgram.y"
    { mri_alias ((yyvsp[(2) - (4)].name), 0, (int) (yyvsp[(4) - (4)].bigint).integer); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 276 "ldgram.y"
    { mri_base((yyvsp[(2) - (2)].etree)); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 278 "ldgram.y"
    { mri_truncate ((unsigned int) (yyvsp[(2) - (2)].bigint).integer); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 282 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 284 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 286 "ldgram.y"
    { lang_add_entry ((yyvsp[(2) - (2)].name), FALSE); }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 291 "ldgram.y"
    { mri_order((yyvsp[(3) - (3)].name)); }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 292 "ldgram.y"
    { mri_order((yyvsp[(2) - (2)].name)); }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 298 "ldgram.y"
    { mri_load((yyvsp[(1) - (1)].name)); }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 299 "ldgram.y"
    { mri_load((yyvsp[(3) - (3)].name)); }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 304 "ldgram.y"
    { mri_only_load((yyvsp[(1) - (1)].name)); }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 306 "ldgram.y"
    { mri_only_load((yyvsp[(3) - (3)].name)); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 310 "ldgram.y"
    { (yyval.name) = NULL; }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 317 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 319 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 323 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(1) - (1)].name)); }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 325 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(2) - (2)].name)); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 327 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(3) - (3)].name)); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 331 "ldgram.y"
    { ldlex_both(); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 333 "ldgram.y"
    { ldlex_popstate(); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 354 "ldgram.y"
    { lang_add_target((yyvsp[(3) - (4)].name)); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 356 "ldgram.y"
    { ldfile_add_library_path ((yyvsp[(3) - (4)].name), FALSE); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 358 "ldgram.y"
    { lang_add_output((yyvsp[(3) - (4)].name), 1); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 360 "ldgram.y"
    { lang_add_output_format ((yyvsp[(3) - (4)].name), (char *) NULL,
					    (char *) NULL, 1); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 363 "ldgram.y"
    { lang_add_output_format ((yyvsp[(3) - (8)].name), (yyvsp[(5) - (8)].name), (yyvsp[(7) - (8)].name), 1); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 365 "ldgram.y"
    { ldfile_set_output_arch ((yyvsp[(3) - (4)].name), bfd_arch_unknown); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 367 "ldgram.y"
    { command_line.force_common_definition = TRUE ; }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 369 "ldgram.y"
    { command_line.inhibit_common_definition = TRUE ; }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 372 "ldgram.y"
    { lang_enter_group (); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 374 "ldgram.y"
    { lang_leave_group (); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 376 "ldgram.y"
    { lang_add_map((yyvsp[(3) - (4)].name)); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 378 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 380 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 382 "ldgram.y"
    {
		  lang_add_nocrossref ((yyvsp[(3) - (4)].nocrossref));
		}
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 387 "ldgram.y"
    { lang_add_insert ((yyvsp[(3) - (3)].name), 0); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 389 "ldgram.y"
    { lang_add_insert ((yyvsp[(3) - (3)].name), 1); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 391 "ldgram.y"
    { lang_add_export_symbol((yyvsp[(3) - (3)].name),FALSE,
            0,
            NULL); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 395 "ldgram.y"
    { lang_add_export_symbol((yyvsp[(3) - (5)].name),FALSE,
            0,
            (yyvsp[(5) - (5)].name)); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 399 "ldgram.y"
    { lang_add_export_symbol((yyvsp[(3) - (3)].name),TRUE,
            0,
            NULL); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 403 "ldgram.y"
    { lang_add_export_symbol((yyvsp[(3) - (5)].name),TRUE,
            0,
            (yyvsp[(5) - (5)].name)); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 407 "ldgram.y"
    { lang_memory_region_alias ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].name)); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 409 "ldgram.y"
    { lang_add_memory_region_map(
            (yyvsp[(3) - (10)].name),
		    exp_get_vma ((yyvsp[(5) - (10)].etree), 0, "region origin"),
		    exp_get_vma ((yyvsp[(7) - (10)].etree), 0, "region length"),
		    exp_get_vma ((yyvsp[(9) - (10)].etree), 0, "alternate origin")
            );
        }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 417 "ldgram.y"
    {
          lang_set_core_number((yyvsp[(3) - (3)].name));   
        }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 421 "ldgram.y"
    {
            lang_set_core_alias ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].name));
        }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 425 "ldgram.y"
    { lang_enter_region_mirror (); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 427 "ldgram.y"
    { lang_leave_region_mirror (); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 432 "ldgram.y"
    { lang_add_to_region_mirror((yyvsp[(1) - (1)].name)); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 434 "ldgram.y"
    { lang_add_to_region_mirror((yyvsp[(3) - (3)].name)); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 437 "ldgram.y"
    { lang_add_input_file((yyvsp[(1) - (1)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 440 "ldgram.y"
    { lang_add_input_file((yyvsp[(3) - (3)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 443 "ldgram.y"
    { lang_add_input_file((yyvsp[(2) - (2)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 446 "ldgram.y"
    { lang_add_input_file((yyvsp[(1) - (1)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 449 "ldgram.y"
    { lang_add_input_file((yyvsp[(3) - (3)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 452 "ldgram.y"
    { lang_add_input_file((yyvsp[(2) - (2)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 455 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 457 "ldgram.y"
    { as_needed = (yyvsp[(3) - (5)].integer); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 459 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 461 "ldgram.y"
    { as_needed = (yyvsp[(5) - (7)].integer); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 463 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 465 "ldgram.y"
    { as_needed = (yyvsp[(4) - (6)].integer); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 481 "ldgram.y"
    { lang_add_entry ((yyvsp[(3) - (4)].name), FALSE); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 483 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 484 "ldgram.y"
    { ldlex_popstate ();
		  lang_add_assignment (exp_assert ((yyvsp[(4) - (7)].etree), (yyvsp[(6) - (7)].name))); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 489 "ldgram.y"
    {
            lang_define_sda_section ((yyvsp[(3) - (8)].name), (yyvsp[(5) - (8)].name), (yyvsp[(7) - (8)].name) );
        }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 498 "ldgram.y"
    {
			  (yyval.cname) = (yyvsp[(1) - (1)].name);
			}
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 502 "ldgram.y"
    {
            (yyval.cname) = (yyvsp[(1) - (1)].name);
        }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 507 "ldgram.y"
    {
			  (yyval.cname) = "*";
			}
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 511 "ldgram.y"
    {
			  (yyval.cname) = "?";
			}
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 518 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(1) - (1)].cname);
			  (yyval.wildcard).sorted = none;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 524 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (5)].cname);
			  (yyval.wildcard).sorted = none;
			  (yyval.wildcard).exclude_name_list = (yyvsp[(3) - (5)].name_list);
			}
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 530 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(3) - (4)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 536 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(3) - (4)].cname);
			  (yyval.wildcard).sorted = by_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 542 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_name_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 548 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 554 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_alignment_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 560 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 566 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(7) - (8)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = (yyvsp[(5) - (8)].name_list);
			}
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 575 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = (yyvsp[(2) - (2)].cname);
			  tmp->next = (yyvsp[(1) - (2)].name_list);
			  (yyval.name_list) = tmp;
			}
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 584 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = (yyvsp[(1) - (1)].cname);
			  tmp->next = NULL;
			  (yyval.name_list) = tmp;
			}
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 595 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = (yyvsp[(1) - (3)].wildcard_list);
			  tmp->spec = (yyvsp[(3) - (3)].wildcard);
			  (yyval.wildcard_list) = tmp;
			}
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 604 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = NULL;
			  tmp->spec = (yyvsp[(1) - (1)].wildcard);
			  (yyval.wildcard_list) = tmp;
			}
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 615 "ldgram.y"
    {
			  struct wildcard_spec tmp;
			  tmp.name = (yyvsp[(1) - (1)].name);
			  tmp.exclude_name_list = NULL;
			  tmp.sorted = none;
			  lang_add_wild (&tmp, NULL, ldgram_had_keep);
			}
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 623 "ldgram.y"
    {
			  lang_add_wild (NULL, (yyvsp[(2) - (3)].wildcard_list), ldgram_had_keep);
			}
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 627 "ldgram.y"
    {
			  lang_add_wild (&(yyvsp[(1) - (4)].wildcard), (yyvsp[(3) - (4)].wildcard_list), ldgram_had_keep);
			}
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 635 "ldgram.y"
    { ldgram_had_keep = TRUE; }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 637 "ldgram.y"
    { ldgram_had_keep = FALSE; }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 643 "ldgram.y"
    {
 		lang_add_attribute(lang_object_symbols_statement_enum);
	      	}
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 648 "ldgram.y"
    {

		  lang_add_attribute(lang_constructors_statement_enum);
		}
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 653 "ldgram.y"
    {
		  constructors_sorted = TRUE;
		  lang_add_attribute (lang_constructors_statement_enum);
		}
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 659 "ldgram.y"
    {
			  lang_add_data ((int) (yyvsp[(1) - (4)].integer), (yyvsp[(3) - (4)].etree));
			}
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 664 "ldgram.y"
    {
			  lang_add_fill ((yyvsp[(3) - (4)].fill));
			}
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 667 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 668 "ldgram.y"
    { ldlex_popstate ();
			  lang_add_assignment (exp_assert ((yyvsp[(4) - (8)].etree), (yyvsp[(6) - (8)].name))); }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 671 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 673 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 688 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 690 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 692 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 694 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 696 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 701 "ldgram.y"
    {
		  (yyval.fill) = exp_get_fill ((yyvsp[(1) - (1)].etree), 0, "fill value");
		}
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 708 "ldgram.y"
    { (yyval.fill) = (yyvsp[(2) - (2)].fill); }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 709 "ldgram.y"
    { (yyval.fill) = (fill_type *) 0; }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 714 "ldgram.y"
    { (yyval.token) = '+'; }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 716 "ldgram.y"
    { (yyval.token) = '-'; }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 718 "ldgram.y"
    { (yyval.token) = '*'; }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 720 "ldgram.y"
    { (yyval.token) = '/'; }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 722 "ldgram.y"
    { (yyval.token) = LSHIFT; }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 724 "ldgram.y"
    { (yyval.token) = RSHIFT; }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 726 "ldgram.y"
    { (yyval.token) = '&'; }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 728 "ldgram.y"
    { (yyval.token) = '|'; }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 738 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ((yyvsp[(2) - (3)].token), (yyvsp[(1) - (3)].name), (yyvsp[(3) - (3)].etree)));
		}
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 742 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ('=', (yyvsp[(1) - (3)].name),
						  exp_binop ((yyvsp[(2) - (3)].token),
							     exp_nameop (NAME,
									 (yyvsp[(1) - (3)].name)),
							     (yyvsp[(3) - (3)].etree))));
		}
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 750 "ldgram.y"
    {
		  lang_add_assignment (exp_provide ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].etree), FALSE));
		}
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 754 "ldgram.y"
    {
		  lang_add_assignment (exp_provide ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].etree), TRUE));
		}
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 777 "ldgram.y"
    { region = lang_memory_region_lookup ((yyvsp[(1) - (1)].name), TRUE); }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 780 "ldgram.y"
    {}
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 782 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 784 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 789 "ldgram.y"
    {
		  region->origin = exp_get_vma ((yyvsp[(3) - (3)].etree), 0, "origin");
		  region->current = region->origin;
		}
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 797 "ldgram.y"
    {
		  region->length = exp_get_vma ((yyvsp[(3) - (3)].etree), -1, "length");
		}
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 804 "ldgram.y"
    { /* dummy action to avoid bison 1.25 error message */ }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 815 "ldgram.y"
    { lang_set_flags (region, (yyvsp[(1) - (1)].name), 0); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 817 "ldgram.y"
    { lang_set_flags (region, (yyvsp[(2) - (2)].name), 1); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 822 "ldgram.y"
    { lang_startup((yyvsp[(3) - (4)].name)); }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 828 "ldgram.y"
    { ldemul_hll((char *)NULL); }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 833 "ldgram.y"
    { ldemul_hll((yyvsp[(3) - (3)].name)); }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 835 "ldgram.y"
    { ldemul_hll((yyvsp[(1) - (1)].name)); }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 843 "ldgram.y"
    { ldemul_syslib((yyvsp[(3) - (3)].name)); }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 849 "ldgram.y"
    { lang_float(TRUE); }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 851 "ldgram.y"
    { lang_float(FALSE); }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 856 "ldgram.y"
    {
		  (yyval.nocrossref) = NULL;
		}
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 860 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = (yyvsp[(1) - (2)].name);
		  n->next = (yyvsp[(2) - (2)].nocrossref);
		  (yyval.nocrossref) = n;
		}
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 869 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = (yyvsp[(1) - (3)].name);
		  n->next = (yyvsp[(3) - (3)].nocrossref);
		  (yyval.nocrossref) = n;
		}
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 879 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 881 "ldgram.y"
    { ldlex_popstate (); (yyval.etree)=(yyvsp[(2) - (2)].etree);}
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 886 "ldgram.y"
    { (yyval.etree) = exp_unop ('-', (yyvsp[(2) - (2)].etree)); }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 888 "ldgram.y"
    { (yyval.etree) = (yyvsp[(2) - (3)].etree); }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 890 "ldgram.y"
    { (yyval.etree) = exp_unop ((int) (yyvsp[(1) - (4)].integer),(yyvsp[(3) - (4)].etree)); }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 892 "ldgram.y"
    { (yyval.etree) = exp_unop ('!', (yyvsp[(2) - (2)].etree)); }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 894 "ldgram.y"
    { (yyval.etree) = (yyvsp[(2) - (2)].etree); }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 896 "ldgram.y"
    { (yyval.etree) = exp_unop ('~', (yyvsp[(2) - (2)].etree));}
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 899 "ldgram.y"
    { (yyval.etree) = exp_binop ('*', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 901 "ldgram.y"
    { (yyval.etree) = exp_binop ('/', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 903 "ldgram.y"
    { (yyval.etree) = exp_binop ('%', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 905 "ldgram.y"
    { (yyval.etree) = exp_binop ('+', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 907 "ldgram.y"
    { (yyval.etree) = exp_binop ('-' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 909 "ldgram.y"
    { (yyval.etree) = exp_binop (LSHIFT , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 911 "ldgram.y"
    { (yyval.etree) = exp_binop (RSHIFT , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 913 "ldgram.y"
    { (yyval.etree) = exp_binop (EQ , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 915 "ldgram.y"
    { (yyval.etree) = exp_binop (NE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 917 "ldgram.y"
    { (yyval.etree) = exp_binop (LE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 919 "ldgram.y"
    { (yyval.etree) = exp_binop (GE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 921 "ldgram.y"
    { (yyval.etree) = exp_binop ('<' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 923 "ldgram.y"
    { (yyval.etree) = exp_binop ('>' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 925 "ldgram.y"
    { (yyval.etree) = exp_binop ('&' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 927 "ldgram.y"
    { (yyval.etree) = exp_binop ('^' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 929 "ldgram.y"
    { (yyval.etree) = exp_binop ('|' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 931 "ldgram.y"
    { (yyval.etree) = exp_trinop ('?' , (yyvsp[(1) - (5)].etree), (yyvsp[(3) - (5)].etree), (yyvsp[(5) - (5)].etree)); }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 933 "ldgram.y"
    { (yyval.etree) = exp_binop (ANDAND , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 935 "ldgram.y"
    { (yyval.etree) = exp_binop (OROR , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 937 "ldgram.y"
    { (yyval.etree) = exp_nameop (DEFINED, (yyvsp[(3) - (4)].name)); }
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 939 "ldgram.y"
    { (yyval.etree) = exp_bigintop ((yyvsp[(1) - (1)].bigint).integer, (yyvsp[(1) - (1)].bigint).str); }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 941 "ldgram.y"
    { (yyval.etree) = exp_nameop (SIZEOF_HEADERS,0); }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 944 "ldgram.y"
    { (yyval.etree) = exp_nameop (ALIGNOF,(yyvsp[(3) - (4)].name)); }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 946 "ldgram.y"
    { (yyval.etree) = exp_nameop (SIZEOF,(yyvsp[(3) - (4)].name)); }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 948 "ldgram.y"
    { (yyval.etree) = exp_nameop (ADDR,(yyvsp[(3) - (4)].name)); }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 950 "ldgram.y"
    { (yyval.etree) = exp_nameop (LOADADDR,(yyvsp[(3) - (4)].name)); }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 952 "ldgram.y"
    { (yyval.etree) = exp_nameop (CONSTANT,(yyvsp[(3) - (4)].name)); }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 954 "ldgram.y"
    { (yyval.etree) = exp_unop (ABSOLUTE, (yyvsp[(3) - (4)].etree)); }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 956 "ldgram.y"
    { (yyval.etree) = exp_unop (ALIGN_K,(yyvsp[(3) - (4)].etree)); }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 958 "ldgram.y"
    { (yyval.etree) = exp_binop (ALIGN_K,(yyvsp[(3) - (6)].etree),(yyvsp[(5) - (6)].etree)); }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 960 "ldgram.y"
    { (yyval.etree) = exp_binop (DATA_SEGMENT_ALIGN, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree)); }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 962 "ldgram.y"
    { (yyval.etree) = exp_binop (DATA_SEGMENT_RELRO_END, (yyvsp[(5) - (6)].etree), (yyvsp[(3) - (6)].etree)); }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 964 "ldgram.y"
    { (yyval.etree) = exp_unop (DATA_SEGMENT_END, (yyvsp[(3) - (4)].etree)); }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 966 "ldgram.y"
    { /* The operands to the expression node are
			     placed in the opposite order from the way
			     in which they appear in the script as
			     that allows us to reuse more code in
			     fold_binary.  */
			  (yyval.etree) = exp_binop (SEGMENT_START,
					  (yyvsp[(5) - (6)].etree),
					  exp_nameop (NAME, (yyvsp[(3) - (6)].name))); }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 975 "ldgram.y"
    { (yyval.etree) = exp_unop (ALIGN_K,(yyvsp[(3) - (4)].etree)); }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 977 "ldgram.y"
    { (yyval.etree) = exp_nameop (NAME,(yyvsp[(1) - (1)].name)); }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 979 "ldgram.y"
    { (yyval.etree) = exp_binop (MAX_K, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree) ); }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 981 "ldgram.y"
    { (yyval.etree) = exp_binop (MIN_K, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree) ); }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 983 "ldgram.y"
    { (yyval.etree) = exp_assert ((yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].name)); }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 985 "ldgram.y"
    { (yyval.etree) = exp_nameop (ORIGIN, (yyvsp[(3) - (4)].name)); }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 987 "ldgram.y"
    { (yyval.etree) = exp_nameop (LENGTH, (yyvsp[(3) - (4)].name)); }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 992 "ldgram.y"
    { (yyval.name) = (yyvsp[(3) - (3)].name); }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 993 "ldgram.y"
    { (yyval.name) = 0; }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 997 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 998 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 1002 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 1003 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 1007 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 276:

/* Line 1806 of yacc.c  */
#line 1008 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 277:

/* Line 1806 of yacc.c  */
#line 1012 "ldgram.y"
    { (yyval.token) = ONLY_IF_RO; }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 1013 "ldgram.y"
    { (yyval.token) = ONLY_IF_RW; }
    break;

  case 279:

/* Line 1806 of yacc.c  */
#line 1014 "ldgram.y"
    { (yyval.token) = SPECIAL; }
    break;

  case 280:

/* Line 1806 of yacc.c  */
#line 1015 "ldgram.y"
    { (yyval.token) = 0; }
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 1019 "ldgram.y"
    { os_flags = lang_set_section_flags((yyvsp[(3) - (4)].name)); }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 1020 "ldgram.y"
    { os_flags = 0; }
    break;

  case 283:

/* Line 1806 of yacc.c  */
#line 1024 "ldgram.y"
    { 
            const char * core_name;

            core_name = lang_get_core_name();
            if (core_name)
              {
                (yyval.name) = concat(".",core_name,(yyvsp[(3) - (4)].name),NULL); 
              }
            else
              (yyval.name) = (yyvsp[(3) - (4)].name);
            }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 1035 "ldgram.y"
    { (yyval.name)=(yyvsp[(1) - (1)].name); }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 1040 "ldgram.y"
    { ldlex_expression(); }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 1044 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 1048 "ldgram.y"
    {
			  lang_enter_output_section_statement((yyvsp[(1) - (10)].name), (yyvsp[(3) - (10)].etree),
							      sectype,
							      (yyvsp[(5) - (10)].etree), (yyvsp[(6) - (10)].etree), (yyvsp[(4) - (10)].etree), (yyvsp[(8) - (10)].token), os_flags);
			}
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 1054 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 1056 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_leave_output_section_statement ((yyvsp[(18) - (18)].fill), (yyvsp[(15) - (18)].name), (yyvsp[(17) - (18)].section_phdr), (yyvsp[(16) - (18)].name));
		}
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 1061 "ldgram.y"
    {}
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 1063 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 1065 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 293:

/* Line 1806 of yacc.c  */
#line 1067 "ldgram.y"
    {
			  lang_enter_overlay ((yyvsp[(3) - (8)].etree), (yyvsp[(6) - (8)].etree));
			}
    break;

  case 294:

/* Line 1806 of yacc.c  */
#line 1072 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 295:

/* Line 1806 of yacc.c  */
#line 1074 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay ((yyvsp[(5) - (16)].etree), (int) (yyvsp[(4) - (16)].integer),
					      (yyvsp[(16) - (16)].fill), (yyvsp[(13) - (16)].name), (yyvsp[(15) - (16)].section_phdr), (yyvsp[(14) - (16)].name));
			}
    break;

  case 297:

/* Line 1806 of yacc.c  */
#line 1084 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 298:

/* Line 1806 of yacc.c  */
#line 1086 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_add_assignment (exp_assop ('=', ".", (yyvsp[(3) - (3)].etree)));
		}
    break;

  case 300:

/* Line 1806 of yacc.c  */
#line 1092 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 301:

/* Line 1806 of yacc.c  */
#line 1094 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 302:

/* Line 1806 of yacc.c  */
#line 1098 "ldgram.y"
    { sectype = noload_section; }
    break;

  case 303:

/* Line 1806 of yacc.c  */
#line 1099 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 304:

/* Line 1806 of yacc.c  */
#line 1100 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 305:

/* Line 1806 of yacc.c  */
#line 1101 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 306:

/* Line 1806 of yacc.c  */
#line 1102 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 308:

/* Line 1806 of yacc.c  */
#line 1107 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 309:

/* Line 1806 of yacc.c  */
#line 1108 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 310:

/* Line 1806 of yacc.c  */
#line 1112 "ldgram.y"
    { (yyval.etree) = (yyvsp[(1) - (3)].etree); }
    break;

  case 311:

/* Line 1806 of yacc.c  */
#line 1113 "ldgram.y"
    { (yyval.etree) = (etree_type *)NULL;  }
    break;

  case 312:

/* Line 1806 of yacc.c  */
#line 1118 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (6)].etree); }
    break;

  case 313:

/* Line 1806 of yacc.c  */
#line 1120 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (10)].etree); }
    break;

  case 314:

/* Line 1806 of yacc.c  */
#line 1124 "ldgram.y"
    { (yyval.etree) = (yyvsp[(1) - (2)].etree); }
    break;

  case 315:

/* Line 1806 of yacc.c  */
#line 1125 "ldgram.y"
    { (yyval.etree) = (etree_type *) NULL;  }
    break;

  case 316:

/* Line 1806 of yacc.c  */
#line 1130 "ldgram.y"
    { (yyval.integer) = 0; }
    break;

  case 317:

/* Line 1806 of yacc.c  */
#line 1132 "ldgram.y"
    { (yyval.integer) = 1; }
    break;

  case 318:

/* Line 1806 of yacc.c  */
#line 1137 "ldgram.y"
    { (yyval.name) = (yyvsp[(2) - (2)].name); }
    break;

  case 319:

/* Line 1806 of yacc.c  */
#line 1138 "ldgram.y"
    { (yyval.name) = DEFAULT_MEMORY_REGION; }
    break;

  case 320:

/* Line 1806 of yacc.c  */
#line 1143 "ldgram.y"
    {
		  (yyval.section_phdr) = NULL;
		}
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 1147 "ldgram.y"
    {
		  struct lang_output_section_phdr_list *n;

		  n = ((struct lang_output_section_phdr_list *)
		       xmalloc (sizeof *n));
		  n->name = (yyvsp[(3) - (3)].name);
		  n->used = FALSE;
		  n->next = (yyvsp[(1) - (3)].section_phdr);
		  (yyval.section_phdr) = n;
		}
    break;

  case 323:

/* Line 1806 of yacc.c  */
#line 1163 "ldgram.y"
    {
			  ldlex_script ();
			  lang_enter_overlay_section ((yyvsp[(2) - (2)].name));
			}
    break;

  case 324:

/* Line 1806 of yacc.c  */
#line 1168 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 325:

/* Line 1806 of yacc.c  */
#line 1170 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay_section ((yyvsp[(9) - (9)].fill), (yyvsp[(8) - (9)].section_phdr));
			}
    break;

  case 330:

/* Line 1806 of yacc.c  */
#line 1187 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 331:

/* Line 1806 of yacc.c  */
#line 1188 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 332:

/* Line 1806 of yacc.c  */
#line 1190 "ldgram.y"
    {
		  lang_new_phdr ((yyvsp[(1) - (6)].name), (yyvsp[(3) - (6)].etree), (yyvsp[(4) - (6)].phdr).filehdr, (yyvsp[(4) - (6)].phdr).phdrs, (yyvsp[(4) - (6)].phdr).at,
				 (yyvsp[(4) - (6)].phdr).flags);
		}
    break;

  case 333:

/* Line 1806 of yacc.c  */
#line 1198 "ldgram.y"
    {
		  (yyval.etree) = (yyvsp[(1) - (1)].etree);

		  if ((yyvsp[(1) - (1)].etree)->type.node_class == etree_name
		      && (yyvsp[(1) - (1)].etree)->type.node_code == NAME)
		    {
		      const char *s;
		      unsigned int i;
		      static const char * const phdr_types[] =
			{
			  "PT_NULL", "PT_LOAD", "PT_DYNAMIC",
			  "PT_INTERP", "PT_NOTE", "PT_SHLIB",
			  "PT_PHDR", "PT_TLS"
			};

		      s = (yyvsp[(1) - (1)].etree)->name.name;
		      for (i = 0;
			   i < sizeof phdr_types / sizeof phdr_types[0];
			   i++)
			if (strcmp (s, phdr_types[i]) == 0)
			  {
			    (yyval.etree) = exp_intop (i);
			    break;
			  }
		      if (i == sizeof phdr_types / sizeof phdr_types[0])
			{
			  if (strcmp (s, "PT_GNU_EH_FRAME") == 0)
			    (yyval.etree) = exp_intop (0x6474e550);
			  else if (strcmp (s, "PT_GNU_STACK") == 0)
			    (yyval.etree) = exp_intop (0x6474e551);
			  else
			    {
			      einfo (_("\
%X%P:%S: error: unknown phdr type `%s' (try integer literal)\n"),
				     s);
			      (yyval.etree) = exp_intop (0);
			    }
			}
		    }
		}
    break;

  case 334:

/* Line 1806 of yacc.c  */
#line 1242 "ldgram.y"
    {
		  memset (&(yyval.phdr), 0, sizeof (struct phdr_info));
		}
    break;

  case 335:

/* Line 1806 of yacc.c  */
#line 1246 "ldgram.y"
    {
		  (yyval.phdr) = (yyvsp[(3) - (3)].phdr);
		  if (strcmp ((yyvsp[(1) - (3)].name), "FILEHDR") == 0 && (yyvsp[(2) - (3)].etree) == NULL)
		    (yyval.phdr).filehdr = TRUE;
		  else if (strcmp ((yyvsp[(1) - (3)].name), "PHDRS") == 0 && (yyvsp[(2) - (3)].etree) == NULL)
		    (yyval.phdr).phdrs = TRUE;
		  else if (strcmp ((yyvsp[(1) - (3)].name), "FLAGS") == 0 && (yyvsp[(2) - (3)].etree) != NULL)
		    (yyval.phdr).flags = (yyvsp[(2) - (3)].etree);
		  else
		    einfo (_("%X%P: error:%S: PHDRS syntax error at `%s'\n"), (yyvsp[(1) - (3)].name));
		}
    break;

  case 336:

/* Line 1806 of yacc.c  */
#line 1258 "ldgram.y"
    {
		  (yyval.phdr) = (yyvsp[(5) - (5)].phdr);
          (yyval.phdr).flags = (yyvsp[(3) - (5)].etree);
        }
    break;

  case 337:

/* Line 1806 of yacc.c  */
#line 1264 "ldgram.y"
    {
		  (yyval.phdr) = (yyvsp[(5) - (5)].phdr);
		  (yyval.phdr).at = (yyvsp[(3) - (5)].etree);
		}
    break;

  case 338:

/* Line 1806 of yacc.c  */
#line 1272 "ldgram.y"
    {
		  (yyval.etree) = NULL;
		}
    break;

  case 339:

/* Line 1806 of yacc.c  */
#line 1276 "ldgram.y"
    {
		  (yyval.etree) = (yyvsp[(2) - (3)].etree);
		}
    break;

  case 340:

/* Line 1806 of yacc.c  */
#line 1282 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("dynamic list"));
		}
    break;

  case 341:

/* Line 1806 of yacc.c  */
#line 1287 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 345:

/* Line 1806 of yacc.c  */
#line 1304 "ldgram.y"
    {
		  lang_append_dynamic_list ((yyvsp[(1) - (2)].versyms));
		}
    break;

  case 346:

/* Line 1806 of yacc.c  */
#line 1312 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("VERSION script"));
		}
    break;

  case 347:

/* Line 1806 of yacc.c  */
#line 1317 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 348:

/* Line 1806 of yacc.c  */
#line 1326 "ldgram.y"
    {
		  ldlex_version_script ();
		}
    break;

  case 349:

/* Line 1806 of yacc.c  */
#line 1330 "ldgram.y"
    {
		  ldlex_popstate ();
		}
    break;

  case 352:

/* Line 1806 of yacc.c  */
#line 1342 "ldgram.y"
    {
		  lang_register_vers_node (NULL, (yyvsp[(2) - (4)].versnode), NULL);
		}
    break;

  case 353:

/* Line 1806 of yacc.c  */
#line 1346 "ldgram.y"
    {
		  lang_register_vers_node ((yyvsp[(1) - (5)].name), (yyvsp[(3) - (5)].versnode), NULL);
		}
    break;

  case 354:

/* Line 1806 of yacc.c  */
#line 1350 "ldgram.y"
    {
		  lang_register_vers_node ((yyvsp[(1) - (6)].name), (yyvsp[(3) - (6)].versnode), (yyvsp[(5) - (6)].deflist));
		}
    break;

  case 355:

/* Line 1806 of yacc.c  */
#line 1357 "ldgram.y"
    {
		  (yyval.deflist) = lang_add_vers_depend (NULL, (yyvsp[(1) - (1)].name));
		}
    break;

  case 356:

/* Line 1806 of yacc.c  */
#line 1361 "ldgram.y"
    {
		  (yyval.deflist) = lang_add_vers_depend ((yyvsp[(1) - (2)].deflist), (yyvsp[(2) - (2)].name));
		}
    break;

  case 357:

/* Line 1806 of yacc.c  */
#line 1368 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node (NULL, NULL);
		}
    break;

  case 358:

/* Line 1806 of yacc.c  */
#line 1372 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(1) - (2)].versyms), NULL);
		}
    break;

  case 359:

/* Line 1806 of yacc.c  */
#line 1376 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(3) - (4)].versyms), NULL);
		}
    break;

  case 360:

/* Line 1806 of yacc.c  */
#line 1380 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node (NULL, (yyvsp[(3) - (4)].versyms));
		}
    break;

  case 361:

/* Line 1806 of yacc.c  */
#line 1384 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(3) - (8)].versyms), (yyvsp[(7) - (8)].versyms));
		}
    break;

  case 362:

/* Line 1806 of yacc.c  */
#line 1391 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, (yyvsp[(1) - (1)].name), ldgram_vers_current_lang, FALSE);
		}
    break;

  case 363:

/* Line 1806 of yacc.c  */
#line 1395 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, (yyvsp[(1) - (1)].name), ldgram_vers_current_lang, TRUE);
		}
    break;

  case 364:

/* Line 1806 of yacc.c  */
#line 1399 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), (yyvsp[(3) - (3)].name), ldgram_vers_current_lang, FALSE);
		}
    break;

  case 365:

/* Line 1806 of yacc.c  */
#line 1403 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), (yyvsp[(3) - (3)].name), ldgram_vers_current_lang, TRUE);
		}
    break;

  case 366:

/* Line 1806 of yacc.c  */
#line 1407 "ldgram.y"
    {
			  (yyval.name) = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = (yyvsp[(4) - (5)].name);
			}
    break;

  case 367:

/* Line 1806 of yacc.c  */
#line 1412 "ldgram.y"
    {
			  struct bfd_elf_version_expr *pat;
			  for (pat = (yyvsp[(7) - (9)].versyms); pat->next != NULL; pat = pat->next);
			  pat->next = (yyvsp[(1) - (9)].versyms);
			  (yyval.versyms) = (yyvsp[(7) - (9)].versyms);
			  ldgram_vers_current_lang = (yyvsp[(6) - (9)].name);
			}
    break;

  case 368:

/* Line 1806 of yacc.c  */
#line 1420 "ldgram.y"
    {
			  (yyval.name) = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = (yyvsp[(2) - (3)].name);
			}
    break;

  case 369:

/* Line 1806 of yacc.c  */
#line 1425 "ldgram.y"
    {
			  (yyval.versyms) = (yyvsp[(5) - (7)].versyms);
			  ldgram_vers_current_lang = (yyvsp[(4) - (7)].name);
			}
    break;

  case 370:

/* Line 1806 of yacc.c  */
#line 1430 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 371:

/* Line 1806 of yacc.c  */
#line 1434 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 372:

/* Line 1806 of yacc.c  */
#line 1438 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 373:

/* Line 1806 of yacc.c  */
#line 1442 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 374:

/* Line 1806 of yacc.c  */
#line 1446 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 375:

/* Line 1806 of yacc.c  */
#line 1450 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "extern", ldgram_vers_current_lang, FALSE);
		}
    break;



/* Line 1806 of yacc.c  */
#line 5169 "/home/horst/work/hightec-development-platform-336/toolchain/binutils-2.20/ld/ldgram.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1460 "ldgram.y"

void
yyerror(arg)
     const char *arg;
{
  if (ldfile_assumed_script)
    einfo (_("%P:%s: warning: file format not recognized; treating as linker script\n"),
	   ldfile_input_filename);
  if (error_index > 0 && error_index < ERROR_NAME_MAX)
     einfo (_("%P%F: error:%S: %s in %s\n"), arg, error_names[error_index-1]);
  else
     einfo (_("%P%F: error:%S: %s\n"), arg);
}

