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
     LEXERLEAF = 258
   };
#endif
/* Tokens.  */
#define LEXERLEAF 258




/* Copy the first part of user declarations.  */
#line 25 "oil.y"


#include "bconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Autoconf lets us know if we have _Bool available, usually. On DJGPP there's
   an occasional bug that I haven't managed to reproduce that leaves
   SIZEOF_BOOL as the null string, so detect that ick_first. */
#if 100 - SIZEOF__BOOL - 1 == 101
/* The bug happened, so play safe. */
  typedef int mybool;
#else
# if SIZEOF__BOOL > 0
  typedef _Bool mybool;
# else
  typedef int mybool;
# endif
#endif

/* Define strdup if it isn't available */
#ifndef HAVE_STRDUP
char* strdup(const char* s)
{
  char* t=malloc(strlen(s)+1);
  if(t) strcpy(t,s);
  return t;
}
#endif

/*
#define YYDEBUG 1
int yydebug=1;
*/

/* Each semantic value represents either a constraint on a node that needs to
   be true for the optimization template to match, or part of the replacement
   for the optimization template. */
struct ickstype
{
  unsigned depth;     /* The number of child nodes this is from the root */
  unsigned long path; /* Bits from the end of this number represent the
			 sequence of lvals (=0) or rvals (=1) this is from the
			 root; depth bits from the end are relevant, and the
			 bit nearest the root is least significant */
  const char* condition; /* Points to a constant string; the condition to generate
			 on that node. Could also point to something the lexer
			 mallocs, but then the lexer has to keep track of being
			 able to free it again. If this is NULL, it means that
			 there isn't a condition other than maybe mustbemesh */
  const char* nodetypename; /* The name of the type of node this condition matches,
		         or NULL for a LEXERLEAF */
  mybool usec;        /* Whether to append the value of c to the condition */
  mybool mustbemesh;  /* Must this node be a mesh or mesh32? */
  mybool cxneeded;    /* True means calculate c and x for the node and the
			 condition is on those; false means append 'condition'
			 to the node itself to form the condition and c and x
			 aren't needed */
  mybool width32;     /* Generate a 32-bit node? */
  mybool ublo;        /* Is this a UBLO (if set, generate conditions
			 to check width)? */
  unsigned long c;    /* The value to append to the condition */
  int replnum;        /* Number of this group for replacements */
  struct ickstype *n1;/* n1 and n2 are pointers to other conditions that */
  struct ickstype *n2;/* also have to be satisified */
};

#define YYSTYPE YYSTYPE

#define MAXOPTNAMELEN 64
char optname[MAXOPTNAMELEN]="undefined";

typedef struct ickstype *YYSTYPE;

void splitend(void);
void splitstart(void);

void treedepthup(YYSTYPE, mybool);
void treefree(YYSTYPE);
void gennodepath(unsigned, unsigned long);
mybool treeshapecond(YYSTYPE, mybool);
YYSTYPE treenscheck(YYSTYPE, YYSTYPE, int);
void treecxcond(YYSTYPE);
void treerepcount(YYSTYPE, int*);
void treerepgen(YYSTYPE, YYSTYPE*, int*);

int countgetchar(void);
int countungetc(int, FILE*);
int cgccol;
int cgcrow;

/* #defines for chaining together template expressions; here, s is the type
   of expression (e.g. select, bitwise and, unary and) that's chaining the
   expressions together and n is the nonterminal that's the rval */
#define BINARYEXPR(s,m,l,r,w) do{ \
                          m=malloc(sizeof(struct ickstype)); \
                          m->n1=l; \
                          m->n2=r; \
                          m->usec=0; \
			  m->condition="->opcode==" s; \
                          m->nodetypename=s; \
                          m->mustbemesh=0; \
                          m->cxneeded=0; \
			  m->depth=0; \
			  m->path=0; \
                          m->replnum=0; \
                          m->width32=w; \
                          m->ublo=0; \
			  treedepthup(m->n1,0); \
			  treedepthup(m->n2,1); \
                          } while(0)

#define UNARYEXPR(s,m,r,w,u) do{ \
                          m=malloc(sizeof(struct ickstype)); \
                          m->n1=0; \
                          m->n2=r; \
                          m->usec=0; \
			  m->condition="->opcode==" s; \
                          m->nodetypename=s; \
                          m->mustbemesh=0; \
                          m->cxneeded=0; \
			  m->depth=0; \
			  m->path=0; \
                          m->replnum=0; \
                          m->width32=w; \
                          m->ublo=u; \
			  treedepthup(m->n2,1); \
                        } while(0)

/* Error handling and lexing */
int yylex(void);
int yyerror(char const *);

