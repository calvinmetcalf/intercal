/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UNKNOWN = 258,
     GETS = 259,
     RESIZE = 260,
     NEXT = 261,
     GO_AHEAD = 262,
     GO_BACK = 263,
     FORGET = 264,
     RESUME = 265,
     STASH = 266,
     RETRIEVE = 267,
     IGNORE = 268,
     REMEMBER = 269,
     ABSTAIN = 270,
     REINSTATE = 271,
     DISABLE = 272,
     ENABLE = 273,
     MANYFROM = 274,
     GIVE_UP = 275,
     READ_OUT = 276,
     WRITE_IN = 277,
     PIN = 278,
     COME_FROM = 279,
     NEXTFROMLABEL = 280,
     NEXTFROMEXPR = 281,
     NEXTFROMGERUND = 282,
     COMPUCOME = 283,
     GERUCOME = 284,
     PREPROC = 285,
     WHILE = 286,
     TRY_AGAIN = 287,
     CREATE = 288,
     COMPUCREATE = 289,
     FROM = 290,
     MAYBE = 291,
     DO = 292,
     PLEASE = 293,
     NOT = 294,
     ONCE = 295,
     AGAIN = 296,
     MESH = 297,
     NOSPOT = 298,
     ick_ONESPOT = 299,
     ick_TWOSPOT = 300,
     ick_TAIL = 301,
     ick_HYBRID = 302,
     MINGLE = 303,
     SELECT = 304,
     UNKNOWNOP = 305,
     SUB = 306,
     BY = 307,
     SLAT = 308,
     BACKSLAT = 309,
     NUMBER = 310,
     UNARY = 311,
     OHOHSEVEN = 312,
     GERUND = 313,
     LABEL = 314,
     BADCHAR = 315,
     INTERSECTION = 316,
     SPLATTERED = 317,
     MESH32 = 318,
     C_AND = 319,
     C_OR = 320,
     C_XOR = 321,
     C_NOT = 322,
     C_LOGICALNOT = 323,
     C_LSHIFTBY = 324,
     C_RSHIFTBY = 325,
     C_NOTEQUAL = 326,
     C_A = 327,
     C_PLUS = 328,
     C_MINUS = 329,
     C_TIMES = 330,
     C_DIVIDEBY = 331,
     C_MODULUS = 332,
     C_GREATER = 333,
     C_LESS = 334,
     C_ISEQUAL = 335,
     C_LOGICALAND = 336,
     C_LOGICALOR = 337,
     AND = 338,
     OR = 339,
     XOR = 340,
     FIN = 341,
     WHIRL = 342,
     WHIRL2 = 343,
     WHIRL3 = 344,
     WHIRL4 = 345,
     WHIRL5 = 346,
     REV_AND = 347,
     REV_OR = 348,
     REV_XOR = 349,
     REV_FIN = 350,
     REV_WHIRL = 351,
     REV_WHIRL2 = 352,
     REV_WHIRL3 = 353,
     REV_WHIRL4 = 354,
     REV_WHIRL5 = 355,
     UNKNOWNID = 356,
     US_ID = 357,
     US_SCALAR = 358,
     US_ARRVAR = 359,
     US_ELEM = 360,
     US_EXPR = 361,
     CLOSESPARK = 362,
     CLOSEEARS = 363,
     OPENSPARK = 364,
     OPENEARS = 365,
     HIGHPREC = 366,
     UNARYPREC = 367,
     LOWPREC = 368
   };
#endif
/* Tokens.  */
#define UNKNOWN 258
#define GETS 259
#define RESIZE 260
#define NEXT 261
#define GO_AHEAD 262
#define GO_BACK 263
#define FORGET 264
#define RESUME 265
#define STASH 266
#define RETRIEVE 267
#define IGNORE 268
#define REMEMBER 269
#define ABSTAIN 270
#define REINSTATE 271
#define DISABLE 272
#define ENABLE 273
#define MANYFROM 274
#define GIVE_UP 275
#define READ_OUT 276
#define WRITE_IN 277
#define PIN 278
#define COME_FROM 279
#define NEXTFROMLABEL 280
#define NEXTFROMEXPR 281
#define NEXTFROMGERUND 282
#define COMPUCOME 283
#define GERUCOME 284
#define PREPROC 285
#define WHILE 286
#define TRY_AGAIN 287
#define CREATE 288
#define COMPUCREATE 289
#define FROM 290
#define MAYBE 291
#define DO 292
#define PLEASE 293
#define NOT 294
#define ONCE 295
#define AGAIN 296
#define MESH 297
#define NOSPOT 298
#define ick_ONESPOT 299
#define ick_TWOSPOT 300
#define ick_TAIL 301
#define ick_HYBRID 302
#define MINGLE 303
#define SELECT 304
#define UNKNOWNOP 305
#define SUB 306
#define BY 307
#define SLAT 308
#define BACKSLAT 309
#define NUMBER 310
#define UNARY 311
#define OHOHSEVEN 312
#define GERUND 313
#define LABEL 314
#define BADCHAR 315
#define INTERSECTION 316
#define SPLATTERED 317
#define MESH32 318
#define C_AND 319
#define C_OR 320
#define C_XOR 321
#define C_NOT 322
#define C_LOGICALNOT 323
#define C_LSHIFTBY 324
#define C_RSHIFTBY 325
#define C_NOTEQUAL 326
#define C_A 327
#define C_PLUS 328
#define C_MINUS 329
#define C_TIMES 330
#define C_DIVIDEBY 331
#define C_MODULUS 332
#define C_GREATER 333
#define C_LESS 334
#define C_ISEQUAL 335
#define C_LOGICALAND 336
#define C_LOGICALOR 337
#define AND 338
#define OR 339
#define XOR 340
#define FIN 341
#define WHIRL 342
#define WHIRL2 343
#define WHIRL3 344
#define WHIRL4 345
#define WHIRL5 346
#define REV_AND 347
#define REV_OR 348
#define REV_XOR 349
#define REV_FIN 350
#define REV_WHIRL 351
#define REV_WHIRL2 352
#define REV_WHIRL3 353
#define REV_WHIRL4 354
#define REV_WHIRL5 355
#define UNKNOWNID 356
#define US_ID 357
#define US_SCALAR 358
#define US_ARRVAR 359
#define US_ELEM 360
#define US_EXPR 361
#define CLOSESPARK 362
#define CLOSEEARS 363
#define OPENSPARK 364
#define OPENEARS 365
#define HIGHPREC 366
#define UNARYPREC 367
#define LOWPREC 368




/* Copy the first part of user declarations.  */
#line 30 "parser.y"

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "sizes.h"
#include "ick.h"
#include "feh.h"
#include "ick_lose.h"

extern int yyerror(const char*);

/* Intervene our ick_first-stage lexer. */
extern int lexer(void);
#define yylex() lexer()

static node *rlist;	/* pointer to current right-hand node list */
/*static node *llist;*/	/* pointer to current left-hand node list */
static node *np;	/* variable for building node lists */

extern int stbeginline;	/* line number of last seen preamble */
static int thisline;	/* line number of beginning of current statement */

extern int mark112;    /* AIS: Mark the tuple for W112 when it's created. */
static int lineuid=65537; /* AIS: a line number not used anywhere else */
static int cacsofar=0; /* AIS: Number of args in a CREATE statement */

static tuple *splat(int);

static tuple *prevtuple = NULL;

#define GETLINENO					\
    {if (stbeginline < 0) thisline = -stbeginline;	\
     else {thisline = stbeginline; stbeginline = 0;}}

