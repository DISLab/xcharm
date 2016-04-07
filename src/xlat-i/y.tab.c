/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 3 "xi-grammar.y" /* yacc.c:339  */

#include <iostream>
#include <string>
#include <string.h>
#include "xi-symbol.h"
#include "sdag/constructs/Constructs.h"
#include "EToken.h"
#include "xi-Chare.h"

// Has to be a macro since YYABORT can only be used within rule actions.
#define ERROR(...) \
  if (xi::num_errors++ == xi::MAX_NUM_ERRORS) { \
    YYABORT;                                    \
  } else {                                      \
    xi::pretty_msg("error", __VA_ARGS__);       \
  }

#define WARNING(...) \
  if (enable_warnings) {                    \
    xi::pretty_msg("warning", __VA_ARGS__); \
  }

using namespace xi;
extern int yylex (void) ;
extern unsigned char in_comment;
extern unsigned int lineno;
extern int in_bracket,in_braces,in_int_expr;
extern std::list<Entry *> connectEntries;
AstChildren<Module> *modlist;

void yyerror(const char *);

namespace xi {

const int MAX_NUM_ERRORS = 10;
int num_errors = 0;

bool enable_warnings = true;

extern int macroDefined(const char *str, int istrue);
extern const char *python_doc;
extern char *fname;
void splitScopedName(const char* name, const char** scope, const char** basename);
void ReservedWord(int token);
}

#line 113 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    MODULE = 258,
    MAINMODULE = 259,
    EXTERN = 260,
    READONLY = 261,
    INITCALL = 262,
    INITNODE = 263,
    INITPROC = 264,
    PUPABLE = 265,
    CHARE = 266,
    MAINCHARE = 267,
    GROUP = 268,
    NODEGROUP = 269,
    ARRAY = 270,
    MESSAGE = 271,
    CONDITIONAL = 272,
    CLASS = 273,
    INCLUDE = 274,
    STACKSIZE = 275,
    THREADED = 276,
    TEMPLATE = 277,
    SYNC = 278,
    IGET = 279,
    EXCLUSIVE = 280,
    IMMEDIATE = 281,
    SKIPSCHED = 282,
    INLINE = 283,
    VIRTUAL = 284,
    MIGRATABLE = 285,
    CREATEHERE = 286,
    CREATEHOME = 287,
    NOKEEP = 288,
    NOTRACE = 289,
    APPWORK = 290,
    VOID = 291,
    CONST = 292,
    PACKED = 293,
    VARSIZE = 294,
    ENTRY = 295,
    FOR = 296,
    FORALL = 297,
    WHILE = 298,
    WHEN = 299,
    OVERLAP = 300,
    ATOMIC = 301,
    IF = 302,
    ELSE = 303,
    PYTHON = 304,
    LOCAL = 305,
    NAMESPACE = 306,
    USING = 307,
    IDENT = 308,
    NUMBER = 309,
    LITERAL = 310,
    CPROGRAM = 311,
    HASHIF = 312,
    HASHIFDEF = 313,
    INT = 314,
    LONG = 315,
    SHORT = 316,
    CHAR = 317,
    FLOAT = 318,
    DOUBLE = 319,
    UNSIGNED = 320,
    ACCEL = 321,
    READWRITE = 322,
    WRITEONLY = 323,
    ACCELBLOCK = 324,
    MEMCRITICAL = 325,
    REDUCTIONTARGET = 326,
    CASE = 327,
    UCHARE = 328,
    REENTRANT = 329
  };
#endif
/* Tokens.  */
#define MODULE 258
#define MAINMODULE 259
#define EXTERN 260
#define READONLY 261
#define INITCALL 262
#define INITNODE 263
#define INITPROC 264
#define PUPABLE 265
#define CHARE 266
#define MAINCHARE 267
#define GROUP 268
#define NODEGROUP 269
#define ARRAY 270
#define MESSAGE 271
#define CONDITIONAL 272
#define CLASS 273
#define INCLUDE 274
#define STACKSIZE 275
#define THREADED 276
#define TEMPLATE 277
#define SYNC 278
#define IGET 279
#define EXCLUSIVE 280
#define IMMEDIATE 281
#define SKIPSCHED 282
#define INLINE 283
#define VIRTUAL 284
#define MIGRATABLE 285
#define CREATEHERE 286
#define CREATEHOME 287
#define NOKEEP 288
#define NOTRACE 289
#define APPWORK 290
#define VOID 291
#define CONST 292
#define PACKED 293
#define VARSIZE 294
#define ENTRY 295
#define FOR 296
#define FORALL 297
#define WHILE 298
#define WHEN 299
#define OVERLAP 300
#define ATOMIC 301
#define IF 302
#define ELSE 303
#define PYTHON 304
#define LOCAL 305
#define NAMESPACE 306
#define USING 307
#define IDENT 308
#define NUMBER 309
#define LITERAL 310
#define CPROGRAM 311
#define HASHIF 312
#define HASHIFDEF 313
#define INT 314
#define LONG 315
#define SHORT 316
#define CHAR 317
#define FLOAT 318
#define DOUBLE 319
#define UNSIGNED 320
#define ACCEL 321
#define READWRITE 322
#define WRITEONLY 323
#define ACCELBLOCK 324
#define MEMCRITICAL 325
#define REDUCTIONTARGET 326
#define CASE 327
#define UCHARE 328
#define REENTRANT 329

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 52 "xi-grammar.y" /* yacc.c:355  */

  AstChildren<Module> *modlist;
  Module *module;
  ConstructList *conslist;
  Construct *construct;
  TParam *tparam;
  TParamList *tparlist;
  Type *type;
  PtrType *ptype;
  NamedType *ntype;
  FuncType *ftype;
  Readonly *readonly;
  Message *message;
  Chare *chare;
	UChare *uchare;
  Entry *entry;
  EntryList *entrylist;
  Parameter *pname;
  ParamList *plist;
  Template *templat;
  TypeList *typelist;
  AstChildren<Member> *mbrlist;
  Member *member;
  TVar *tvar;
  TVarList *tvarlist;
  Value *val;
  ValueList *vallist;
  MsgVar *mv;
  MsgVarList *mvlist;
  PUPableClass *pupable;
  IncludeFile *includeFile;
  const char *strval;
  int intval;
  unsigned int cattr; // actually Chare::attrib_t, but referring to that creates nasty #include issues
  SdagConstruct *sc;
  IntExprConstruct *intexpr;
  WhenConstruct *when;
  SListConstruct *slist;
  CaseListConstruct *clist;
  OListConstruct *olist;
  SdagEntryConstruct *sentry;
  XStr* xstrptr;
  AccelBlock* accelBlock;

#line 346 "y.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 375 "y.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  57
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1475

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  91
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  118
/* YYNRULES -- Number of rules.  */
#define YYNRULES  372
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  727

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,     2,
      83,    84,    82,     2,    79,    89,    90,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    75,
      80,    88,    81,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    86,     2,    87,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    77,     2,    78,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   197,   197,   202,   205,   210,   211,   216,   217,   222,
     224,   225,   226,   228,   229,   230,   232,   233,   234,   235,
     236,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   271,
     273,   274,   277,   278,   279,   280,   281,   282,   285,   287,
     294,   298,   305,   307,   312,   313,   317,   319,   321,   323,
     325,   337,   339,   341,   343,   349,   351,   353,   355,   357,
     359,   361,   363,   365,   367,   369,   371,   379,   381,   383,
     387,   389,   394,   395,   400,   401,   405,   407,   409,   411,
     413,   415,   417,   419,   421,   423,   425,   427,   429,   431,
     433,   437,   438,   445,   447,   451,   455,   457,   461,   465,
     467,   469,   471,   473,   475,   479,   481,   483,   485,   487,
     491,   493,   497,   499,   503,   507,   512,   513,   517,   521,
     526,   527,   532,   533,   543,   545,   549,   551,   556,   557,
     561,   563,   568,   569,   573,   578,   579,   583,   585,   589,
     591,   596,   597,   601,   602,   605,   609,   611,   615,   617,
     622,   623,   627,   629,   633,   635,   639,   643,   647,   653,
     657,   659,   663,   665,   669,   673,   677,   681,   683,   687,
     695,   705,   706,   711,   712,   714,   716,   725,   727,   729,
     733,   735,   739,   743,   745,   747,   749,   751,   755,   757,
     762,   769,   773,   775,   777,   778,   780,   782,   784,   788,
     790,   792,   798,   804,   813,   815,   817,   823,   831,   833,
     836,   840,   844,   846,   851,   853,   861,   863,   865,   867,
     869,   871,   873,   875,   877,   879,   881,   884,   894,   911,
     927,   929,   933,   938,   939,   941,   948,   950,   954,   956,
     958,   960,   962,   964,   966,   968,   970,   972,   974,   976,
     978,   980,   982,   984,   986,   988,   997,   999,  1001,  1006,
    1007,  1009,  1018,  1019,  1021,  1027,  1033,  1039,  1047,  1054,
    1062,  1069,  1071,  1073,  1075,  1082,  1083,  1084,  1087,  1088,
    1089,  1090,  1097,  1103,  1112,  1119,  1125,  1131,  1139,  1141,
    1145,  1147,  1151,  1153,  1157,  1159,  1164,  1165,  1169,  1171,
    1173,  1177,  1179,  1183,  1185,  1189,  1191,  1193,  1201,  1204,
    1207,  1209,  1211,  1215,  1217,  1219,  1221,  1223,  1225,  1227,
    1229,  1231,  1233,  1235,  1237,  1241,  1243,  1245,  1247,  1249,
    1251,  1253,  1256,  1259,  1261,  1263,  1265,  1267,  1269,  1280,
    1281,  1283,  1287,  1291,  1295,  1299,  1303,  1309,  1311,  1315,
    1318,  1322,  1326
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MODULE", "MAINMODULE", "EXTERN",
  "READONLY", "INITCALL", "INITNODE", "INITPROC", "PUPABLE", "CHARE",
  "MAINCHARE", "GROUP", "NODEGROUP", "ARRAY", "MESSAGE", "CONDITIONAL",
  "CLASS", "INCLUDE", "STACKSIZE", "THREADED", "TEMPLATE", "SYNC", "IGET",
  "EXCLUSIVE", "IMMEDIATE", "SKIPSCHED", "INLINE", "VIRTUAL", "MIGRATABLE",
  "CREATEHERE", "CREATEHOME", "NOKEEP", "NOTRACE", "APPWORK", "VOID",
  "CONST", "PACKED", "VARSIZE", "ENTRY", "FOR", "FORALL", "WHILE", "WHEN",
  "OVERLAP", "ATOMIC", "IF", "ELSE", "PYTHON", "LOCAL", "NAMESPACE",
  "USING", "IDENT", "NUMBER", "LITERAL", "CPROGRAM", "HASHIF", "HASHIFDEF",
  "INT", "LONG", "SHORT", "CHAR", "FLOAT", "DOUBLE", "UNSIGNED", "ACCEL",
  "READWRITE", "WRITEONLY", "ACCELBLOCK", "MEMCRITICAL", "REDUCTIONTARGET",
  "CASE", "UCHARE", "REENTRANT", "';'", "':'", "'{'", "'}'", "','", "'<'",
  "'>'", "'*'", "'('", "')'", "'&'", "'['", "']'", "'='", "'-'", "'.'",
  "$accept", "File", "ModuleEList", "OptExtern", "OptSemiColon", "Name",
  "QualName", "Module", "ConstructEList", "ConstructList", "ConstructSemi",
  "Construct", "TParam", "TParamList", "TParamEList", "OptTParams",
  "BuiltinType", "NamedType", "QualNamedType", "SimpleType", "OnePtrType",
  "PtrType", "FuncType", "BaseType", "BaseDataType", "RestrictedType",
  "Type", "ArrayDim", "Dim", "DimList", "Readonly", "ReadonlyMsg",
  "OptVoid", "MAttribs", "MAttribList", "MAttrib", "CAttribs",
  "CAttribList", "PythonOptions", "ArrayAttrib", "ArrayAttribs",
  "ArrayAttribList", "CAttrib", "OptConditional", "MsgArray", "Var",
  "VarList", "Message", "OptBaseList", "BaseList", "Chare", "Group",
  "NodeGroup", "ArrayIndexType", "Array", "TChare", "TGroup", "TNodeGroup",
  "TArray", "TMessage", "UChare", "UChareArray", "OptTypeInit",
  "OptNameInit", "TVar", "TVarList", "TemplateSpec", "Template",
  "MemberEList", "MemberList", "NonEntryMember", "InitNode", "InitProc",
  "PUPableClass", "IncludeFile", "Member", "MemberBody", "UnexpectedToken",
  "Entry", "AccelBlock", "EReturn", "EAttribs", "EAttribList", "EAttrib",
  "DefaultParameter", "CPROGRAM_List", "CCode", "ParamBracketStart",
  "ParamBraceStart", "ParamBraceEnd", "Parameter", "AccelBufferType",
  "AccelInstName", "AccelArrayParam", "AccelParameter", "ParamList",
  "AccelParamList", "EParameters", "AccelEParameters", "OptStackSize",
  "OptSdagCode", "Slist", "Olist", "CaseList", "OptTraceName",
  "WhenConstruct", "NonWhenConstruct", "SingleConstruct", "HasElse",
  "IntExpr", "EndIntExpr", "StartIntExpr", "SEntry", "SEntryList",
  "SParamBracketStart", "SParamBracketEnd", "HashIFComment",
  "HashIFDefComment", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,    59,    58,   123,   125,    44,
      60,    62,    42,    40,    41,    38,    91,    93,    61,    45,
      46
};
# endif

