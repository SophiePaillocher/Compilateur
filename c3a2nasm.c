#include "code3a.h"
#include "tabsymboles.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

#define printverb(format) if(nasm_verbose){ printf(format); }
#define printverba(format, ...) if(nasm_verbose){ printf(format, __VA_ARGS__); }

#define NB_REGISTRES 4

#define REG_NONE 0
#define REG_EAX 1
#define REG_EBX 2
#define REG_ECX 3 // last registers used -> needed for operations (e.g. idiv)
#define REG_EDX 4 // last registers used -> needed for operations (e.g. idiv)

void _nasm_instr(char *opcode, char *op1, char *op2, char *op3, char *comment);
void _nasm_instr_relative(char *opcode,  char *op1, char *op2, int offset, char *comment);
void _nasm_etiquette(char* etiquette);

// tableau global avec code à 3 adresses
extern code3a_ code3a;
//extern operande **desctemp;

int varlocs = 0;
int arguments = 0;
int i_ligne; // ligne courante du code
operande *desc_reg[NB_REGISTRES+1] = {NULL,NULL,NULL,NULL,NULL}; //1st unused
char *nomreg[NB_REGISTRES+1] = {NULL,"eax","ebx","ecx","edx"}; //1st unused
int nasm_verbose = 0;

/******************************************************************************/

void _nasm_comment(char *comment) {
  if(comment != NULL) {
    printverba("\t\t ; %s", comment);
  }
  printf("\n");
}

/******************************************************************************/

int _is_reg_free(int regnum){
  return  desc_reg[regnum] == NULL ||
          desc_reg[regnum]->u.oper_temp.last_use < i_ligne; // temp obsolète
}

/******************************************************************************/

void _affiche_desc_registres(operande *oper){
  int regnum;
  printverb(" [");
  for(regnum = 1; regnum <= 4; regnum++){
    if(desc_reg[regnum]==NULL){
      printverb("_");
    }
    else{
      printverba("t%d{",(desc_reg[regnum])->u.oper_temp.oper_tempnum);
      if(_is_reg_free(regnum)){ // temp obsolète
        printverb("dead}");
      }
      else{
        printverba("l=%d}",(desc_reg[regnum])->u.oper_temp.last_use);
      }
    }
    if(regnum != 4){
      printverb(", ");
    }
  }
  printverb("]");
}

/******************************************************************************/

// ALLOCATION DE REGISTRES SIMPLE. oper est forcément un temporaire
int new_registre(operande *oper){
  int regnum;
  //printverb(";            Before alloc reg:\n");
  printverba("; Allouer t%d", oper->u.oper_temp.oper_tempnum);
  _affiche_desc_registres(oper);
  printverb(" =>")
  for(regnum = 1; regnum <= 4; regnum++){
    if( _is_reg_free(regnum) ) {
      desc_reg[regnum] = oper;
      //printverb(";            After alloc reg:\n");
      _affiche_desc_registres(oper);
      printverb("\n");
      return regnum;
    }
  }
  // TODO FIXME!
  erreur("Pas de registre disponible");
  return REG_NONE;
}

/******************************************************************************/

/* Cas "faciles" :
   * constante renvoyée sous forme de chaîne de caractères ou
   * variable renvoyée sous forme de décalage par rapport à étiquette / registre
*/
char *varconst2nasm(operande *oper){
  if(oper->oper_type != O_VARIABLE && oper->oper_type != O_CONSTANTE){
    erreur("Opérande n'est pas une variable ou constante");
  }
  char *result = malloc(sizeof(char)*150);
  if(oper->oper_type == O_CONSTANTE){
    sprintf(result,"%d",oper->u.oper_valeur);
  }
  else if(oper->oper_type == O_VARIABLE){
    if(oper->u.oper_var.oper_portee == P_VARIABLE_GLOBALE) {
      if(oper->u.oper_var.oper_indice){
        operande *indice = oper->u.oper_var.oper_indice;
        char *indchar = malloc(sizeof(char) * 20);
        if(indice->oper_type == O_CONSTANTE){
          sprintf(indchar,"%d",indice->u.oper_valeur * 4);
        }
        else{ // O_TEMPORAIRE
          indchar = nomreg[indice->u.oper_temp.emplacement];
          _nasm_instr("add",indchar,indchar,NULL, "indice tableau fois deux");
          _nasm_instr("add",indchar,indchar,NULL, "indice tableau fois quatre");
        }
        sprintf(result,"dword [%s+%s]", oper->u.oper_var.oper_nom, indchar);
      }
      else {
        sprintf(result,"dword [%s]", oper->u.oper_var.oper_nom);
      }
    }
    else{
      int adresse = oper->u.oper_var.oper_adresse;
      if (oper->u.oper_var.oper_portee == P_VARIABLE_LOCALE){
        sprintf(result,"dword [ebp - %d]", 4 + adresse );
      }
      else{ // P_ARGUMENT
        sprintf(result,"dword [ebp + %d]", 4 + 4 * (arguments) - adresse);
      }
    }
  }
  return result;
}


