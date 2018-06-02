%option c++ prefix="gr8_scanner_" outfile="gr8_scanner.cpp"
%option stack noyywrap yylineno 8bit debug

%{

// make relevant includes before including the parser's tab file
#include <string>
#include <vector>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "gr8_parser.tab.h"

// don't change this
#define yyerror LexerError

%}


%x X_STRING X_STRING_END X_STRING_SP X_IGNORE
%x X_OCOMMENT
%x X_INDENTATION X_CLOSE_INDENTATION

%{
// INDENTATION AND TERMNIATORS
bool start = true;
bool semi_colon = true;
int block_size_counter = 0;
int blocks_to_close = 0;
static std::vector<int> blocks = { 0 }; // Indentation level.
%}

%{
/**
 * SHORTHANDS (PCRE-like)
 * _W -> \w, but without '_' (cooler this way, for gr8).
 * _D -> \d.
 * _H -> \h (shorter than [[:blank:]]).
 */
%}
_W                     [A-Za-z]
_D                     [0-9]
_H                     [ \t]


%{
/**
 * SHORTCUTS
 * INT7        -> 01 00000006
 * INT10       -> 0 1 23
 * EXPONENTIAL -> 1.2e-3 | .2e3 | 1e-3
 * DOUBLE      -> 0.0 | .1 | ^       // \d*\.\d+([Ee][+-]?\d+)?|\d+[Ee]\d+
 */
%}
INT7                   0[0-6]*[1-6]+
INT10                  0|[1-9][0-9]*
DOUBLE                 ({_D}*\.{_D}+|{_D}+\.)([Ee][+-]?{_D}+)?|{_D}+[Ee][+-]?{_D}+

%{
/**
 * TODO
 * overflow 
 */
%}

%%



"small"                       return tSMALL;
"huge"                        return tHUGE;
"news"                        return tNEWS;
"fake"                        return tFAKE;
"initially"                   return tINITIALLY;
"use"                         return tUSE;
"public"                      return tPUBLIC;
"define"                      return tDEFINE;
"procedure"                   return tPROCEDURE;
"function"                    return tFUNCTION;
"on"                          return tON;
"as"                          return tAS;
"do"                          return tDO;
"uses"                        return tUSES;
"for"                         return tFOR;
"return"                      return tRETURN;
"plus"                        return '+';
"minus"                       return '-';
"times"                       return '*';
"over"                        return '/';
"modulus"                     return '%';
"not"                         return '!';
"and"                         return '&';
"or"                          return '|';
"assign"                      return tASSIGN;
"to"                          return tTO;
"cell"                        return tCELL;
"at"                          return tAT;
"above"                       return '>';
"below"                       return '<';
"equals"                      return tEQUALS;
"input"                       return tINPUT;
"objects"                     return tOBJECTS;
"if"                          return tIF;
"then"                        return tTHEN;
"elsif"                       return tELSIF;
"else"                        return tELSE;
"stop"                        return tSTOP;
"again"                       return tAGAIN;
"post"                        return tPOST;
"tweet"                       return tTWEET;
"sweeping"                    return tSWEEPING;
"from"                        return tFROM;
"by"                          return tBY;
"null"                        return tNULL;



"<<"                          yy_push_state(X_OCOMMENT);
<X_OCOMMENT>"<<"              yy_push_state(X_OCOMMENT);
<X_OCOMMENT>">>"              yy_pop_state();
<X_OCOMMENT>.|\n              ; /* Operational comments' content (ignore). */

"!!".*$                       ; /* Explicative comments (ignore). */


{_W}[A-Za-z0-9-]*             yylval.s = new std::string(yytext); return tIDENTIFIER;


\"                            yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING,X_IGNORE>\"         yy_pop_state(); return tSTRING;
<X_STRING>~                   yy_push_state(X_STRING_SP);
<X_STRING>.                   *yylval.s += yytext;
<X_STRING,X_IGNORE>\n         yyerror("newline in string");

<X_STRING_SP>{
	 \"                       *yylval.s += yytext + 1; yy_pop_state();
	"~"                       *yylval.s += yytext; yy_pop_state();
	"n"                       *yylval.s += "\n"; yy_pop_state();
	"r"                       *yylval.s += "\r"; yy_pop_state();
	"t"                       *yylval.s += "\t"; yy_pop_state();
	[1-6][0-6]{0,2}           *yylval.s += std::string(1, strtol(yytext, nullptr, 7)); yy_pop_state();
	"0"                       yy_push_state(X_STRING_END);
	 .                        yyless(0); yy_pop_state();
}


<X_STRING_END>[1-6]{1,2}      {
	*yylval.s += std::string(1, strtol(yytext, nullptr, 7)); // http://www.cplusplus.com/reference/string/string/string/ (6) fill constructor
	yy_pop_state(); // Keep processing the string (X_STRING_END -> X_STRING_SP).
}
<X_STRING_END>.               {
	yy_pop_state(); // Nothing useful anymore; return to the string special character scan (X_STRING_END -> X_STRING_SP).
	yy_pop_state(); // Get back to the string scan (X_STRING_SP -> X_STRING).
	yy_pop_state(); // Get back to the inital state (X_STRING -> INITIAL).
	yy_push_state(X_IGNORE); // We need to ignore the rest of the supposed string.
}
<X_IGNORE>~\"                 ; /* Ignore. */
<X_IGNORE>.                   ; /* Ignore. */