#define YYPACT_NINF -541

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-541)))

#define YYTABLE_NINF -324

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     144,  1312,  1312,    45,  -541,   144,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,   100,   100,  -541,  -541,  -541,
     438,  -541,  -541,  -541,    79,  1312,   148,  1312,  1312,   159,
     884,    33,   390,   438,  -541,  -541,  -541,  1392,    51,   114,
    -541,   108,  -541,  -541,  -541,  -541,   -25,   953,   158,   158,
      -4,   114,   134,   134,   134,   134,   175,   201,  1312,   236,
     228,     4,   438,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,   261,  -541,  -541,  -541,  -541,   238,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  1392,  -541,   165,  -541,  -541,  -541,
    -541,   260,   157,  -541,  -541,   249,   250,   255,    47,  -541,
     114,   438,   108,   263,   126,   -25,   264,   484,  1410,   249,
     250,   255,  -541,    70,   114,  -541,   114,   114,   267,   114,
     265,  -541,    69,  1312,  1312,  1312,  1312,  1096,   262,   266,
     286,  1312,  -541,  -541,  -541,   422,   262,  1312,   272,   134,
     134,   134,   134,   262,   201,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,   314,  -541,  -541,  -541,   205,  -541,  -541,  1379,
    -541,  -541,  -541,  -541,  -541,  -541,  1312,   287,   319,   -25,
     330,   -25,   291,  -541,   301,   308,    75,  -541,   309,  -541,
     -43,    87,    40,   305,    53,   114,  -541,  -541,   306,   321,
     328,   335,   335,   335,   335,  -541,  1312,   327,   337,   331,
    1168,  1312,   368,  1312,  -541,  -541,   332,   341,   345,  1312,
      71,  1312,   344,   343,   266,   335,   238,  1312,  1312,  1312,
    1312,  1312,  1312,  -541,  -541,  -541,  -541,   346,  -541,   347,
    -541,   328,  -541,  -541,   348,   350,   349,   374,   -25,  -541,
     114,  1312,  -541,   351,  -541,   -25,   158,  1379,   158,   158,
    1379,   158,  -541,  -541,    69,  -541,   114,   167,   167,   167,
     167,   375,  -541,   368,  -541,   335,   335,  -541,   286,   414,
     367,   193,  -541,   376,   422,  -541,  1312,   167,  -541,   335,
     335,   335,   335,   335,   178,  1379,  -541,   352,   -25,   330,
     -25,   -25,  -541,   -43,   382,  -541,   380,  -541,   386,   355,
     388,   114,   393,   392,  -541,   353,  -541,  -541,   173,  -541,
    -541,  -541,  -541,  -541,  -541,   167,   167,  -541,  -541,  1410,
       0,   401,  1410,  -541,  -541,  -541,  -541,  -541,  -541,   335,
    -541,   167,   167,   167,   167,   167,  -541,   414,  -541,  1334,
    -541,  -541,  -541,  -541,  -541,  -541,   397,  -541,  -541,  -541,
     408,  -541,    88,   409,  -541,   114,  -541,   685,   453,   416,
     423,   173,  -541,  -541,  -541,  -541,  1312,  -541,  -541,  -541,
     167,  -541,  -541,  -541,  -541,  -541,   419,  -541,  1312,   -25,
     420,   421,  1410,   158,   158,   158,  -541,  -541,   900,  1024,
    -541,   238,  -541,  -541,   427,  -541,   425,     1,   430,  1410,
    -541,   424,   434,   435,   439,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,  -541,  -541,  -541,  -541,   454,  -541,
     437,  -541,  -541,  -541,   441,   446,   431,   352,  1312,  -541,
     443,   451,  -541,  -541,   230,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,  -541,   495,  -541,   952,   521,   352,  -541,
    -541,  -541,  -541,   108,  -541,  1312,  -541,  -541,   450,   448,
     450,   483,   461,   485,   450,   462,  -541,   251,   -25,  -541,
    -541,  -541,   530,   352,  -541,   -25,   506,   -25,   131,   482,
     531,   568,  -541,   493,   -25,  1359,   502,   317,   264,   494,
     521,   498,  -541,   510,   499,   497,  -541,   -25,   483,   258,
    -541,   512,   511,   -25,   497,   450,   508,   450,   515,   485,
     450,   518,   -25,   519,  1359,  -541,   238,  -541,  -541,   545,
    -541,   320,   493,   -25,   450,  -541,   578,   380,  -541,  -541,
     523,  -541,  -541,   264,   590,   -25,   549,   -25,   568,   493,
     -25,  1359,   264,  -541,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  1312,   528,   526,   520,   -25,   541,   -25,   251,  -541,
     352,  -541,  -541,   251,   569,   543,   539,   497,   550,   -25,
     497,   551,  -541,   547,  1410,   877,  -541,   264,   -25,   562,
     561,  -541,   565,   733,  -541,   -25,   450,   740,  -541,   264,
     751,  -541,  1312,  1312,   -25,   564,  -541,  1312,   497,   -25,
    -541,   569,   251,  -541,   572,   -25,   251,  -541,  -541,   251,
     569,  -541,   149,   122,   566,  1312,   573,   788,   570,  -541,
     574,   -25,   575,   576,   579,  -541,  -541,  1312,  1240,   577,
    1312,  1312,  -541,   257,  -541,   251,  -541,   -25,  -541,   497,
     -25,  -541,   569,   226,   581,   323,  1312,  -541,   288,  -541,
     582,   497,   798,   580,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,   805,   251,  -541,   -25,   251,  -541,   583,   497,   585,
    -541,   812,  -541,   251,  -541,   587,  -541
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     0,     0,     0,     2,     3,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,     9,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     1,     4,    62,
       0,    60,    61,    86,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    83,    84,     0,     0,     0,
      58,    67,   371,   372,   251,   289,   282,     0,   140,   140,
     140,     0,   148,   148,   148,   148,     0,   142,     0,     0,
       0,   148,     0,    75,   212,   213,    69,    76,    77,    78,
      79,    81,    82,     0,    80,    68,   215,   214,     7,   246,
     238,   239,   240,   241,   242,   244,   245,   243,   236,    73,
     237,    74,    65,   110,     0,    96,    97,    98,    99,   107,
     108,     0,    94,   113,   114,   125,   126,   127,   131,   252,
       0,     0,    66,     0,   283,   282,     0,     0,     0,   119,
     120,   121,   122,   133,     0,   141,     0,     0,     0,     0,
     228,   216,     0,     0,     0,     0,     0,     0,     0,   155,
       0,     0,   218,   230,   217,     0,     0,     0,     0,   148,
     148,   148,   148,     0,   142,   203,   204,   205,   206,   207,
       8,    63,   128,   106,   109,   100,   101,   104,   105,    92,
     112,   115,   116,   117,   129,   130,     0,     0,     0,   282,
     279,   282,     0,   290,     0,     0,   123,   124,     0,   132,
     136,   222,   219,     0,   224,     0,   159,   160,     0,   150,
      94,   170,   170,   170,   170,   154,     0,     0,   157,     0,
       0,     0,     0,     0,   146,   147,     0,   144,   168,     0,
     122,     0,   200,     0,   155,   170,     7,     0,     0,     0,
       0,     0,     0,   102,   103,    88,    89,    90,    93,     0,
      87,    94,    72,    59,     0,   280,     0,     0,   282,   250,
       0,     0,   369,   136,   138,   282,   140,     0,   140,   140,
       0,   140,   229,   149,     0,   111,     0,     0,     0,     0,
       0,     0,   179,     0,   156,   170,   170,   143,     0,   161,
     191,     0,   198,   193,     0,   202,     0,     0,    71,   170,
     170,   170,   170,   170,     0,     0,    95,     0,   282,   279,
     282,   282,   287,   136,     0,   137,     0,   134,     0,     0,
       0,     0,     0,     0,   151,   172,   171,   208,     0,   174,
     175,   176,   177,   178,   158,     0,     0,   145,   162,     0,
     161,     0,     0,   197,   194,   195,   196,   199,   201,   170,
     189,     0,     0,     0,     0,     0,   187,   161,    91,     0,
      70,   285,   281,   286,   284,   139,     0,   370,   135,   223,
       0,   220,     0,     0,   225,     0,   235,     0,     0,     0,
       0,     0,   231,   232,   180,   181,     0,   167,   169,   192,
       0,   182,   183,   184,   185,   186,     0,   313,   291,   282,
     308,     0,     0,   140,   140,   140,   173,   255,     0,     0,
     233,     7,   234,   211,   163,   190,     0,   161,     0,     0,
     312,     0,     0,     0,     0,   275,   258,   259,   260,   261,
     267,   268,   269,   262,   263,   264,   265,   266,   152,   270,
       0,   272,   273,   274,     0,   256,    58,     0,     0,   209,
       0,     0,   188,   288,     0,   292,   294,   309,   118,   221,
     227,   226,   153,   271,     0,   254,     0,     0,     0,   164,
     165,   277,   276,   278,   293,     0,   257,   358,     0,     0,
       0,     0,     0,   329,     0,     0,   318,     0,   282,   248,
     347,   319,   316,     0,   364,   282,     0,   282,     0,   367,
       0,     0,   328,     0,   282,     0,     0,     0,     0,     0,
       0,     0,   362,     0,     0,     0,   365,   282,     0,     0,
     331,     0,     0,   282,     0,     0,     0,     0,     0,   329,
       0,     0,   282,     0,   325,   327,     7,   322,   357,     0,
     247,     0,     0,   282,     0,   363,     0,     0,   368,   330,
       0,   346,   324,     0,     0,   282,     0,   282,     0,     0,
     282,     0,     0,   348,   326,   320,   317,   295,   296,   297,
     315,     0,     0,   310,     0,   282,     0,   282,     0,   355,
       0,   332,   345,     0,   359,     0,     0,     0,     0,   282,
       0,     0,   344,     0,     0,     0,   314,     0,   282,     0,
       0,   366,     0,     0,   353,   282,     0,     0,   334,     0,
       0,   335,     0,     0,   282,     0,   311,     0,     0,   282,
     356,   359,     0,   360,     0,   282,     0,   342,   333,     0,
     359,   298,     0,     0,     0,     0,     0,     0,     0,   354,
       0,   282,     0,     0,     0,   340,   306,     0,     0,     0,
       0,     0,   304,     0,   249,     0,   350,   282,   361,     0,
     282,   343,   359,     0,     0,     0,     0,   300,     0,   307,
       0,     0,     0,     0,   341,   303,   302,   301,   299,   305,
     349,     0,     0,   337,   282,     0,   351,     0,     0,     0,
     336,     0,   352,     0,   338,     0,   339
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -541,  -541,   651,  -541,  -255,    -1,   -63,   596,   613,   -29,
    -541,  -541,  -541,  -259,  -541,  -220,  -541,  -153,   -78,   -72,
     -71,   -69,  -170,   513,   536,  -541,   -83,  -541,  -541,  -251,
    -541,  -541,   -80,   478,   356,  -541,   -46,   369,  -541,  -541,
    -149,   362,  -541,   231,  -541,  -541,  -333,  -541,  -191,   276,
    -541,  -541,  -541,     7,  -541,  -541,  -541,  -541,  -541,  -541,
    -541,  -541,  -541,   359,  -541,   360,   615,  -541,  -103,   280,
     617,  -541,  -541,   466,  -541,  -541,  -541,  -541,   275,  -541,
     297,  -541,   206,  -541,  -541,   364,   -84,   115,   -62,  -513,
    -541,  -541,  -455,  -541,  -541,  -392,   117,  -442,  -541,  -541,
     200,  -525,   155,  -470,   194,  -496,  -541,  -471,  -495,  -498,
    -540,  -464,  -541,   212,   233,   185,  -541,  -541
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,    70,   201,   240,   142,     5,    61,    71,
      72,    73,   277,   278,   279,   210,   143,   241,   144,   159,
     160,   161,   162,   163,   148,   149,   280,   346,   293,   294,
     104,   105,   166,   181,   256,   257,   173,   238,   493,   248,
     178,   249,   239,   369,   481,   370,   371,   106,   307,   356,
     107,   108,   109,   179,   110,   195,   196,   197,   198,   199,
     111,   112,   373,   322,   262,   263,   408,   114,   359,   409,
     410,   116,   117,   171,   184,   411,   412,   131,   413,    74,
     150,   439,   474,   475,   504,   286,   542,   429,   518,   224,
     430,   602,   662,   645,   603,   431,   604,   390,   572,   540,
     519,   536,   551,   563,   533,   520,   565,   537,   634,   543,
     576,   525,   529,   530,   295,   398,    75,    76
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      55,    56,   156,    81,   164,   145,   146,    86,   147,   167,
     169,   328,   567,   170,   584,   260,   152,   368,   368,   186,
     305,   242,   243,   244,   580,   568,   521,   582,   258,   545,
     253,   154,   165,   153,   265,   497,   554,   417,   349,   564,
     451,   352,   345,   292,   132,    57,   527,   174,   175,   176,
     534,   308,   309,   310,   426,   187,   522,   487,   155,   550,
     552,   337,   145,   146,    78,   147,    82,    83,   564,   521,
     612,   222,   216,   188,   327,   606,   388,   637,  -166,   622,
     640,   541,   168,   630,   214,   225,   546,   483,   632,   484,
     172,   585,   395,   587,   594,   564,   590,   182,   315,   236,
     316,   230,   261,   231,   232,   609,   234,   227,   667,   629,
     607,   118,   227,   614,   647,   326,   153,   552,   237,    77,
     297,   621,   217,   298,   365,   366,   658,   670,   151,   153,
     648,   673,   215,   300,   674,   284,   301,   287,   381,   382,
     383,   384,   385,   267,   268,   269,   270,     1,     2,   702,
    -193,   668,  -193,   228,   260,   229,   669,   170,   228,   321,
     700,   711,   653,   153,   153,   675,   657,    80,   631,   660,
     296,   434,   655,   379,   406,    59,   247,    60,   721,    87,
      88,    89,    90,    91,   153,   251,   479,   717,   420,   701,
     719,    98,    99,   264,   165,   100,   686,   704,   725,    79,
     271,    80,   681,   219,   342,   360,   361,   362,   483,   220,
     683,   347,   221,   407,   389,   281,   348,   292,   350,   351,
     172,   353,   693,   695,   380,   203,   698,   343,   355,   204,
     676,   713,   677,   153,    84,   678,    85,   209,   679,   680,
     716,   261,   357,   376,   358,   311,    80,   374,   375,   247,
     724,  -210,   507,   386,   391,   387,   393,   394,   320,   507,
     323,   177,   414,   415,   273,   274,   329,   330,   331,   332,
     333,   334,   189,   190,   191,   192,   193,   194,   421,   422,
     423,   424,   425,    80,   501,   502,   416,   180,   402,   419,
     344,   183,   508,   509,   510,   511,   512,   513,   514,   508,
     509,   510,   511,   512,   513,   514,   428,  -289,   185,   677,
     705,   595,   678,   200,  -289,   679,   680,   445,   507,   205,
     206,   207,   208,   515,   254,   255,   597,   355,    85,  -289,
     515,   211,   212,   233,  -289,    85,   579,   213,   699,   218,
     677,  -289,   223,   678,   235,   448,   679,   680,   250,   428,
     266,   214,   252,   452,   453,   454,   133,   158,   508,   509,
     510,   511,   512,   513,   514,   282,   428,   145,   146,   709,
     147,   677,   283,    80,   678,   288,   289,   679,   680,   135,
     136,   137,   138,   139,   140,   141,   285,   598,   599,   515,
     290,   291,   299,   303,    85,  -321,   119,   120,   121,   122,
     304,   123,   124,   125,   126,   127,   677,   600,   209,   678,
     707,   306,   679,   680,   312,   444,   313,   245,   314,   317,
     318,   503,   319,   324,   325,   335,   338,   447,   336,   339,
     128,   368,   405,   340,   538,   389,   400,   292,   477,    63,
     259,    -5,    -5,    64,    -5,    -5,    -5,    -5,    -5,    -5,
      -5,    -5,    -5,    -5,    -5,   372,    -5,    -5,   133,   158,
      -5,   341,   363,   577,   321,   129,   396,   397,   130,   583,
     399,   553,   401,   562,   403,    80,   404,   498,   592,   418,
     432,   135,   136,   137,   138,   139,   140,   141,   601,    65,
      66,   433,   435,   407,   441,    67,    68,   446,   442,   449,
     482,   615,   562,   617,   523,   450,   620,    69,   488,   492,
     605,    -5,   507,   480,    -9,    -5,   -64,   486,   489,   490,
     133,   627,   507,   491,   494,   496,   500,   619,   495,   562,
     499,   505,   507,   524,   526,   639,   528,    80,   531,   535,
     532,   643,   601,   135,   136,   137,   138,   139,   140,   141,
     539,   654,   508,   509,   510,   511,   512,   513,   514,   544,
     664,   548,   508,   509,   510,   511,   512,   513,   514,   507,
      85,   672,   508,   509,   510,   511,   512,   513,   514,   507,
     566,   575,   569,   515,   571,   573,   574,   689,    85,  -323,
     581,   507,   588,   515,   586,   591,   516,   593,   517,   596,
     623,   611,   616,   515,   624,   625,   703,   626,   549,   508,
     509,   510,   511,   512,   513,   514,   628,   633,   635,   508,
     509,   510,   511,   512,   513,   514,   636,   642,   638,   641,
     718,   508,   509,   510,   511,   512,   513,   514,   649,   650,
     515,   661,   663,   651,   665,    85,   666,   671,   684,   687,
     515,   690,   688,   682,   691,   608,    58,   692,   696,   714,
     710,   720,   515,   722,   661,   726,   103,   613,   706,    62,
     202,   226,   272,   354,   367,   364,   661,   661,   485,   697,
     661,   436,   377,   440,   378,   113,   437,   115,  -253,  -253,
    -253,   443,  -253,  -253,  -253,   708,  -253,  -253,  -253,  -253,
    -253,   302,   506,   392,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,   507,  -253,   478,  -253,  -253,   644,
     570,   507,   646,   618,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,   507,   589,  -253,  -253,  -253,  -253,  -253,  -253,
     578,   547,   610,     0,     0,     0,     0,     0,     0,     0,
       0,   438,     0,     0,   508,   509,   510,   511,   512,   513,
     514,   508,   509,   510,   511,   512,   513,   514,     0,   507,
       0,     0,   508,   509,   510,   511,   512,   513,   514,   507,
       0,     0,     0,     0,     0,   515,   507,     0,     0,     0,
     652,     0,   515,   507,     0,     0,     0,   656,     0,     0,
       0,     0,     0,   515,     0,     0,     0,     0,   659,   508,
     509,   510,   511,   512,   513,   514,     0,     0,     0,   508,
     509,   510,   511,   512,   513,   514,   508,   509,   510,   511,
     512,   513,   514,   508,   509,   510,   511,   512,   513,   514,
     515,     0,     0,     0,     0,   685,     0,     0,     0,     0,
     515,     0,     0,     0,     0,   712,     0,   515,     0,     0,
       0,     0,   715,   597,   515,     0,     0,     1,     2,   723,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,   455,    98,    99,     0,     0,   100,     0,     0,     0,
       0,     0,     0,   133,   158,     0,     0,     0,     0,     0,
       0,   456,     0,   457,   458,   459,   460,   461,   462,     0,
      80,   463,   464,   465,   466,   467,   135,   136,   137,   138,
     139,   140,   141,     0,   598,   599,     0,     0,     0,   468,
     469,     0,     0,   455,     0,     0,     0,   101,     0,     0,
       0,   102,     0,     0,     0,     0,   470,     0,     0,   157,
     471,   472,     0,   456,   473,   457,   458,   459,   460,   461,
     462,     0,     0,   463,   464,   465,   466,   467,     0,   133,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   468,   469,     0,     0,     0,    80,     0,     0,     0,
       0,     0,   135,   136,   137,   138,   139,   140,   141,     0,
       0,     0,   471,   472,     0,     0,   473,     6,     7,     8,
       0,     9,    10,    11,     0,    12,    13,    14,    15,    16,
       0,     0,     0,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
     133,   134,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,     0,    45,     0,    46,   476,     0,     0,
       0,     0,     0,   135,   136,   137,   138,   139,   140,   141,
      48,     0,     0,    49,    50,    51,    52,    53,    54,     6,
       7,     8,     0,     9,    10,    11,     0,    12,    13,    14,
      15,    16,     0,     0,     0,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,     0,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,   245,    45,     0,    46,    47,
     246,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,     0,    49,    50,    51,    52,    53,
      54,     6,     7,     8,     0,     9,    10,    11,     0,    12,
      13,    14,    15,    16,     0,     0,     0,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,     0,    45,     0,
      46,    47,   246,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,    49,    50,    51,
      52,    53,    54,     6,     7,     8,     0,     9,    10,    11,
       0,    12,    13,    14,    15,    16,     0,     0,     0,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,     0,
      45,     0,    46,    47,   694,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    49,
      50,    51,    52,    53,    54,     6,     7,     8,     0,     9,
      10,    11,     0,    12,    13,    14,    15,    16,     0,     0,
       0,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,     0,     0,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,     0,    45,     0,    46,    47,     0,     0,     0,     0,
     133,   158,     0,     0,     0,     0,     0,     0,    48,     0,
       0,    49,    50,    51,    52,    53,    54,    80,     0,     0,
       0,     0,     0,   135,   136,   137,   138,   139,   140,   141,
     555,   556,   557,   511,   558,   559,   560,     0,     0,     0,
       0,     0,     0,     0,     0,   133,   158,     0,   427,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   133,   134,
       0,   561,    80,   275,   276,     0,    85,     0,   135,   136,
     137,   138,   139,   140,   141,    80,   133,   158,     0,     0,
       0,   135,   136,   137,   138,   139,   140,   141,     0,     0,
       0,     0,     0,    80,     0,     0,     0,     0,     0,   135,
     136,   137,   138,   139,   140,   141
};