/******************************************************************************/

int oper2reg(operande *oper, int regnum){ // regnum can be REG_NONE
  int result;
  if(regnum != REG_NONE && !_is_reg_free(regnum)){
    //warning_1s("; register %s not free!", nomreg[regnum]);
    int anotherreg = new_registre(desc_reg[regnum]);
    int oldreg = desc_reg[regnum]->u.oper_temp.emplacement;
    _nasm_instr("mov",nomreg[anotherreg],nomreg[oldreg],NULL,"free reg");
    desc_reg[anotherreg] = desc_reg[oldreg];
    desc_reg[oldreg] = NULL;
  }
  if(oper->oper_type == O_CONSTANTE || oper->oper_type == O_VARIABLE){
    operande *constvartemp = code3a_new_temporaire();
    constvartemp->u.oper_temp.last_use = i_ligne;
    if(regnum == REG_NONE){ result = new_registre(constvartemp); }
    else{ result = regnum; }
    _nasm_instr("mov", nomreg[result],
                      varconst2nasm(oper), NULL, NULL);
    return result;
  }
  else{ // oper is a temporary
    int tempreg = oper->u.oper_temp.emplacement;
    if(tempreg == REG_NONE ){
      erreur("Temporaire non initialisé !");
      return REG_NONE; // remove warning
    }
    // FIXME beaks if register constant values change
    else if(tempreg > REG_NONE){
      if(regnum != REG_NONE && tempreg != regnum){
        _nasm_instr("mov", nomreg[regnum], nomreg[tempreg], NULL, "new reg for t");
        desc_reg[regnum]=desc_reg[tempreg];
        desc_reg[tempreg]=NULL;
        oper->u.oper_temp.emplacement = regnum;
      }
      return tempreg;
    }
    else{
      erreur("TODO: récupérer temp de la pile");
      return REG_NONE; // remove warning
    }
  }
}

/******************************************************************************/

void c3a2nasm_division(operande *oper1, operande *oper2, operande *result){
  int restoreeax = 0;
  int restoreedx = 0;
  char *oper2string;
  // Problème : si eax ou edx sont nécessaires dans idiv, e.g. dans des
  // indices de tableaux
  if(!_is_reg_free(REG_EAX)){
    restoreeax = 1;
    _nasm_instr("push", "eax", NULL,NULL,"push eax avant division");
  }
  if(!_is_reg_free(REG_EDX)){
    restoreedx = 1;
    _nasm_instr("push", "edx", NULL, NULL,"push eax avant division");
  }
  oper2reg(oper1, REG_EAX);
  _nasm_instr("mov", "edx", "0", NULL, "oper1 idiv -> eax");
  if(oper2->oper_type == O_CONSTANTE){
    oper2string = nomreg[oper2reg(oper2, REG_NONE)];
  }
  else if (oper2->oper_type == O_TEMPORAIRE) {
    oper2string = nomreg[oper2->u.oper_temp.emplacement];
  }
  else { // O_VARIABLE
    oper2string = varconst2nasm(oper2);
  }
  _nasm_instr("idiv", oper2string, NULL, NULL, "effectue division");
  result->u.oper_temp.emplacement = REG_EAX;
  if(restoreedx){
    _nasm_instr("pop", "edx", NULL, NULL, "restore edx");
  }
  if(restoreeax){ // eax wasn't free
    result->u.oper_temp.emplacement = new_registre(result); // will not be REG_EAX
    _nasm_instr("mov", nomreg[result->u.oper_temp.emplacement], "eax", NULL, NULL);
    _nasm_instr("pop", "eax", NULL, NULL, "rétablir eax") ;
  }
  else{
    result->u.oper_temp.emplacement = REG_EAX;
    desc_reg[REG_EAX] = result;
  }
}

/******************************************************************************/

void c3a2nasm_arith(char *opcode, operande *oper1, operande *oper2, operande *result){
  // result is always temp in arith operation (by construction of c3a)
  //TODO optimisation - echanger oper1 et oper2 pour add et imul
  //TODO optimisation - calculer const op const directement
  int oper1reg = oper2reg(oper1, REG_NONE);
  char *oper2string;
  if(oper2->oper_type == O_TEMPORAIRE){
    oper2string = nomreg[oper2reg(oper2, REG_NONE)]; //temp in reg
  }
  else{
    oper2string = varconst2nasm(oper2);
  }
  _nasm_instr(opcode, nomreg[oper1reg], oper2string, NULL, NULL);
  desc_reg[oper1reg] = result;
  result->u.oper_temp.emplacement = oper1reg;
}

