%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%{
/** TO HELP */
// Empty sequence node:
inline cdk::sequence_node* empty_seq_node(int lineno) {
    return new cdk::sequence_node(lineno);
}

// Size of gr8 datatypes:
struct SIZE {
public:
  static const int SMALL = 4;
  static const int HUGE  = 8;
  static const int FAKE  = 4;
  static const int NEWS  = 4;
  static const int ZERO  = 0;
};
%}

%union {
  int                   i;  /* integer value */
  double                d;  /* double value */
  std::string          *s;  /* symbol name or string literal */
  cdk::basic_node      *node; /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  basic_type           *type;
  gr8::block_node      *blocknode;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tSMALL tHUGE tNEWS tFAKE tUSE tPUBLIC tDEFINE tPROCEDURE tFUNCTION tON tAS tDO tUSES tFOR tRETURN tASSIGN tTO tEQUALS tINPUT tSTOP tAGAIN tPOST tTWEET tSWEEPING tFROM tBY tNULL tIF tELSIF


%nonassoc tTHEN
%nonassoc tELSE tINITIALLY
%nonassoc pLOWER

%left '|'
%left '&'
%nonassoc '!'
%left tEQUALS
%left '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tOBJECTS
%nonassoc '?'
%nonassoc pUNARY tCELL tAT
%nonassoc '('


%type <node> declaration variable function function_dec function_def arg instruction assignment conditional elsif iteration
%type <sequence> declarations parameters_dec parameters_def args instructions expressions
%type <expression> expression initially function_call
%type <lvalue> leftvalue
%type <type> gr8_type ltypes ltype_s ltype_h rtypes rtype type
%type <blocknode> block
%type <i> literal_int
%type <s> strings


%{
/**
 * NOTES TO SELF:
 * - ltypes  -> types appearing on the left;
 * - ltype_s -> "small" appearing on the left; small small fake
 * - ltype_h -> "huge" appearing on the left;  huge  huge  fake
 *
 * - rtypes  -> types appearing on the right;
 * - rtype   -> "news" appearing on the right; fake  fake  news

 * -----

 * TODO:
 * clean if
 * corpos vs. blocks

 * -----

 * QUESTIONS:
 * %token<i|d> tINPUT ???
 * 
 */
//-- The rules below will be included in yyparse, the main parsing function.
%}


%%


file          : declarations                            { compiler->ast($1); }
              |                                         { compiler->ast(empty_seq_node(LINE)); }
              ;

declarations  : declaration                             { $$ = new cdk::sequence_node(LINE, $1); }
              | declarations declaration                { $$ = new cdk::sequence_node(LINE, $2, $1); }
              ;

declaration   : variable ';'                            { $$ = $1; }
              | function                                { $$ = $1; }
              ;


variable      : gr8_type tIDENTIFIER                      { $$ = new gr8::variable_declaration_node(LINE, false, false, $1, $2, nullptr); }
              | gr8_type tIDENTIFIER initially            { $$ = new gr8::variable_declaration_node(LINE, false, false, $1, $2,      $3); }
              | tUSE gr8_type tIDENTIFIER                 { $$ = new gr8::variable_declaration_node(LINE, false, true , $2, $3, nullptr); }
              | tPUBLIC gr8_type tIDENTIFIER              { $$ = new gr8::variable_declaration_node(LINE, true,  false, $2, $3, nullptr); }
              | tPUBLIC gr8_type tIDENTIFIER initially    { $$ = new gr8::variable_declaration_node(LINE, true,  false, $2, $3,      $4); }
              ;

initially     : '(' tINITIALLY expression ')'             { $$ = $3; }
              ;


function      : function_dec ';'                          { $$ = $1; }
              | function_def                              { $$ = $1; }
              ;

function_dec  : tPUBLIC gr8_type tFUNCTION tIDENTIFIER parameters_dec {
                    $$ = new gr8::function_declaration_node(LINE, true,  $2, $4, $5);
                }
              | gr8_type tFUNCTION tIDENTIFIER parameters_dec         {
                    $$ = new gr8::function_declaration_node(LINE, false, $1, $3, $4);
                }
              | tPUBLIC tPROCEDURE tIDENTIFIER parameters_dec         {
                    $$ = new gr8::function_declaration_node(LINE, true,  new basic_type(SIZE::ZERO, basic_type::TYPE_VOID), $3, $4);
                }
              | tPROCEDURE tIDENTIFIER parameters_dec                 {
                    $$ = new gr8::function_declaration_node(LINE, false, new basic_type(SIZE::ZERO, basic_type::TYPE_VOID), $2, $3);
                }
              ;

parameters_dec: tUSES args  { $$ = $2; }
              |             { $$ = empty_seq_node(LINE); }
              ;

function_def  : tDEFINE tPUBLIC gr8_type tFUNCTION tIDENTIFIER parameters_def tAS block {
                    $$ = new gr8::function_definition_node(LINE, true,  $3, $5, $6, $8);
                }
              | tDEFINE gr8_type tFUNCTION tIDENTIFIER parameters_def tAS block         {
                    $$ = new gr8::function_definition_node(LINE, false, $2, $4, $5, $7);
                }
              | tDEFINE tPUBLIC tPROCEDURE tIDENTIFIER parameters_def tAS block         {
                    $$ = new gr8::function_definition_node(LINE, true,  new basic_type(SIZE::ZERO, basic_type::TYPE_VOID), $4, $5, $7);
                }
              | tDEFINE tPROCEDURE tIDENTIFIER parameters_def tAS block                 {
                    $$ = new gr8::function_definition_node(LINE, false, new basic_type(SIZE::ZERO, basic_type::TYPE_VOID), $3, $4, $6);
                }
              ;

parameters_def: tON args                  { $$ = $2; }
              |                           { $$ = empty_seq_node(LINE); }
              ;

args          : arg                       { $$ = new cdk::sequence_node(LINE, $1);     }
              | args ',' arg              { $$ = new cdk::sequence_node(LINE, $3, $1); }
              ;

arg           : gr8_type tIDENTIFIER      { $$ = new gr8::variable_declaration_node(LINE, false, false, $1, $2, nullptr); }
              ;


gr8_type      : ltypes                    { $$ = $1; /* TODO */ }
              | rtypes                    { $$ = $1; /* TODO */ }
              ;

ltypes        : ltype_s tFAKE             { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_POINTER); $$->_subtype = $1; }
              | ltype_h tFAKE             { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_POINTER); $$->_subtype = $1; }
              | type                      { $$ = $1; }
              ;