static const yytype_int16 yycheck[] =
{
       1,     2,    86,    66,    87,    77,    77,    69,    77,    89,
      90,   266,   537,    91,   554,   185,    79,    17,    17,    15,
     240,   174,   175,   176,   549,   538,   497,   552,   181,   527,
     179,    56,    36,    76,   187,   477,   534,   370,   297,   535,
     432,   300,   293,    86,    73,     0,   510,    93,    94,    95,
     514,   242,   243,   244,   387,   101,   498,   449,    83,   530,
     531,   281,   134,   134,    65,   134,    67,    68,   564,   540,
     583,   155,   150,   102,   265,   573,   335,   617,    78,   592,
     620,   523,    86,   608,    37,   157,   528,    86,   613,    88,
      86,   555,   343,   557,   564,   591,   560,    98,   251,    30,
     253,   164,   185,   166,   167,   576,   169,    37,   648,   607,
     574,    78,    37,   584,   627,   264,    76,   588,    49,    40,
      80,   591,   151,    83,   315,   316,   639,   652,    77,    76,
     628,   656,    85,    80,   659,   219,    83,   221,   329,   330,
     331,   332,   333,   189,   190,   191,   192,     3,     4,   689,
      79,   649,    81,    83,   324,    85,   651,   235,    83,    88,
     685,   701,   633,    76,    76,   660,   637,    53,   610,   640,
      83,    83,   636,   326,     1,    75,   177,    77,   718,     6,
       7,     8,     9,    10,    76,   178,   441,   712,   379,   687,
     715,    18,    19,   186,    36,    22,   667,   692,   723,    51,
     193,    53,    80,    77,   288,   308,   309,   310,    86,    83,
     665,   295,    86,    40,    83,   216,   296,    86,   298,   299,
      86,   301,   677,   678,   327,    60,   681,   290,   306,    64,
      81,   702,    83,    76,    75,    86,    77,    80,    89,    90,
     711,   324,    75,   321,    77,   246,    53,    54,    55,   250,
     721,    78,     1,    75,   338,    77,   340,   341,   259,     1,
     261,    86,   365,   366,    59,    60,   267,   268,   269,   270,
     271,   272,    11,    12,    13,    14,    15,    16,   381,   382,
     383,   384,   385,    53,    54,    55,   369,    86,   351,   372,
     291,    55,    41,    42,    43,    44,    45,    46,    47,    41,
      42,    43,    44,    45,    46,    47,   389,    56,    80,    83,
      84,   566,    86,    75,    56,    89,    90,   420,     1,    59,
      60,    61,    62,    72,    38,    39,     6,   405,    77,    78,
      72,    82,    82,    66,    83,    77,    78,    82,    81,    76,
      83,    83,    78,    86,    79,   429,    89,    90,    86,   432,
      78,    37,    86,   433,   434,   435,    36,    37,    41,    42,
      43,    44,    45,    46,    47,    78,   449,   439,   439,    81,
     439,    83,    53,    53,    86,    84,    75,    89,    90,    59,
      60,    61,    62,    63,    64,    65,    56,    67,    68,    72,
      82,    82,    87,    87,    77,    78,     6,     7,     8,     9,
      79,    11,    12,    13,    14,    15,    83,    87,    80,    86,
      87,    76,    89,    90,    87,   416,    79,    49,    87,    87,
      79,   484,    77,    79,    81,    79,    78,   428,    81,    79,
      40,    17,    79,    84,   518,    83,    81,    86,   439,     1,
      18,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    88,    18,    19,    36,    37,
      22,    87,    87,   547,    88,    75,    84,    87,    78,   553,
      84,   533,    84,   535,    81,    53,    84,   478,   562,    78,
      83,    59,    60,    61,    62,    63,    64,    65,   571,    51,
      52,    83,    83,    40,    78,    57,    58,    78,    75,    79,
      75,   585,   564,   587,   505,    84,   590,    69,    84,    55,
     572,    73,     1,    86,    83,    77,    78,    87,    84,    84,
      36,   605,     1,    84,    87,    79,    75,   589,    87,   591,
      87,    36,     1,    83,    86,   619,    53,    53,    77,    77,
      55,   624,   625,    59,    60,    61,    62,    63,    64,    65,
      20,   635,    41,    42,    43,    44,    45,    46,    47,    53,
     644,    79,    41,    42,    43,    44,    45,    46,    47,     1,
      77,   655,    41,    42,    43,    44,    45,    46,    47,     1,
      78,    84,    88,    72,    86,    75,    87,   671,    77,    78,
      78,     1,    77,    72,    86,    77,    75,    78,    77,    54,
     601,    78,    53,    72,    76,    79,   690,    87,    77,    41,
      42,    43,    44,    45,    46,    47,    75,    48,    75,    41,
      42,    43,    44,    45,    46,    47,    87,    80,    78,    78,
     714,    41,    42,    43,    44,    45,    46,    47,    76,    78,
      72,   642,   643,    78,    80,    77,   647,    75,    75,    79,
      72,    76,    78,    87,    78,    77,     5,    78,    81,    79,
      78,    78,    72,    78,   665,    78,    70,    77,    87,    56,
     134,   158,   194,   304,   318,   313,   677,   678,   447,   680,
     681,   405,   323,   408,   324,    70,     1,    70,     3,     4,
       5,   411,     7,     8,     9,   696,    11,    12,    13,    14,
      15,   235,   496,   339,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,     1,    50,   439,    52,    53,   624,
     540,     1,   625,   588,    59,    60,    61,    62,    63,    64,
      65,    66,     1,   559,    69,    70,    71,    72,    73,    74,
     548,   528,   577,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    41,    42,    43,    44,    45,    46,    47,    -1,     1,
      -1,    -1,    41,    42,    43,    44,    45,    46,    47,     1,
      -1,    -1,    -1,    -1,    -1,    72,     1,    -1,    -1,    -1,
      77,    -1,    72,     1,    -1,    -1,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    77,    41,
      42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    41,
      42,    43,    44,    45,    46,    47,    41,    42,    43,    44,
      45,    46,    47,    41,    42,    43,    44,    45,    46,    47,
      72,    -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,
      72,    -1,    -1,    -1,    -1,    77,    -1,    72,    -1,    -1,
      -1,    -1,    77,     6,    72,    -1,    -1,     3,     4,    77,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     1,    18,    19,    -1,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    21,    -1,    23,    24,    25,    26,    27,    28,    -1,
      53,    31,    32,    33,    34,    35,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    68,    -1,    -1,    -1,    49,
      50,    -1,    -1,     1,    -1,    -1,    -1,    73,    -1,    -1,
      -1,    77,    -1,    -1,    -1,    -1,    66,    -1,    -1,    16,
      70,    71,    -1,    21,    74,    23,    24,    25,    26,    27,
      28,    -1,    -1,    31,    32,    33,    34,    35,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    -1,    59,    60,    61,    62,    63,    64,    65,    -1,
      -1,    -1,    70,    71,    -1,    -1,    74,     3,     4,     5,
      -1,     7,     8,     9,    -1,    11,    12,    13,    14,    15,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    -1,    50,    -1,    52,    53,    -1,    -1,
      -1,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
       4,     5,    -1,     7,     8,     9,    -1,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    -1,    -1,    69,    70,    71,    72,    73,
      74,     3,     4,     5,    -1,     7,     8,     9,    -1,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    -1,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    -1,    50,    -1,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    -1,    -1,    69,    70,    71,
      72,    73,    74,     3,     4,     5,    -1,     7,     8,     9,
      -1,    11,    12,    13,    14,    15,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    -1,
      50,    -1,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    69,
      70,    71,    72,    73,    74,     3,     4,     5,    -1,     7,
       8,     9,    -1,    11,    12,    13,    14,    15,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    -1,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    -1,    50,    -1,    52,    53,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,
      -1,    69,    70,    71,    72,    73,    74,    53,    -1,    -1,
      -1,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      41,    42,    43,    44,    45,    46,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    72,    53,    54,    55,    -1,    77,    -1,    59,    60,
      61,    62,    63,    64,    65,    53,    36,    37,    -1,    -1,
      -1,    59,    60,    61,    62,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    92,    93,    98,     3,     4,     5,     7,
       8,     9,    11,    12,    13,    14,    15,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    50,    52,    53,    66,    69,
      70,    71,    72,    73,    74,    96,    96,     0,    93,    75,
      77,    99,    99,     1,     5,    51,    52,    57,    58,    69,
      94,   100,   101,   102,   170,   207,   208,    40,    96,    51,
      53,    97,    96,    96,    75,    77,   179,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    18,    19,
      22,    73,    77,    98,   121,   122,   138,   141,   142,   143,
     145,   151,   152,   157,   158,   161,   162,   163,    78,     6,
       7,     8,     9,    11,    12,    13,    14,    15,    40,    75,
      78,   168,   100,    36,    37,    59,    60,    61,    62,    63,
      64,    65,    97,   107,   109,   110,   111,   112,   115,   116,
     171,    77,    97,    76,    56,    83,   177,    16,    37,   110,
     111,   112,   113,   114,   117,    36,   123,   123,    86,   123,
     109,   164,    86,   127,   127,   127,   127,    86,   131,   144,
      86,   124,    96,    55,   165,    80,    15,   127,   100,    11,
      12,    13,    14,    15,    16,   146,   147,   148,   149,   150,
      75,    95,   115,    60,    64,    59,    60,    61,    62,    80,
     106,    82,    82,    82,    37,    85,   109,   100,    76,    77,
      83,    86,   177,    78,   180,   110,   114,    37,    83,    85,
      97,    97,    97,    66,    97,    79,    30,    49,   128,   133,
      96,   108,   108,   108,   108,    49,    54,    96,   130,   132,
      86,   144,    86,   131,    38,    39,   125,   126,   108,    18,
     113,   117,   155,   156,   144,   108,    78,   127,   127,   127,
     127,   144,   124,    59,    60,    54,    55,   103,   104,   105,
     117,    96,    78,    53,   177,    56,   176,   177,    84,    75,
      82,    82,    86,   119,   120,   205,    83,    80,    83,    87,
      80,    83,   164,    87,    79,   106,    76,   139,   139,   139,
     139,    96,    87,    79,    87,   108,   108,    87,    79,    77,
      96,    88,   154,    96,    79,    81,   131,   139,    95,    96,
      96,    96,    96,    96,    96,    79,    81,   106,    78,    79,
      84,    87,   177,    97,    96,   120,   118,   177,   123,   104,
     123,   123,   104,   123,   128,   109,   140,    75,    77,   159,
     159,   159,   159,    87,   132,   139,   139,   125,    17,   134,
     136,   137,    88,   153,    54,    55,   109,   154,   156,   108,
     159,   139,   139,   139,   139,   139,    75,    77,   104,    83,
     188,   177,   176,   177,   177,   120,    84,    87,   206,    84,
      81,    84,    97,    81,    84,    79,     1,    40,   157,   160,
     161,   166,   167,   169,   159,   159,   117,   137,    78,   117,
     139,   159,   159,   159,   159,   159,   137,    84,   117,   178,
     181,   186,    83,    83,    83,    83,   140,     1,    86,   172,
     169,    78,    75,   160,    96,   159,    78,    96,   177,    79,
      84,   186,   123,   123,   123,     1,    21,    23,    24,    25,
      26,    27,    28,    31,    32,    33,    34,    35,    49,    50,
      66,    70,    71,    74,   173,   174,    53,    96,   171,    95,
      86,   135,    75,    86,    88,   134,    87,   186,    84,    84,
      84,    84,    55,   129,    87,    87,    79,   188,    96,    87,
      75,    54,    55,    97,   175,    36,   173,     1,    41,    42,
      43,    44,    45,    46,    47,    72,    75,    77,   179,   191,
     196,   198,   188,    96,    83,   202,    86,   202,    53,   203,
     204,    77,    55,   195,   202,    77,   192,   198,   177,    20,
     190,   188,   177,   200,    53,   200,   188,   205,    79,    77,
     198,   193,   198,   179,   200,    41,    42,    43,    45,    46,
      47,    72,   179,   194,   196,   197,    78,   192,   180,    88,
     191,    86,   189,    75,    87,    84,   201,   177,   204,    78,
     192,    78,   192,   177,   201,   202,    86,   202,    77,   195,
     202,    77,   177,    78,   194,    95,    54,     6,    67,    68,
      87,   117,   182,   185,   187,   179,   200,   202,    77,   198,
     206,    78,   180,    77,   198,   177,    53,   177,   193,   179,
     177,   194,   180,    96,    76,    79,    87,   177,    75,   200,
     192,   188,   192,    48,   199,    75,    87,   201,    78,   177,
     201,    78,    80,   117,   178,   184,   187,   180,   200,    76,
      78,    78,    77,   198,   177,   202,    77,   198,   180,    77,
     198,    96,   183,    96,   177,    80,    96,   201,   200,   199,
     192,    75,   177,   192,   192,   199,    81,    83,    86,    89,
      90,    80,    87,   183,    75,    77,   198,    79,    78,   177,
      76,    78,    78,   183,    54,   183,    81,    96,   183,    81,
     192,   200,   201,   177,   199,    84,    87,    87,    96,    81,
      78,   201,    77,   198,    79,    77,   198,   192,   177,   192,
      78,   201,    78,    77,   198,   192,    78
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    91,    92,    93,    93,    94,    94,    95,    95,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    97,    97,
      98,    98,    99,    99,   100,   100,   101,   101,   101,   101,
     101,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   103,   103,   103,
     104,   104,   105,   105,   106,   106,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   108,   109,   110,   110,   111,   112,   112,   113,   114,
     114,   114,   114,   114,   114,   115,   115,   115,   115,   115,
     116,   116,   117,   117,   118,   119,   120,   120,   121,   122,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   129,   129,   130,   131,   131,   132,   132,   133,
     133,   134,   134,   135,   135,   136,   137,   137,   138,   138,
     139,   139,   140,   140,   141,   141,   142,   143,   144,   144,
     145,   145,   146,   146,   147,   148,   149,   150,   150,   151,
     152,   153,   153,   154,   154,   154,   154,   155,   155,   155,
     156,   156,   157,   158,   158,   158,   158,   158,   159,   159,
     160,   160,   161,   161,   161,   161,   161,   161,   161,   162,
     162,   162,   162,   162,   163,   163,   163,   163,   164,   164,
     165,   166,   167,   167,   167,   167,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   169,   169,   169,
     170,   170,   171,   172,   172,   172,   173,   173,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   175,   175,   175,   176,
     176,   176,   177,   177,   177,   177,   177,   177,   178,   179,
     180,   181,   181,   181,   181,   182,   182,   182,   183,   183,
     183,   183,   183,   183,   184,   185,   185,   185,   186,   186,
     187,   187,   188,   188,   189,   189,   190,   190,   191,   191,
     191,   192,   192,   193,   193,   194,   194,   194,   195,   195,
     196,   196,   196,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   199,
     199,   199,   200,   201,   202,   203,   203,   204,   204,   205,
     206,   207,   208
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       3,     3,     1,     4,     0,     2,     3,     2,     2,     2,
       7,     5,     5,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     0,     1,     0,     3,     1,     1,     1,     1,
       2,     2,     3,     3,     2,     2,     2,     1,     1,     2,
       1,     2,     2,     1,     1,     2,     2,     2,     8,     1,
       1,     1,     1,     2,     2,     1,     1,     1,     2,     2,
       2,     1,     2,     1,     1,     3,     0,     2,     4,     6,
       0,     1,     0,     3,     1,     3,     1,     1,     0,     3,
       1,     3,     0,     1,     1,     0,     3,     1,     3,     1,
       1,     0,     1,     0,     2,     5,     1,     2,     3,     6,
       0,     2,     1,     3,     5,     5,     5,     5,     4,     3,
       6,     6,     5,     5,     5,     5,     5,     4,     7,     5,
       7,     0,     2,     0,     2,     2,     2,     3,     2,     3,
       1,     3,     4,     2,     2,     2,     2,     2,     1,     4,
       0,     2,     1,     1,     1,     1,     2,     2,     2,     3,
       6,     9,     3,     6,     3,     6,     9,     9,     1,     3,
       1,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     7,     5,    13,
       5,     2,     1,     0,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     3,     0,     1,     5,     5,     5,     4,     3,     1,
       1,     1,     3,     4,     3,     1,     1,     1,     1,     4,
       3,     4,     4,     4,     3,     7,     5,     6,     1,     3,
       1,     3,     3,     2,     3,     2,     0,     3,     1,     1,
       4,     1,     2,     1,     2,     1,     2,     1,     1,     0,
       4,     3,     5,     5,     4,     4,    11,     9,    12,    14,
       6,     8,     5,     7,     3,     5,     4,     1,     4,    11,
       9,    12,    14,     6,     8,     5,     7,     3,     1,     0,
       2,     4,     1,     1,     1,     2,     5,     1,     3,     1,
       1,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
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

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
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
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
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
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
      yychar = yylex ();
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 198 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.modlist) = (yyvsp[0].modlist); modlist = (yyvsp[0].modlist); }
#line 2230 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 202 "xi-grammar.y" /* yacc.c:1646  */
    { 
		  (yyval.modlist) = 0; 
		}
