#ifndef __SYNTABS__
#define __SYNTABS__

/* Abbréviations: 
  * n -> noeud
  * l -> liste
  * dec -> déclaration
  * exp -> expression
  * instr -> instruction
  * prog -> programme
*/

/* Noeud du type liste d'instructions */
typedef struct n_l_instr_ n_l_instr;
/* Noeud du type instruction */
typedef struct n_instr_ n_instr;
/* Noeud du type expression */
typedef struct n_exp_ n_exp;
/* Noeud du type liste d'expressions */
typedef struct n_l_exp_ n_l_exp;
/* Noeud du type variable (utilisée dans une expression) */
typedef struct n_var_ n_var;
/* Noeud du type liste de déclarations */
typedef struct n_l_dec_ n_l_dec;
/* Noeud du type déclaration */
typedef struct n_dec_ n_dec;
/* Noeud du type programme (racine de l'arbre abstrait) */
typedef struct n_prog_ n_prog;
/* Noeud du type appel à fonction - expression ou instruction */
typedef struct n_appel_ n_appel;

/*-------------------------------------------------------------------------*/
/* Programme - noeud utilisé pour créer la racine de l'arbre abstrait à l'axiome */
struct n_prog_ {
  n_l_dec *variables; /* variables globales */
  n_l_dec *fonctions; /* fonctions du programme */
};

n_prog *cree_n_prog(n_l_dec *variables, n_l_dec *fonctions);
/*-------------------------------------------------------------------------*/

/* Déclaration de fonction, variable simple ou variable tableau */
struct n_dec_ {
  enum {foncDec, varDec, tabDec} type; 
  char *nom;
  union {
    struct{n_l_dec *param; n_l_dec *variables; n_instr *corps;} foncDec_;
    struct{int type;} varDec_;
    struct{int taille;} tabDec_;
  } u;
};

/* Crée un noeud du type déclaration de variable simple */
n_dec *cree_n_dec_var(char *nom);
/* Crée un noeud du type déclaration de variable tableau, avec la taille donnée */
n_dec *cree_n_dec_tab(char *nom, int taille);
/* Crée un noeud du type déclaration de fonction avec ses paramètres, variables locales et corps de fonction */
n_dec *cree_n_dec_fonc(char *nom, n_l_dec *param, n_l_dec *variables, n_instr *corps);

/*-------------------------------------------------------------------------*/

/* ATTENTION : non => négation logique
   Le opérateurs unaires ont op2 = NULL par convention */
typedef enum {plus, moins, fois, divise, egal, inferieur, ou, et, non} operation; 

/* Expression arithmétique ou logique (opExp) ou atomique (varExp, intExp, 
   appelExp, lireExp) */
struct n_exp_ {
  enum{varExp, opExp, intExp, appelExp, lireExp} type;
  union{
    struct{operation op; struct n_exp_ *op1; struct n_exp_ *op2;} opExp_;
    n_var *var;
    int entier;
    n_appel *appel;
  }u;
};

/* Crée un noeud de type expression arithmétique ou logique binaire - op2=NULL 
   pour opération unaire - négation */
n_exp *cree_n_exp_op(operation type, n_exp *op1, n_exp *op2);
/* Crée un noeud de type expression atomique - un nombre entier */
n_exp *cree_n_exp_entier(int entier);
/* Crée un noeud de type expression atomique - un accès à une variable (simple 
ou indicée) */
n_exp *cree_n_exp_var(n_var *var);
/* Crée un noeud de type expression atomique - un appel de fonction */
n_exp *cree_n_exp_appel(n_appel *app);
/* Crée un noeud de type expression atomique - un appel à la fonction spéciale 
lire */
n_exp *cree_n_exp_lire(void);

/*-------------------------------------------------------------------------*/

/* Instruction, peut être de plusieurs types et avoir plusieurs fils, selon le 
   type */