/******************************************************************************/

void c3a2nasm_affect(operande *result, operande *oper1){
  char *oper1char, *resultchar;
  int oper1reg;
  if(result->oper_type == O_VARIABLE && oper1->oper_type == O_VARIABLE){
    char *oper1chartemp = varconst2nasm(oper1);
    operande *tempvar = code3a_new_temporaire();
    tempvar->u.oper_temp.last_use = i_ligne;
    oper1char = nomreg[new_registre(tempvar)];
    _nasm_instr("mov", oper1char, oper1chartemp, NULL,
                "affect: lval and rval are var => rval var to temp");
  }
  else if (oper1->oper_type == O_CONSTANTE){
    oper1char = varconst2nasm(oper1);
  }
  else{ //oper1->oper_type == O_VARIABLE or oper1->oper_type == O_TEMPORAIRE
    oper1reg = oper2reg(oper1, REG_NONE);
    oper1char = nomreg[oper1reg];
  }
  if(result->oper_type == O_VARIABLE){
    resultchar = varconst2nasm(result);
    _nasm_instr("mov", resultchar, oper1char, NULL, "affect: lval var");
  }
  else{ // result->oper_type == O_TEMPORAIRE
    if(oper1->oper_type == O_CONSTANTE){
      if(result->u.oper_temp.emplacement == REG_NONE){
        result->u.oper_temp.emplacement = new_registre(result);
      }
      resultchar = nomreg[result->u.oper_temp.emplacement];
      _nasm_instr("mov", resultchar, oper1char, NULL, "affect: lval temp");
    }
    else{ // oper1->oper_type == O_VARIABLE
      result->u.oper_temp.emplacement = oper1reg;
      desc_reg[oper1reg] = result;
    }
  }
}

/******************************************************************************/

void c3a2nasm_finfonction(int varlocs){
  if(varlocs){
    printf("\tadd\tesp, %d", varlocs);
    _nasm_comment("desallocation variables locales");
  }
  _nasm_instr("pop", "ebp", NULL, NULL, "restaure la valeur de ebp") ;
  _nasm_instr("ret", NULL, NULL, NULL, NULL);
}

/******************************************************************************/

void sauvegarde_registres_avant_appel(){
  int regnum;
  for(regnum=1; regnum<=4; regnum++){
    if(!_is_reg_free(regnum)){
      _nasm_instr("push", nomreg[regnum], NULL, NULL, "sauvegarde avant appel") ;
    }
  }
}

/******************************************************************************/

void recupere_registres_apres_appel(int excepte){
  int regnum; // excepte vient d'etre alloué, il n'était pas là avant
  for(regnum=4; regnum>=1; regnum--){ // effectue lecture à l'envers
    if(!_is_reg_free(regnum) && regnum != excepte){
      _nasm_instr("pop", nomreg[regnum], NULL, NULL, "recupere apres appel") ;
    }
  }
}

/******************************************************************************/

void c3a2nasm_allouer(operande *var){
  if(!var) { // valeur de retour d'une fonction
    sauvegarde_registres_avant_appel();
    _nasm_instr("sub", "esp", "4", NULL, "allocation valeur de retour");
  }
  else{ // variable locale, 4 octets (dword)
    printf("\tsub\tesp, 4\t; allocation variable locale %s\n", var->u.oper_var.oper_nom);
    varlocs += 4;
  }
}

/******************************************************************************/

void c3a2nasm_debutfonction(char *nomfonction){
  varlocs = 0;
  arguments = tabsymboles.tab[rechercheExecutable(nomfonction)].complement;
  _nasm_instr("push", "ebp", NULL, NULL, "sauvegarde la valeur de ebp") ;
  _nasm_instr("mov", "ebp", "esp", NULL, "nouvelle valeur de ebp");
}

/******************************************************************************/

void c3a2nasm_write(operande *oper){
  //pas besoin de sauvegarder eax. "ecrire" est une instruction, donc on est sûr
  //qu'on n'est pas en plein milieu d'une évaluation d'expression
  if(oper->oper_type == O_TEMPORAIRE && oper->u.oper_temp.emplacement != REG_EAX){
    oper2reg(oper, REG_EAX);
  }
  else if (oper->oper_type != O_TEMPORAIRE){
    _nasm_instr("mov", "eax", varconst2nasm(oper), NULL, NULL) ;
  }
  _nasm_instr("call", "iprintLF", NULL, NULL, NULL);
}