#line 2238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 206 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.modlist) = new AstChildren<Module>(lineno, (yyvsp[-1].module), (yyvsp[0].modlist)); }
#line 2244 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 210 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 2250 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 212 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 1; }
#line 2256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 216 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 2262 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 218 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 1; }
#line 2268 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 223 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2274 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 224 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(MODULE); YYABORT; }
#line 2280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 225 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(MAINMODULE); YYABORT; }
#line 2286 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 226 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(EXTERN); YYABORT; }
#line 2292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 228 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(INITCALL); YYABORT; }
#line 2298 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 229 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(INITNODE); YYABORT; }
#line 2304 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 230 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(INITPROC); YYABORT; }
#line 2310 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 232 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(CHARE); }
#line 2316 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 233 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(MAINCHARE); }
#line 2322 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 234 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(GROUP); }
#line 2328 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 235 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(NODEGROUP); }
#line 2334 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 236 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ARRAY); }
#line 2340 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 240 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(INCLUDE); YYABORT; }
#line 2346 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 241 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(STACKSIZE); YYABORT; }
#line 2352 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 242 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(THREADED); YYABORT; }
#line 2358 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 243 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(TEMPLATE); YYABORT; }
#line 2364 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 244 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(SYNC); YYABORT; }
#line 2370 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 245 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(IGET); YYABORT; }
#line 2376 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 246 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(EXCLUSIVE); YYABORT; }
#line 2382 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 247 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(IMMEDIATE); YYABORT; }
#line 2388 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 248 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(SKIPSCHED); YYABORT; }
#line 2394 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 249 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(INLINE); YYABORT; }
#line 2400 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 250 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(VIRTUAL); YYABORT; }
#line 2406 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 251 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(MIGRATABLE); YYABORT; }
#line 2412 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 252 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(CREATEHERE); YYABORT; }
#line 2418 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 253 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(CREATEHOME); YYABORT; }
#line 2424 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 254 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(NOKEEP); YYABORT; }
#line 2430 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 255 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(NOTRACE); YYABORT; }
#line 2436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 256 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(APPWORK); YYABORT; }
#line 2442 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 259 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(PACKED); YYABORT; }
#line 2448 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 260 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(VARSIZE); YYABORT; }
#line 2454 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 261 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ENTRY); YYABORT; }
#line 2460 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 262 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(FOR); YYABORT; }
#line 2466 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 263 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(FORALL); YYABORT; }
#line 2472 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 264 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(WHILE); YYABORT; }
#line 2478 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 265 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(WHEN); YYABORT; }
#line 2484 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 266 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(OVERLAP); YYABORT; }
#line 2490 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 267 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ATOMIC); YYABORT; }
#line 2496 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 268 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(IF); YYABORT; }
#line 2502 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 269 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ELSE); YYABORT; }
#line 2508 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 271 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(LOCAL); YYABORT; }
#line 2514 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 273 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(USING); YYABORT; }
#line 2520 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 274 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ACCEL); YYABORT; }
#line 2526 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 277 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(ACCELBLOCK); YYABORT; }
#line 2532 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 278 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(MEMCRITICAL); YYABORT; }
#line 2538 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 279 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(REDUCTIONTARGET); YYABORT; }
#line 2544 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 280 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(CASE); YYABORT; }
#line 2550 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 281 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(UCHARE); YYABORT; }
#line 2556 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 282 "xi-grammar.y" /* yacc.c:1646  */
    { ReservedWord(REENTRANT); YYABORT; }