ltype_s       : tSMALL                    { $$ = new basic_type(SIZE::SMALL, basic_type::TYPE_INT); }
              | ltype_s tSMALL            { $$ = new basic_type(SIZE::FAKE,  basic_type::TYPE_POINTER); $$->_subtype = $1; }
              ;

ltype_h       : tHUGE                     { $$ = new basic_type(SIZE::HUGE, basic_type::TYPE_DOUBLE); }
              | ltype_h tHUGE             { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_POINTER); $$->_subtype = $1; }
              ;

rtypes        : rtype tNEWS               { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_POINTER); $$->_subtype = $1; }
              ;

rtype         : tFAKE                     { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_STRING); /* Tricky, since it's in the reverse order. */ }
              | rtype tFAKE               { $$ = new basic_type(SIZE::FAKE, basic_type::TYPE_POINTER); $$->_subtype = $1; }
              ;

type          : tSMALL                    { $$ = new basic_type(SIZE::SMALL, basic_type::TYPE_INT   ); }
              | tHUGE                     { $$ = new basic_type(SIZE::HUGE,  basic_type::TYPE_DOUBLE); }
              | tNEWS                     { $$ = new basic_type(SIZE::NEWS,  basic_type::TYPE_STRING); }
              ;


block         : '{' declarations '}'                    { $$ = new gr8::block_node(LINE, $2, empty_seq_node(LINE)); }
              | '{' instructions '}'                    { $$ = new gr8::block_node(LINE, empty_seq_node(LINE), $2); }
              | '{' declarations instructions '}'       { $$ = new gr8::block_node(LINE, $2, $3); }
              ;

instructions  : instruction                             { $$ = new cdk::sequence_node(LINE, $1); }
              | instructions instruction                { $$ = new cdk::sequence_node(LINE, $2, $1); }
              ;

instruction   : expression ';'                          { $$ = new gr8::evaluation_node(LINE, $1); }
              | assignment ';'                          { $$ = $1; }
              | tTWEET expression ';'                   { $$ = new gr8::tweet_node (LINE, $2); }
              | tPOST expression ';'                    { $$ = new gr8::post_node  (LINE, $2); }
              | tAGAIN literal_int ';'                  { $$ = new gr8::again_node (LINE, $2); }
              | tSTOP literal_int ';'                   { $$ = new gr8::stop_node  (LINE, $2); }
              | tAGAIN ';'                              { $$ = new gr8::again_node (LINE,  1); }
              | tSTOP ';'                               { $$ = new gr8::stop_node  (LINE,  1); }
              | tRETURN expression ';'                  { $$ = new gr8::return_node(LINE, $2); }
              | tRETURN ';'                             { $$ = new gr8::return_node(LINE, nullptr); }
              | conditional                             { $$ = $1; }
              | iteration                               { $$ = $1; }
              | block                                   { $$ = $1; }
              ;