/******************************************************************************/

void c3a2nasm_read(operande *result){
  int restoreeax = 0;
  if(!_is_reg_free(REG_EAX)){
    restoreeax = 1;
    _nasm_instr("push", "eax", NULL, NULL, "sauvegarder eax - TODO si besoin") ;
  }
  _nasm_instr("mov", "eax", "sinput", NULL, NULL);
  _nasm_instr("call", "readline", NULL, NULL, NULL);
  _nasm_instr("mov", "eax", "sinput", NULL, NULL);
  _nasm_instr("call", "atoi", NULL, NULL, NULL);
  if(restoreeax){ // eax wasn't free
    result->u.oper_temp.emplacement = new_registre(result); // will not be REG_EAX
    _nasm_instr("mov", nomreg[result->u.oper_temp.emplacement], "eax", NULL, NULL);
    _nasm_instr("pop", "eax", NULL, NULL, "rétablir eax") ;
  }
  else{
    result->u.oper_temp.emplacement = REG_EAX;
    desc_reg[REG_EAX]=result;
  }
}

/******************************************************************************/

void c3a2nasm_jump(char *opcode, operande *oper1, operande *oper2, operande *cible){
  char *oper1string;
  if((oper1->oper_type == O_VARIABLE && oper2->oper_type == O_VARIABLE) ||
      oper1->oper_type == O_CONSTANTE || oper1->oper_type == O_TEMPORAIRE
    ){
    oper1string = nomreg[oper2reg(oper1, REG_NONE)];
  }
  else{
    oper1string = varconst2nasm(oper1);
  }
  char *oper2string;
  if(oper2->oper_type == O_TEMPORAIRE){
    oper2string = nomreg[oper2reg(oper2, REG_NONE)]; //temp in reg
  }
  else{
    oper2string = varconst2nasm(oper2);
  }
  _nasm_instr("cmp", oper1string, oper2string, NULL, NULL);
  _nasm_instr(opcode, cible->u.oper_nom, NULL, NULL, "saut");
}

/******************************************************************************/

void c3a2nasm_appel(operande *foncname, operande *result){
  _nasm_instr("call", foncname->u.oper_nom, NULL, NULL, NULL);
  int tabfonc = rechercheExecutable(&foncname->u.oper_nom[1]);
  int nbparam = tabsymboles.tab[tabfonc].complement;
  if(nbparam != 0) { // desallouer les arguments
    printf("\tadd\tesp, %d", 4 * nbparam);
    printverb("\t\t; desallocation parametres");
    printf("\n");
  }
  if(result){
    result->u.oper_temp.emplacement = new_registre(result);
    _nasm_instr("pop",nomreg[result->u.oper_temp.emplacement],NULL,NULL,"récupère valeur de retour");
    recupere_registres_apres_appel(result->u.oper_temp.emplacement);
  }
  else{
    _nasm_instr("add","esp","4",NULL,"desalloue valeur de retour ignorée");
  }
}

/******************************************************************************/

void c3a2nasm_param(operande *oper){
  char *argchar;
  if(oper->oper_type == O_TEMPORAIRE) {
    argchar = nomreg[oper->u.oper_temp.emplacement];
  }
  else{
    argchar = varconst2nasm(oper);
  }
  _nasm_instr("push", argchar, NULL, NULL, "empile argument");
}

/******************************************************************************/

void c3a2nasm_val_ret(operande *oper){
  char *argchar;
  if(oper->oper_type == O_TEMPORAIRE) {
    argchar = nomreg[oper->u.oper_temp.emplacement];
  }
  else if(oper->oper_type == O_VARIABLE){
    argchar = nomreg[oper2reg(oper, REG_NONE)];
  }
  else{
    argchar = varconst2nasm(oper);
  }
  _nasm_instr_relative("mov", argchar, "ebp", (arguments)*4 + 8, "ecriture de la valeur de retour");
}

/******************************************************************************/

void c3a2nasm_verbose(operation_3a *operation){
  printverb("; ------------------------------------------------\n; ");
  if(nasm_verbose){
    printverba("Ligne %d ",i_ligne);
    code3a_affiche_ligne_code(operation);
  }
  printverb("\n");
}

/******************************************************************************/

