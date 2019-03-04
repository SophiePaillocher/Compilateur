#include <stdio.h>
#include "syntabs.h"
#include "util.h"

void parcours_n_prog(n_prog *n);
void parcours_l_instr(n_l_instr *n);
void parcours_instr(n_instr *n);
void parcours_instr_si(n_instr *n);
void parcours_instr_tantque(n_instr *n);
void parcours_instr_affect(n_instr *n);
void parcours_instr_appel(n_instr *n);
void parcours_instr_retour(n_instr *n);
void parcours_instr_ecrire(n_instr *n);
void parcours_l_exp(n_l_exp *n);
void parcours_exp(n_exp *n);
void parcours_varExp(n_exp *n);
void parcours_opExp(n_exp *n);
void parcours_intExp(n_exp *n);
void parcours_lireExp(n_exp *n);
void parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
void parcours_var(n_var *n);
void parcours_var_simple(n_var *n);
void parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);

int trace_abs = 1;

/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
  char *fct = "prog";
  //parcours_balise_ouvrante(fct, trace_abs);

  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void parcours_l_instr(n_l_instr *n)
{
  char *fct = "l_instr";
  if(n){
  //parcours_balise_ouvrante(fct, trace_abs);
  parcours_instr(n->tete);
  parcours_l_instr(n->queue);
  //parcours_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr(n_instr *n)
{
  if(n){
    if(n->type == blocInst) parcours_l_instr(n->u.liste);
    else if(n->type == affecteInst) parcours_instr_affect(n);
    else if(n->type == siInst) parcours_instr_si(n);
    else if(n->type == tantqueInst) parcours_instr_tantque(n);
    else if(n->type == appelInst) parcours_instr_appel(n);
    else if(n->type == retourInst) parcours_instr_retour(n);
    else if(n->type == ecrireInst) parcours_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_si(n_instr *n)
{
  char *fct = "instr_si";
  //parcours_balise_ouvrante(fct, trace_abs);

  parcours_exp(n->u.si_.test);
  parcours_instr(n->u.si_.alors);
  if(n->u.si_.sinon){
    parcours_instr(n->u.si_.sinon);
  }
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  char *fct = "instr_tantque";
  //parcours_balise_ouvrante(fct, trace_abs);

  parcours_exp(n->u.tantque_.test);
  parcours_instr(n->u.tantque_.faire);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  char *fct = "instr_affect";
  //parcours_balise_ouvrante(fct, trace_abs);


  parcours_var(n->u.affecte_.var);
  parcours_exp(n->u.affecte_.exp);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  char *fct = "instr_appel";
  //parcours_balise_ouvrante(fct, trace_abs);


  parcours_appel(n->u.appel);
  //parcours_balise_fermante(fct, trace_abs);
}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)
{
  char *fct = "appel";
  //parcours_balise_ouvrante(fct, trace_abs);
  //parcours_xml_texte( n->fonction, trace_abs);
  parcours_l_exp(n->args);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  char *fct = "instr_retour";
  //parcours_balise_ouvrante(fct, trace_abs);
  parcours_exp(n->u.retour_.expression);
  //parcours_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  char *fct = "instr_ecrire";
  //parcours_balise_ouvrante(fct, trace_abs);
  parcours_exp(n->u.ecrire_.expression);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{
  char *fct = "l_exp";
  //parcours_balise_ouvrante(fct, trace_abs);

  if(n){
    parcours_exp(n->tete);
    parcours_l_exp(n->queue);
  }
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_exp(n_exp *n)
{

  if(n->type == varExp) parcours_varExp(n);
  else if(n->type == opExp) parcours_opExp(n);
  else if(n->type == intExp) parcours_intExp(n);
  else if(n->type == appelExp) parcours_appelExp(n);
  else if(n->type == lireExp) parcours_lireExp(n);
}

/*-------------------------------------------------------------------------*/

void parcours_varExp(n_exp *n)
{
  char *fct = "varExp";
  //parcours_balise_ouvrante(fct, trace_abs);
  parcours_var(n->u.var);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/
void parcours_opExp(n_exp *n)
{
  char *fct = "opExp";
  //parcours_balise_ouvrante(fct, trace_abs);
/*
  if(n->u.opExp_.op == plus) parcours_xml_texte("plus", trace_abs);
  else if(n->u.opExp_.op == moins) parcours_xml_texte("moins", trace_abs);
  else if(n->u.opExp_.op == fois) parcours_xml_texte("fois", trace_abs);
  else if(n->u.opExp_.op == divise) parcours_xml_texte("divise", trace_abs);
  else if(n->u.opExp_.op == egal) parcours_xml_texte("egal", trace_abs);
  else if(n->u.opExp_.op == inferieur) parcours_xml_texte("inf", trace_abs);
  else if(n->u.opExp_.op == ou) parcours_xml_texte("ou", trace_abs);
  else if(n->u.opExp_.op == et) parcours_xml_texte("et", trace_abs);
  else if(n->u.opExp_.op == non) parcours_xml_texte("non", trace_abs);
  //else if(n->u.opExp_.op == prio) parcours_xml_texte("()", trace_abs);
*/
  if( n->u.opExp_.op1 != NULL ) {
    parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    parcours_exp(n->u.opExp_.op2);
  }
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_intExp(n_exp *n)
{
  char texte[ 50 ]; // Max. 50 chiffres

  //parcours_element( "intExp", texte, trace_abs );
}

/*-------------------------------------------------------------------------*/
void parcours_lireExp(n_exp *n)
{
  char *fct = "lireExp";
  //parcours_balise_ouvrante(fct, trace_abs);
  //parcours_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void parcours_appelExp(n_exp *n)
{
  char *fct = "appelExp";
  //parcours_balise_ouvrante(fct, trace_abs);
  parcours_appel(n->u.appel);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  char *fct = "l_dec";

  if( n ){
    //parcours_balise_ouvrante(fct, trace_abs);
    parcours_dec(n->tete);
    parcours_l_dec(n->queue);
    //parcours_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)
{

  if(n){
    if(n->type == foncDec) {
      parcours_foncDec(n);
    }
    else if(n->type == varDec) {
      parcours_varDec(n);
    }
    else if(n->type == tabDec) {
      parcours_tabDec(n);
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_foncDec(n_dec *n)
{
  char *fct = "foncDec";
  //parcours_balise_ouvrante(fct, trace_abs);
  //  parcours_xml_texte( n->nom, trace_abs );
  parcours_l_dec(n->u.foncDec_.param);
  parcours_l_dec(n->u.foncDec_.variables);
  parcours_instr(n->u.foncDec_.corps);
  //parcours_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{
  if(rechercheDeclarative(n->nom)!=-1)
    ajouteIdentificateur( n->nom,  int portee, int type, int adresse, int complement);

  //parcours_element("varDec", n->nom, trace_abs);

}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{
  char texte[100]; // Max. 100 chars nom tab + taille
  //parcours_element( "tabDec", texte, trace_abs );
}

/*-------------------------------------------------------------------------*/

void parcours_var(n_var *n)
{
  if(n->type == simple) {
    parcours_var_simple(n);
  }
  else if(n->type == indicee) {
    parcours_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/
void parcours_var_simple(n_var *n)
{
  //parcours_element("var_simple", n->nom, trace_abs);
}

/*-------------------------------------------------------------------------*/
void parcours_var_indicee(n_var *n)
{
  char *fct = "var_indicee";
  //parcours_balise_ouvrante(fct, trace_abs);
  //parcours_element("var_base_tableau", n->nom, trace_abs);
  parcours_exp( n->u.indicee_.indice );
  //parcours_balise_fermante(fct, trace_abs);
}
/*-------------------------------------------------------------------------*/
