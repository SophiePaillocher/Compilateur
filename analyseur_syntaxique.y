%{
#include<stdlib.h>
#include<stdio.h>
#define YYDEBUG 1
//#include"syntabs.h" // pour syntaxe abstraite
//extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
%}
%union {double dval; int ival; char[] sval;} 		// A verifier (dval util?)
%token OU
%token ET
%token NON
%token EGAL
%token INFERIEUR
%token SUPERIEUR
%token PLUS
%token MOINS
%token FOIS
%token DIVISE
%token PARENTHESE_OUVRANTE
%token PARENTHESE_FERMANTE
%token <sval> IDENTIF
%token <ival> NOMBRE			// A Verifier	(ival ou dval)
%token POINT_VIRGULE
%token CROCHET_OUVRANT
%token CROCHET_FERMANT
%token ACCOLADE_OUVRANTE
%token ACCOLADE_FERMANTE
%token SI
%token ALORS
%token SINON
%token TANTQUE
%token FAIRE
%token ENTIER
%token RETOUR
%token LIRE
%token ECRIRE
%token VIRGULE

% type <ival> expressionArithmetique conjonction comparaison somme produit negation expressionPrioritaire var fonction	// A verifier (ival?)

%start programme
%%

// Axiome de la grammaire
programme : ensembleDecDef
	|
	;
ensembleDecDef : decDef ensembleDecDef
	|	decDef
	;
decDef : ligneDeclarationsVars
	| definitionFct
	;


// grammaire des expressions arithmetiques

expressionArithmetique : expressionArithmetique OU conjonction 	{$$ = $1 || $3;}
		|	conjonction											{$$ = $1;}
		;
conjonction : conjonction ET comparaison	{$$ = $1 && $3;}
		|	comparaison						{$$ = $1;}
		;
comparaison	: comparaison EGAL somme		{$$ = $1 == $3;}
		|	comparaison INFERIEUR somme		{$$ = $1 < $3;}
		|	comparaison SUPERIEUR somme		{$$ = $1 > $3;}
		| somme								{$$ = $1;}
		;
somme : somme PLUS produit		{$$ = $1 + $3;}
	|	somme MOINS produit		{$$ = $1 - $3;}
	|	produit					{$$ = $1;}
	;
produit : produit FOIS negation		{$$ = $1 * $3;}
	|	produit DIVISE negation		{$$ = $1 / $3;}
	|	negation					{$$ = $1;}
	;
negation : NON expressionPrioritaire		{$$ = ! $2;}
	| expressionPrioritaire					{$$ = $1;}
	;
expressionPrioritaire : PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE		{$$ = $2;}
		|	var																				{$$ = $1;}
		|	NOMBRE																			{$$ = $1;}
		| 	fonction																		{$$ = $1;}
		;
var : IDENTIF																				{$$ = $1;}	
	| 	IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT						{$$ = $1[$3];}				// A verifier
	;
fonction : LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE					{$$ = lire();}			// A verifier
	| 	IDENTIF PARENTHESE_OUVRANTE argument PARENTHESE_FERMANTE		{$$ = $1($3);}		// A verifier
	;
argument : listArg		{$$ = $1;}			// A Verifier
		|				{$$ = /*$1*/;}		// A verifier
		;

listArg : expressionArithmetique					{$$ = $1;}		// A verifier
		|	expressionArithmetique VIRGULE listArg		{$$ = $1;}	// A verifier
		;


// Grammaire des instructions

instruction : affectation
	|	condition
	|	boucle
	|	retour
	|	appelFonction
	|	blocInstructions
	|	POINT_VIRGULE
	;
affectation : var EGAL expressionArithmetique POINT_VIRGULE ;
condition : SI expressionArithmetique ALORS blocInstructions
	|	SI expressionArithmetique ALORS blocInstructions SINON blocInstructions
	;
boucle : TANTQUE expressionArithmetique FAIRE blocInstructions ;
retour : RETOUR expressionArithmetique POINT_VIRGULE ;

appelFonction : fonction POINT_VIRGULE
	|	ECRIRE PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE POINT_VIRGULE
	;
blocInstructions : ACCOLADE_OUVRANTE listInstructions ACCOLADE_FERMANTE ;
listInstructions : instruction listInstructions
	|
	;


// Grammaire des declarations de variables

ligneDeclarationsVars : listDeclarationsVars POINT_VIRGULE ;
listDeclarationsVars : declarationVar VIRGULE listDeclarationsVars								// Modifier les noms
	|	declarationVar
	;
declarationVar : ENTIER IDENTIF
	|	ENTIER IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT
	;


// Grammaire des definitions de fonctions

definitionFct : IDENTIF PARENTHESE_OUVRANTE declarationsArgs PARENTHESE_FERMANTE blocDeclarationsVarsLocales blocInstructions ;


blocDeclarationsVarsLocales : ligneDeclarationsVars
	|
	;

declarationsArgs : listDeclarationsVars
	|
	;





%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
