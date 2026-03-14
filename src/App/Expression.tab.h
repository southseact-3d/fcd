#ifndef YY_YY_EXPRESSION_TAB_H_INCLUDED
# define YY_YY_EXPRESSION_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    FUNC = 258,                    /* FUNC  */
    ONE = 259,                     /* ONE  */
    NUM = 260,                     /* NUM  */
    IDENTIFIER = 261,              /* IDENTIFIER  */
    UNIT = 262,                    /* UNIT  */
    USUNIT = 263,                  /* USUNIT  */
    INTEGER = 264,                 /* INTEGER  */
    CONSTANT = 265,                /* CONSTANT  */
    CELLADDRESS = 266,             /* CELLADDRESS  */
    EQ = 267,                      /* EQ  */
    NEQ = 268,                     /* NEQ  */
    LT = 269,                      /* LT  */
    GT = 270,                      /* GT  */
    GTE = 271,                     /* GTE  */
    LTE = 272,                     /* LTE  */
    STRING = 273,                  /* STRING  */
    MINUSSIGN = 274,               /* MINUSSIGN  */
    PROPERTY_REF = 275,            /* PROPERTY_REF  */
    DOCUMENT = 276,                /* DOCUMENT  */
    OBJECT = 277,                  /* OBJECT  */
    EXPONENT = 278,                /* EXPONENT  */
    NUM_AND_UNIT = 279,            /* NUM_AND_UNIT  */
    NEG = 280,                     /* NEG  */
    POS = 281                      /* POS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_EXPRESSION_TAB_H_INCLUDED  */