#line 2562 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 286 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 2568 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 288 "xi-grammar.y" /* yacc.c:1646  */
    {
		  char *tmp = new char[strlen((yyvsp[-3].strval))+strlen((yyvsp[0].strval))+3];
		  sprintf(tmp,"%s::%s", (yyvsp[-3].strval), (yyvsp[0].strval));
		  (yyval.strval) = tmp;
		}
#line 2578 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 295 "xi-grammar.y" /* yacc.c:1646  */
    { 
		    (yyval.module) = new Module(lineno, (yyvsp[-1].strval), (yyvsp[0].conslist)); 
		}
#line 2586 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 299 "xi-grammar.y" /* yacc.c:1646  */
    {  
		    (yyval.module) = new Module(lineno, (yyvsp[-1].strval), (yyvsp[0].conslist)); 
		    (yyval.module)->setMain();
		}
#line 2595 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 306 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.conslist) = 0; }
#line 2601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 308 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.conslist) = (yyvsp[-2].conslist); }
#line 2607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 312 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.conslist) = 0; }
#line 2613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 314 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.conslist) = new ConstructList(lineno, (yyvsp[-1].construct), (yyvsp[0].conslist)); }
#line 2619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 318 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = new UsingScope((yyvsp[0].strval), false); }
#line 2625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 320 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = new UsingScope((yyvsp[0].strval), true); }
#line 2631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 322 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].member)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].member); }
#line 2637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 324 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].message)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].message); }
#line 2643 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 326 "xi-grammar.y" /* yacc.c:1646  */
    {
                  Entry *e = new Entry(lineno, 0, (yyvsp[-4].type), (yyvsp[-2].strval), (yyvsp[0].plist), 0, 0, 0, (yylsp[-6]).first_line, (yyloc).last_line);
                  int isExtern = 1;
                  e->setExtern(isExtern);
                  e->targs = (yyvsp[-1].tparlist);
                  e->label = new XStr;
                  (yyvsp[-3].ntype)->print(*e->label);
                  (yyval.construct) = e;
                }
#line 2657 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 338 "xi-grammar.y" /* yacc.c:1646  */
    { if((yyvsp[-2].conslist)) (yyvsp[-2].conslist)->recurse<int&>((yyvsp[-4].intval), &Construct::setExtern); (yyval.construct) = (yyvsp[-2].conslist); }
#line 2663 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 340 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = new Scope((yyvsp[-3].strval), (yyvsp[-1].conslist)); }
#line 2669 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 342 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = (yyvsp[-1].construct); }
#line 2675 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 344 "xi-grammar.y" /* yacc.c:1646  */
    {
          ERROR("preceding construct must be semicolon terminated",
                (yyloc).first_column, (yyloc).last_column);
          YYABORT;
        }
#line 2685 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 350 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].module)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].module); }
#line 2691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 352 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].chare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].chare); }
#line 2697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 354 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].chare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].chare); }
#line 2703 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 356 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].chare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].chare); }
#line 2709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 358 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].chare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].chare); }
#line 2715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 360 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].templat)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].templat); }
#line 2721 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 362 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].uchare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].uchare); }
#line 2727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 364 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[0].uchare)->setExtern((yyvsp[-1].intval)); (yyval.construct) = (yyvsp[0].uchare); }
#line 2733 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 366 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = NULL; }
#line 2739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 368 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = NULL; }
#line 2745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 370 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.construct) = (yyvsp[0].accelBlock); }
#line 2751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 372 "xi-grammar.y" /* yacc.c:1646  */
    {
          ERROR("invalid construct",
                (yyloc).first_column, (yyloc).last_column);
          YYABORT;
        }