void c3a2nasm_generer(){
  operation_3a i_oper;
  printf("%%include\t'%s'\n","io.asm");
  /* Variables globales */
  printf("%s","\nsection\t.bss\n");
  printf("%s", "sinput:\tresb\t255\t;reserve a 255 byte space in memory for the users input string\n");
  //i_oper = code3a.liste[0];
  for(i_ligne=0; i_ligne < code3a.next &&
                 code3a.liste[i_ligne].op_code == alloc; i_ligne++){
    i_oper = code3a.liste[i_ligne];
    c3a2nasm_verbose(&i_oper);
    printf("%s:\tresd\t%d\n", i_oper.op_oper2->u.oper_nom, i_oper.op_oper1->u.oper_valeur);
  }
  printf("%s","\nsection\t.text\n");
  printf("%s","global _start\n");
  printf("%s","_start:\n");
  _nasm_instr("call", "fmain", NULL, NULL, NULL);
  _nasm_instr("mov", "eax", "1" , NULL, "1 est le code de SYS_EXIT");
  _nasm_instr("int", "0x80", NULL, NULL, "exit");
  for(; i_ligne < code3a.next; i_ligne++){ // liste de déc. fonctions
    i_oper = code3a.liste[i_ligne];
    c3a2nasm_verbose(&i_oper);
    if(i_oper.op_etiq){
      printf("%s:\n",i_oper.op_etiq);
    }
    switch(i_oper.op_code) {
      case func_begin : // début de fonction
        c3a2nasm_debutfonction(i_oper.op_etiq+1);
        break;
      case func_end : // fin de fonction
        c3a2nasm_finfonction(varlocs);
        break;
      case func_call : // appel de fonction
        c3a2nasm_appel(i_oper.op_oper1,i_oper.op_result);
        break;
      case jump :
        _nasm_instr("jmp", i_oper.op_oper1->u.oper_nom, NULL, NULL, NULL);
        break;
      case func_val_ret : // appel de fonction
        c3a2nasm_val_ret(i_oper.op_oper1);

        break;
      case func_param :
        c3a2nasm_param(i_oper.op_oper1);
        break;
      case arith_add :
        c3a2nasm_arith("add",i_oper.op_oper1, i_oper.op_oper2, i_oper.op_result);
        break;
      case arith_sub :
        c3a2nasm_arith("sub",i_oper.op_oper1, i_oper.op_oper2, i_oper.op_result);
        break;
      case arith_mult :
        c3a2nasm_arith("imul",i_oper.op_oper1, i_oper.op_oper2, i_oper.op_result);
        break;
      case arith_div : // TODO FIXME
        c3a2nasm_division(i_oper.op_oper1, i_oper.op_oper2, i_oper.op_result);
        break;
      case alloc :
        c3a2nasm_allouer(i_oper.op_oper2);
        break;
      case assign : // affectation i_oper.op_result <- i_oper.op_oper1
        c3a2nasm_affect(i_oper.op_result, i_oper.op_oper1);
        break;
      case sys_write :
        c3a2nasm_write(i_oper.op_oper1);
        break;
      case sys_read :
        c3a2nasm_read(i_oper.op_result);
        break;
      case jump_if_equal :
        c3a2nasm_jump("je",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      case jump_if_not_equal :
        c3a2nasm_jump("jne",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      case jump_if_less :
        c3a2nasm_jump("jl",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      case jump_if_greater :
        c3a2nasm_jump("jg",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      case jump_if_greater_or_equal :
        c3a2nasm_jump("jge",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      case jump_if_less_or_equal :
        c3a2nasm_jump("jle",i_oper.op_oper1,i_oper.op_oper2,i_oper.op_result);
        break;
      default:
        erreur("Opération en code 3 adresses non reconnue");
    }
  }
}


/******************************************************************************/

/* Par convention, les derniers opérandes sont nuls si l'opération a moins de
   3 arguments */
void _nasm_instr(char *opcode, char *op1, char *op2, char *op3, char *comment) {
  printf("\t%s", opcode);
  if(op1 != NULL) {
    printf("\t%s", op1);
    if(op2 != NULL) {
      printf(", %s", op2);
      if(op3 != NULL) {
        printf(", %s", op3);
      }
    }
  }
  _nasm_comment(comment);
}

/******************************************************************************/

void _nasm_instr_relative(char *opcode,  char *op1, char *op2, int offset, char *comment) {
  if(offset < 0){
    printf("\t%s\tdword [%s - %d], %s", opcode, op2, -offset, op1);
  }
  else{
    printf("\t%s\tdword [%s + %d], %s", opcode, op2, offset, op1);
  }
  /* printifm("\t%s\t[%s + %d], %s", opcode, op2, offset, op1); */
  _nasm_comment(comment);
}

/******************************************************************************/

void _nasm_etiquette(char* etiquette) {
  printf("%s:\n", etiquette);
}