#define ACTION(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->ick_lineno = thisline; x->u.node = nn;}
#define TARGET(x, nt, nn)	\
    {x = newtuple(); x->type = nt; x->ick_lineno = thisline; x->u.target = nn;}
#define ACTARGET(x, nt, nn, nn2)\
    {x = newtuple(); x->type = nt; x->ick_lineno = thisline;\
      x->u.node = nn; x->u.target = nn2;}
/* AIS : The macro above was added for ABSTAIN expr FROM. */
#define NEWFANGLED mark112 = 1; /* AIS: Added the mention of mark112 */ \
      if (ick_traditional) ick_lose(IE111,iyylineno,(char*)NULL); else

#define DESTACKSE1 sparkearsstack[sparkearslev--/32] >>= 1
#define DESTACKSPARKEARS DESTACKSE1; DESTACKSE1



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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 83 "parser.y"
{
    int		numval;		/* a numeric value */
    tuple	*tuple;		/* a code tuple */
    node	*node;		/* an expression-tree node */
}
/* Line 187 of yacc.c.  */
#line 378 "parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 391 "parser.c"

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
# if YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   670

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  114
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  145
/* YYNRULES -- Number of states.  */
#define YYNSTATES  230

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   368

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    14,    18,    23,    25,
      27,    30,    33,    35,    39,    41,    44,    46,    49,    53,
      57,    61,    64,    67,    70,    73,    76,    79,    82,    86,
      90,    95,   100,   103,   106,   109,   112,   115,   117,   119,
     121,   123,   125,   128,   131,   133,   136,   139,   142,   146,
     149,   153,   155,   157,   159,   161,   164,   166,   168,   171,
     173,   176,   178,   180,   182,   184,   186,   188,   192,   194,
     196,   198,   200,   203,   206,   209,   212,   215,   219,   223,
     226,   228,   232,   234,   236,   238,   242,   244,   246,   248,
     250,   252,   256,   258,   262,   264,   266,   270,   274,   277,
     280,   282,   284,   286,   288,   292,   296,   298,   300,   304,
     308,   312,   316,   318,   320,   322,   324,   328,   332,   336,
     340,   342,   346,   350,   354,   358,   360,   362,   364,   366,
     368,   370,   374,   379,   384,   388,   392,   394,   396,   398,
     401,   403,   406,   408,   410,   412
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     115,     0,    -1,    -1,   115,   116,    -1,   119,   117,    -1,
     119,    57,   117,    -1,    59,   119,   117,    -1,    59,   119,
      57,   117,    -1,     1,    -1,   118,    -1,   118,    40,    -1,
     118,    41,    -1,   120,    -1,   120,    31,   120,    -1,    37,
      -1,    37,    39,    -1,    36,    -1,    36,    39,    -1,   128,
       4,   147,    -1,   131,     4,   139,    -1,   148,     4,   147,
      -1,    59,     6,    -1,     9,   147,    -1,    10,   147,    -1,
      11,   134,    -1,    12,   134,    -1,    13,   134,    -1,    14,
     134,    -1,    15,    35,    59,    -1,    15,    35,   126,    -1,
      15,   147,    35,    59,    -1,    15,   147,    35,   126,    -1,
      16,    59,    -1,    16,   126,    -1,    22,   136,    -1,    21,
     138,    -1,    23,   129,    -1,    20,    -1,     7,    -1,     8,
      -1,    32,    -1,    24,    -1,    28,   126,    -1,    28,   147,
      -1,    25,    -1,    26,   126,    -1,    26,   147,    -1,    33,
     121,    -1,    34,   147,   122,    -1,    33,   146,    -1,    34,
     154,   146,    -1,   121,    -1,    60,    -1,     1,    -1,   124,
      -1,   123,   124,    -1,   123,    -1,   125,    -1,   125,   121,
      -1,   125,    -1,   121,   125,    -1,   140,    -1,   130,    -1,
     148,    -1,   131,    -1,   101,    -1,    58,    -1,   126,    61,
      58,    -1,   130,    -1,   131,    -1,   130,    -1,   140,    -1,
      45,    55,    -1,    44,    55,    -1,    45,    55,    -1,    46,
      55,    -1,    47,    55,    -1,    46,    56,    55,    -1,    47,
      56,    55,    -1,    42,    55,    -1,   127,    -1,   134,    61,
     127,    -1,   130,    -1,   140,    -1,   131,    -1,   135,    61,
     136,    -1,   135,    -1,   130,    -1,   140,    -1,   133,    -1,
     131,    -1,   137,    61,   138,    -1,   137,    -1,   147,    52,
     139,    -1,   147,    -1,   141,    -1,   131,    51,   142,    -1,
     131,    51,   143,    -1,   154,   142,    -1,   154,   143,    -1,
     141,    -1,   145,    -1,   154,    -1,   145,    -1,   132,    51,
     142,    -1,   132,    51,   143,    -1,    60,    -1,   154,    -1,
     154,    49,   147,    -1,   154,    48,   147,    -1,   154,   146,
     147,    -1,   130,    53,   147,    -1,   140,    -1,   144,    -1,
     153,    -1,   144,    -1,   154,    49,   147,    -1,   154,    48,
     147,    -1,   154,   146,   147,    -1,   130,    53,   147,    -1,
     152,    -1,   152,    49,   147,    -1,   152,    48,   147,    -1,
     152,   146,   147,    -1,   130,    53,   147,    -1,   140,    -1,
     144,    -1,    42,    -1,    44,    -1,    45,    -1,   133,    -1,
     150,    56,    55,    -1,   155,    56,   147,   107,    -1,   156,
      56,   147,   108,    -1,   155,   149,   107,    -1,   156,   149,
     108,    -1,   151,    -1,   130,    -1,   151,    -1,    56,   154,
      -1,   152,    -1,    56,   154,    -1,   109,    -1,   107,    -1,
     110,    -1,   108,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   168,   168,   169,   181,   183,   185,   187,   189,   204,
     206,   209,   226,   227,   266,   267,   268,   269,   273,   274,
     275,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   304,   307,   309,   312,   315,   316,   319,
     321,   325,   327,   328,   333,   334,   336,   339,   340,   342,
     343,   348,   355,   362,   369,   372,   376,   378,   387,   387,
     389,   389,   391,   397,   403,   411,   417,   427,   435,   446,
     461,   462,   468,   468,   468,   469,   470,   475,   475,   475,
     475,   476,   477,   483,   484,   488,   489,   491,   493,   494,
     496,   497,   498,   502,   503,   506,   511,   516,   519,   520,
     521,   532,   537,   538,   542,   543,   544,   545,   546,   556,
     564,   565,   566,   567,   577,   582,   583,   587,   587,   587,
     592,   594,   616,   623,   632,   633,   636,   637,   640,   641,
     651,   652,   656,   657,   660,   661
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UNKNOWN", "GETS", "RESIZE", "NEXT",
  "GO_AHEAD", "GO_BACK", "FORGET", "RESUME", "STASH", "RETRIEVE", "IGNORE",
  "REMEMBER", "ABSTAIN", "REINSTATE", "DISABLE", "ENABLE", "MANYFROM",
  "GIVE_UP", "READ_OUT", "WRITE_IN", "PIN", "COME_FROM", "NEXTFROMLABEL",
  "NEXTFROMEXPR", "NEXTFROMGERUND", "COMPUCOME", "GERUCOME", "PREPROC",
  "WHILE", "TRY_AGAIN", "CREATE", "COMPUCREATE", "FROM", "MAYBE", "DO",
  "PLEASE", "NOT", "ONCE", "AGAIN", "MESH", "NOSPOT", "ick_ONESPOT",
  "ick_TWOSPOT", "ick_TAIL", "ick_HYBRID", "MINGLE", "SELECT", "UNKNOWNOP",
  "SUB", "BY", "SLAT", "BACKSLAT", "NUMBER", "UNARY", "OHOHSEVEN",
  "GERUND", "LABEL", "BADCHAR", "INTERSECTION", "SPLATTERED", "MESH32",
  "C_AND", "C_OR", "C_XOR", "C_NOT", "C_LOGICALNOT", "C_LSHIFTBY",
  "C_RSHIFTBY", "C_NOTEQUAL", "C_A", "C_PLUS", "C_MINUS", "C_TIMES",
  "C_DIVIDEBY", "C_MODULUS", "C_GREATER", "C_LESS", "C_ISEQUAL",
  "C_LOGICALAND", "C_LOGICALOR", "AND", "OR", "XOR", "FIN", "WHIRL",
  "WHIRL2", "WHIRL3", "WHIRL4", "WHIRL5", "REV_AND", "REV_OR", "REV_XOR",
  "REV_FIN", "REV_WHIRL", "REV_WHIRL2", "REV_WHIRL3", "REV_WHIRL4",
  "REV_WHIRL5", "UNKNOWNID", "US_ID", "US_SCALAR", "US_ARRVAR", "US_ELEM",
  "US_EXPR", "CLOSESPARK", "CLOSEEARS", "OPENSPARK", "OPENEARS",
  "HIGHPREC", "UNARYPREC", "LOWPREC", "$accept", "program", "command",
  "mtperform", "preproc", "please", "perform", "unknownstatement",
  "unknownsif", "unknownsin", "unknownatom", "unknownaid", "gerunds",
  "variable", "lvalue", "scalar2s", "scalar", "ick_array", "oparray",
  "constant", "varlist", "initem", "inlist", "outitem", "outlist",
  "byexpr", "subscr", "subscr1", "sublist", "sublist1", "osubscr",
  "osubscr1", "unop", "expr", "notanlvalue", "limexpr", "preftype",
  "lunambig", "limunambig", "nlunambig", "unambig", "eitherspark",
  "eitherears", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   114,   115,   115,   116,   116,   116,   116,   116,   117,
     117,   117,   118,   118,   119,   119,   119,   119,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   121,   121,   121,   122,   122,   123,
     123,   124,   124,   124,   124,   125,   126,   126,   127,   127,
     128,   128,   129,   130,   130,   131,   131,   132,   132,   133,
     134,   134,   135,   135,   135,   136,   136,   137,   137,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   142,   142,
     143,   143,   143,   144,   144,   145,   146,   147,   147,   147,
     147,   147,   147,   147,   148,   148,   148,   148,   148,   148,
     149,   149,   149,   149,   149,   149,   149,   150,   150,   150,
     151,   151,   151,   151,   151,   151,   152,   152,   153,   153,
     154,   154,   155,   155,   156,   156
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     3,     3,     4,     1,     1,
       2,     2,     1,     3,     1,     2,     1,     2,     3,     3,
       3,     2,     2,     2,     2,     2,     2,     2,     3,     3,
       4,     4,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     2,     2,     3,     2,
       3,     1,     1,     1,     1,     2,     1,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     3,     3,     2,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     1,     3,     1,     1,     3,     3,     2,     2,
       1,     1,     1,     1,     3,     3,     1,     1,     3,     3,
       3,     3,     1,     1,     1,     1,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     3,     4,     4,     3,     3,     1,     1,     1,     2,
       1,     2,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     8,    16,    14,     0,     3,     0,    17,
      15,     0,    53,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,    37,     0,     0,     0,    41,    44,     0,
       0,    40,     0,     0,   127,   128,   129,     0,     0,     0,
       0,     0,    52,    65,   143,   145,   142,   144,     4,     9,
      12,    51,    56,    54,    59,     0,    62,    64,     0,   130,
      61,    95,   115,   103,    63,     0,   138,   140,   114,     0,
       0,     0,     0,     6,     0,   137,     0,   112,   113,    22,
     136,   107,    23,     0,     0,     0,     0,    80,    68,    69,
      24,    25,    26,    27,     0,     0,    66,    32,    33,     0,
      87,    90,    89,    92,    35,    88,    82,    84,    86,    34,
      83,     0,    36,    45,    46,    42,    43,   106,    47,    62,
      64,    61,    49,    63,     0,   107,    79,    73,    74,    75,
       0,    76,     0,   137,   139,     5,    21,    10,    11,     0,
      60,    55,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   137,   125,   126,     0,   120,     0,     0,
       7,   141,     0,     0,     0,     0,     0,    28,    29,     0,
       0,     0,     0,    72,    48,    57,    50,    77,    78,    13,
      18,   119,    19,    94,     0,     0,   100,    96,    97,   101,
     102,   104,   105,    20,   131,   117,   116,   118,     0,     0,
     134,     0,     0,     0,     0,   135,   111,   109,   108,   110,
      81,    30,    31,    67,    91,    85,    58,     0,     0,     0,
      98,    99,   132,   124,   122,   121,   123,   133,    93,   102
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     7,    48,    49,     8,    50,    51,   174,    52,
      53,    54,    98,    87,    55,   112,    75,    76,    58,    59,
      90,   108,   109,   103,   104,   182,    77,    61,   187,   188,
      78,    63,   122,   183,    64,   156,    65,    80,    67,    68,
      81,    70,    71
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -140
static const yytype_int16 yypact[] =
{
    -140,   129,  -140,  -140,   -31,   -25,     6,  -140,   290,  -140,
    -140,   362,  -140,  -140,  -140,   438,   438,   248,   248,   248,
     248,   152,   102,  -140,   207,   248,   -27,  -140,  -140,   457,
     457,  -140,   451,   438,   -34,   -32,   -29,    84,   128,   -17,
     432,    24,  -140,  -140,  -140,  -140,  -140,  -140,  -140,   149,
       5,   -51,   478,  -140,  -140,    48,   165,     9,    14,  -140,
      66,  -140,  -140,  -140,    69,    33,     8,  -140,  -140,    74,
     547,   553,   432,  -140,   -17,    41,    49,  -140,  -140,  -140,
    -140,    94,  -140,   -32,   -29,    43,    53,  -140,  -140,  -140,
      52,    52,    52,    52,   162,    83,  -140,  -140,    55,   -34,
    -140,    49,  -140,    59,  -140,  -140,  -140,    49,    67,  -140,
    -140,    77,  -140,    55,  -140,    55,  -140,  -140,   -51,   -15,
      49,  -140,  -140,  -140,   -51,    94,  -140,  -140,  -140,  -140,
      89,  -140,    91,  -140,   157,  -140,  -140,  -140,  -140,   432,
    -140,  -140,   438,   438,   438,   438,   438,   438,    95,   438,
     438,   438,   438,    71,  -140,  -140,    45,   167,   438,    28,
    -140,  -140,   438,   438,   438,   438,   248,  -140,    55,   176,
      99,   207,   248,  -140,  -140,   472,   438,  -140,  -140,  -140,
    -140,  -140,  -140,   110,   125,   130,  -140,  -140,  -140,  -140,
     560,  -140,  -140,  -140,  -140,  -140,  -140,  -140,    78,   438,
    -140,   438,   438,   438,   104,  -140,  -140,  -140,  -140,  -140,
    -140,  -140,    55,  -140,  -140,  -140,   -51,   438,   438,   438,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,  -140,    -5,  -140,   185,    64,   -28,  -140,  -140,
     155,   -46,   -10,    70,  -140,  -140,    -8,    29,   -63,   -23,
      57,  -140,    37,  -140,    85,    38,   101,   -41,  -139,  -131,
     218,   -39,   -40,    81,   -30,   192,  -140,    63,   171,  -140,
      47,  -140,  -140
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -142
static const yytype_int16 yytable[] =
{
      56,   102,   123,    56,   118,   140,    73,   191,     9,    88,
      88,    88,    88,   144,    10,   192,   100,   106,   111,   113,
     115,   126,   123,   127,   119,    34,   128,    35,    36,   151,
     136,   133,    56,  -137,  -137,   135,   139,    57,   143,    74,
      57,   165,     4,     5,   119,  -137,    89,    89,    89,    89,
      43,   220,   142,   101,   107,    69,  -136,  -136,    69,   221,
     145,   120,   153,   153,    56,   146,   133,   160,  -136,    57,
     -71,    66,   140,   147,    66,    91,    92,    93,   175,    69,
     125,   120,   185,   185,   168,   176,   134,    69,   192,   148,
      44,    45,    46,    47,   162,    66,    79,    82,   129,    69,
     145,    57,    95,    66,   186,   186,   189,   189,   131,    60,
     114,   116,    60,   166,   124,    66,   170,   203,   169,    69,
     171,   161,   149,   150,   199,   105,   110,   185,   172,     2,
       3,    56,   173,   121,   117,    66,   205,   133,   133,   129,
     130,    60,   163,   164,   177,   123,   178,   216,   102,   186,
     194,   189,   200,   121,   117,   185,   185,   213,    88,   212,
      96,    97,   217,   100,   106,     4,     5,   119,    57,   -70,
     140,   154,   154,    60,   184,   184,   218,   186,   186,   189,
     189,   219,   133,   131,   132,   222,    69,    94,     6,   137,
     138,    11,   190,   190,    34,    89,    35,    36,    37,    38,
     101,   107,    66,   179,   120,  -141,  -141,   141,    74,   215,
     133,   133,   227,  -137,  -137,   201,   202,  -141,   143,   184,
      96,   167,    69,   180,   181,  -137,    62,   117,   193,    62,
     195,   196,   197,   198,    96,   211,   210,   190,    66,   204,
      60,   157,   157,   206,   207,   208,   209,   184,   184,    99,
      62,    83,    84,    85,    86,   228,   214,   209,    62,    44,
      45,    46,    47,   159,     0,   229,   229,     0,     0,     0,
      62,     0,   105,   110,     0,     0,   121,     0,     0,     0,
     223,     0,   224,   225,   226,     0,     0,     0,   155,   155,
      62,    12,    83,    84,    85,    86,     0,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,     0,     0,     0,
      23,    24,    25,    26,    27,    28,    29,     0,    30,     0,
       0,     0,    31,    32,    33,     0,     0,     0,     0,     0,
       0,     0,    34,     0,    35,    36,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,    39,    40,     0,    41,
      42,     0,     0,     0,     0,     0,     0,    62,     0,     0,
       0,     0,     0,    12,     0,     0,     0,     0,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,     0,
       0,     0,    23,    24,    25,    26,    27,    28,    29,     0,
      30,    43,     0,    62,    31,    32,    33,    44,    45,    46,
      47,     0,     0,     0,    34,     0,    35,    36,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    72,
       0,    41,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,     0,     0,     0,     0,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,     0,
       0,     0,    23,    24,    25,    26,    27,    28,    29,     0,
      30,     0,     0,    43,    31,    32,    33,     0,     0,    44,
      45,    46,    47,     0,    34,     0,    35,    36,    37,    38,
      34,     0,    35,    36,    37,    38,     0,     0,    39,     0,
       0,    41,    42,    34,    74,    35,    36,    37,    38,    34,
       0,    35,    36,    37,    38,     0,     0,    39,     0,     0,
       0,   117,     0,    74,    34,    96,    35,    36,    37,    38,
      34,     0,    35,    36,    37,    38,     0,     0,    39,     0,
       0,     0,     0,    43,    39,     0,     0,     0,     0,    44,
      45,    46,    47,     0,     0,    44,    45,    46,    47,     0,
       0,     0,    43,     0,     0,     0,     0,     0,    44,    45,
      46,    47,     0,     0,    44,    45,    46,    47,     0,     0,
       0,     0,     0,    43,     0,     0,     0,     0,     0,    44,
      45,    46,    47,     0,     0,    44,    45,    46,    47,    34,
       0,    35,    36,    37,    38,    34,     0,    35,    36,    37,
      38,     0,    34,   152,    35,    36,    37,    38,     0,   158,
       0,     0,     0,     0,     0,     0,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,    45,    46,    47,     0,     0,
      44,    45,    46,    47,     0,     0,     0,     0,     0,    46,
      47
};

static const yytype_int16 yycheck[] =
{
       8,    24,    32,    11,    32,    51,    11,   146,    39,    17,
      18,    19,    20,     4,    39,   146,    24,    25,    45,    29,
      30,    55,    52,    55,    32,    42,    55,    44,    45,    69,
       6,    39,    40,    48,    49,    40,    31,     8,    53,    56,
      11,    81,    36,    37,    52,    60,    17,    18,    19,    20,
     101,   190,     4,    24,    25,     8,    48,    49,    11,   190,
      51,    32,    70,    71,    72,    51,    74,    72,    60,    40,
       4,     8,   118,     4,    11,    18,    19,    20,   124,    32,
      33,    52,   145,   146,    94,   125,    39,    40,   219,    56,
     107,   108,   109,   110,    53,    32,    15,    16,    55,    52,
      51,    72,    21,    40,   145,   146,   145,   146,    55,     8,
      29,    30,    11,    61,    33,    52,    61,   157,    35,    72,
      61,    74,    48,    49,    53,    24,    25,   190,    61,     0,
       1,   139,    55,    32,    60,    72,   108,   145,   146,    55,
      56,    40,    48,    49,    55,   175,    55,   175,   171,   190,
      55,   190,   107,    52,    60,   218,   219,    58,   166,   169,
      58,    59,    52,   171,   172,    36,    37,   175,   139,     4,
     216,    70,    71,    72,   145,   146,    51,   218,   219,   218,
     219,    51,   190,    55,    56,   107,   139,    35,    59,    40,
      41,     6,   145,   146,    42,   166,    44,    45,    46,    47,
     171,   172,   139,   139,   175,    48,    49,    52,    56,   172,
     218,   219,   108,    48,    49,    48,    49,    60,    53,   190,
      58,    59,   175,   142,   143,    60,     8,    60,   147,    11,
     149,   150,   151,   152,    58,    59,   166,   190,   175,   158,
     139,    70,    71,   162,   163,   164,   165,   218,   219,    42,
      32,    44,    45,    46,    47,   217,   171,   176,    40,   107,
     108,   109,   110,    71,    -1,   218,   219,    -1,    -1,    -1,
      52,    -1,   171,   172,    -1,    -1,   175,    -1,    -1,    -1,
     199,    -1,   201,   202,   203,    -1,    -1,    -1,    70,    71,
      72,     1,    44,    45,    46,    47,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    45,    46,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,    -1,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,   139,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    -1,
      28,   101,    -1,   175,    32,    33,    34,   107,   108,   109,
     110,    -1,    -1,    -1,    42,    -1,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,
      -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    -1,
      28,    -1,    -1,   101,    32,    33,    34,    -1,    -1,   107,
     108,   109,   110,    -1,    42,    -1,    44,    45,    46,    47,
      42,    -1,    44,    45,    46,    47,    -1,    -1,    56,    -1,
      -1,    59,    60,    42,    56,    44,    45,    46,    47,    42,
      -1,    44,    45,    46,    47,    -1,    -1,    56,    -1,    -1,
      -1,    60,    -1,    56,    42,    58,    44,    45,    46,    47,
      42,    -1,    44,    45,    46,    47,    -1,    -1,    56,    -1,
      -1,    -1,    -1,   101,    56,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,    -1,    -1,   107,   108,   109,   110,    -1,
      -1,    -1,   101,    -1,    -1,    -1,    -1,    -1,   107,   108,
     109,   110,    -1,    -1,   107,   108,   109,   110,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,   110,    -1,    -1,   107,   108,   109,   110,    42,
      -1,    44,    45,    46,    47,    42,    -1,    44,    45,    46,
      47,    -1,    42,    56,    44,    45,    46,    47,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,   109,   110,    -1,    -1,
     107,   108,   109,   110,    -1,    -1,    -1,    -1,    -1,   109,
     110
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   115,     0,     1,    36,    37,    59,   116,   119,    39,
      39,   119,     1,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    20,    21,    22,    23,    24,    25,    26,
      28,    32,    33,    34,    42,    44,    45,    46,    47,    56,
      57,    59,    60,   101,   107,   108,   109,   110,   117,   118,
     120,   121,   123,   124,   125,   128,   130,   131,   132,   133,
     140,   141,   144,   145,   148,   150,   151,   152,   153,   154,
     155,   156,    57,   117,    56,   130,   131,   140,   144,   147,
     151,   154,   147,    44,    45,    46,    47,   127,   130,   131,
     134,   134,   134,   134,    35,   147,    58,    59,   126,    42,
     130,   131,   133,   137,   138,   140,   130,   131,   135,   136,
     140,    45,   129,   126,   147,   126,   147,    60,   121,   130,
     131,   140,   146,   148,   147,   154,    55,    55,    55,    55,
      56,    55,    56,   130,   154,   117,     6,    40,    41,    31,
     125,   124,     4,    53,     4,    51,    51,     4,    56,    48,
      49,   146,    56,   130,   140,   144,   149,   152,    56,   149,
     117,   154,    53,    48,    49,   146,    61,    59,   126,    35,
      61,    61,    61,    55,   122,   125,   146,    55,    55,   120,
     147,   147,   139,   147,   131,   132,   141,   142,   143,   145,
     154,   142,   143,   147,    55,   147,   147,   147,   147,    53,
     107,    48,    49,   146,   147,   108,   147,   147,   147,   147,
     127,    59,   126,    58,   138,   136,   121,    52,    51,    51,
     142,   143,   107,   147,   147,   147,   147,   108,   139,   154
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
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
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
        case 4:
#line 182 "parser.y"
    {(yyvsp[(2) - (2)].tuple)->label = 0; (yyvsp[(2) - (2)].tuple)->exechance = (yyvsp[(1) - (2)].numval) * 100;}
    break;

  case 5:
#line 184 "parser.y"
    {(yyvsp[(3) - (3)].tuple)->label = 0; (yyvsp[(3) - (3)].tuple)->exechance = (yyvsp[(1) - (3)].numval) * (yyvsp[(2) - (3)].numval);}
    break;

  case 6:
#line 186 "parser.y"
    {checklabel((yyvsp[(1) - (3)].numval)); (yyvsp[(3) - (3)].tuple)->label = (yyvsp[(1) - (3)].numval); (yyvsp[(3) - (3)].tuple)->exechance = (yyvsp[(2) - (3)].numval) * 100;}
    break;

  case 7:
#line 188 "parser.y"
    {checklabel((yyvsp[(1) - (4)].numval)); (yyvsp[(4) - (4)].tuple)->label = (yyvsp[(1) - (4)].numval); (yyvsp[(4) - (4)].tuple)->exechance = (yyvsp[(2) - (4)].numval) * (yyvsp[(3) - (4)].numval);}
    break;

  case 8:
#line 190 "parser.y"
    {/* AIS: catch errors which occur after the end of a statement
		    (highly likely when comments are being written, as the
		    start of them will be parsed as an UNKNOWN) */
		  yyerrok; yyclearin; cacsofar=0;
		  if(prevtuple) {prevtuple->type=SPLATTERED; splat(0);}
		  else splat(1); /* this is the first statement */
		}
    break;

  case 9:
#line 205 "parser.y"
    {(yyvsp[(1) - (1)].tuple)->onceagainflag = onceagain_NORMAL; prevtuple = (yyval.tuple) = (yyvsp[(1) - (1)].tuple);}
    break;

  case 10:
#line 207 "parser.y"
    {NEWFANGLED {(yyvsp[(1) - (2)].tuple)->onceagainflag = onceagain_ONCE;
			     prevtuple = (yyval.tuple) = (yyvsp[(1) - (2)].tuple);}}
    break;

  case 11:
#line 210 "parser.y"
    {NEWFANGLED {(yyvsp[(1) - (2)].tuple)->onceagainflag = onceagain_AGAIN;
			     prevtuple = (yyval.tuple) = (yyvsp[(1) - (2)].tuple);}}
    break;

  case 12:
#line 226 "parser.y"
    {(yyval.tuple) = (yyvsp[(1) - (1)].tuple);}
    break;

  case 13:
#line 228 "parser.y"
    {
  if(!multithread) ick_lose(IE405, iyylineno, (char*)NULL);
  NEWFANGLED{
  /*	(x)  DO a WHILE b
    is equivalent to
    #11 (l0) DO REINSTATE (l3) <weave on>
    #10 (l1) DO COME FROM (l2) AGAIN
     #9	     DO b
     #8	     DO COME FROM (l0)
     #7	     DO NOTHING
     #6	     DO NOTHING
     #5 (l2) DO NOTHING
     #4	     DO GIVE UP
     #3	     DO COME FROM (l0)
     #2 (x)  DO a
     #1 (l3) DON'T ABSTAIN FROM (l1) AGAIN <weave off> */
  tuple* temptuple;
  TARGET(temptuple, COME_FROM, lineuid+2);
  temptuple->label=lineuid+1; temptuple->preproc=1; /* #10 */
  TARGET(temptuple, COME_FROM, lineuid+0); temptuple->preproc=1; /* #8 */
  ACTION(temptuple, PREPROC, 0); temptuple->preproc=1; /* #7 */
  ACTION(temptuple, PREPROC, 0); temptuple->preproc=1; /* #6 */
  ACTION(temptuple, PREPROC, 0);
  temptuple->label=lineuid+2; temptuple->preproc=1; /* #5 */
  ACTION(temptuple, GIVE_UP, 0); temptuple->preproc=1; /* #4 */
  TARGET(temptuple, COME_FROM, lineuid+0); temptuple->preproc=1; /* #3 */
  TARGET(temptuple, REINSTATE, lineuid+3); temptuple->setweave=1;
  temptuple->label=lineuid+0; temptuple->preproc=1; /* #11 */
  TARGET(temptuple, ABSTAIN, lineuid+1); temptuple->label=lineuid+3; /* #1 */
  temptuple->preproc=1; temptuple->setweave=-1; temptuple->exechance=-100; 
  politesse += 3; /* Keep the politeness checker happy */
  ppinit(11); tupleswap(10,9); tupleswap(11,2); lineuid+=4; /* #2, #9 */
  tuples[ick_lineno-10].onceagainflag=onceagain_AGAIN;
  tuples[ick_lineno-1].onceagainflag=onceagain_AGAIN;
  (yyval.tuple)=&(tuples[ick_lineno-2]);
}}
    break;

  case 14:
#line 266 "parser.y"
    {GETLINENO; (yyval.numval) = 1;}
    break;

  case 15:
#line 267 "parser.y"
    {GETLINENO; (yyval.numval) = -1;}
    break;

  case 16:
#line 268 "parser.y"
    {NEWFANGLED {GETLINENO; (yyval.numval) = 101;}}
    break;

  case 17:
#line 269 "parser.y"
    {NEWFANGLED {GETLINENO; (yyval.numval) = -101;}}
    break;

  case 18:
#line 273 "parser.y"
    {ACTION((yyval.tuple), GETS,      cons(GETS,(yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node)));}
    break;

  case 19:
#line 274 "parser.y"
    {ACTION((yyval.tuple), RESIZE,    cons(RESIZE,(yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node)));}
    break;

  case 20:
#line 276 "parser.y"
    {/* AIS: This is for variableconstants, and an error otherwise.*/
	   if(variableconstants) ACTION((yyval.tuple), GETS, cons(GETS,(yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node)))
	     else {yyerrok; yyclearin; (yyval.tuple)=splat(1);}}
    break;

  case 21:
#line 279 "parser.y"
    {TARGET((yyval.tuple), NEXT,      (yyvsp[(1) - (2)].numval));}
    break;

  case 22:
#line 280 "parser.y"
    {ACTION((yyval.tuple), FORGET,    (yyvsp[(2) - (2)].node));}
    break;

  case 23:
#line 281 "parser.y"
    {ACTION((yyval.tuple), RESUME,    (yyvsp[(2) - (2)].node));}
    break;

  case 24:
#line 282 "parser.y"
    {ACTION((yyval.tuple), STASH,     rlist);}
    break;

  case 25:
#line 283 "parser.y"
    {ACTION((yyval.tuple), RETRIEVE,  rlist);}
    break;

  case 26:
#line 284 "parser.y"
    {ACTION((yyval.tuple), IGNORE,    rlist);}
    break;

  case 27:
#line 285 "parser.y"
    {ACTION((yyval.tuple), REMEMBER,  rlist);}
    break;

  case 28:
#line 286 "parser.y"
    {stbeginline=0; TARGET((yyval.tuple), ABSTAIN,   (yyvsp[(3) - (3)].numval));}
    break;

  case 29:
#line 287 "parser.y"
    {ACTION((yyval.tuple), DISABLE,   rlist);}
    break;

  case 30:
#line 288 "parser.y"
    {/* AIS */ NEWFANGLED {stbeginline=0; ACTARGET((yyval.tuple), FROM, (yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].numval));}}
    break;

  case 31:
#line 289 "parser.y"
    {/* AIS */ NEWFANGLED {(yyval.tuple) = newtuple(); (yyval.tuple)->type = MANYFROM; (yyval.tuple)->ick_lineno = thisline; \
	  {node* tempnode=newnode(); (yyval.tuple)->u.node = tempnode; tempnode->lval=(yyvsp[(2) - (4)].node); tempnode->rval=rlist; tempnode->opcode=MANYFROM;}}}
    break;

  case 32:
#line 291 "parser.y"
    {stbeginline=0; TARGET((yyval.tuple), REINSTATE, (yyvsp[(2) - (2)].numval));}
    break;

  case 33:
#line 292 "parser.y"
    {ACTION((yyval.tuple), ENABLE,    rlist);}
    break;

  case 34:
#line 293 "parser.y"
    {ACTION((yyval.tuple), WRITE_IN,  (yyvsp[(2) - (2)].node));}
    break;

  case 35:
#line 294 "parser.y"
    {ACTION((yyval.tuple), READ_OUT,  (yyvsp[(2) - (2)].node));}
    break;

  case 36:
#line 295 "parser.y"
    {ACTION((yyval.tuple), PIN,       (yyvsp[(2) - (2)].node));}
    break;

  case 37:
#line 296 "parser.y"
    {ACTION((yyval.tuple), GIVE_UP,   0);}
    break;

  case 38:
#line 297 "parser.y"
    {/* AIS */ NEWFANGLED {ACTION((yyval.tuple), GO_AHEAD,  0);}}
    break;

  case 39:
#line 298 "parser.y"
    {/* AIS */ NEWFANGLED {ACTION((yyval.tuple), GO_BACK,   0);}}
    break;

  case 40:
#line 299 "parser.y"
    {/* AIS */ NEWFANGLED {ACTION((yyval.tuple),TRY_AGAIN,0);}}
    break;

  case 41:
#line 300 "parser.y"
    {/* AIS: Modified */ NEWFANGLED {TARGET((yyval.tuple),COME_FROM,(yyvsp[(1) - (1)].numval));}}
    break;

  case 42:
#line 301 "parser.y"
    {/* AIS: COME FROM gerund */
				 NEWFANGLED{ACTION((yyval.tuple),GERUCOME,rlist);
				 compucomesused=1; gerucomesused=1;}}
    break;

  case 43:
#line 304 "parser.y"
    {/* AIS */NEWFANGLED {ACTION((yyval.tuple),COMPUCOME,(yyvsp[(2) - (2)].node));
				 compucomesused=1;}}
    break;

  case 44:
#line 307 "parser.y"
    {NEWFANGLED {TARGET((yyval.tuple),NEXTFROMLABEL,(yyvsp[(1) - (1)].numval));}
				 nextfromsused=1;}
    break;

  case 45:
#line 309 "parser.y"
    {NEWFANGLED{ACTION((yyval.tuple),NEXTFROMGERUND,rlist);
				 compucomesused=1; gerucomesused=1;}
				 nextfromsused=1;}
    break;

  case 46:
#line 312 "parser.y"
    {NEWFANGLED {ACTION((yyval.tuple),NEXTFROMEXPR,(yyvsp[(2) - (2)].node));
				 compucomesused=1; nextfromsused=1;}}
    break;

  case 47:
#line 315 "parser.y"
    {NEWFANGLED{ACTARGET((yyval.tuple),CREATE,(yyvsp[(2) - (2)].node),(yyvsp[(1) - (2)].numval)); cacsofar=0;}}
    break;

  case 48:
#line 316 "parser.y"
    {NEWFANGLED{ACTION((yyval.tuple),COMPUCREATE,
					     cons(INTERSECTION,(yyvsp[(2) - (3)].node),(yyvsp[(3) - (3)].node))); cacsofar=0;}}
    break;

  case 49:
#line 319 "parser.y"
    {NEWFANGLED{ACTARGET((yyval.tuple),CREATE,(yyvsp[(2) - (2)].node),(yyvsp[(1) - (2)].numval));
                                 cacsofar=0;}}
    break;

  case 50:
#line 322 "parser.y"
    {NEWFANGLED{ACTION((yyval.tuple),COMPUCREATE,
				 cons(INTERSECTION,(yyvsp[(2) - (3)].node),(yyvsp[(3) - (3)].node))); cacsofar=0;}}
    break;

  case 51:
#line 325 "parser.y"
    {NEWFANGLED {ACTION((yyval.tuple),UNKNOWN,(yyvsp[(1) - (1)].node)); cacsofar=0;}}
    break;

  case 52:
#line 327 "parser.y"
    {yyclearin; yyerrok; (yyval.tuple) = splat(1); cacsofar=0;}
    break;

  case 53:
#line 328 "parser.y"
    {yyclearin; yyerrok; (yyval.tuple) = splat(1); cacsofar=0;}
    break;

  case 54:
#line 333 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); intern(ick_TWOSPOT,cacsofar+++1601);}
    break;

  case 55:
#line 334 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));
					   intern(ick_TWOSPOT,cacsofar+++1601);}
    break;

  case 56:
