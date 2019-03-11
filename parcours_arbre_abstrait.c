#include <stdio.h>
#include "syntabs.h"
#include "util.h"
#include "tabsymboles.h"
#include "parcours_arbre_abstrait.h"


extern tabsymboles_ tabsymboles;

extern int portee;
extern int adresseGlobaleCourante;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;


int trace_tabsymb = 1;                  // à Editer

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
void parcours_appelExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
void parcours_var(n_var *n);
void parcours_var_indicee(n_var *n);
void parcours_appel(n_appel *n);


/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
  entreeProgramme();

  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);
  char * m = "main";
  if(rechercheExecutable(m) != -1)
  {

    if(!(tabsymboles.tab[rechercheExecutable("main")].complement == 0))
    {
        erreur("Main ne peut pas prendre d'argument");
    }

  }
  else {erreur("Main introuvable");}
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void parcours_l_instr(n_l_instr *n)
{

  if(n)
  {

  parcours_instr(n->tete);
  parcours_l_instr(n->queue);

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

  parcours_exp(n->u.si_.test);
  parcours_instr(n->u.si_.alors);
  if(n->u.si_.sinon)
  {
    parcours_instr(n->u.si_.sinon);
  }

}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{

  parcours_exp(n->u.tantque_.test);
  parcours_instr(n->u.tantque_.faire);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{

  parcours_var(n->u.affecte_.var);
  parcours_exp(n->u.affecte_.exp);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{

  parcours_appel(n->u.appel);

}
/*-------------------------------------------------------------------------*/

void parcours_appel(n_appel *n)
{

  if(rechercheExecutable(n->fonction) != -1)
  {
    //printf("complement: %d",tabsymboles.tab[rechercheExecutable(n->fonction)].complement);
    //printf("argsSize: %d",n->args->size);
    if(tabsymboles.tab[rechercheExecutable(n->fonction)].complement == n->args->size)
    {
      parcours_l_exp(n->args);
    }

  }
 else { erreur("Fonction non déclarée");}

}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{

  parcours_exp(n->u.retour_.expression);

}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{

  parcours_exp(n->u.ecrire_.expression);

}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{

  if(n)
  {
    parcours_exp(n->tete);
    parcours_l_exp(n->queue);
  }

}

/*-------------------------------------------------------------------------*/

void parcours_exp(n_exp *n)
{
  if(n->type == varExp) parcours_varExp(n);
  else if(n->type == opExp) parcours_opExp(n);
  else if(n->type == appelExp) parcours_appelExp(n);
}

/*-------------------------------------------------------------------------*/

void parcours_varExp(n_exp *n)
{

  parcours_var(n->u.var);

}

/*-------------------------------------------------------------------------*/
void parcours_opExp(n_exp *n)
{

  if( n->u.opExp_.op1 != NULL ) {
    parcours_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
    parcours_exp(n->u.opExp_.op2);
  }

}

/*-------------------------------------------------------------------------*/

void parcours_appelExp(n_exp *n)
{

  parcours_appel(n->u.appel);

}

/*-------------------------------------------------------------------------*/

void parcours_l_dec(n_l_dec *n)
{
  if(n)
  {
    parcours_dec(n->tete);
    parcours_l_dec(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_dec(n_dec *n)
{

  if(n)
  {
    if(rechercheDeclarative(n->nom)==-1)
    {
      if (n->type == foncDec)
      {
        parcours_foncDec(n);
      }
      else if (n->type == varDec)
      {
        parcours_varDec(n);
      }
      else if (n->type == tabDec)
      {
        parcours_tabDec(n);
      }
    }
    else {
      if (n->type == foncDec)
      {
        erreur("Fonction déjà déclarée");
      }
      else if (n->type == varDec)
      {
        erreur("Variable déjà déclarée");
      }
      else if (n->type == tabDec)
      {
        erreur("Tableau déjà déclaré");
      }
    }
  }
}

/*-------------------------------------------------------------------------*/

void parcours_foncDec(n_dec *n)
{

  if(n->u.foncDec_.param == NULL)
  {
    ajouteIdentificateur(n->nom, portee, T_FONCTION, adresseLocaleCourante, 0);
  }
  else
  {
    ajouteIdentificateur(n->nom, portee, T_FONCTION, adresseLocaleCourante, n->u.foncDec_.param->size);
  }

  entreeFonction();
  parcours_l_dec(n->u.foncDec_.param);

  entreeBlocFonction();
  parcours_l_dec(n->u.foncDec_.variables);

  parcours_instr(n->u.foncDec_.corps);

  sortieFonction(trace_tabsymb);
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{

  if(portee == P_ARGUMENT)
  {
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseArgumentCourant, 0);
    adresseArgumentCourant += 4;
  }

  else if(portee == P_VARIABLE_LOCALE)
  {
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseLocaleCourante, 0);
    adresseLocaleCourante += 4;
  }

  else if(portee == P_VARIABLE_GLOBALE)
  {
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseGlobaleCourante, 0);
    adresseGlobaleCourante += 4;
  }

}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{

  if(portee == P_VARIABLE_GLOBALE)
  {
    ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
    adresseGlobaleCourante += (4 * n->u.tabDec_.taille);
  }
  else erreur("Un tableau ne peut être déclaré que comme variable globale");
}

/*-------------------------------------------------------------------------*/

void parcours_var(n_var *n)
{
  if(rechercheExecutable(n->nom) != -1)
  {
    if(n->type == simple)
    {
      if(!(tabsymboles.tab[rechercheExecutable(n->nom)].type == T_ENTIER))
      {
              erreur("Usage incorrect de la variable");
      }

    }
    else if(n->type == indicee)
    {
      if(tabsymboles.tab[rechercheExecutable(n->nom)].type == T_TABLEAU_ENTIER)
        parcours_var_indicee(n);
      else
      {
        erreur("Usage incorrect du tableau");
      }
    }
  }
  else erreur("Variable ou tableau non déclaré.e");
}

/*-------------------------------------------------------------------------*/
void parcours_var_indicee(n_var *n)
{
  parcours_exp( n->u.indicee_.indice );
}
/*-------------------------------------------------------------------------*/