assignment    : tASSIGN expression tTO leftvalue        { $$ = new cdk::assignment_node(LINE, $4, $2); }
              ;

literal_int   : tINTEGER                                { $$ = $1; }
              ;

conditional   : tIF expression tTHEN block                       { $$ = new gr8::if_node(LINE, $2, $4); }
              | tIF expression tTHEN block tELSE block           { $$ = new gr8::if_else_node(LINE, $2, $4, $6); }
              | tIF expression tTHEN block tELSIF elsif          { $$ = new gr8::if_else_node(LINE, $2, $4, $6); }
              ;

elsif         : expression tTHEN block tELSE block               { $$ = new gr8::if_else_node(LINE, $1, $3, $5); }
              | expression tTHEN block tELSIF elsif              { $$ = new gr8::if_else_node(LINE, $1, $3, $5); }
              | expression tTHEN block                           { $$ = new gr8::if_node(LINE, $1, $3); }
              ;

iteration     : tSWEEPING leftvalue tFROM expression tTO expression tDO block                {
                    $$ = new gr8::sweeping_from_to_by_do_node(LINE, $2, $4, $6, new cdk::integer_node(LINE, 1), $8);
                }
              | tSWEEPING leftvalue tFROM expression tTO expression tBY expression tDO block {
                    $$ = new gr8::sweeping_from_to_by_do_node(LINE, $2, $4, $6, $8, $10);
                }
              ;

expressions   : expression                            { $$ = new cdk::sequence_node(LINE, $1);     }
              | expressions ',' expression            { $$ = new cdk::sequence_node(LINE, $3, $1); }
              ;

expression    : '(' expression ')'                    { $$ = $2; }
              | '+' expression %prec pUNARY           { $$ = new gr8::plus_node(LINE, $2    ); }
              | '-' expression %prec pUNARY           { $$ = new cdk::neg_node (LINE, $2    ); }
              | '!' expression                        { $$ = new cdk::not_node (LINE, $2    ); }
              | expression '+' expression             { $$ = new cdk::add_node (LINE, $1, $3); }
              | expression '-' expression             { $$ = new cdk::sub_node (LINE, $1, $3); }
              | expression '*' expression             { $$ = new cdk::mul_node (LINE, $1, $3); }
              | expression '/' expression             { $$ = new cdk::div_node (LINE, $1, $3); }
              | expression '%' expression             { $$ = new cdk::mod_node (LINE, $1, $3); }
              | expression '<' expression             { $$ = new cdk::lt_node  (LINE, $1, $3); }
              | expression '>' expression             { $$ = new cdk::gt_node  (LINE, $1, $3); }
              | expression tEQUALS expression         { $$ = new cdk::eq_node  (LINE, $1, $3); }
              | expression '&' expression             { $$ = new cdk::and_node (LINE, $1, $3); }
              | expression '|' expression             { $$ = new cdk::or_node  (LINE, $1, $3); }
              | leftvalue '?'                         { $$ = new gr8::address_node(LINE, $1); }
              | expression tOBJECTS                   { $$ = new gr8::objects_node(LINE, $1); }
              | leftvalue                %prec pLOWER { $$ = new cdk::rvalue_node (LINE, $1); }
              | function_call                         { $$ = $1; }
              | tINPUT                                { $$ = new gr8::input_node(LINE/*, $1->val ???*/); }
              | tNULL                                 { $$ = new gr8::null_node   (LINE);     }
              | tINTEGER                              { $$ = new cdk::integer_node(LINE, $1); }
              | tDOUBLE                               { $$ = new cdk::double_node (LINE, $1); }
              | strings                               { $$ = new cdk::string_node (LINE, $1); }
              ;

leftvalue     : tIDENTIFIER                           { $$ = new cdk::identifier_node(LINE, $1); }
              | tCELL expression tAT expression       { $$ = new gr8::cell_node(LINE, $4, $2); }
              ;

function_call : tUSE expressions tFOR tIDENTIFIER     { $$ = new gr8::function_invocation_node(LINE, $4,               $2); }
              | tDO tIDENTIFIER                       { $$ = new gr8::function_invocation_node(LINE, $2, empty_seq_node(LINE)); }
              ;

strings       : tSTRING                               { $$ = $1; /* Pass the string here to account for str concat. */ }
              | strings tSTRING                       { $$ = new std::string(*$1 + *$2); delete $1; delete $2; /* Seen in class.*/ }
              ;

%%