#line 2761 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 380 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparam) = new TParamType((yyvsp[0].type)); }
#line 2767 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 382 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparam) = new TParamVal((yyvsp[0].strval)); }
#line 2773 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 384 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparam) = new TParamVal((yyvsp[0].strval)); }
#line 2779 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 388 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = new TParamList((yyvsp[0].tparam)); }
#line 2785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 390 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = new TParamList((yyvsp[-2].tparam), (yyvsp[0].tparlist)); }
#line 2791 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 394 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = 0; }
#line 2797 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 396 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = (yyvsp[0].tparlist); }
#line 2803 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 400 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = 0; }
#line 2809 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 402 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tparlist) = (yyvsp[-1].tparlist); }
#line 2815 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 406 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("int"); }
#line 2821 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 408 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("long"); }
#line 2827 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 410 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("short"); }
#line 2833 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 412 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("char"); }
#line 2839 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 414 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned int"); }
#line 2845 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 416 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned long"); }
#line 2851 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 418 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned long"); }
#line 2857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 420 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned long long"); }
#line 2863 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 422 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned short"); }
#line 2869 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 424 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("unsigned char"); }
#line 2875 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 426 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("long long"); }
#line 2881 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 428 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("float"); }
#line 2887 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 430 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("double"); }
#line 2893 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 432 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("long double"); }
#line 2899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 434 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new BuiltinType("void"); }
#line 2905 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 437 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.ntype) = new NamedType((yyvsp[-1].strval),(yyvsp[0].tparlist)); }
#line 2911 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 438 "xi-grammar.y" /* yacc.c:1646  */
    { 
                    const char* basename, *scope;
                    splitScopedName((yyvsp[-1].strval), &scope, &basename);
                    (yyval.ntype) = new NamedType(basename, (yyvsp[0].tparlist), scope);
                }
#line 2921 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 446 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 2927 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 448 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ntype); }
#line 2933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 452 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.ptype) = new PtrType((yyvsp[-1].type)); }
#line 2939 "y.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 456 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[-1].ptype)->indirect(); (yyval.ptype) = (yyvsp[-1].ptype); }
#line 2945 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 458 "xi-grammar.y" /* yacc.c:1646  */
    { (yyvsp[-1].ptype)->indirect(); (yyval.ptype) = (yyvsp[-1].ptype); }
#line 2951 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 462 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.ftype) = new FuncType((yyvsp[-7].type), (yyvsp[-4].strval), (yyvsp[-1].plist)); }
#line 2957 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 466 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 2963 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 468 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ptype); }
#line 2969 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 470 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ptype); }
#line 2975 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 472 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ftype); }
#line 2981 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 474 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ConstType((yyvsp[0].type)); }
#line 2987 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 476 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ConstType((yyvsp[-1].type)); }
#line 2993 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 480 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 2999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 482 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ptype); }
#line 3005 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 484 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].ptype); }
#line 3011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 486 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ConstType((yyvsp[0].type)); }
#line 3017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 488 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ConstType((yyvsp[-1].type)); }
#line 3023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 492 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ReferenceType((yyvsp[-1].type)); }
#line 3029 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 494 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 3035 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 498 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = new ReferenceType((yyvsp[-1].type)); }
#line 3041 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 500 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 3047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 504 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = new Value((yyvsp[0].strval)); }
#line 3053 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 508 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = (yyvsp[-1].val); }
#line 3059 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 512 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.vallist) = 0; }
#line 3065 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 514 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.vallist) = new ValueList((yyvsp[-1].val), (yyvsp[0].vallist)); }
#line 3071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 518 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.readonly) = new Readonly(lineno, (yyvsp[-2].type), (yyvsp[-1].strval), (yyvsp[0].vallist)); }
#line 3077 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 522 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.readonly) = new Readonly(lineno, (yyvsp[-3].type), (yyvsp[-1].strval), (yyvsp[0].vallist), 1); }
#line 3083 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 526 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0;}
#line 3089 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 528 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0;}
#line 3095 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 532 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3101 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 534 "xi-grammar.y" /* yacc.c:1646  */
    { 
		  /*
		  printf("Warning: Message attributes are being phased out.\n");
		  printf("Warning: Please remove them from interface files.\n");
		  */
		  (yyval.intval) = (yyvsp[-1].intval); 
		}
#line 3113 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 544 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[0].intval); }
#line 3119 "y.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 546 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-2].intval) | (yyvsp[0].intval); }
#line 3125 "y.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 550 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3131 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 552 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3137 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 556 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = 0; }
#line 3143 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 558 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[-1].cattr); }
#line 3149 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 562 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[0].cattr); }
#line 3155 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 564 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[-2].cattr) | (yyvsp[0].cattr); }
#line 3161 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 568 "xi-grammar.y" /* yacc.c:1646  */
    { python_doc = NULL; (yyval.intval) = 0; }
#line 3167 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 570 "xi-grammar.y" /* yacc.c:1646  */
    { python_doc = (yyvsp[0].strval); (yyval.intval) = 0; }
#line 3173 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 574 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = Chare::CPYTHON; }
#line 3179 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 578 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = 0; }
#line 3185 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 580 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[-1].cattr); }
#line 3191 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 584 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[0].cattr); }
#line 3197 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 586 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = (yyvsp[-2].cattr) | (yyvsp[0].cattr); }
#line 3203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 590 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = Chare::CMIGRATABLE; }
#line 3209 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 592 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.cattr) = Chare::CPYTHON; }
#line 3215 "y.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 596 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3221 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 598 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 1; }
#line 3227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 601 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3233 "y.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 603 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 1; }
#line 3239 "y.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 606 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mv) = new MsgVar((yyvsp[-3].type), (yyvsp[-2].strval), (yyvsp[-4].intval), (yyvsp[-1].intval)); }
#line 3245 "y.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 610 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mvlist) = new MsgVarList((yyvsp[0].mv)); }
#line 3251 "y.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 612 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mvlist) = new MsgVarList((yyvsp[-1].mv), (yyvsp[0].mvlist)); }
#line 3257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 616 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.message) = new Message(lineno, (yyvsp[0].ntype)); }
#line 3263 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 618 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.message) = new Message(lineno, (yyvsp[-3].ntype), (yyvsp[-1].mvlist)); }
#line 3269 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 622 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.typelist) = 0; }
#line 3275 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 624 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.typelist) = (yyvsp[0].typelist); }
#line 3281 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 628 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.typelist) = new TypeList((yyvsp[0].ntype)); }
#line 3287 "y.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 630 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.typelist) = new TypeList((yyvsp[-2].ntype), (yyvsp[0].typelist)); }
#line 3293 "y.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 634 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new Chare(lineno, (yyvsp[-3].cattr)|Chare::CCHARE, (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3299 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 636 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new MainChare(lineno, (yyvsp[-3].cattr), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3305 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 640 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new Group(lineno, (yyvsp[-3].cattr), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3311 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 644 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new NodeGroup(lineno, (yyvsp[-3].cattr), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3317 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 648 "xi-grammar.y" /* yacc.c:1646  */
    {/*Stupid special case for [1D] indices*/
			char *buf=new char[40];
			sprintf(buf,"%sD",(yyvsp[-2].strval));
			(yyval.ntype) = new NamedType(buf); 
		}
#line 3327 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 654 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.ntype) = new NamedType((yyvsp[-1].strval)); }
#line 3333 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 658 "xi-grammar.y" /* yacc.c:1646  */
    {  (yyval.chare) = new Array(lineno, (yyvsp[-4].cattr), (yyvsp[-3].ntype), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3339 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 660 "xi-grammar.y" /* yacc.c:1646  */
    {  (yyval.chare) = new Array(lineno, (yyvsp[-3].cattr), (yyvsp[-4].ntype), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3345 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 664 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new Chare(lineno, (yyvsp[-3].cattr)|Chare::CCHARE, new NamedType((yyvsp[-2].strval)), (yyvsp[-1].typelist), (yyvsp[0].mbrlist));}
#line 3351 "y.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 666 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new MainChare(lineno, (yyvsp[-3].cattr), new NamedType((yyvsp[-2].strval)), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3357 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 670 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new Group(lineno, (yyvsp[-3].cattr), new NamedType((yyvsp[-2].strval)), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3363 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 674 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new NodeGroup( lineno, (yyvsp[-3].cattr), new NamedType((yyvsp[-2].strval)), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3369 "y.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 678 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.chare) = new Array( lineno, 0, (yyvsp[-3].ntype), new NamedType((yyvsp[-2].strval)), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); }
#line 3375 "y.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 682 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.message) = new Message(lineno, new NamedType((yyvsp[-1].strval))); }
#line 3381 "y.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 684 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.message) = new Message(lineno, new NamedType((yyvsp[-4].strval)), (yyvsp[-2].mvlist)); }
#line 3387 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 688 "xi-grammar.y" /* yacc.c:1646  */
    { 
			//$$ = new Chare(lineno, $2|Chare::CCHARE, $3, $4, $5); 
		  //WARNING("'uchare' construct found", @$.first_column, @$.last_column);
      YYABORT;
		}
#line 3397 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 696 "xi-grammar.y" /* yacc.c:1646  */
    {  
		  //WARNING("'uchare array' construct found", @$.first_column, @$.last_column);
			//$$ = new UChareArray(lineno, $3, $2, $4, $5, $6); 
			(yyval.uchare) = new UChareArray(lineno, (yyvsp[-3].cattr), (yyvsp[-4].ntype), (yyvsp[-2].ntype), (yyvsp[-1].typelist), (yyvsp[0].mbrlist)); 
      //YYABORT;
		}
#line 3408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 705 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = 0; }
#line 3414 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 707 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 3420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 711 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = 0; }
#line 3426 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 713 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 3432 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 715 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 3438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 717 "xi-grammar.y" /* yacc.c:1646  */
    {
		  XStr typeStr;
		  (yyvsp[0].ntype)->print(typeStr);
		  char *tmp = strdup(typeStr.get_string());
		  (yyval.strval) = tmp;
		}
#line 3449 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 726 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvar) = new TType(new NamedType((yyvsp[-1].strval)), (yyvsp[0].type)); }
#line 3455 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 728 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvar) = new TFunc((yyvsp[-1].ftype), (yyvsp[0].strval)); }
#line 3461 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 730 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvar) = new TName((yyvsp[-2].type), (yyvsp[-1].strval), (yyvsp[0].strval)); }
#line 3467 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 734 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvarlist) = new TVarList((yyvsp[0].tvar)); }
#line 3473 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 736 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvarlist) = new TVarList((yyvsp[-2].tvar), (yyvsp[0].tvarlist)); }
#line 3479 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 740 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.tvarlist) = (yyvsp[-1].tvarlist); }
#line 3485 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 744 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.templat) = new Template((yyvsp[-1].tvarlist), (yyvsp[0].chare)); (yyvsp[0].chare)->setTemplate((yyval.templat)); }
#line 3491 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 746 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.templat) = new Template((yyvsp[-1].tvarlist), (yyvsp[0].chare)); (yyvsp[0].chare)->setTemplate((yyval.templat)); }
#line 3497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 748 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.templat) = new Template((yyvsp[-1].tvarlist), (yyvsp[0].chare)); (yyvsp[0].chare)->setTemplate((yyval.templat)); }
#line 3503 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 750 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.templat) = new Template((yyvsp[-1].tvarlist), (yyvsp[0].chare)); (yyvsp[0].chare)->setTemplate((yyval.templat)); }
#line 3509 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 752 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.templat) = new Template((yyvsp[-1].tvarlist), (yyvsp[0].message)); (yyvsp[0].message)->setTemplate((yyval.templat)); }
#line 3515 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 756 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mbrlist) = 0; }
#line 3521 "y.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 758 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mbrlist) = (yyvsp[-2].mbrlist); }
#line 3527 "y.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 762 "xi-grammar.y" /* yacc.c:1646  */
    { 
                  if (!connectEntries.empty()) {
                    (yyval.mbrlist) = new AstChildren<Member>(connectEntries);
		  } else {
		    (yyval.mbrlist) = 0; 
                  }
		}
#line 3539 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 770 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.mbrlist) = new AstChildren<Member>(-1, (yyvsp[-1].member), (yyvsp[0].mbrlist)); }
#line 3545 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 774 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].readonly); }
#line 3551 "y.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 776 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].readonly); }
#line 3557 "y.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 779 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].member); }
#line 3563 "y.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 781 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].pupable); }
#line 3569 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 783 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].includeFile); }
#line 3575 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 785 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new ClassDeclaration(lineno,(yyvsp[0].strval)); }
#line 3581 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 789 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno, (yyvsp[0].strval), 1); }
#line 3587 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 791 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno, (yyvsp[-3].strval), 1); }
#line 3593 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 793 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno,
				    strdup((std::string((yyvsp[-6].strval)) + '<' +
					    ((yyvsp[-4].tparlist))->to_string() + '>').c_str()),
				    1);
		}
#line 3603 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 799 "xi-grammar.y" /* yacc.c:1646  */
    {
		  WARNING("deprecated use of initcall. Use initnode or initproc instead",
		          (yylsp[-2]).first_column, (yylsp[-2]).last_column, (yylsp[-2]).first_line);
		  (yyval.member) = new InitCall(lineno, (yyvsp[0].strval), 1);
		}
#line 3613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 805 "xi-grammar.y" /* yacc.c:1646  */
    {
		  WARNING("deprecated use of initcall. Use initnode or initproc instead",
		          (yylsp[-5]).first_column, (yylsp[-5]).last_column, (yylsp[-5]).first_line);
		  (yyval.member) = new InitCall(lineno, (yyvsp[-3].strval), 1);
		}
#line 3623 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 814 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno, (yyvsp[0].strval), 0); }
#line 3629 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 816 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno, (yyvsp[-3].strval), 0); }
#line 3635 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 818 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = new InitCall(lineno,
				    strdup((std::string((yyvsp[-6].strval)) + '<' +
					    ((yyvsp[-4].tparlist))->to_string() + '>').c_str()),
				    0);
		}
