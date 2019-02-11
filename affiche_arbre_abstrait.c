#include <stdio.h>
#include "syntabs.h"
#include "util.h"

void affiche_n_prog(n_prog *n);
void affiche_l_instr(n_l_instr *n);
void affiche_instr(n_instr *n);
void affiche_instr_si(n_instr *n);
void affiche_instr_tantque(n_instr *n);
void affiche_instr_affect(n_instr *n);
void affiche_instr_appel(n_instr *n);
void affiche_instr_retour(n_instr *n);
void affiche_instr_ecrire(n_instr *n);
void affiche_l_exp(n_l_exp *n);
void affiche_exp(n_exp *n);
void affiche_varExp(n_exp *n);
void affiche_opExp(n_exp *n);
void affiche_intExp(n_exp *n);
void affiche_lireExp(n_exp *n);
void affiche_appelExp(n_exp *n);
void affiche_l_dec(n_l_dec *n);
void affiche_dec(n_dec *n);
void affiche_foncDec(n_dec *n);
void affiche_varDec(n_dec *n);
void affiche_tabDec(n_dec *n);
void affiche_var(n_var *n);
void affiche_var_simple(n_var *n);
void affiche_var_indicee(n_var *n);
void affiche_appel(n_appel *n);

int trace_abs = 1;

/*-------------------------------------------------------------------------*/