struct n_instr_ {
  enum {affecteInst, siInst, tantqueInst, appelInst, retourInst, ecrireInst, 
        videInst, blocInst } type;
  union{
    struct{n_var *var; n_exp *exp;} affecte_;    
    struct{n_exp *test; struct n_instr_ *alors; struct n_instr_ *sinon;} si_;
    struct{n_exp *test; struct n_instr_ *faire;} tantque_;
    n_appel *appel;
    struct{n_exp *expression;} retour_;
    struct{n_exp *expression;} ecrire_;
    n_l_instr *liste;
  }u;
};

/* Crée un noeud de type instruction "si". Si il n'y a pas de "sinon", passer 
   sinon=NULL */
n_instr *cree_n_instr_si(n_exp *test, n_instr *alors, n_instr *sinon);
/* Crée un noeud de type instruction contenant un bloc d'instructions  */
n_instr *cree_n_instr_bloc(n_l_instr *liste);
/* Crée un noeud de type instruction "tantque" */
n_instr *cree_n_instr_tantque(n_exp *test, n_instr *faire);
/* Crée un noeud de type instruction d'affectation */
n_instr *cree_n_instr_affect(n_var *var, n_exp *exp);
/* Crée un noeud de type instruction d'appel de fonction  */
n_instr *cree_n_instr_appel(n_appel *appel);
/* Crée un noeud de type instruction retour de fonction  */
n_instr *cree_n_instr_retour(n_exp *expression);
/* Crée un noeud de type instruction spéciale "ecrire" */
n_instr *cree_n_instr_ecrire(n_exp *expression);
/* Crée un noeud de type instruction vide */
n_instr *cree_n_instr_vide(void);

/*-------------------------------------------------------------------------*/
struct n_appel_{
  char *fonction; 
  n_l_exp *args; /* Liste d'expressions passées en parmètre */
};

/* Crée un noeud de type appel de fonction - peu importe si instruction ou 
   expression */
n_appel *cree_n_appel(char *fonction, n_l_exp *args);

/*-------------------------------------------------------------------------*/
/* Accès à une variable dans une partie gauche d'affectation ou expression */
struct n_var_ {
  enum {simple, indicee} type;
  char *nom;
  union {
    struct{n_exp *indice;} indicee_;
  }u;
};

/* Crée un noeud de type accès à une variable simple (dans une expression ou à 
   gauche d'une affectation) */
n_var *cree_n_var_simple(char *nom);
/* Crée un noeud de type accès à une variable indicée - accès à une case de 
   tableau (dans une expression ou à gauche d'une affectation) */
n_var *cree_n_var_indicee(char *nom, n_exp *indice);

/*-------------------------------------------------------------------------*/
/* Liste chaînée d'expressions */
struct n_l_exp_ {
  n_exp *tete;
  struct n_l_exp_ *queue;
};

/* Crée un noeud de type liste d'expressions (dans un appel de fonction). Pour
   marquer la fin de la liste, passer queue=NULL */
n_l_exp *cree_n_l_exp(n_exp *tete, n_l_exp *queue);

/*-------------------------------------------------------------------------*/
/* Liste chaînée d'instructions */
struct n_l_instr_ {
  n_instr *tete;
  struct n_l_instr_ *queue;
};

/* Crée un noeud de type liste d'instructions. Pour marquer la fin de la liste, 
   passer queue=NULL */
n_l_instr *cree_n_l_instr(n_instr *tete, n_l_instr *queue);

/*-------------------------------------------------------------------------*/
/* Liste chaînée de déclarations de fonctions ou de variables */
struct n_l_dec_{
  n_dec *tete;
  struct n_l_dec_ *queue;
};

/* Crée un noeud de type liste de déclarations. Pour marquer la fin de la liste, 
   passer queue=NULL */
n_l_dec *cree_n_l_dec(n_dec *tete, n_l_dec *queue);
/*-------------------------------------------------------------------------*/

#endif
