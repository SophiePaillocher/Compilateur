#include "code3a.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "tabsymboles.h"

#define MAX_TEMP 20

// tableau global avec code à 3 adresses
code3a_ code3a;
// taille du tableau, peut augmenter si besoin avec realloc
int code3asize = CODE_BLOCK_SIZE;
// compteur des temporaires, pour générer des noms uniques
int global_temp_counter = 0;
// descripteur des temporaires, vecteur de pointeurs vers opérande temp.
operande **desctemp; 
// Comme pour code3asize, taille du descripteur desctemp, peut augmenter
int desctempsize = MAX_TEMP;
// étiquette de la prochaine ligne, on retarde l'ajout de l'étiquette
char *_next_etiq = NULL;
int code3a_verbose = 0;

/******************************************************************************/

void code3a_init() {
  code3a.liste = malloc(code3asize*sizeof(operation_3a));  
  code3a.next = 0;
  desctemp = malloc(desctempsize * sizeof(operande *));
}

/******************************************************************************/

void code3a_ajoute_etiquette(char *etiquette){
  if(_next_etiq){   
    warning_1s("Étiquette précédente ignorée: %s",_next_etiq);
  }
  // étiquette pas ajoutée à la ligne courante, mais à la prochaine
  _next_etiq = etiquette;
}

/******************************************************************************/

void update_lastuse(operande *op_oper1, int is_lvalue) {
  if(op_oper1) {
    // temporaire en rvalue (utiliser sa valeur pour calculer une autre)
    if(op_oper1->oper_type == O_TEMPORAIRE && !is_lvalue) { 
      int tempnum = op_oper1->u.oper_temp.oper_tempnum;
      desctemp[tempnum]->u.oper_temp.last_use = code3a.next;
    }
    // les indices de tableaux sont des rvalue (utilisation du temporaire)
    else if(op_oper1->oper_type == O_VARIABLE){
      if(op_oper1->u.oper_var.oper_indice && op_oper1->u.oper_var.oper_indice->oper_type == O_TEMPORAIRE) {
        int tempnum = op_oper1->u.oper_var.oper_indice->u.oper_temp.oper_tempnum;
        desctemp[tempnum]->u.oper_temp.last_use = code3a.next;    
      }      
    }
  }  
}

/******************************************************************************/

void code3a_ajoute_instruction(instrcode op_code, operande *op_oper1, 
                               operande *op_oper2, operande *op_result, 
                               char *comment){  
  if(code3a.next >= code3asize){
    code3asize += CODE_BLOCK_SIZE;
    code3a.liste = realloc(code3a.liste,code3asize*sizeof(operation_3a));
  }
  code3a.liste[code3a.next].op_code = op_code;
  code3a.liste[code3a.next].op_oper1 = op_oper1;
  code3a.liste[code3a.next].op_oper2 = op_oper2;
  code3a.liste[code3a.next].op_result = op_result;  
  code3a.liste[code3a.next].comment = comment;  
  if(_next_etiq) { // cette opération est étiquetée
    code3a.liste[code3a.next].op_etiq = _next_etiq;
    _next_etiq = NULL;
  }
  else{
    code3a.liste[code3a.next].op_etiq = _next_etiq;
  }
  // mise à jour de last_use pour les temporaires
  update_lastuse(op_oper1, 0);
  update_lastuse(op_oper2, 0);
  update_lastuse(op_result, 1);
  code3a.next++;  
}

/******************************************************************************/

operande *code3a_new_temporaire(){
  operande *newtemp = malloc(sizeof(operande));
  newtemp->oper_type = O_TEMPORAIRE;
  newtemp->u.oper_temp.oper_tempnum = global_temp_counter;
  newtemp->u.oper_temp.last_use = -1;
  if(global_temp_counter >= desctempsize ) { // temp_desc too short, must realloc
    desctempsize += MAX_TEMP;
    desctemp = realloc(desctemp, desctempsize * sizeof(operande *));
  }
  desctemp[global_temp_counter] = newtemp;
  global_temp_counter++;
  return newtemp;
}

/******************************************************************************/

operande *code3a_new_constante(int valeur){
  operande *newlit = malloc(sizeof(operande));
  newlit->oper_type = O_CONSTANTE;
  newlit->u.oper_valeur = valeur;
  return newlit;
}

/******************************************************************************/

operande *code3a_new_etiquette(char *nom){
  operande *newetiq = malloc(sizeof(operande));
  newetiq->oper_type = O_ETIQUETTE;
  newetiq->u.oper_nom = malloc(sizeof(char)*102);
  strcpy(newetiq->u.oper_nom,nom);
  return newetiq;
}

/******************************************************************************/

