/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 83 "parser.y"
{
    int		numval;		/* a numeric value */
    tuple	*tuple;		/* a code tuple */
    node	*node;		/* an expression-tree node */
}
/* Line 1489 of yacc.c.  */
#line 281 "parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