#line 3645 "y.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 824 "xi-grammar.y" /* yacc.c:1646  */
    {
                  InitCall* rtn = new InitCall(lineno, (yyvsp[-3].strval), 0);
                  rtn->setAccel();
                  (yyval.member) = rtn;
		}
#line 3655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 832 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.pupable) = new PUPableClass(lineno,(yyvsp[0].ntype),0); }
#line 3661 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 834 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.pupable) = new PUPableClass(lineno,(yyvsp[-2].ntype),(yyvsp[0].pupable)); }
#line 3667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 837 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.includeFile) = new IncludeFile(lineno,(yyvsp[0].strval)); }
#line 3673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 841 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].member); }
#line 3679 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 845 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[0].entry); }
#line 3685 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 847 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyvsp[0].entry)->tspec = (yyvsp[-1].tvarlist);
                  (yyval.member) = (yyvsp[0].entry);
                }
#line 3694 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 852 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = (yyvsp[-1].member); }
#line 3700 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 854 "xi-grammar.y" /* yacc.c:1646  */
    {
          ERROR("invalid SDAG member",
                (yyloc).first_column, (yyloc).last_column);
          YYABORT;
        }
#line 3710 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 862 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3716 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 864 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3722 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 866 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3728 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 868 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3734 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 870 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3740 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 872 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3746 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 874 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3752 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 876 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3758 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 878 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3764 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 880 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3770 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 882 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.member) = 0; }
#line 3776 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 885 "xi-grammar.y" /* yacc.c:1646  */
    { 
                  (yyval.entry) = new Entry(lineno, (yyvsp[-5].intval), (yyvsp[-4].type), (yyvsp[-3].strval), (yyvsp[-2].plist), (yyvsp[-1].val), (yyvsp[0].sentry), (const char *) NULL, (yylsp[-6]).first_line, (yyloc).last_line);
		  if ((yyvsp[0].sentry) != 0) { 
		    (yyvsp[0].sentry)->con1 = new SdagConstruct(SIDENT, (yyvsp[-3].strval));
                    (yyvsp[0].sentry)->entry = (yyval.entry);
                    (yyvsp[0].sentry)->con1->entry = (yyval.entry);
                    (yyvsp[0].sentry)->param = new ParamList((yyvsp[-2].plist));
                  }
		}
#line 3790 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 895 "xi-grammar.y" /* yacc.c:1646  */
    { 
                  Entry *e = new Entry(lineno, (yyvsp[-3].intval), 0, (yyvsp[-2].strval), (yyvsp[-1].plist),  0, (yyvsp[0].sentry), (const char *) NULL, (yylsp[-4]).first_line, (yyloc).last_line);
                  if ((yyvsp[0].sentry) != 0) {
		    (yyvsp[0].sentry)->con1 = new SdagConstruct(SIDENT, (yyvsp[-2].strval));
                    (yyvsp[0].sentry)->entry = e;
                    (yyvsp[0].sentry)->con1->entry = e;
                    (yyvsp[0].sentry)->param = new ParamList((yyvsp[-1].plist));
                  }
		  if (e->param && e->param->isCkMigMsgPtr()) {
		    WARNING("CkMigrateMsg chare constructor is taken for granted",
		            (yyloc).first_column, (yyloc).last_column);
		    (yyval.entry) = NULL;
		  } else {
		    (yyval.entry) = e;
		  }
		}
#line 3811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 912 "xi-grammar.y" /* yacc.c:1646  */
    {
                  int attribs = SACCEL;
                  const char* name = (yyvsp[-7].strval);
                  ParamList* paramList = (yyvsp[-6].plist);
                  ParamList* accelParamList = (yyvsp[-5].plist);
		  XStr* codeBody = new XStr((yyvsp[-3].strval));
                  const char* callbackName = (yyvsp[-1].strval);

                  (yyval.entry) = new Entry(lineno, attribs, new BuiltinType("void"), name, paramList, 0, 0, 0 );
                  (yyval.entry)->setAccelParam(accelParamList);
                  (yyval.entry)->setAccelCodeBody(codeBody);
                  (yyval.entry)->setAccelCallbackName(new XStr(callbackName));
                }
#line 3829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 928 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.accelBlock) = new AccelBlock(lineno, new XStr((yyvsp[-2].strval))); }
#line 3835 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 930 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.accelBlock) = new AccelBlock(lineno, NULL); }
#line 3841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 934 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.type) = (yyvsp[0].type); }
#line 3847 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 938 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = 0; }
#line 3853 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 940 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-1].intval); }
#line 3859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 942 "xi-grammar.y" /* yacc.c:1646  */
    { ERROR("invalid entry method attribute list",
		        (yyloc).first_column, (yyloc).last_column);
		  YYABORT;
		}
#line 3868 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 949 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[0].intval); }
#line 3874 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 951 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = (yyvsp[-2].intval) | (yyvsp[0].intval); }
#line 3880 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 955 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = STHREADED; }
#line 3886 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 957 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SSYNC; }
#line 3892 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 959 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SIGET; }
#line 3898 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 961 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SLOCKED; }
#line 3904 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 963 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SCREATEHERE; }
#line 3910 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 965 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SCREATEHOME; }
#line 3916 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 967 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SNOKEEP; }
#line 3922 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 969 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SNOTRACE; }
#line 3928 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 971 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SAPPWORK; }
#line 3934 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 973 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SIMMEDIATE; }
#line 3940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 975 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SSKIPSCHED; }
#line 3946 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 977 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SINLINE; }
#line 3952 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 979 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SLOCAL; }
#line 3958 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 981 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SPYTHON; }
#line 3964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 983 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SMEM; }
#line 3970 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 985 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SREDUCE; }
#line 3976 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 987 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = SREENTRANT; }
#line 3982 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 989 "xi-grammar.y" /* yacc.c:1646  */
    {
		  ERROR("invalid entry method attribute",
		        (yylsp[0]).first_column, (yylsp[0]).last_column);
		  yyclearin;
		  yyerrok;
		}
#line 3993 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 998 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = new Value((yyvsp[0].strval)); }
#line 3999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1000 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = new Value((yyvsp[0].strval)); }
#line 4005 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1002 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = new Value((yyvsp[0].strval)); }
#line 4011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1006 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = ""; }
#line 4017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1008 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 4023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1010 "xi-grammar.y" /* yacc.c:1646  */
    {  /*Returned only when in_bracket*/
			char *tmp = new char[strlen((yyvsp[-2].strval))+strlen((yyvsp[0].strval))+3];
			sprintf(tmp,"%s, %s", (yyvsp[-2].strval), (yyvsp[0].strval));
			(yyval.strval) = tmp;
		}
#line 4033 "y.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1018 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = ""; }
#line 4039 "y.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1020 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 4045 "y.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1022 "xi-grammar.y" /* yacc.c:1646  */
    {  /*Returned only when in_bracket*/
			char *tmp = new char[strlen((yyvsp[-4].strval))+strlen((yyvsp[-2].strval))+strlen((yyvsp[0].strval))+3];
			sprintf(tmp,"%s[%s]%s", (yyvsp[-4].strval), (yyvsp[-2].strval), (yyvsp[0].strval));
			(yyval.strval) = tmp;
		}
#line 4055 "y.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1028 "xi-grammar.y" /* yacc.c:1646  */
    { /*Returned only when in_braces*/
			char *tmp = new char[strlen((yyvsp[-4].strval))+strlen((yyvsp[-2].strval))+strlen((yyvsp[0].strval))+3];
			sprintf(tmp,"%s{%s}%s", (yyvsp[-4].strval), (yyvsp[-2].strval), (yyvsp[0].strval));
			(yyval.strval) = tmp;
		}
#line 4065 "y.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1034 "xi-grammar.y" /* yacc.c:1646  */
    { /*Returned only when in_braces*/
			char *tmp = new char[strlen((yyvsp[-4].strval))+strlen((yyvsp[-2].strval))+strlen((yyvsp[0].strval))+3];
			sprintf(tmp,"%s(%s)%s", (yyvsp[-4].strval), (yyvsp[-2].strval), (yyvsp[0].strval));
			(yyval.strval) = tmp;
		}
#line 4075 "y.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1040 "xi-grammar.y" /* yacc.c:1646  */
    { /*Returned only when in_braces*/
			char *tmp = new char[strlen((yyvsp[-2].strval))+strlen((yyvsp[0].strval))+3];
			sprintf(tmp,"(%s)%s", (yyvsp[-2].strval), (yyvsp[0].strval));
			(yyval.strval) = tmp;
		}
#line 4085 "y.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1048 "xi-grammar.y" /* yacc.c:1646  */
    {  /*Start grabbing CPROGRAM segments*/
			in_bracket=1;
			(yyval.pname) = new Parameter(lineno, (yyvsp[-2].type),(yyvsp[-1].strval));
		}
#line 4094 "y.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1055 "xi-grammar.y" /* yacc.c:1646  */
    { 
                   /*Start grabbing CPROGRAM segments*/
			in_braces=1;
			(yyval.intval) = 0;
		}
#line 4104 "y.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1063 "xi-grammar.y" /* yacc.c:1646  */
    { 
			in_braces=0;
			(yyval.intval) = 0;
		}
#line 4113 "y.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1070 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.pname) = new Parameter(lineno, (yyvsp[0].type));}
#line 4119 "y.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1072 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.pname) = new Parameter(lineno, (yyvsp[-2].type),(yyvsp[-1].strval)); (yyval.pname)->setConditional((yyvsp[0].intval)); }
#line 4125 "y.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1074 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.pname) = new Parameter(lineno, (yyvsp[-3].type),(yyvsp[-2].strval),0,(yyvsp[0].val));}
#line 4131 "y.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1076 "xi-grammar.y" /* yacc.c:1646  */
    { /*Stop grabbing CPROGRAM segments*/
			in_bracket=0;
			(yyval.pname) = new Parameter(lineno, (yyvsp[-2].pname)->getType(), (yyvsp[-2].pname)->getName() ,(yyvsp[-1].strval));
		}
#line 4140 "y.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1082 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = Parameter::ACCEL_BUFFER_TYPE_READONLY; }
#line 4146 "y.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1083 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = Parameter::ACCEL_BUFFER_TYPE_READWRITE; }
#line 4152 "y.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1084 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intval) = Parameter::ACCEL_BUFFER_TYPE_WRITEONLY; }
#line 4158 "y.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1087 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.xstrptr) = new XStr((yyvsp[0].strval)); }
#line 4164 "y.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1088 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.xstrptr) = new XStr(""); *((yyval.xstrptr)) << *((yyvsp[-3].xstrptr)) << "->" << (yyvsp[0].strval); }
#line 4170 "y.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1089 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.xstrptr) = new XStr(""); *((yyval.xstrptr)) << *((yyvsp[-2].xstrptr)) << "." << (yyvsp[0].strval); }
#line 4176 "y.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1091 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyval.xstrptr) = new XStr("");
                  *((yyval.xstrptr)) << *((yyvsp[-3].xstrptr)) << "[" << *((yyvsp[-1].xstrptr)) << "]";
                  delete (yyvsp[-3].xstrptr);
                  delete (yyvsp[-1].xstrptr);
                }