operande *code3a_new_var(char *nom, int portee, int adresse){    
  operande *newvar = malloc(sizeof(operande));
  newvar->oper_type = O_VARIABLE;
  newvar->u.oper_var.oper_nom = malloc(sizeof(char)*102);
  sprintf(newvar->u.oper_var.oper_nom, "v%s",nom); // préfixer le nom d'un "v"  
  newvar->u.oper_var.oper_portee = portee;
  newvar->u.oper_var.oper_adresse = adresse;
  newvar->u.oper_var.oper_indice = NULL;  
  return newvar;
}

/******************************************************************************/

void _code3a_affiche_operande(operande *oper, int affiche_last_use){
  if(oper->oper_type == O_ETIQUETTE){
    printf("%s",oper->u.oper_nom);
  }
  else if(oper->oper_type == O_TEMPORAIRE){
    printf("t%d",oper->u.oper_temp.oper_tempnum); 
    if(affiche_last_use) {
      printf("{l=%d}",oper->u.oper_temp.last_use);
    }
  }
  else if(oper->oper_type == O_CONSTANTE){
    printf("%d",oper->u.oper_valeur);
  }
  else if(oper->oper_type == O_VARIABLE){    
    printf("%s",oper->u.oper_var.oper_nom);
    if(oper->u.oper_var.oper_indice != NULL){
      printf("[");
      _code3a_affiche_operande(oper->u.oper_var.oper_indice, 0);
      printf("]");
    }    
    char oper_portee;
    switch(oper->u.oper_var.oper_portee){
      case P_VARIABLE_GLOBALE : oper_portee='G'; break;
      case P_VARIABLE_LOCALE : oper_portee='L'; break;
      case P_ARGUMENT : oper_portee='A'; break;
      default: erreur("Portée de variable invalide dans code 3 adresses");
    }
    if(code3a_verbose){
      printf("{p=%c,a=%d}",oper_portee, oper->u.oper_var.oper_adresse);
    }    
  }
  else{
    erreur("Type d'opérande invalide dans code 3 adresses");
  }    
}

/******************************************************************************/

int _is_controle(instrcode opcode){
  switch(opcode){
    case jump_if_equal:
    case jump_if_greater:
    case jump_if_greater_or_equal:
    case jump_if_less: 
    case jump_if_less_or_equal:
    case jump_if_not_equal : 
      return 1;
    default: return 0;
  }  
}

/******************************************************************************/

void code3a_affiche_ligne_code(operation_3a *i_oper){
  if(i_oper->op_etiq != NULL){
      printf(" >%8s", i_oper->op_etiq);
  }
  else{
    printf("%10s", "");
  }
  printf(" : ");
  if(_is_controle(i_oper->op_code)){
    printf("if ");
    _code3a_affiche_operande(i_oper->op_oper1, 0); 
    switch(i_oper->op_code){
      case jump_if_equal            : printf(" == "); break;
      case jump_if_not_equal        : printf(" != "); break;
      case jump_if_less             : printf(" < ");  break;
      case jump_if_less_or_equal    : printf(" <= "); break;
      case jump_if_greater          : printf(" > ");  break;
      case jump_if_greater_or_equal : printf(" >= "); break;
      default : erreur("Instruction à 3 adresses non reconnue");
    }
    _code3a_affiche_operande(i_oper->op_oper2, 0); 
    printf(" goto ");
    _code3a_affiche_operande(i_oper->op_result, 0); // destination
  }
  else {    
    if(i_oper->op_result){
      _code3a_affiche_operande(i_oper->op_result, code3a_verbose);
      printf(" = ");
    }
    switch(i_oper->op_code) {          
      case func_call   : printf("call ");   break;
      case func_param  : printf("param ");  break;
      case func_val_ret: printf("ret ");    break;
      case func_begin  : printf("fbegin"); break;
      case func_end    : printf("fend");   break;        
      case sys_write   : printf("write ");  break;
      case sys_read    : printf("read ");   break;
      case jump        : printf("goto ");   break;
      case alloc       : printf("alloc ");  break;      
      default : ;//nothing
    }      
    if(i_oper->op_oper1){          
      _code3a_affiche_operande(i_oper->op_oper1, 0);            
    }
    switch(i_oper->op_code) {
      case arith_add   : printf(" +");    break;
      case arith_sub   : printf(" -");    break;
      case arith_mult  : printf(" *");    break;
      case arith_div   : printf(" /");    break;
      default : ;// nothing
    }
    if(i_oper->op_oper2){
      printf(" ");    
      _code3a_affiche_operande(i_oper->op_oper2, 0);         
    }
  }    
  if(i_oper->comment){
    printf("\t\t; %s",i_oper->comment);
  }
}

/******************************************************************************/

void code3a_affiche_code(){
  int i_ligne;  
  for(i_ligne=0;i_ligne<code3a.next;i_ligne++){
    printf("%04d",i_ligne); //TODO really print the code    
    code3a_affiche_ligne_code(&code3a.liste[i_ligne]);
    printf("\n");
  }
}

