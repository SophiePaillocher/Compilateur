#ifndef __CODE3A__
#define __CODE3A__

// BLOC DE CODE3A ALLOUÉ - Peut grandir dynamiquement selon les besoins
#define CODE_BLOCK_SIZE 40

// TYPES D'OPÉRANDES
#define O_CONSTANTE 1
#define O_ETIQUETTE 2
#define O_TEMPORAIRE 3
#define O_VARIABLE 4

typedef enum {arith_add, arith_sub, arith_mult, arith_div, //expressions arith.
              func_call, func_param, func_val_ret, func_begin, func_end, //fonctions
              assign, //affectation et transferts temporaires <-> variables
              alloc, // allouer de la place pour variables et temporaires
              //instructions de contrôle (si, tantque) et logiques/comparaison:
              jump, jump_if_less, jump_if_less_or_equal, jump_if_equal, 
              jump_if_not_equal, jump_if_greater, jump_if_greater_or_equal,
              sys_read, sys_write // lire et écrire (interruptions système)
             } instrcode; // codes d'instruction 3 adresses valides

typedef struct operande_ operande;

struct operande_ {
  int oper_type;
  union {
    int oper_valeur; // constante (entier simple) p.ex. 7, 18, 365 ...
    char *oper_nom; // nom d'une étiquette p.ex. "fmain", "e0" ...
    struct {
        int oper_tempnum; // numéro d'un temporaire
        int  last_use;    // ligne où ce temp. est utilisé la dernière fois
    } oper_temp; // temporaire utilisé dans éval d'expressions
    struct {
        char *oper_nom;
        int oper_portee; // prend 1 valeur parmi les 3 constantes définies dans
        // tabsymboles.h: P_VARIABLE_GLOBALE, P_VARIABLE_LOCALE ou P_ARGUMENT
        int oper_adresse;        
        struct operande_ *oper_indice;
    } oper_var; // variable locale, globale ou argument de fonction
  } u;
} ;

// Enregistrement décrivant une ligne de code 3a
typedef struct {
  char *op_etiq;       // étiquette optionnelle, si la ligne est cible d'un saut
  instrcode op_code;   // code de l'instruction (parmi les instrcode possibles)
  operande *op_oper1;  // opérande 1 (ou NULL)
  operande *op_oper2;  // opérande 2 (ou NULL)
  operande *op_result; // résultat ou destination du saut (ou NULL)
  char *comment;       // commentaire explicatif - utile pour le débogage
} operation_3a;

// Type 'code 3 adresses' avec tableau et pointeur pour la prochaine ligne dispo
typedef struct {
  operation_3a *liste;
  int next;   
} code3a_;

/*
 * Initialiser le tableau du code trois adresses. Cette fonction doit
 * être appelée au début de la génération de code, exatement une fois
 */
void code3a_init();
/* 
 * Ajouter une étiquette à la prochaine ligne de code trois adresses
 * qui sera ajoutée au tableau (et non pas à la dernière)
 * @param nom L'étiquette de la prochaine ligne.
 */
void code3a_ajoute_etiquette(char *nom);
/*
 * Ajoute une instruction au tableau du code trois adresses. Si le 
 * tableau devient trop petit, il est automatiquement redimmensionné.
 * Uniquemnet le op_code est obligatoire, les autres paramètres sont
 * optionnels. Passer "NULL" s'ils ne sont pas pertinents.
 * @param op_code le code de l'opération, de l'enum instrcode
 * @param op_oper1 Première opérande (ou 1ère adresse)
 * @param op_oper2 Deuxième opérande (ou 2ème adresse)
 * @param op_result Opérande résultat (ou 3ème adresse)
 * @param comment Un commentaire associé à la ligne.
 */
void code3a_ajoute_instruction(instrcode op_code, operande *op_oper1, 
                               operande *op_oper2, operande *op_result,
                               char *comment);
/* 
 * Crée une nouvelle valeur temporaire et la renvoie sous forme d'opérande.
 * @return Une nouvelle valeur temporaire à utiliser comme adresse
 */
operande *code3a_new_temporaire();
/* 
 * Crée une opérande de type constante (littéral) qui sert de "wrapper" à 
 * une valeur entière.
 * @param valeur Une valeur entière à envelopper dans cette opérande
 * @return Une opérande constante à partir de la valeur donnée
 */
operande *code3a_new_constante(int valeur);
/* 
 * Crée une opérande de type étiquette qui sert de "wrapper" à 
 * une étiquette donnée.
 * @param nom Une étiquette à envelopper dans cette opérande
 * @return Une opérande étiquette à partir de l'étiquette donnée
 */
operande *code3a_new_etiquette(char *nom);
/* 
 * Crée une opérande de type variable qui sert de "wrapper" à 
 * une variable donnée.
 * @param nom Le nom de la variable à envelopper dans cette opérande
 * @param portee Une valeur parmi P_ARGUMENT, P_VARIABLE_LOCALE ou
 *               P_VARIABLE_GLOBALE (définies dans tabsymboles.h)
 * @param adresse Adresse de la variable, calculée dans la table des
 *                symboles et copiée dans le code trois adresses
 * @return Une opérande variable à partir de la variable donnée
 */
operande *code3a_new_var(char *nom, int portee, int adresse);                               
/*
 * Cette fonction affiche le code à trois adresses créé dans le tableau
 * code3a. Selon la valeur de code3a_verbose, affiche plus ou moins de 
 * détails.
 */ 
void code3a_affiche_code();
/* Fonction qui affiche une ligne de code trois adresses. Utilisée
 * uniquement pour le debug 
 * param i_oper La ligne à afficher 
 */
void code3a_affiche_ligne_code(operation_3a *i_oper);

// Déclaré dans code3a.c
extern code3a_ code3a;

#endif