/* Split the output file */
#define SPLITMAX 20
int splitcount=SPLITMAX;
int filenumber=0;
mybool inloop=0;


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
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 254 "oil-oil.c"

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
#define YYLAST   345

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  89
/* YYNRULES -- Number of states.  */
#define YYNSTATES  244

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   258

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    29,     9,     2,    11,    28,    17,    10,
       7,     8,    26,    25,     2,     4,     2,    27,     2,    13,
      16,    15,    22,    23,    14,     2,     2,     2,     2,     2,
       6,    30,     5,    19,    21,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    18,     2,     2,     2,
       2,     2,     2,     2,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    24,     2,    12,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    12,    17,    19,    21,    25,
      29,    33,    35,    37,    41,    45,    51,    57,    62,    67,
      72,    77,    82,    87,    92,    97,   102,   108,   114,   120,
     126,   131,   137,   143,   149,   155,   159,   165,   171,   175,
     181,   187,   191,   197,   203,   207,   213,   219,   223,   229,
     235,   239,   245,   251,   255,   261,   267,   271,   277,   283,
     287,   293,   299,   303,   309,   315,   320,   325,   330,   337,
     344,   349,   356,   363,   368,   375,   382,   387,   394,   401,
     406,   413,   420,   425,   432,   439,   442,   447,   452,   454
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      32,     0,    -1,    -1,    32,    33,    -1,    34,     4,     5,
      38,    -1,     6,     3,     4,     3,    -1,     5,    -1,    35,
      -1,     7,    37,     8,    -1,     9,    37,     9,    -1,    10,
      37,    10,    -1,    35,    -1,     3,    -1,    36,    11,    36,
      -1,    36,    12,    36,    -1,    36,    12,    13,    14,    36,
      -1,    36,    12,    15,    16,    36,    -1,    17,    13,    14,
      36,    -1,    17,    15,    16,    36,    -1,    18,    13,    14,
      36,    -1,    18,    15,    16,    36,    -1,    19,    13,    14,
      36,    -1,    19,    15,    16,    36,    -1,    20,    13,    14,
      36,    -1,    20,    15,    16,    36,    -1,    21,    13,    14,
      36,    -1,    21,    16,    13,    14,    36,    -1,    21,    15,
      13,    14,    36,    -1,    21,    22,    13,    14,    36,    -1,
      21,    23,    13,    14,    36,    -1,    21,    15,    16,    36,
      -1,    21,    16,    15,    16,    36,    -1,    21,    15,    15,
      16,    36,    -1,    21,    22,    15,    16,    36,    -1,    21,
      23,    15,    16,    36,    -1,    36,    17,    36,    -1,    36,
      17,    13,    14,    36,    -1,    36,    17,    15,    16,    36,
      -1,    36,    24,    36,    -1,    36,    24,    13,    14,    36,
      -1,    36,    24,    15,    16,    36,    -1,    36,    20,    36,
      -1,    36,    20,    13,    14,    36,    -1,    36,    20,    15,
      16,    36,    -1,    36,    25,    36,    -1,    36,    25,    13,
      14,    36,    -1,    36,    25,    15,    16,    36,    -1,    36,
       4,    36,    -1,    36,     4,    13,    14,    36,    -1,    36,
       4,    15,    16,    36,    -1,    36,    26,    36,    -1,    36,
      26,    13,    14,    36,    -1,    36,    26,    15,    16,    36,
      -1,    36,    27,    36,    -1,    36,    27,    13,    14,    36,
      -1,    36,    27,    15,    16,    36,    -1,    36,    28,    36,
      -1,    36,    28,    13,    14,    36,    -1,    36,    28,    15,
      16,    36,    -1,    36,     5,    36,    -1,    36,     5,    13,
      14,    36,    -1,    36,     5,    15,    16,    36,    -1,    36,
       6,    36,    -1,    36,     6,    13,    14,    36,    -1,    36,
       6,    15,    16,    36,    -1,    12,    13,    14,    36,    -1,
      12,    15,    16,    36,    -1,    36,    29,    30,    36,    -1,
      36,    29,    30,    13,    14,    36,    -1,    36,    29,    30,
      15,    16,    36,    -1,    36,    30,    30,    36,    -1,    36,
      30,    30,    13,    14,    36,    -1,    36,    30,    30,    15,
      16,    36,    -1,    36,    17,    17,    36,    -1,    36,    17,
      17,    13,    14,    36,    -1,    36,    17,    17,    15,    16,
      36,    -1,    36,    24,    24,    36,    -1,    36,    24,    24,
      13,    14,    36,    -1,    36,    24,    24,    15,    16,    36,
      -1,    36,     5,     5,    36,    -1,    36,     5,     5,    13,
      14,    36,    -1,    36,     5,     5,    15,    16,    36,    -1,
      36,     6,     6,    36,    -1,    36,     6,     6,    13,    14,
      36,    -1,    36,     6,     6,    15,    16,    36,    -1,    29,
      36,    -1,    29,    13,    14,    36,    -1,    29,    15,    16,
      36,    -1,    36,    -1,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   198,   198,   198,   200,   258,   270,   272,   274,   275,
     276,   278,   279,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   354,   355,   356,   358
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LEXERLEAF", "'-'", "'>'", "'<'", "'('",
  "')'", "'\"'", "'''", "'$'", "'~'", "'1'", "'6'", "'3'", "'2'", "'&'",
  "'V'", "'?'", "'^'", "'@'", "'4'", "'5'", "'|'", "'+'", "'*'", "'/'",
  "'%'", "'!'", "'='", "$accept", "input", "optimization", "template",
  "expr3", "expr", "expr2", "replacement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,    45,    62,    60,    40,    41,    34,
      39,    36,   126,    49,    54,    51,    50,    38,    86,    63,
      94,    64,    52,    53,   124,    43,    42,    47,    37,    33,
      61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    31,    32,    32,    33,    33,    33,    34,    35,    35,
      35,    36,    36,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    38
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     4,     4,     1,     1,     3,     3,
       3,     1,     1,     3,     3,     5,     5,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     5,     5,     5,     5,
       4,     5,     5,     5,     5,     3,     5,     5,     3,     5,
       5,     3,     5,     5,     3,     5,     5,     3,     5,     5,
       3,     5,     5,     3,     5,     5,     3,     5,     5,     3,
       5,     5,     3,     5,     5,     4,     4,     4,     6,     6,
       4,     6,     6,     4,     6,     6,     4,     6,     6,     4,
       6,     6,     4,     6,     6,     2,     4,     4,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     6,     0,     0,     0,     0,     3,     0,
       7,     0,    12,     0,     0,     0,     0,     0,     0,     0,
      11,    88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
       9,    10,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    47,     0,
       0,     0,    59,     0,     0,     0,    62,    13,     0,     0,
      14,     0,     0,     0,    35,     0,     0,    41,     0,     0,
       0,    38,     0,     0,    44,     0,     0,    50,     0,     0,
      53,     0,     0,    56,     0,     0,    89,     4,    65,    66,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
       0,    30,     0,     0,     0,     0,     0,     0,    86,    87,
       0,     0,     0,     0,    79,     0,     0,     0,     0,    82,
       0,     0,     0,     0,     0,     0,     0,     0,    73,     0,
       0,     0,     0,     0,     0,    76,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    67,     0,     0,    70,
      27,    32,    26,    31,    28,    33,    29,    34,    48,    49,
       0,     0,    60,    61,     0,     0,    63,    64,    15,    16,
      36,    37,     0,     0,    42,    43,    39,    40,     0,     0,
      45,    46,    51,    52,    54,    55,    57,    58,     0,     0,
       0,     0,    80,    81,    83,    84,    74,    75,    77,    78,
      68,    69,    71,    72
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     8,     9,    20,    21,    22,   127
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -20
static const yytype_int16 yypact[] =
{
     -20,    10,   -20,   -20,     1,   210,   210,   210,   -20,     3,
     -20,    17,   -20,   -10,    -2,    29,    63,   120,   192,    89,
     -20,    83,    31,    34,    46,    67,    65,    68,    74,    72,
      77,    87,    81,   101,   102,   105,   106,   115,    25,   125,
     134,   141,   116,   118,   -20,   188,    70,   162,    15,   231,
      54,   240,    -1,   249,   258,   267,   276,    51,   109,   -20,
     -20,   -20,   107,   -20,    15,    15,    15,    15,    15,    15,
      15,    15,    15,    15,    15,   152,   132,    15,   153,   139,
     156,   157,   160,   163,    15,    15,   164,   167,   -20,   285,
     166,   168,   -20,   294,   173,   172,   -20,   -20,   175,   174,
     -20,   178,   180,   303,   -20,   190,   186,   -20,   202,   205,
     312,   -20,   204,   207,   -20,   211,   208,   -20,   212,   216,
     -20,   219,   220,   -20,   321,   330,   -20,   -20,   -20,   -20,
     -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,    15,
      15,   -20,    15,    15,    15,    15,    15,    15,   -20,   -20,
      15,    15,   221,   226,   -20,    15,    15,   223,   229,   -20,
      15,    15,    15,    15,    15,    15,   234,   235,   -20,    15,
      15,    15,    15,   243,   238,   -20,    15,    15,    15,    15,
      15,    15,    15,    15,   246,   247,   -20,   252,   253,   -20,
     -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,
      15,    15,   -20,   -20,    15,    15,   -20,   -20,   -20,   -20,
     -20,   -20,    15,    15,   -20,   -20,   -20,   -20,    15,    15,
     -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,    15,    15,
      15,    15,   -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,
     -20,   -20,   -20,   -20
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -20,   -20,   -20,   -20,     0,   -19,    53,   -20
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      44,    10,    12,    27,    11,    28,     5,    25,     6,     7,
       2,    29,   108,    30,   109,     3,     4,     5,    12,     6,
       7,    26,     5,   110,     6,     7,    88,    92,    96,    97,
     100,   104,   107,   111,   114,   117,   120,   123,    75,    59,
      76,    77,    31,    60,    32,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,    61,    12,   141,    23,
      24,     5,   126,     6,     7,   148,   149,   101,    63,   102,
     154,   103,    62,    12,   159,    89,    33,     5,    34,     6,
       7,   124,    64,    90,   168,    91,    66,    45,    46,    47,
      65,   175,    12,    67,    48,    49,     5,    69,     6,     7,
      50,    68,    42,    51,    43,   186,   189,    52,    53,    54,
      55,    56,    57,    58,     5,    70,     6,     7,    71,    72,
     190,   191,    73,   192,   193,   194,   195,   196,   197,    74,
      84,   198,   199,    35,    85,    36,   202,   203,    78,   125,
      79,   206,   207,   208,   209,   210,   211,    80,   140,    81,
     214,   215,   216,   217,    82,   143,    83,   220,   221,   222,
     223,   224,   225,   226,   227,    12,   139,   142,    93,     5,
     144,     6,     7,   145,   146,    94,     0,    95,   150,   147,
     155,   232,   233,   151,   156,   234,   235,   160,   161,   162,
     163,    12,   164,   236,   237,     5,   165,     6,     7,   238,
     239,    86,   170,    87,   169,    37,     0,    38,    39,   240,
     241,   242,   243,    12,    40,    41,   171,     5,   176,     6,
       7,   172,    13,   177,   179,   178,   180,    14,    15,    16,
      17,    18,   181,   182,    12,   200,   183,   204,     5,    19,
       6,     7,   201,    12,    98,   205,    99,     5,   212,     6,
       7,   213,    12,   105,   219,   106,     5,   218,     6,     7,
     228,    12,   112,   229,   113,     5,   230,     6,     7,   231,
      12,   115,     0,   116,     5,     0,     6,     7,     0,    12,
     118,     0,   119,     5,     0,     6,     7,     0,    12,   121,
       0,   122,     5,     0,     6,     7,     0,    12,   152,     0,
     153,     5,     0,     6,     7,     0,    12,   157,     0,   158,
       5,     0,     6,     7,     0,    12,   166,     0,   167,     5,
       0,     6,     7,     0,    12,   173,     0,   174,     5,     0,
       6,     7,     0,    12,   184,     0,   185,     5,     0,     6,
       7,     0,     0,   187,     0,   188
};