void affiche_n_prog(n_prog *n)
{
  char *fct = "prog";
  affiche_balise_ouvrante(fct, trace_abs);

  affiche_l_dec(n->variables);
  affiche_l_dec(n->fonctions); 
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void affiche_l_instr(n_l_instr *n)
{
  char *fct = "l_instr";
  if(n){
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_instr(n->tete);
  affiche_l_instr(n->queue);
  affiche_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_instr(n_instr *n)
{
  if(n){
    if(n->type == blocInst) affiche_l_instr(n->u.liste);
    else if(n->type == affecteInst) affiche_instr_affect(n);
    else if(n->type == siInst) affiche_instr_si(n);
    else if(n->type == tantqueInst) affiche_instr_tantque(n);
    else if(n->type == appelInst) affiche_instr_appel(n);
    else if(n->type == retourInst) affiche_instr_retour(n);
    else if(n->type == ecrireInst) affiche_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_instr_si(n_instr *n)
{  
  char *fct = "instr_si";
  affiche_balise_ouvrante(fct, trace_abs);

  affiche_exp(n->u.si_.test);
  affiche_instr(n->u.si_.alors);
  if(n->u.si_.sinon){
    affiche_instr(n->u.si_.sinon);
  }
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_tantque(n_instr *n)
{
  char *fct = "instr_tantque";
  affiche_balise_ouvrante(fct, trace_abs);

  affiche_exp(n->u.tantque_.test);
  affiche_instr(n->u.tantque_.faire);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_affect(n_instr *n)
{
  char *fct = "instr_affect";
  affiche_balise_ouvrante(fct, trace_abs);


  affiche_var(n->u.affecte_.var);
  affiche_exp(n->u.affecte_.exp);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_appel(n_instr *n)
{
  char *fct = "instr_appel";
  affiche_balise_ouvrante(fct, trace_abs);


  affiche_appel(n->u.appel);
  affiche_balise_fermante(fct, trace_abs);
}
/*-------------------------------------------------------------------------*/

void affiche_appel(n_appel *n)
{
  char *fct = "appel";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_xml_texte( n->fonction, trace_abs);
  affiche_l_exp(n->args);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_retour(n_instr *n)
{
  char *fct = "instr_retour";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_exp(n->u.retour_.expression);
  affiche_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void affiche_instr_ecrire(n_instr *n)
{
  char *fct = "instr_ecrire";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_exp(n->u.ecrire_.expression);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_l_exp(n_l_exp *n)
{
  char *fct = "l_exp";
  affiche_balise_ouvrante(fct, trace_abs);

  if(n){
    affiche_exp(n->tete);
    affiche_l_exp(n->queue);
  }
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_exp(n_exp *n)
{

  if(n->type == varExp) affiche_varExp(n);
  else if(n->type == opExp) affiche_opExp(n);
  else if(n->type == intExp) affiche_intExp(n);
  else if(n->type == appelExp) affiche_appelExp(n);
  else if(n->type == lireExp) affiche_lireExp(n);
}

/*-------------------------------------------------------------------------*/

void affiche_varExp(n_exp *n)
{
  char *fct = "varExp";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_var(n->u.var);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/
void affiche_opExp(n_exp *n)
{
  char *fct = "opExp";
  affiche_balise_ouvrante(fct, trace_abs);
  if(n->u.opExp_.op == plus) affiche_xml_texte("plus", trace_abs);
  else if(n->u.opExp_.op == moins) affiche_xml_texte("moins", trace_abs);
  else if(n->u.opExp_.op == fois) affiche_xml_texte("fois", trace_abs);
  else if(n->u.opExp_.op == divise) affiche_xml_texte("divise", trace_abs);
  else if(n->u.opExp_.op == egal) affiche_xml_texte("egal", trace_abs);
  else if(n->u.opExp_.op == inferieur) affiche_xml_texte("inf", trace_abs);
  else if(n->u.opExp_.op == ou) affiche_xml_texte("ou", trace_abs);
  else if(n->u.opExp_.op == et) affiche_xml_texte("et", trace_abs);
  else if(n->u.opExp_.op == non) affiche_xml_texte("non", trace_abs);  
  if( n->u.opExp_.op1 != NULL ) {
    affiche_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    affiche_exp(n->u.opExp_.op2);
  }
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_intExp(n_exp *n)
{
  char texte[ 50 ]; // Max. 50 chiffres
  sprintf(texte, "%d", n->u.entier);
  affiche_element( "intExp", texte, trace_abs );
}

/*-------------------------------------------------------------------------*/
void affiche_lireExp(n_exp *n)
{
  char *fct = "lireExp";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void affiche_appelExp(n_exp *n)
{
  char *fct = "appelExp";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_appel(n->u.appel);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_l_dec(n_l_dec *n)
{
  char *fct = "l_dec";

  if( n ){
    affiche_balise_ouvrante(fct, trace_abs);
    affiche_dec(n->tete);
    affiche_l_dec(n->queue);
    affiche_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_dec(n_dec *n)
{

  if(n){
    if(n->type == foncDec) {
      affiche_foncDec(n);
    }
    else if(n->type == varDec) {
      affiche_varDec(n);
    }
    else if(n->type == tabDec) { 
      affiche_tabDec(n);
    }
  }
}

/*-------------------------------------------------------------------------*/

void affiche_foncDec(n_dec *n)
{
  char *fct = "foncDec";
  affiche_balise_ouvrante(fct, trace_abs);
  //  affiche_xml_texte( n->nom, trace_abs );
  affiche_l_dec(n->u.foncDec_.param);
  affiche_l_dec(n->u.foncDec_.variables);
  affiche_instr(n->u.foncDec_.corps);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_varDec(n_dec *n)
{
  affiche_element("varDec", n->nom, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_tabDec(n_dec *n)
{
  char texte[100]; // Max. 100 chars nom tab + taille
  sprintf(texte, "%s[%d]", n->nom, n->u.tabDec_.taille);
  affiche_element( "tabDec", texte, trace_abs );
}

/*-------------------------------------------------------------------------*/

void affiche_var(n_var *n)
{
  if(n->type == simple) {
    affiche_var_simple(n);
  }
  else if(n->type == indicee) {
    affiche_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/
void affiche_var_simple(n_var *n)
{
  affiche_element("var_simple", n->nom, trace_abs);
}

/*-------------------------------------------------------------------------*/
void affiche_var_indicee(n_var *n)
{
  char *fct = "var_indicee";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_element("var_base_tableau", n->nom, trace_abs);
  affiche_exp( n->u.indicee_.indice );
  affiche_balise_fermante(fct, trace_abs);
}
/*-------------------------------------------------------------------------*/