#line 336 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 57:
#line 339 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 58:
#line 340 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 59:
#line 342 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 60:
#line 343 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 61:
#line 348 "parser.y"
    {(yyval.node)=cons(US_ELEM,0,(yyvsp[(1) - (1)].node));
				   if(createsused){
				     opoverused=1; if(!firstslat)
				       firstslat=(yyvsp[(1) - (1)].node); else
				       prevslat->nextslat=(yyvsp[(1) - (1)].node);
				     prevslat=(yyvsp[(1) - (1)].node);
				     (yyvsp[(1) - (1)].node)->nextslat=0;}}
    break;

  case 62:
#line 355 "parser.y"
    {(yyval.node)=cons(US_SCALAR,0,(yyvsp[(1) - (1)].node));
				   if(createsused){
				     opoverused=1; if(!firstslat)
				       firstslat=(yyvsp[(1) - (1)].node); else
				       prevslat->nextslat=(yyvsp[(1) - (1)].node);
				     prevslat=(yyvsp[(1) - (1)].node);
				     (yyvsp[(1) - (1)].node)->nextslat=0;}}
    break;

  case 63:
#line 362 "parser.y"
    {(yyval.node)=cons(US_EXPR,0,(yyvsp[(1) - (1)].node));
				   if(createsused){
				     opoverused=1; if(!firstslat)
				       firstslat=(yyvsp[(1) - (1)].node); else
				       prevslat->nextslat=(yyvsp[(1) - (1)].node);
				     prevslat=(yyvsp[(1) - (1)].node);
				     (yyvsp[(1) - (1)].node)->nextslat=0;}}
    break;

  case 64:
