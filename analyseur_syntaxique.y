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

%token OU 
%token ET  
%token NON  
%token EGAL  
%token INFERIEUR  
%token PLUS  
%token MOINS  
%token FOIS 
%token DIVISE 
%token PARENTHESE_OUVRANTE 
%token PARENTHESE_FERMANTE 
%token IDENTIF
%token NOMBRE
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


%start disjonction
%%

// grammaire des expressions arithmetiques 
disjonction : disjonction OU conjonction
		|	conjonction
		;
conjonction : conjonction ET comparaison
		|	comparaison
		;
comparaison	: comparaison EGAL somme
		|	comparaison INFERIEUR somme
		| somme
		;
somme : somme PLUS produit
	|	somme MOINS produit
	|	produit
	;
produit : produit FOIS negation
	|	 produit DIVISE negation
	|	negation
	;
negation : NON expression
	| expression
	;
expression : PARENTHESE_OUVRANTE disjonction PARENTHESE_FERMANTE
		|	var
		|	NOMBRE
		| 	fonction
		;
var : IDENTIF
	| 	IDENTIF CROCHET_OUVRANT NOMBRE CROCHET_FERMANT
	;
fonction : LIRE PARENTHESE_OUVANTE PARENTHESE_FERMANTE
	| 	IDENTIF PARENTHESE_OUVRANTE listarg PARENTHESE_FERMANTE
	;
listarg : disjonction
	|	disjonction VIRGULE listarg
	|
	;
//TODO: compléter avec les productions de la grammaire

%%

int yyerror(char *s) {
  fprintf(stderr, "erreur de syntaxe ligne %d\n", yylineno);
  fprintf(stderr, "%s\n", s);
  fclose(yyin);
  exit(1);
}
