%{
#include<stdlib.h>
#include<stdio.h>
#define YYDEBUG 1
#include"syntabs.h" // pour syntaxe abstraite
extern n_prog *n;   // pour syntaxe abstraite
extern FILE *yyin;    // declare dans compilo
extern int yylineno;  // declare dans analyseur lexical
int yylex();          // declare dans analyseur lexical
int yyerror(char *s); // declare ci-dessous
%}

%code requires
{
#include"syntabs.h" 
}

%union {
	int ival; 
	char* sval; 
	n_prog* prog;
	n_l_dec* l_dec;
	n_dec* dec;
	n_l_instr* l_instr; 
	n_instr* instr;
	n_l_exp* l_exp;
	n_exp* exp;
	n_var* var;
	n_appel* appel;
}
 		
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
%token <ival> NOMBRE			
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


%type <prog>  programme 	
%type <l_dec> listDeclarationsVar listDefinitionFct ensembleDeclarationVar ensembleDefinitionFct ligneDeclarationsVar suiteDeclarationsVar blocDeclarationVarLocales declarationsArgs 		
%type <dec>declarationVar definitionFct 		
%type <l_instr> ensembleInstructions 
%type <instr> instruction affectation condition boucle retour appelFonction blocInstructions 		
%type <l_exp> argument listArg 		
%type <exp> expressionArithmetique conjonction comparaison somme produit negation expressionPrioritaire		
%type <var> varAcces 		
%type <appel> fonction 		

%start programme

%%

// Axiome de la grammaire

programme : listDeclarationsVar listDefinitionFct							{$$ = cree_n_prog($1, $2);};
listDeclarationsVar : 	ensembleDeclarationVar 								{$$ = $1;}
		|																	{$$ = NULL;}
		;
ensembleDeclarationVar : ensembleDeclarationVar ligneDeclarationsVar		{$$ = cree_n_l_dec($1, $2);}	
		|	ligneDeclarationsVar											{$$ = cree_n_l_dec($1, NULL);}
		;

listDefinitionFct : ensembleDefinitionFct									{$$ = $1;}
		| 																	{$$ = NULL;}			
		;		
ensembleDefinitionFct : ensembleDefinitionFct definitionFct 				{$$ = cree_n_l_dec($1, $2);}
		|	definitionFct													{$$ = cree_n_l_dec($1, NULL);}
		; 


// grammaire des expressions arithmetiques
expressionArithmetique : expressionArithmetique OU conjonction 	{$$ = cree_n_exp_op(ou, $1, $3);}
		|	conjonction											{$$ = $1;}
		;
conjonction : conjonction ET comparaison	{$$ = cree_n_exp_op(et, $1, $3);}
		|	comparaison						{$$ = $1;}
		;
comparaison	: comparaison EGAL somme		{$$ = cree_n_exp_op(egal, $1, $3);}
		|	comparaison INFERIEUR somme		{$$ = cree_n_exp_op(inferieur, $1, $3);}
		| somme								{$$ = $1;}
		;
somme : somme PLUS produit			{$$ = cree_n_exp_op(plus, $1, $3);}
		|	somme MOINS produit		{$$ = cree_n_exp_op(moins, $1, $3);}
		|	produit					{$$ = $1;}
		;
produit : produit FOIS negation			{$$ = cree_n_exp_op(fois, $1, $3);}
		|	produit DIVISE negation		{$$ = cree_n_exp_op(divise, $1, $3);}
		|	negation					{$$ = $1;}
		;
negation : NON negation				{$$ = cree_n_exp_op(non, $2, NULL);}
		| expressionPrioritaire		{$$ = $1;}
		;
expressionPrioritaire : PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE		{$$ = $2;}
		|	varAcces																		{$$ = cree_n_exp_var($1);}
		|	NOMBRE																			{$$ = cree_n_exp_entier($1);}		//Attention Valeur (pas pointeur)
		| 	fonction																		{$$ = cree_n_exp_appel($1);}
		|  	LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE									{$$ = cree_n_exp_lire();}
		;