#line 369 "parser.y"
    {(yyval.node)=cons(US_ARRVAR,0,(yyvsp[(1) - (1)].node));}
    break;

  case 65:
#line 372 "parser.y"
    {(yyval.node)=newnode(); (yyval.node)->opcode=US_ID; (yyval.node)->constant=(yyvsp[(1) - (1)].numval);}
    break;

  case 66:
#line 377 "parser.y"
    {rlist = np = newnode(); np->constant = (yyvsp[(1) - (1)].numval);}
    break;

  case 67:
#line 379 "parser.y"
    {
		    np->rval = newnode();
		    np = np->rval;
		    np->constant = (yyvsp[(3) - (3)].numval);
		}
    break;

  case 72:
#line 392 "parser.y"
    {
		  (yyval.node) = newnode();
		  (yyval.node)->opcode = ick_TWOSPOT;
		  (yyval.node)->constant = intern(ick_TWOSPOT, (yyvsp[(2) - (2)].numval));
		}
    break;

  case 73:
#line 398 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = ick_ONESPOT;
		    (yyval.node)->constant = intern(ick_ONESPOT, (yyvsp[(2) - (2)].numval));
		}
    break;

  case 74:
#line 404 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = ick_TWOSPOT;
		    (yyval.node)->constant = intern(ick_TWOSPOT, (yyvsp[(2) - (2)].numval));
		}
    break;

  case 75:
#line 412 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = ick_TAIL;
		    (yyval.node)->constant = intern(ick_TAIL, (yyvsp[(2) - (2)].numval));
		}
    break;

  case 76:
#line 418 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = ick_HYBRID;
		    (yyval.node)->constant = intern(ick_HYBRID, (yyvsp[(2) - (2)].numval));
		}
    break;

  case 77:
#line 428 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = (yyvsp[(2) - (3)].numval);
		    (yyval.node)->rval = newnode();
		    (yyval.node)->rval->opcode = ick_TAIL;
		    (yyval.node)->rval->constant = intern(ick_TAIL, (yyvsp[(3) - (3)].numval));
		}
    break;

  case 78:
#line 436 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = (yyvsp[(2) - (3)].numval);
		    (yyval.node)->rval = newnode();
		    (yyval.node)->rval->opcode = ick_HYBRID;
		    (yyval.node)->rval->constant = intern(ick_HYBRID, (yyvsp[(3) - (3)].numval));
		}
    break;

  case 79:
#line 447 "parser.y"
    {
		    /* enforce the 16-bit constant constraint */
		    if ((unsigned int)(yyvsp[(2) - (2)].numval) > ick_Max_small)
			ick_lose(IE017, iyylineno, (char *)NULL);
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = MESH;
		    if(variableconstants) /* AIS */
		      (yyval.node)->constant = intern(MESH, (yyvsp[(2) - (2)].numval));
		    else
		      (yyval.node)->constant = (yyvsp[(2) - (2)].numval);
		}
    break;

  case 80:
#line 461 "parser.y"
    {rlist = np = (yyvsp[(1) - (1)].node);}
    break;

  case 81:
#line 462 "parser.y"
    {np = np->rval = (yyvsp[(3) - (3)].node);
							/* newnode(); */ }
    break;

  case 85:
#line 469 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));}
    break;

  case 86:
#line 470 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (1)].node),0);}
    break;

  case 91:
#line 476 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));}
    break;

  case 92:
#line 477 "parser.y"
    {(yyval.node)=cons(INTERSECTION,(yyvsp[(1) - (1)].node),0);}
    break;

  case 93:
#line 483 "parser.y"
    {(yyval.node) = cons(BY, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 94:
#line 484 "parser.y"
    {(yyval.node) = cons(BY, (yyvsp[(1) - (1)].node), 0);}
    break;

  case 95:
#line 488 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 96:
#line 489 "parser.y"
    {(yyval.node) = cons(SUB, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 97:
#line 491 "parser.y"
    {(yyval.node) = cons(SUB, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 98:
#line 493 "parser.y"
    {(yyval.node) = cons(INTERSECTION, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));}
    break;

  case 99:
#line 494 "parser.y"
    {(yyval.node) = cons(INTERSECTION, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));}
    break;

  case 100:
#line 496 "parser.y"
    {(yyval.node) = cons(INTERSECTION, (yyvsp[(1) - (1)].node), 0);}
    break;

  case 101:
#line 497 "parser.y"
    {(yyval.node) = cons(INTERSECTION, (yyvsp[(1) - (1)].node), 0);}
    break;

  case 102:
#line 498 "parser.y"
    {(yyval.node) = cons(INTERSECTION, (yyvsp[(1) - (1)].node), 0);}
    break;

  case 103:
#line 502 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 104:
#line 504 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (3)].node); (yyval.node)->rval = cons(SUB, (yyval.node)->rval, (yyvsp[(3) - (3)].node));}
    break;

  case 105:
#line 507 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (3)].node); (yyval.node)->rval = cons(SUB, (yyval.node)->rval, (yyvsp[(3) - (3)].node));}
    break;

  case 106:
#line 511 "parser.y"
    {(yyval.node) = newnode(); (yyval.node)->opcode = BADCHAR;
					 (yyval.node)->constant = (yyvsp[(1) - (1)].numval);}
    break;

  case 107:
#line 516 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 108:
#line 519 "parser.y"
    {(yyval.node) = cons(SELECT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 109:
#line 520 "parser.y"
    {(yyval.node) = cons(MINGLE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 110:
#line 521 "parser.y"
    {(yyval.node) = cons(UNKNOWNOP, (yyvsp[(2) - (3)].node),
					 cons(INTERSECTION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)));
   					 if(useickec && createsused) {
					  if(!firstslat) firstslat=(yyvsp[(1) - (3)].node);
                                          else prevslat->nextslat=(yyvsp[(1) - (3)].node);
				     	  (yyvsp[(1) - (3)].node)->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					  (yyvsp[(3) - (3)].node)->nextslat=0; opoverused=1;
  					  intern(ick_TWOSPOT, 1601);
					  intern(ick_TWOSPOT, 1602);
					  intern(ick_TWOSPOT, 1603);}}
    break;

  case 111:
#line 532 "parser.y"
    {NEWFANGLED{(yyval.node) = cons(SLAT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
					 opoverused=1; if(!firstslat)
					 firstslat=(yyvsp[(3) - (3)].node); else
					 prevslat->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					 (yyvsp[(3) - (3)].node)->nextslat=0;}}
    break;

  case 112:
#line 537 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 113:
#line 538 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 114:
#line 542 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 115:
#line 543 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 116:
#line 544 "parser.y"
    {(yyval.node) = cons(SELECT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 117:
#line 545 "parser.y"
    {(yyval.node) = cons(MINGLE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 118:
#line 546 "parser.y"
    {(yyval.node) = cons(UNKNOWNOP, (yyvsp[(2) - (3)].node),
					 cons(INTERSECTION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)));
   					 if(useickec && createsused) {
					  if(!firstslat) firstslat=(yyvsp[(1) - (3)].node);
                                          else prevslat->nextslat=(yyvsp[(1) - (3)].node);
				     	  (yyvsp[(1) - (3)].node)->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					  (yyvsp[(3) - (3)].node)->nextslat=0; opoverused=1;
  					  intern(ick_TWOSPOT, 1601);
					  intern(ick_TWOSPOT, 1602);
					  intern(ick_TWOSPOT, 1603);}}
    break;

  case 119:
#line 556 "parser.y"
    {NEWFANGLED{(yyval.node) = cons(SLAT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
					 opoverused=1; if(!firstslat)
					 firstslat=(yyvsp[(3) - (3)].node); else
					 prevslat->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					 (yyvsp[(3) - (3)].node)->nextslat=0;}}
    break;

  case 120:
#line 564 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 121:
#line 565 "parser.y"
    {(yyval.node) = cons(SELECT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 122:
#line 566 "parser.y"
    {(yyval.node) = cons(MINGLE, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));}
    break;

  case 123:
#line 567 "parser.y"
    {(yyval.node) = cons(UNKNOWNOP, (yyvsp[(2) - (3)].node),
					 cons(INTERSECTION, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)));
   					 if(useickec && createsused) {
					  if(!firstslat) firstslat=(yyvsp[(1) - (3)].node);
                                          else prevslat->nextslat=(yyvsp[(1) - (3)].node);
				     	  (yyvsp[(1) - (3)].node)->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					  (yyvsp[(3) - (3)].node)->nextslat=0; opoverused=1;
  					  intern(ick_TWOSPOT, 1601);
					  intern(ick_TWOSPOT, 1602);
					  intern(ick_TWOSPOT, 1603);}}
    break;

  case 124:
#line 577 "parser.y"
    {NEWFANGLED{(yyval.node) = cons(SLAT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
					 opoverused=1; if(!firstslat)
					 firstslat=(yyvsp[(3) - (3)].node); else
					 prevslat->nextslat=(yyvsp[(3) - (3)].node); prevslat=(yyvsp[(3) - (3)].node);
					 (yyvsp[(3) - (3)].node)->nextslat=0;}}
    break;

  case 125:
#line 582 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 126:
#line 583 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 127:
#line 587 "parser.y"
    {(yyval.numval)=MESH; }
    break;

  case 128:
#line 587 "parser.y"
    {(yyval.numval)=ick_ONESPOT;}
    break;

  case 129:
#line 587 "parser.y"
    {(yyval.numval)=ick_TWOSPOT;}
    break;

  case 130:
#line 592 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 131:
#line 595 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = (yyvsp[(2) - (3)].numval);
		    (yyval.node)->rval = newnode();
		    (yyval.node)->rval->opcode = (yyvsp[(1) - (3)].numval);
		    if((yyvsp[(1) - (3)].numval) == MESH) {
		      /* enforce the 16-bit constant constraint */
		      if ((unsigned int)(yyvsp[(3) - (3)].numval) > ick_Max_small)
			ick_lose(IE017, iyylineno, (char *)NULL);
		      if(variableconstants) /* AIS, patched by JH */
			(yyval.node)->rval->constant = intern(MESH, (yyvsp[(3) - (3)].numval));
		      else
			(yyval.node)->rval->constant = (yyvsp[(3) - (3)].numval);
		    }
		    else {
		      (yyval.node)->rval->constant = intern((yyvsp[(1) - (3)].numval), (yyvsp[(3) - (3)].numval));
		    }
		}
    break;

  case 132:
#line 617 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = (yyvsp[(2) - (4)].numval);
		    (yyval.node)->rval = (yyvsp[(3) - (4)].node);
		    DESTACKSPARKEARS;
		}
    break;

  case 133:
#line 624 "parser.y"
    {
		    (yyval.node) = newnode();
		    (yyval.node)->opcode = (yyvsp[(2) - (4)].numval);
		    (yyval.node)->rval = (yyvsp[(3) - (4)].node);
		    DESTACKSPARKEARS;
		}
    break;

  case 134:
#line 632 "parser.y"
    {(yyval.node) = (yyvsp[(2) - (3)].node); DESTACKSPARKEARS;}
    break;

  case 135:
#line 633 "parser.y"
    {(yyval.node) = (yyvsp[(2) - (3)].node); DESTACKSPARKEARS;}
    break;

  case 136:
#line 636 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 137:
#line 637 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 138:
#line 640 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 139:
#line 642 "parser.y"
    {(yyval.node)=newnode(); (yyval.node)->opcode = (yyvsp[(1) - (2)].numval); (yyval.node)->rval = (yyvsp[(2) - (2)].node);}
    break;

  case 140:
#line 651 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 141:
#line 653 "parser.y"
    {(yyval.node)=newnode(); (yyval.node)->opcode = (yyvsp[(1) - (2)].numval); (yyval.node)->rval = (yyvsp[(2) - (2)].node);}
    break;


/* Line 1267 of yacc.c.  */
#line 2746 "parser.c"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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
      if (yyn != YYPACT_NINF)
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
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


#line 664 "parser.y"


static tuple *splat(int gentuple)
/* try to recover from an invalid statement. */
{
    tuple *sp;
    int tok, i;
    extern ick_bool re_send_token;

    /*
     * The idea
     * here is to skip to the ick_next DO, PLEASE or label, then unget that token.
     * which we can do with a tricky flag on the lexer (re_send_token).
     */

    if(re_send_token == ick_TRUE) /* By AIS */
    {
      /* We're still cleaning up from the previous error. */
      return prevtuple;
    }

    /*	fprintf(stderr,"attempting to splat at line %d....\n",iyylineno); */
    /* AIS: Set the flag to true the first time round, false for subsequent
       iterations. That way, if the error was triggered on a DO or label,
       we use that token as the start of the next statement. */
    for(i = 0,re_send_token = ick_TRUE;;i++,re_send_token = ick_FALSE) {
	tok = lexer();
	if (!tok)
	{
	    re_send_token = ick_TRUE;
	    tok = ' ';		/* scanner must not see a NUL */
	    break;
	}
	else if (tok == DO || tok == PLEASE || tok == LABEL
		 /* AIS */ || tok == MAYBE) {
	    re_send_token = ick_TRUE;
	    break;
	}
    }
    /*
	fprintf(stderr,"found %d on line %d after %d other tokens.\n",
		tok,iyylineno,i);
     */

    /* generate a placeholder tuple for the text line */
    if(gentuple /* AIS */) {TARGET(sp, SPLATTERED, 0); prevtuple=sp;}
    else sp=NULL;

    return(sp);
}

/* parser.y ends here */