static const yytype_int16 yycheck[] =
{
      19,     1,     3,    13,     3,    15,     7,     4,     9,    10,
       0,    13,    13,    15,    15,     5,     6,     7,     3,     9,
      10,     4,     7,    24,     9,    10,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    13,     8,
      15,    16,    13,     9,    15,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    10,     3,    77,     6,
       7,     7,    62,     9,    10,    84,    85,    13,     3,    15,
      89,    17,     5,     3,    93,     5,    13,     7,    15,     9,
      10,    30,    14,    13,   103,    15,    14,     4,     5,     6,
      16,   110,     3,    16,    11,    12,     7,    16,     9,    10,
      17,    14,    13,    20,    15,   124,   125,    24,    25,    26,
      27,    28,    29,    30,     7,    14,     9,    10,    16,    14,
     139,   140,    16,   142,   143,   144,   145,   146,   147,    14,
      14,   150,   151,    13,    16,    15,   155,   156,    13,    30,
      15,   160,   161,   162,   163,   164,   165,    13,    16,    15,
     169,   170,   171,   172,    13,    16,    15,   176,   177,   178,
     179,   180,   181,   182,   183,     3,    14,    14,     6,     7,
      14,     9,    10,    16,    14,    13,    -1,    15,    14,    16,
      14,   200,   201,    16,    16,   204,   205,    14,    16,    14,
      16,     3,    14,   212,   213,     7,    16,     9,    10,   218,
     219,    13,    16,    15,    14,    13,    -1,    15,    16,   228,
     229,   230,   231,     3,    22,    23,    14,     7,    14,     9,
      10,    16,    12,    16,    16,    14,    14,    17,    18,    19,
      20,    21,    16,    14,     3,    14,    16,    14,     7,    29,
       9,    10,    16,     3,    13,    16,    15,     7,    14,     9,
      10,    16,     3,    13,    16,    15,     7,    14,     9,    10,
      14,     3,    13,    16,    15,     7,    14,     9,    10,    16,
       3,    13,    -1,    15,     7,    -1,     9,    10,    -1,     3,
      13,    -1,    15,     7,    -1,     9,    10,    -1,     3,    13,
      -1,    15,     7,    -1,     9,    10,    -1,     3,    13,    -1,
      15,     7,    -1,     9,    10,    -1,     3,    13,    -1,    15,
       7,    -1,     9,    10,    -1,     3,    13,    -1,    15,     7,
      -1,     9,    10,    -1,     3,    13,    -1,    15,     7,    -1,
       9,    10,    -1,     3,    13,    -1,    15,     7,    -1,     9,
      10,    -1,    -1,    13,    -1,    15
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    32,     0,     5,     6,     7,     9,    10,    33,    34,
      35,     3,     3,    12,    17,    18,    19,    20,    21,    29,
      35,    36,    37,    37,    37,     4,     4,    13,    15,    13,
      15,    13,    15,    13,    15,    13,    15,    13,    15,    16,
      22,    23,    13,    15,    36,     4,     5,     6,    11,    12,
      17,    20,    24,    25,    26,    27,    28,    29,    30,     8,
       9,    10,     5,     3,    14,    16,    14,    16,    14,    16,
      14,    16,    14,    16,    14,    13,    15,    16,    13,    15,
      13,    15,    13,    15,    14,    16,    13,    15,    36,     5,
      13,    15,    36,     6,    13,    15,    36,    36,    13,    15,
      36,    13,    15,    17,    36,    13,    15,    36,    13,    15,
      24,    36,    13,    15,    36,    13,    15,    36,    13,    15,
      36,    13,    15,    36,    30,    30,    35,    38,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    14,
      16,    36,    14,    16,    14,    16,    14,    16,    36,    36,
      14,    16,    13,    15,    36,    14,    16,    13,    15,    36,
      14,    16,    14,    16,    14,    16,    13,    15,    36,    14,
      16,    14,    16,    13,    15,    36,    14,    16,    14,    16,
      14,    16,    14,    16,    13,    15,    36,    13,    15,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      14,    16,    36,    36,    14,    16,    36,    36,    36,    36,
      36,    36,    14,    16,    36,    36,    36,    36,    14,    16,
      36,    36,    36,    36,    36,    36,    36,    36,    14,    16,
      14,    16,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36
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
#line 201 "oil.y"
    {
  static YYSTYPE tempmem[10];
  static int replcount[10];
  /* Handle splitting the file. */
  if(splitcount) splitcount--;
  if(!splitcount && !inloop)
  {
    splitcount=SPLITMAX;
    splitend();
    ++filenumber;
    splitstart();
  }
  /* This is where we actually generate the optimizer code. */
  /* Tree-shape and is-constant conditions */
  printf("  checknodeactbits(np);\n");
  if(treeshapecond((yyvsp[(1) - (4)]),1)) printf("  if(1"); printf(")\n  do\n  {\n");
  /* Nodesame and cxdata conditions */
  {
    int i=10;
    YYSTYPE temp;
    while(--i)
    {
      temp=treenscheck((yyvsp[(1) - (4)]),0,i);
      if(temp)
      {
	printf("    x%d=np",i);
	gennodepath(temp->depth,temp->path);
	printf("->constant; c%d=np",i);
	gennodepath(temp->depth,temp->path);
	printf("->optdata;\n");
      }
      tempmem[i]=temp;
      replcount[i]=0; /* we need to zero this somewhere, may as well be here */
    }
    treecxcond((yyvsp[(1) - (4)]));
  }
  /* If we reach this point in the generated code, we have an optimizer
     template match. */
  printf("    OPTING(%s);\n",optname);

  /* We now need to replace np with its replacement. This is done by creating
     a new nodetree, copying across tempmem'd nodes where necessary, and then
     substituting one for the other. (This is an inefficient but general way
     to do this.) One special case is needed; because pointers into the root
     node need to continue pointing there, the temporary node tp is copied
     member-for-member and then freed again. To make coding optimizations
     easier, the root stays as the same width no matter what. */
  printf("    tp=newnode();\n");
  treerepcount((yyvsp[(4) - (4)]),replcount);
  treerepgen((yyvsp[(4) - (4)]),tempmem,replcount);
  printf("    nodefree(np->lval); nodefree(np->rval);\n");
  printf("    tempw=np->width; *np=*tp; np->width=tempw; free(tp);\n");
  printf("  } while(0);\n\n");
  /* Free the template and replacement now they're finished being used. */
  treefree((yyvsp[(1) - (4)]));
  treefree((yyvsp[(4) - (4)]));
}
    break;

  case 5:
#line 259 "oil.y"
    {
  if(!(yyvsp[(2) - (4)])->mustbemesh||!(yyvsp[(4) - (4)])->mustbemesh)
  {
    yyerror("syntax error in <#..#()->()> construct");
    free((yyvsp[(2) - (4)])); free((yyvsp[(4) - (4)]));
    YYERROR;
  }
  printf("  r=%luLU; while(r<=%luLU) {\n",(yyvsp[(2) - (4)])->c,(yyvsp[(4) - (4)])->c);
  free((yyvsp[(2) - (4)])); free((yyvsp[(4) - (4)]));
  inloop=1;
}
    break;

  case 6:
#line 270 "oil.y"
    {printf("  r++;\n  }\n"); inloop=0;}
    break;

  case 8:
#line 274 "oil.y"
    {(yyval)=(yyvsp[(2) - (3)]);}
    break;

  case 9:
#line 275 "oil.y"
    {(yyval)=(yyvsp[(2) - (3)]);}
    break;

  case 10:
#line 276 "oil.y"
    {(yyval)=(yyvsp[(2) - (3)]);}
    break;

  case 13:
#line 281 "oil.y"
    {BINARYEXPR("MINGLE",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 14:
#line 282 "oil.y"
    {BINARYEXPR("SELECT",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 15:
#line 283 "oil.y"
    {BINARYEXPR("SELECT",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 16:
#line 284 "oil.y"
    {BINARYEXPR("SELECT",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 17:
#line 285 "oil.y"
    {UNARYEXPR("AND",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 18:
#line 286 "oil.y"
    {UNARYEXPR("AND",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 19:
#line 287 "oil.y"
    {UNARYEXPR("OR",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 20:
#line 288 "oil.y"
    {UNARYEXPR("OR",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 21:
#line 289 "oil.y"
    {UNARYEXPR("XOR",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 22:
#line 290 "oil.y"
    {UNARYEXPR("XOR",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 23:
#line 291 "oil.y"
    {UNARYEXPR("FIN",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 24:
#line 292 "oil.y"
    {UNARYEXPR("FIN",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 25:
#line 293 "oil.y"
    {UNARYEXPR("WHIRL",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 26:
#line 294 "oil.y"
    {UNARYEXPR("WHIRL2",(yyval),(yyvsp[(5) - (5)]),0,1);}
    break;

  case 27:
#line 295 "oil.y"
    {UNARYEXPR("WHIRL3",(yyval),(yyvsp[(5) - (5)]),0,1);}
    break;

  case 28:
#line 296 "oil.y"
    {UNARYEXPR("WHIRL4",(yyval),(yyvsp[(5) - (5)]),0,1);}
    break;

  case 29:
#line 297 "oil.y"
    {UNARYEXPR("WHIRL5",(yyval),(yyvsp[(5) - (5)]),0,1);}
    break;

  case 30:
#line 298 "oil.y"
    {UNARYEXPR("WHIRL",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 31:
#line 299 "oil.y"
    {UNARYEXPR("WHIRL2",(yyval),(yyvsp[(5) - (5)]),1,1);}
    break;

  case 32:
#line 300 "oil.y"
    {UNARYEXPR("WHIRL3",(yyval),(yyvsp[(5) - (5)]),1,1);}
    break;

  case 33:
#line 301 "oil.y"
    {UNARYEXPR("WHIRL4",(yyval),(yyvsp[(5) - (5)]),1,1);}
    break;

  case 34:
#line 302 "oil.y"
    {UNARYEXPR("WHIRL5",(yyval),(yyvsp[(5) - (5)]),1,1);}
    break;

  case 35:
#line 303 "oil.y"
    {BINARYEXPR("C_AND",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 36:
#line 304 "oil.y"
    {BINARYEXPR("C_AND",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 37:
#line 305 "oil.y"
    {BINARYEXPR("C_AND",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 38:
#line 306 "oil.y"
    {BINARYEXPR("C_OR",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 39:
#line 307 "oil.y"
    {BINARYEXPR("C_OR",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 40:
#line 308 "oil.y"
    {BINARYEXPR("C_OR",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 41:
#line 309 "oil.y"
    {BINARYEXPR("C_XOR",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 42:
#line 310 "oil.y"
    {BINARYEXPR("C_XOR",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 43:
#line 311 "oil.y"
    {BINARYEXPR("C_XOR",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 44:
#line 312 "oil.y"
    {BINARYEXPR("C_PLUS",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 45:
#line 313 "oil.y"
    {BINARYEXPR("C_PLUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 46:
#line 314 "oil.y"
    {BINARYEXPR("C_PLUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 47:
#line 315 "oil.y"
    {BINARYEXPR("C_MINUS",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 48:
#line 316 "oil.y"
    {BINARYEXPR("C_MINUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 49:
#line 317 "oil.y"
    {BINARYEXPR("C_MINUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 50:
#line 318 "oil.y"
    {BINARYEXPR("C_TIMES",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 51:
#line 319 "oil.y"
    {BINARYEXPR("C_TIMES",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 52:
#line 320 "oil.y"
    {BINARYEXPR("C_TIMES",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 53:
#line 321 "oil.y"
    {BINARYEXPR("C_DIVIDEBY",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 54:
#line 322 "oil.y"
    {BINARYEXPR("C_DIVIDEBY",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 55:
#line 323 "oil.y"
    {BINARYEXPR("C_DIVIDEBY",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 56:
#line 324 "oil.y"
    {BINARYEXPR("C_MODULUS",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 57:
#line 325 "oil.y"
    {BINARYEXPR("C_MODULUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 58:
#line 326 "oil.y"
    {BINARYEXPR("C_MODULUS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 59:
#line 327 "oil.y"
    {BINARYEXPR("C_GREATER",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 60:
#line 328 "oil.y"
    {BINARYEXPR("C_GREATER",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 61:
#line 329 "oil.y"
    {BINARYEXPR("C_GREATER",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 62:
#line 330 "oil.y"
    {BINARYEXPR("C_LESS",(yyval),(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),1);}
    break;

  case 63:
#line 331 "oil.y"
    {BINARYEXPR("C_LESS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),0);}
    break;

  case 64:
#line 332 "oil.y"
    {BINARYEXPR("C_LESS",(yyval),(yyvsp[(1) - (5)]),(yyvsp[(5) - (5)]),1);}
    break;

  case 65:
#line 333 "oil.y"
    {UNARYEXPR("C_NOT",(yyval),(yyvsp[(4) - (4)]),0,1);}
    break;

  case 66:
#line 334 "oil.y"
    {UNARYEXPR("C_NOT",(yyval),(yyvsp[(4) - (4)]),1,1);}
    break;

  case 67:
#line 335 "oil.y"
    {BINARYEXPR("C_NOTEQUAL",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),0);}
    break;

  case 68:
#line 336 "oil.y"
    {BINARYEXPR("C_NOTEQUAL",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 69:
#line 337 "oil.y"
    {BINARYEXPR("C_NOTEQUAL",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 70:
#line 338 "oil.y"
    {BINARYEXPR("C_ISEQUAL",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),0);}
    break;

  case 71:
#line 339 "oil.y"
    {BINARYEXPR("C_ISEQUAL",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 72:
#line 340 "oil.y"
    {BINARYEXPR("C_ISEQUAL",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 73:
#line 341 "oil.y"
    {BINARYEXPR("C_LOGICALAND",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),0);}
    break;

  case 74:
#line 342 "oil.y"
    {BINARYEXPR("C_LOGICALAND",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 75:
#line 343 "oil.y"
    {BINARYEXPR("C_LOGICALAND",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 76:
#line 344 "oil.y"
    {BINARYEXPR("C_LOGICALOR",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),0);}
    break;

  case 77:
#line 345 "oil.y"
    {BINARYEXPR("C_LOGICALOR",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 78:
#line 346 "oil.y"
    {BINARYEXPR("C_LOGICALOR",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 79:
#line 347 "oil.y"
    {BINARYEXPR("C_RSHIFTBY",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),1);}
    break;

  case 80:
#line 348 "oil.y"
    {BINARYEXPR("C_RSHIFTBY",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 81:
#line 349 "oil.y"
    {BINARYEXPR("C_RSHIFTBY",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 82:
#line 350 "oil.y"
    {BINARYEXPR("C_LSHIFTBY",(yyval),(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]),1);}
    break;

  case 83:
#line 351 "oil.y"
    {BINARYEXPR("C_LSHIFTBY",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),0);}
    break;

  case 84:
#line 352 "oil.y"
    {BINARYEXPR("C_LSHIFTBY",(yyval),(yyvsp[(1) - (6)]),(yyvsp[(6) - (6)]),1);}
    break;

  case 85:
#line 353 "oil.y"
    {UNARYEXPR("C_LOGICALNOT",(yyval),(yyvsp[(2) - (2)]),0,0);}
    break;

  case 86:
#line 354 "oil.y"
    {UNARYEXPR("C_LOGICALNOT",(yyval),(yyvsp[(4) - (4)]),0,0);}
    break;

  case 87:
#line 355 "oil.y"
    {UNARYEXPR("C_LOGICALNOT",(yyval),(yyvsp[(4) - (4)]),1,0);}
    break;


/* Line 1267 of yacc.c.  */
#line 2127 "oil-oil.c"
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


#line 360 "oil.y"


#define MAXTOFREE 1000

char* tofree[MAXTOFREE]={0};
int tfi=0;

int yylex(void)
{
  int c;
  unsigned long acc;

  /* Whitespace is completely insignificant here, even inside && and other
     two-character operators. Just to be different, though, it /is/ significant
     inside constructs like .1 and #{1}2; in such cases, it isn't allowed. */
  c=countgetchar();
  while(isspace(c)) c=countgetchar();
  while(c==';'||c=='[')
  {
    /* Comments go from a semicolon/hybrid to the end of the line. */
    if(c==';')
    {
      c=countgetchar();
      while(c!='\n') c=countgetchar();
      while(isspace(c)) c=countgetchar();
    }
    /* Square brackets set the name for optimizations. */
    if(c=='[')
    {
      int i=0;
      c=countgetchar();
      while(c!=']')
      {
	optname[i++]=c;
	c=countgetchar();
	if(i==MAXOPTNAMELEN-1) {i=0; yyerror("optimization name too long");}
      }
      optname[i]=0;
      c=countgetchar();
      while(isspace(c)) c=countgetchar();
    }
  }
  if(c==EOF) return 0;
  switch(c)
  {
  case '#':
    c=countgetchar();
    if(c!='{')
    {
      acc=0;
      while(isdigit(c))
      {
	acc*=10;
	acc+=(c-'0');
	c=countgetchar();
      }
      yylval=malloc(sizeof(struct ickstype));
      yylval->depth=0;
      yylval->path=0;
      yylval->condition="->constant==";
      yylval->nodetypename=0;
      yylval->usec=1;
      yylval->mustbemesh=1;
      yylval->cxneeded=0;
      yylval->c=acc;
      yylval->replnum=0;
      yylval->n1=0;
      yylval->n2=0;
      yylval->ublo=0;
      yylval->width32=1; /* generate MESH32 not MESH; we can still AND16 it,
			    etc., if necessary */
      countungetc(c, stdin);
      return LEXERLEAF;
    }
    countungetc(c, stdin);
    c='#';
    /* fall through */
  case '_':
  case ':':
  case '.':
    yylval=malloc(sizeof(struct ickstype));
    yylval->depth=0;
    yylval->path=0;
    yylval->condition=0; /* _ or # */
    yylval->width32=1; /* should never matter, but you never
			  know... */
    yylval->ublo=0;
    if(c==':') yylval->condition="->width==32";
    if(c=='.') {yylval->condition="->width==16"; yylval->width32=0;}
    yylval->nodetypename=0;
    yylval->usec=0;
    yylval->mustbemesh=c=='#';
    yylval->cxneeded=0;
    c=countgetchar();
    if(c=='{')
    {
      /* Create a new node to hold the c/x condition */
      yylval->n1=malloc(sizeof(struct ickstype));
      yylval->n1->depth=0;
      yylval->n1->path=0;
      {
	static char buf[512];
	int bi=0;
	c=countgetchar();
	while(c!='}')
	{
	  buf[bi++]=c;
	  if(bi==511) {yyerror("{quoted} string too long"); bi=0;}
	  c=countgetchar();
	}
	buf[bi]=0;
	yylval->n1->condition=tofree[tfi++]=strdup(buf);
	if(tfi==MAXTOFREE) {yyerror("Too many {quoted} strings"); tfi--;}
	c=countgetchar();
      }
      yylval->n1->nodetypename=0;
      yylval->n1->usec=0;
      yylval->n1->mustbemesh=0;
      yylval->n1->cxneeded=1;
      yylval->n1->n1=0;
      yylval->n1->n2=0;
      yylval->n1->width32=yylval->width32;
      yylval->n1->ublo=0;
    }
    else yylval->n1=0;
    yylval->replnum=0;
    if(yylval->n1) yylval->n1->replnum=c-'0'; else yylval->replnum=c-'0';
    yylval->n2=0;
    return LEXERLEAF;
  default:
    return c;
  }
}

void treedepthup(YYSTYPE v, mybool i)
{
  if(!v) return;
  treedepthup(v->n1,i);
  treedepthup(v->n2,i);
  v->depth++;
  v->path<<=1;
  v->path|=i;
  if(v->depth>30) yyerror("Nesting too deep in template or replacement\n");
}

void treefree(YYSTYPE v)
{
  if(!v) return;
  treefree(v->n1);
  treefree(v->n2);
  free(v);
}

void gennodepath(unsigned depth, unsigned long path)
{
  while(depth--)
  {
    if(path&1) printf("->rval"); else printf("->lval");
    path>>=1;
  }
}

mybool treeshapecond(YYSTYPE v, mybool firstopt)
{
  if(!v) return firstopt;
  /* To prevent possibly dereferencing a null pointer, check the root ick_first */
  if(v->mustbemesh) /* it's a must-be-constant constraint */
  {
    printf(firstopt?"  if((np":" &&\n     (np");
    gennodepath(v->depth,v->path);
    printf("->opcode==MESH || np");
    gennodepath(v->depth,v->path);
    printf("->opcode==MESH32)");
    firstopt=0;
  }
  if(v->condition&&!v->cxneeded) /* it's a tree-shape constraint */
  {
    printf(firstopt?"  if(np":" &&\n     np");
    gennodepath(v->depth,v->path);
    printf("%s",v->condition);
    if(v->usec) printf("%luLU",v->c);
    firstopt=0;
  }
  if(v->ublo) /* generate a width check */
  {
    printf(firstopt?"  if(np":" &&\n     np");
    gennodepath(v->depth,v->path);
    printf("->width==%d",v->width32?32:16);
    firstopt=0;
  }
  firstopt=treeshapecond(v->n1,firstopt);
  return treeshapecond(v->n2,firstopt);
}

YYSTYPE treenscheck(YYSTYPE v, YYSTYPE prev, int replnum)
{
  if(!v) return prev;
  prev=treenscheck(v->n1,prev,replnum);
  prev=treenscheck(v->n2,prev,replnum);
  if(v->replnum!=replnum) return prev;
  if(prev)
  {
    printf("    if(!nodessame(np");
    gennodepath(prev->depth,prev->path);
    printf(",np");
    gennodepath(v->depth,v->path);
    printf(")) break;\n");
  }
  return v;
}

void treecxcond(YYSTYPE v)
{
  if(!v) return;
  if(v->cxneeded&&strcmp(v->condition,"1"))
  {
    if(v->replnum)
      printf("    x=x%d; c=c%d; ",v->replnum,v->replnum);
    else
    {
      printf("    x=np");
      gennodepath(v->depth,v->path);
      printf("->constant;\n    c=np");
      gennodepath(v->depth,v->path);
      printf("->optdata;\n    ");
    }
    printf("if(!(%s)) break;\n",v->condition);
  }
  treecxcond(v->n1);
  treecxcond(v->n2);
}

void treerepcount(YYSTYPE v, int* rc)
{
  if(!v) return;
  if(!(v->nodetypename)&&v->replnum&&!(v->cxneeded)) rc[v->replnum]++;
  treerepcount(v->n1, rc);
  treerepcount(v->n2, rc);
}

void treerepgen(YYSTYPE v, YYSTYPE* refs, int* rc)
{
  if(!v) return;
  /* We absolutely have to generate the root node ick_first here, because otherwise
     the nodes in question won't exist. */
  if(v->nodetypename) /* Generate an intermediate node */
  {
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=%s;\n    tp",v->nodetypename);
    gennodepath(v->depth,v->path);
    printf("->width=%d;\n",v->width32?32:16);
    /* optdata will be filled in by checknodeactbits before the ick_next idiom is
       tested; constant is irrelevant, lval and rval are NULL at present and
       will be filled in by later recursions of this function, and I seriously
       hope that nextslat is never filled in by an optimizer idiom. */
  }
  else if(v->replnum&&!(v->cxneeded))
  {
    /* Copy a node from the template. The node ought not to be allocated at
       this point, so we can safely just ick_assign to it with a new malloced
       node. */
    if(refs[v->replnum])
    {
      if(rc[v->replnum]>1||!refs[v->replnum]->depth)
      {
	/* The node actually has to be copied, either because another copy is
	   needed or because it's np itself that's being copied over. */
	rc[v->replnum]--;
	printf("    tp");
	gennodepath(v->depth,v->path);
	printf("=nodecopy(np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf(");\n");
      }
      else
      {
	/* This can be optimized slightly by moving rather than copying,
	   zeroing backlinks so that the node won't be freed. */
	rc[v->replnum]--;
	printf("    tp");
	gennodepath(v->depth,v->path);
	printf("=np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf(";\n    np");
	gennodepath(refs[v->replnum]->depth,refs[v->replnum]->path);
	printf("=0;\n");
      }
    }
    else yyerror("Replacement isn't in the template");
  }
  else if(v->cxneeded)
  {
    /* Generate a constant node based on an expression (#{expr}0). */
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=MESH32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->width=32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->constant=(%s);\n",v->condition);
  }
  else if(v->mustbemesh&&!v->n1)
  {
    /* Generate a constant node based on a constant (#65535). */
    printf("    MAYBENEWNODE(tp");
    gennodepath(v->depth,v->path);
    printf(");\n    tp");
    gennodepath(v->depth,v->path);
    printf("->opcode=MESH32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->width=32;\n    tp");
    gennodepath(v->depth,v->path);
    printf("->constant=(%luLU);\n",v->c);
  }
  else if(v->mustbemesh&&v->n1)
    /* let this node's n1 handle it */ ;
  else
    yyerror("can't figure out how to generate a replacement node\n");
  treerepgen(v->n1,refs,rc);
  treerepgen(v->n2,refs,rc);
}

int yyerror(char const *s)
{
  fprintf(stderr,"Error at (%d:%d): \"%s\"\n",cgcrow,cgccol,s);
  return 0; /* this return value is ignored anyway */
}

static int cgcpushback=0;

int countgetchar(void)
{
  int c=getchar();
  if(cgcpushback) {cgcpushback=0; return c;}
  cgccol++;
  if(c=='\n') {cgccol=0; cgcrow++;}
  return c;
}

int countungetc(int c, FILE* f)
{
  ungetc(c,f);
  cgcpushback=1;
  return c;
}

void splitstart(void)
{
  static char fname[]="oilout00.c";
  if(filenumber>255)
  {
    filenumber=255;
    fprintf(stdout,"Input file too long.\n");
  }
  sprintf(fname,"oilout%02x.c",filenumber);
  freopen(fname,"w",stdout);
  puts("/* Automatically generated output, edit source and recompile to "
       "change */");
  printf("#include \"oil.h\"\n"
	 "int optimize_pass1_%x(node *np)\n"
	 "{"
	 "  int opted=0;\n"
	 "  unsigned long c,c1,c2,c3,c4,c5,c6,c7,c8,c9;\n"
	 "  unsigned long x,x1,x2,x3,x4,x5,x6,x7,x8,x9,r;\n"
	 "  int tempw;\n"
	 "  node *tp;\n", filenumber);
}

void splitend(void)
{
  /* Disabling warnings about unused variables. gcc will optimize this right
     out, and in any case the raise(SIGSEGV) will be unreachable (but will
     cause a pretty recognizable error because it'll be caught by the handler
     for SIGSEGV and output an internal error, with an obvious debug backtrace
     if -U is used). */
  printf("  c=c1=c2=c3=c4=c5=c6=c7=c8=c9=0;\n"
	 "  x=x1=x2=x3=x4=x5=x6=x7=x8=x9=r=0;\n"
	 "  if(c+c1+c2+c3+c4+c5+c6+c7+c8+c9+r+\n"
	 "     x+x1+x2+x3+x4+x5+x6+x7+x8+x9) raise(SIGSEGV);\n");
  printf("  return opted;\n}\n");
  fclose(stdout);
}

int main(void)
{
  int e,i;
  /*
    "  if(!np) return 0;\n"
    "  if(np->lval) opted|=optimize_pass1(np->lval);\n"
    "  if(np->rval) opted|=optimize_pass1(np->rval);\n"
  */
  splitstart();
  cgccol=0;
  cgcrow=1;
  e=yyparse();
  while(tfi--) free(tofree[tfi]);
  splitend();
  freopen("oilout-m.c","w",stdout);
  puts("/* Automatically generated output, edit source and recompile to "
       "change */");
  puts("#include \"ick.h\"");
  i=filenumber+1;
  while(i--) printf("extern int optimize_pass1_%x(node*);\n",i);
  puts("int optimize_pass1(node* np)\n"
       "{\n"
       "  int opted=0;\n"
       "  if(!np) return 0;\n"
       "  if(np->lval) opted|=optimize_pass1(np->lval);\n"
       "  if(np->rval) opted|=optimize_pass1(np->rval);");
  i=filenumber+1;
  while(i--) printf("  opted|=optimize_pass1_%x(np);\n",i);
  puts("  return opted;\n"
       "}");
  return e;
}

