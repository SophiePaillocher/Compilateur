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

%union {double dval; int ival; char* sval;} 		// A verifier (dval util?)
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

%type <ival> expressionArithmetique conjonction comparaison somme produit negation expressionPrioritaire var fonction	// A verifier (ival?)
%type <sval> argument listArg

%left PLUS MOINS
%left FOIS DIVISE

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
//expAriValue : expressionArithmetique {printf(cree_n_exp_op(operation type, n_exp *op1, n_exp *op2);}
expressionArithmetique : expressionArithmetique OU conjonction 	{$$ = cree_n_exp_op(ou, $1, $3);}
		|	conjonction											{$$ = $1}
		;
conjonction : conjonction ET comparaison	{$$ = $1 && $3; cree_n_exp_op(et, $1, $3);}
		|	comparaison						{$$ = $1;}
		;
comparaison	: comparaison EGAL somme		{$$ = cree_n_exp_op(egal, $1, $3);}
		|	comparaison INFERIEUR somme		{$$ = cree_n_exp_op(inferieur, $1, $3);}
//		|	comparaison SUPERIEUR somme		{$$ = cree_n_exp_op(operation type, n_exp *op1, n_exp *op2);}
		| somme								{$$ = $1;}
		;
somme : somme PLUS produit		{$$ = cree_n_exp_op(plus, $1, $3);}
	|	somme MOINS produit		{$$ = cree_n_exp_op(moins, $1, $3);}
	|	produit					{$$ = $1;}
	;
produit : produit FOIS negation		{$$ = cree_n_exp_op(fois, $1, $3);}
	|	produit DIVISE negation		{$$ = cree_n_exp_op(divise, $1, $3);}
	|	negation					{$$ = $1;}
	;
negation : NON negation			{$$ = cree_n_exp_op(non, $2, NULL);}
	| expressionPrioritaire		{$$ = $1;}
	;
expressionPrioritaire : PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE		{$$ = $2}
		|	var																				{$$ = $1;}
		|	NOMBRE																			{$$ = $1;}
		| 	fonction																		{$$ = $1;}
		;
var : IDENTIF																				{$$ = cree_n_exp_var($1);}	
	| 	IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT						{$$ = cree_n_exp_var($1[$3]);}		// A verifier!		
	;
fonction : LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE					{$$ = cree_n_exp_lire();}			
	| 	IDENTIF PARENTHESE_OUVRANTE argument PARENTHESE_FERMANTE		{$$ = cree_n_exp_appel($1);}	//A verifier	
	;
argument : listArg		{$$ = $1;}			// A Verifier
		|				{$$ = NULL;}		// A verifier
		;

listArg : expressionArithmetique						{$$ = }		// A verifier
		|	expressionArithmetique VIRGULE listArg		{$$ = }	// A verifier
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
affectation : var EGAL expressionArithmetique POINT_VIRGULE 							{$$ = cree_n_instr_affect($1, $3);};
condition : SI expressionArithmetique ALORS blocInstructions							{$$ = cree_n_instr_si($2, $4, NULL);}
	|	SI expressionArithmetique ALORS blocInstructions SINON blocInstructions			{$$ = cree_n_instr_si($2, $4, $6);}
	;
boucle : TANTQUE expressionArithmetique FAIRE blocInstructions 							{$$ = cree_n_instr_tantque($2, $4);};
retour : RETOUR expressionArithmetique POINT_VIRGULE ;

appelFonction : fonction POINT_VIRGULE															{$$ = cree_n_instr_appel($1);}
	|	ECRIRE PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE POINT_VIRGULE		{$$ = cree_n_instr_ecrire($3);}
	;
blocInstructions : ACCOLADE_OUVRANTE listInstructions ACCOLADE_FERMANTE 				{$$ = cree_n_instr_bloc($2);};
listInstructions : instruction listInstructions											{$$ = 
	|
	;


// Grammaire des declarations de variables

ligneDeclarationsVars : listDeclarationsVars POINT_VIRGULE ;
listDeclarationsVars : declarationVar VIRGULE listDeclarationsVars								// Modifier les noms
	|	declarationVar
	;
declarationVar : ENTIER IDENTIF															{$$ = cree_n_dec_var($2);}
	|	ENTIER IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT			{$$ = cree_n_dec_tab($2, $4);}
	;


// Grammaire des definitions de fonctions

definitionFct : IDENTIF PARENTHESE_OUVRANTE declarationsArgs PARENTHESE_FERMANTE blocDeclarationsVarsLocales blocInstructions ;		{cree_n_dec_fonc($1, $3, $5, $6);
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