varAcces : IDENTIF																			{$$ = cree_n_var_simple($1);}	
		|	IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT					{$$ = cree_n_var_indicee($1, $3);}	
		;
fonction : IDENTIF PARENTHESE_OUVRANTE argument PARENTHESE_FERMANTE							{$$ = cree_n_appel($1, $3);};
argument : listArg		{$$ = $1;}			
		|				{$$ = NULL;}		
		;

listArg : expressionArithmetique						{$$ = cree_n_l_exp($1, NULL);}		
		|	listArg VIRGULE expressionArithmetique		{$$ = cree_n_l_exp($1, $3);}	
		;


// Grammaire des instructions

instruction : affectation																	{$$ = $1;}
		|	condition																		{$$ = $1;}
		|	boucle																			{$$ = $1;}
		|	retour																			{$$ = $1;}
		|	appelFonction																	{$$ = $1;}
		|	blocInstructions																{$$ = $1;}
		|	POINT_VIRGULE																	{$$ = cree_n_instr_vide();}
		;
affectation : varAcces EGAL expressionArithmetique POINT_VIRGULE 							{$$ = cree_n_instr_affect($1, $3);};
condition : SI expressionArithmetique ALORS blocInstructions								{$$ = cree_n_instr_si($2, $4, NULL);}
		|	SI expressionArithmetique ALORS blocInstructions SINON blocInstructions			{$$ = cree_n_instr_si($2, $4, $6);}
		;
boucle : TANTQUE expressionArithmetique FAIRE blocInstructions 								{$$ = cree_n_instr_tantque($2, $4);};
retour : RETOUR expressionArithmetique POINT_VIRGULE 										{$$ = cree_n_instr_retour($2);};

appelFonction : fonction POINT_VIRGULE																{$$ = cree_n_instr_appel($1);}		// Manque Lire...
		|	ECRIRE PARENTHESE_OUVRANTE expressionArithmetique PARENTHESE_FERMANTE POINT_VIRGULE		{$$ = cree_n_instr_ecrire($3);}
		;
blocInstructions : ACCOLADE_OUVRANTE ensembleInstructions ACCOLADE_FERMANTE 				{$$ = cree_n_instr_bloc($2);}
		|	ACCOLADE_OUVRANTE ACCOLADE_FERMANTE 										{$$ = NULL;}
		;
ensembleInstructions : ensembleInstructions	instruction 										{$$ = cree_n_l_instr($1, $2);};
		|	instruction																	{$$ = cree_n_l_instr($1, NULL);};
		;



// Grammaire des declarations de variables

ligneDeclarationsVar : suiteDeclarationsVar POINT_VIRGULE 									{$$ = $1;};
suiteDeclarationsVar: suiteDeclarationsVar VIRGULE declarationVar							{$$ = cree_n_l_dec($1, $3);}				
		|	declarationVar																	{$$ = cree_n_l_dec($1, NULL);}
		;
declarationVar : ENTIER IDENTIF																{$$ = cree_n_dec_var($2);}
		|	ENTIER IDENTIF CROCHET_OUVRANT expressionArithmetique CROCHET_FERMANT			{$$ = cree_n_dec_tab($2, $4);}
		;


// Grammaire des definitions de fonctions

definitionFct : IDENTIF PARENTHESE_OUVRANTE declarationsArgs PARENTHESE_FERMANTE blocDeclarationVarLocales blocInstructions 		{$$ = cree_n_dec_fonc($1, $3, $5, $6);};

blocDeclarationVarLocales : ligneDeclarationsVar				{$$ = $1;}
		|														{$$ = NULL;}
		;

declarationsArgs : suiteDeclarationsVar						{$$ = $1;}
		|														{$$ = NULL;}
		;



%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