{INT7}                        {
	yylval.i = strtol(yytext, nullptr,  7);
	if ( yylval.i > 255 ) {
		yyerror("Overflow!");
	}
	return tINTEGER;
}
{INT10}                       yylval.i = strtol(yytext, nullptr, 10); return tINTEGER;
{DOUBLE}                      yylval.d = strtod(yytext, nullptr    ); return tDOUBLE;


[?(),]                        return *yytext;


"..."\n                       ; /* Ignore line feed here, since the next line is still (continuation of) a logic line. */

\n                            ; /* Ignore line feed. It is handled in other rules. */
\r                            ; /* Ignore carriage return. */
^{_H}+$                       ; /* Ignore empty lines. */

^.+$                         {
	yy_push_state(X_INDENTATION);
	block_size_counter = 0;
	yyless(0); // Check the input again, to be processed later inside the X_INDENTATION state.
}
<X_INDENTATION>" "            { block_size_counter++; }
<X_INDENTATION>\t             {
	if ( (block_size_counter % 8) == 0 ) {
		block_size_counter += 8;
	} else {
		while ( (block_size_counter % 8) != 0 ) {
			block_size_counter++;
		}
	}
}
<X_INDENTATION>.              {
	bool nice_block = false;
	//std::cout << "X_INDENTATION: block_size_counter: " << block_size_counter << " blocks.back(): " << blocks.back() << std::endl;
	//yy_pop_state();
	yyless(0);

	if ( block_size_counter > blocks.back() ) {
		yy_pop_state();
		blocks.push_back( block_size_counter );
		blocks_to_close++;
		//std::cout << "OPENING { " << std::endl;
		return '{';
	}

	if ( block_size_counter < blocks.back() ) {
		//std::cout << "#### should send semi-colon?" << std::endl;
		if ( semi_colon ) {
			// Insert semi-colon before closing the block.
			semi_colon = false;
			//std::cout << "#### sending semi-colon" << std::endl;
			return ';';
		}
		yy_pop_state();
		semi_colon = true; // For the next time we want to close a newly opened block.
		for ( std::vector<int>::size_type i = 0; i < blocks.size(); i++ ) {
			// Check if the new indentation is in the vector.
			//std::cout << "FOR: block_size_counter: " << block_size_counter << " blocks[i]: " << blocks[i] << std::endl; 
			if ( block_size_counter == blocks[i] ) {
				nice_block = true;
				yy_push_state(X_CLOSE_INDENTATION);
				blocks_to_close = blocks.size() - 1 - i;
				//std::cout << "@@ blocks_to_close: " << blocks_to_close <<  " blocks.size(): " << blocks.size() << std::endl;
				for ( int c = 0; c < blocks_to_close; c++ ) {
					// Cleanup, very rudimentary...
					blocks.pop_back();
				}
				//std::cout << "@@ blocks_to_close: " << blocks_to_close <<  " blocks.size(): " << blocks.size() << std::endl;
				break;
			}
		}
		if ( !nice_block ) {
			yyerror("Unclosed block!");
		}
	} else {
		yy_pop_state();
		if ( start ) {
			start = false;
		} else {
			return ';';
		}
	}
}

<X_CLOSE_INDENTATION>.        {
	//std::cout << "!! closing indentation blocks_to_close: " << blocks_to_close << std::endl;
	yyless(0);
	if ( --blocks_to_close == 0 ) {
		yy_pop_state();
	}
	//std::cout << "CLOSING { " << std::endl;
	return '}';
}
<X_CLOSE_INDENTATION><<EOF>>  {
	//std::cout << "!! closing indentation EOF blocks_to_close: " << blocks_to_close << std::endl;
	yyless(0);
	if ( --blocks_to_close == 0 ) {
		yy_pop_state();
	}
	return '}';
}

<<EOF>>                       {
	//std::cout << "#### EOF should send semi-colon?" << std::endl;
	if ( semi_colon ) {
		// Insert semi-colon before closing the block.
		semi_colon = false;
		//std::cout << "#### EOF sending semi-colon" << std::endl;
		return ';';
	}
	blocks_to_close = blocks.size() - 1;
	blocks.pop_back();
	//std::cout << "!! EOF blocks_to_close: " << blocks_to_close << std::endl;
	if ( blocks_to_close > 0 ) {
		//yyless(0);
		yy_push_state(X_CLOSE_INDENTATION);
	} else {
		return 0;
	}
}


{_H}                          ; /* Ignore whitespace that is not for indentation. */

.                             /*std::cout << std::string(yytext) << std::endl;*/ yyerror("Unknown character");

%%