#line 4187 "y.tab.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1098 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyval.xstrptr) = new XStr("");
                  *((yyval.xstrptr)) << *((yyvsp[-3].xstrptr)) << "[" << (yyvsp[-1].strval) << "]";
                  delete (yyvsp[-3].xstrptr);
                }
#line 4197 "y.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1104 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyval.xstrptr) = new XStr("");
                  *((yyval.xstrptr)) << *((yyvsp[-3].xstrptr)) << "(" << *((yyvsp[-1].xstrptr)) << ")";
                  delete (yyvsp[-3].xstrptr);
                  delete (yyvsp[-1].xstrptr);
                }
#line 4208 "y.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1113 "xi-grammar.y" /* yacc.c:1646  */
    {
                  in_bracket = 0;
                  (yyval.pname) = new Parameter(lineno, (yyvsp[-2].pname)->getType(), (yyvsp[-2].pname)->getName(), (yyvsp[-1].strval));
                }
#line 4217 "y.tab.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1120 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyval.pname) = new Parameter(lineno, (yyvsp[-4].type), (yyvsp[-3].strval));
                  (yyval.pname)->setAccelInstName((yyvsp[-1].xstrptr));
                  (yyval.pname)->setAccelBufferType((yyvsp[-6].intval));
                }
#line 4227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1126 "xi-grammar.y" /* yacc.c:1646  */
    {
		  (yyval.pname) = new Parameter(lineno, (yyvsp[-4].type), (yyvsp[-3].strval));
                  (yyval.pname)->setAccelInstName((yyvsp[-1].xstrptr));
                  (yyval.pname)->setAccelBufferType(Parameter::ACCEL_BUFFER_TYPE_READWRITE);
		}
#line 4237 "y.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1132 "xi-grammar.y" /* yacc.c:1646  */
    {
                  (yyval.pname) = (yyvsp[-3].pname);
                  (yyval.pname)->setAccelInstName((yyvsp[-1].xstrptr));
                  (yyval.pname)->setAccelBufferType((yyvsp[-5].intval));
		}
#line 4247 "y.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1140 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = new ParamList((yyvsp[0].pname)); }
#line 4253 "y.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1142 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = new ParamList((yyvsp[-2].pname),(yyvsp[0].plist)); }
#line 4259 "y.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1146 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = new ParamList((yyvsp[0].pname)); }
#line 4265 "y.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1148 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = new ParamList((yyvsp[-2].pname),(yyvsp[0].plist)); }
#line 4271 "y.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1152 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = (yyvsp[-1].plist); }
#line 4277 "y.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1154 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = new ParamList(new Parameter(0, new BuiltinType("void"))); }
#line 4283 "y.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1158 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = (yyvsp[-1].plist); }
#line 4289 "y.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1160 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.plist) = 0; }
#line 4295 "y.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 1164 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = 0; }
#line 4301 "y.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 1166 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.val) = new Value((yyvsp[0].strval)); }
#line 4307 "y.tab.c" /* yacc.c:1646  */
    break;

  case 318:
#line 1170 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sentry) = 0; }
#line 4313 "y.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 1172 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sentry) = new SdagEntryConstruct((yyvsp[0].sc)); }
#line 4319 "y.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 1174 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sentry) = new SdagEntryConstruct((yyvsp[-2].slist)); }
#line 4325 "y.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 1178 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.slist) = new SListConstruct((yyvsp[0].sc)); }
#line 4331 "y.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 1180 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.slist) = new SListConstruct((yyvsp[-1].sc), (yyvsp[0].slist));  }
#line 4337 "y.tab.c" /* yacc.c:1646  */
    break;

  case 323:
#line 1184 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.olist) = new OListConstruct((yyvsp[0].sc)); }
#line 4343 "y.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 1186 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.olist) = new OListConstruct((yyvsp[-1].sc), (yyvsp[0].slist)); }
#line 4349 "y.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 1190 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.clist) = new CaseListConstruct((yyvsp[0].when)); }
#line 4355 "y.tab.c" /* yacc.c:1646  */
    break;

  case 326:
#line 1192 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.clist) = new CaseListConstruct((yyvsp[-1].when), (yyvsp[0].clist)); }
#line 4361 "y.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 1194 "xi-grammar.y" /* yacc.c:1646  */
    {
		  ERROR("case blocks can only contain when clauses",
		        (yylsp[0]).first_column, (yylsp[0]).last_column);
		  (yyval.clist) = 0;
		}
#line 4371 "y.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 1202 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 4377 "y.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 1204 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.strval) = 0; }
#line 4383 "y.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 1208 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = new WhenConstruct((yyvsp[-2].entrylist), 0); }
#line 4389 "y.tab.c" /* yacc.c:1646  */
    break;

  case 331:
#line 1210 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = new WhenConstruct((yyvsp[-1].entrylist), (yyvsp[0].sc)); }
#line 4395 "y.tab.c" /* yacc.c:1646  */
    break;

  case 332:
#line 1212 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = new WhenConstruct((yyvsp[-3].entrylist), (yyvsp[-1].slist)); }
#line 4401 "y.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 1216 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4407 "y.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 1218 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4413 "y.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 1220 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4419 "y.tab.c" /* yacc.c:1646  */
    break;

  case 336:
#line 1222 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4425 "y.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 1224 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4431 "y.tab.c" /* yacc.c:1646  */
    break;

  case 338:
#line 1226 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4437 "y.tab.c" /* yacc.c:1646  */
    break;

  case 339:
#line 1228 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4443 "y.tab.c" /* yacc.c:1646  */
    break;

  case 340:
#line 1230 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4449 "y.tab.c" /* yacc.c:1646  */
    break;

  case 341:
#line 1232 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4455 "y.tab.c" /* yacc.c:1646  */
    break;

  case 342:
#line 1234 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4461 "y.tab.c" /* yacc.c:1646  */
    break;

  case 343:
#line 1236 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4467 "y.tab.c" /* yacc.c:1646  */
    break;

  case 344:
#line 1238 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.when) = 0; }
#line 4473 "y.tab.c" /* yacc.c:1646  */
    break;

  case 345:
#line 1242 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new AtomicConstruct((yyvsp[-1].strval), (yyvsp[-3].strval), (yylsp[-2]).first_line); }
#line 4479 "y.tab.c" /* yacc.c:1646  */
    break;

  case 346:
#line 1244 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new OverlapConstruct((yyvsp[-1].olist)); }
#line 4485 "y.tab.c" /* yacc.c:1646  */
    break;

  case 347:
#line 1246 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = (yyvsp[0].when); }
#line 4491 "y.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 1248 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new CaseConstruct((yyvsp[-1].clist)); }
#line 4497 "y.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 1250 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ForConstruct((yyvsp[-8].intexpr), (yyvsp[-6].intexpr), (yyvsp[-4].intexpr), (yyvsp[-1].slist)); }
#line 4503 "y.tab.c" /* yacc.c:1646  */
    break;

  case 350:
#line 1252 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ForConstruct((yyvsp[-6].intexpr), (yyvsp[-4].intexpr), (yyvsp[-2].intexpr), (yyvsp[0].sc)); }
#line 4509 "y.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 1254 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ForallConstruct(new SdagConstruct(SIDENT, (yyvsp[-9].strval)), (yyvsp[-6].intexpr),
		             (yyvsp[-4].intexpr), (yyvsp[-2].intexpr), (yyvsp[0].sc)); }
#line 4516 "y.tab.c" /* yacc.c:1646  */
    break;

  case 352:
#line 1257 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ForallConstruct(new SdagConstruct(SIDENT, (yyvsp[-11].strval)), (yyvsp[-8].intexpr),
		             (yyvsp[-6].intexpr), (yyvsp[-4].intexpr), (yyvsp[-1].slist)); }
#line 4523 "y.tab.c" /* yacc.c:1646  */
    break;

  case 353:
#line 1260 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new IfConstruct((yyvsp[-3].intexpr), (yyvsp[-1].sc), (yyvsp[0].sc)); }
#line 4529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 1262 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new IfConstruct((yyvsp[-5].intexpr), (yyvsp[-2].slist), (yyvsp[0].sc)); }
#line 4535 "y.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 1264 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new WhileConstruct((yyvsp[-2].intexpr), (yyvsp[0].sc)); }
#line 4541 "y.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 1266 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new WhileConstruct((yyvsp[-4].intexpr), (yyvsp[-1].slist)); }
#line 4547 "y.tab.c" /* yacc.c:1646  */
    break;

  case 357:
#line 1268 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new AtomicConstruct((yyvsp[-1].strval), NULL, (yyloc).first_line); }
#line 4553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 1270 "xi-grammar.y" /* yacc.c:1646  */
    {
		  ERROR("unknown SDAG construct or malformed entry method declaration.\n"
		        "You may have forgotten to terminate a previous entry method declaration with a"
		        " semicolon or forgotten to mark a block of sequential SDAG code as 'atomic'",
		        (yyloc).first_column, (yyloc).last_column);
		  YYABORT;
		}
#line 4565 "y.tab.c" /* yacc.c:1646  */
    break;

  case 359:
#line 1280 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = 0; }
#line 4571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 1282 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ElseConstruct((yyvsp[0].sc)); }
#line 4577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 1284 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.sc) = new ElseConstruct((yyvsp[-1].slist)); }
#line 4583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 362:
#line 1288 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.intexpr) = new IntExprConstruct((yyvsp[0].strval)); }
#line 4589 "y.tab.c" /* yacc.c:1646  */
    break;

  case 363:
#line 1292 "xi-grammar.y" /* yacc.c:1646  */
    { in_int_expr = 0; (yyval.intval) = 0; }
#line 4595 "y.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 1296 "xi-grammar.y" /* yacc.c:1646  */
    { in_int_expr = 1; (yyval.intval) = 0; }
#line 4601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 1300 "xi-grammar.y" /* yacc.c:1646  */
    {
		  (yyval.entry) = new Entry(lineno, 0, 0, (yyvsp[-1].strval), (yyvsp[0].plist), 0, 0, 0, (yyloc).first_line, (yyloc).last_line);
		}
#line 4609 "y.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 1304 "xi-grammar.y" /* yacc.c:1646  */
    {
		  (yyval.entry) = new Entry(lineno, 0, 0, (yyvsp[-4].strval), (yyvsp[0].plist), 0, 0, (yyvsp[-2].strval), (yyloc).first_line, (yyloc).last_line);
		}
#line 4617 "y.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 1310 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.entrylist) = new EntryList((yyvsp[0].entry)); }
#line 4623 "y.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 1312 "xi-grammar.y" /* yacc.c:1646  */
    { (yyval.entrylist) = new EntryList((yyvsp[-2].entry),(yyvsp[0].entrylist)); }
#line 4629 "y.tab.c" /* yacc.c:1646  */
    break;

  case 369:
#line 1316 "xi-grammar.y" /* yacc.c:1646  */
    { in_bracket=1; }
#line 4635 "y.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 1319 "xi-grammar.y" /* yacc.c:1646  */
    { in_bracket=0; }
#line 4641 "y.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 1323 "xi-grammar.y" /* yacc.c:1646  */
    { if (!macroDefined((yyvsp[0].strval), 1)) in_comment = 1; }
#line 4647 "y.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 1327 "xi-grammar.y" /* yacc.c:1646  */
    { if (!macroDefined((yyvsp[0].strval), 0)) in_comment = 1; }
#line 4653 "y.tab.c" /* yacc.c:1646  */
    break;


#line 4657 "y.tab.c" /* yacc.c:1646  */
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
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  yyerror_range[1] = yylloc;

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
                      yytoken, &yylval, &yylloc);
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

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

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

#if !defined yyoverflow || YYERROR_VERBOSE
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
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
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
  return yyresult;
}
#line 1330 "xi-grammar.y" /* yacc.c:1906  */


void yyerror(const char *msg) { }
