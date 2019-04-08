#include <stdio.h>
#include <stdlib.h>
#include "syntabs.h"
#include "util.h"
#include "tabsymboles.h"
#include "parcours_arbre_abstrait.h"
#include "code3a.h"
#include "c3a2nasm.h"

extern tabsymboles_ tabsymboles;

extern int portee;
extern int adresseGlobaleCourante;
extern int adresseLocaleCourante;
extern int adresseArgumentCourant;

extern int trace_tabsymb;

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
operande * parcours_exp(n_exp *n);
operande * parcours_varExp(n_exp *n);
operande * parcours_opExp(n_exp *n);
operande * parcours_intExp(n_exp *n);
operande * parcours_appelExp(n_exp *n);
operande * parcours_lireExp(n_exp *n);
void parcours_l_dec(n_l_dec *n);
void parcours_dec(n_dec *n);
void parcours_foncDec(n_dec *n);
void parcours_varDec(n_dec *n);
void parcours_tabDec(n_dec *n);
operande * parcours_var(n_var *n);
operande * parcours_var_indicee(n_var *n);
operande* parcours_appel(n_appel *n, int call_type);


/*-------------------------------------------------------------------------*/

void parcours_n_prog(n_prog *n)
{
  entreeProgramme();
  code3a_init();
  parcours_l_dec(n->variables);
  parcours_l_dec(n->fonctions);

  if(rechercheExecutable("main") != -1)
  {

    if(!(tabsymboles.tab[rechercheExecutable("main")].complement == 0))
    {
        erreur("Main ne peut pas prendre d'argument");
    }

  }
  else {erreur("Main introuvable");}
}

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
  operande * op_test = parcours_exp(n->u.si_.test);

  char * etiquette_name_1 = code3a_new_etiquette_name();
  code3a_ajoute_instruction(jump_if_equal, op_test, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_1), "Test Si");
  
  parcours_instr(n->u.si_.alors);
  
  if(n->u.si_.sinon)
  {
    char * etiquette_name_2 = code3a_new_etiquette_name();
    code3a_ajoute_instruction(jump, code3a_new_etiquette(etiquette_name_2), NULL, NULL, "Saut du sinon Si");
    code3a_ajoute_etiquette(etiquette_name_1);

    parcours_instr(n->u.si_.sinon);
    
    code3a_ajoute_etiquette(etiquette_name_2);
  }
  else
  {
    code3a_ajoute_etiquette(etiquette_name_1);
  }
}

/*-------------------------------------------------------------------------*/

void parcours_instr_tantque(n_instr *n)
{
  char * etiquette_name_debut = code3a_new_etiquette_name();
  code3a_ajoute_etiquette(etiquette_name_debut);

  operande * op_test = parcours_exp(n->u.tantque_.test);

  char * etiquette_name_fin = code3a_new_etiquette_name();
  code3a_ajoute_instruction(jump_if_equal, op_test, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_fin), "Test condition Tantque");

  parcours_instr(n->u.tantque_.faire);
  
  code3a_ajoute_instruction(jump, code3a_new_etiquette(etiquette_name_debut), NULL, NULL, "Retour au test Tantque");
  code3a_ajoute_etiquette(etiquette_name_fin);
}

/*-------------------------------------------------------------------------*/

void parcours_instr_affect(n_instr *n)
{
  operande * op_var = parcours_var(n->u.affecte_.var);
  operande * op_exp = parcours_exp(n->u.affecte_.exp);

  code3a_ajoute_instruction(assign, op_exp, NULL, op_var, "Affectation d'une variable");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_appel(n_instr *n)
{
  
  parcours_appel(n->u.appel, 0);
  
}
/*-------------------------------------------------------------------------*/

operande * parcours_appel(n_appel *n, int call_type)
{
  if(rechercheExecutable(n->fonction) != -1)
  {
    operande * op_result = code3a_new_temporaire();
    code3a_ajoute_instruction(alloc, code3a_new_constante(1), NULL, NULL, "Allocation valeur de retour");
    
    if(( n->args == NULL ) && ( tabsymboles.tab[rechercheExecutable(n->fonction)].complement == 0 ))
    {
      parcours_l_exp(n->args);
    }
    else if(tabsymboles.tab[rechercheExecutable(n->fonction)].complement == n->args->size)
    {
      parcours_l_exp(n->args);
    }
    else {erreur("Fonction appelée avec un nombre incorrect d'argument");}

    switch(call_type)
    {
      case 0: code3a_ajoute_instruction(func_call, code3a_new_func(n->fonction), NULL, NULL, "Appel fonction instruction");
                    break;
      case 1: code3a_ajoute_instruction(func_call, code3a_new_func(n->fonction), NULL, op_result, "Appel fonction Expression");
                  break;
      default: ;
    }  

    return op_result;
  }
 else {erreur("Fonction non déclarée");}
}

/*-------------------------------------------------------------------------*/

void parcours_instr_retour(n_instr *n)
{
  operande * op_ret = parcours_exp(n->u.retour_.expression);
  
  code3a_ajoute_instruction(func_val_ret, op_ret, NULL, NULL, "Retour");
  code3a_ajoute_instruction(func_end, NULL, NULL, NULL, "fend");
}

/*-------------------------------------------------------------------------*/

void parcours_instr_ecrire(n_instr *n)
{
  operande * message = parcours_exp(n->u.ecrire_.expression);
  code3a_ajoute_instruction(sys_write, message, NULL, NULL, "Appel ecrire()");
}

/*-------------------------------------------------------------------------*/

void parcours_l_exp(n_l_exp *n)
{
  if(n)
  {
    operande * op_args = parcours_exp(n->tete);
    code3a_ajoute_instruction(func_param, op_args, NULL, NULL, "Param");
    parcours_l_exp(n->queue);
  }
}

/*-------------------------------------------------------------------------*/

operande * parcours_exp(n_exp *n)
{
  if(n->type == varExp) return parcours_varExp(n);
  else if(n->type == opExp) return parcours_opExp(n);
  else if(n->type == intExp) return parcours_intExp(n); 
  else if(n->type == appelExp) return parcours_appelExp(n); 
  else if(n->type == lireExp) return parcours_lireExp(n); 
  return NULL; // Wallah c'est à cause des warnings!

}

/*-------------------------------------------------------------------------*/

operande * parcours_varExp(n_exp *n)
{

  return parcours_var(n->u.var);
  
}

/*-------------------------------------------------------------------------*/
operande * parcours_opExp(n_exp *n)
{
  operande * op_oper1 = NULL;
  operande * op_oper2 = NULL;
  operande * op_result;
  char * etiquette_name_1;
  char * etiquette_name_2;
  char * comment = "";
  
  if( n->u.opExp_.op1 != NULL ) 
  {  
    op_oper1 = parcours_exp(n->u.opExp_.op1);
    
    if( n->u.opExp_.op2 != NULL ) 
    {
      op_oper2 = parcours_exp(n->u.opExp_.op2);
      
      op_result = code3a_new_temporaire();

      switch(n->u.opExp_.op)
      {
        case plus : comment = "Addition"; 
                    code3a_ajoute_instruction(arith_add, op_oper1, op_oper2, op_result, comment);
                    break;
        case moins :  comment = "Soustration"; 
                      code3a_ajoute_instruction(arith_sub, op_oper1, op_oper2, op_result, comment);
                      break;
        case fois : comment = "Multiplication"; 
                    code3a_ajoute_instruction(arith_mult, op_oper1, op_oper2, op_result, comment);
                    break;
        case divise : comment = "Division"; 
                      code3a_ajoute_instruction(arith_div, op_oper1, op_oper2, op_result, comment);
                      break;
        case egal : etiquette_name_1 = code3a_new_etiquette_name();
                    code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, op_result, "Initialisation Egal");
                    code3a_ajoute_instruction(jump_if_equal, op_oper1, op_oper2, code3a_new_etiquette(etiquette_name_1), "Test Egal");
                    code3a_ajoute_instruction(assign, code3a_new_constante(0), NULL, op_result, "Correction Egal");
                    code3a_ajoute_etiquette(etiquette_name_1);
                    break;            
        case inferieur :  etiquette_name_1 = code3a_new_etiquette_name();
                          code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, op_result, "Initialisation Inferieur");
                          code3a_ajoute_instruction(jump_if_less, op_oper1, op_oper2, code3a_new_etiquette(etiquette_name_1), "Test Inferieur");
                          code3a_ajoute_instruction(assign, code3a_new_constante(0), NULL, op_result, "Correction Inferieur");
                          code3a_ajoute_etiquette(etiquette_name_1);
                          break;
        case ou : etiquette_name_1 = code3a_new_etiquette_name();
                  etiquette_name_2 = code3a_new_etiquette_name();
                  code3a_ajoute_instruction(assign, code3a_new_constante(0), NULL, op_result, "Initialisation Ou");
                  code3a_ajoute_instruction(jump_if_equal, op_oper1, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_1), "Test operande 1 Ou");
                  code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, op_result, "Correction 1 Ou");
                  code3a_ajoute_instruction(jump, code3a_new_etiquette(etiquette_name_2), NULL, NULL, "Fin Ou");
                  code3a_ajoute_etiquette(etiquette_name_1);
                  code3a_ajoute_instruction(jump_if_equal, op_oper2, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_2), "Test operande 2 Ou");
                  code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, op_result, "Correction 2 Ou");
                  code3a_ajoute_etiquette(etiquette_name_2);
                  break;
        case et : etiquette_name_1 = code3a_new_etiquette_name();
                  code3a_ajoute_instruction(assign, code3a_new_constante(0), NULL, op_result, "Initialisation Et");
                  code3a_ajoute_instruction(jump_if_equal, op_oper1, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_1), "Test operande 1 Et");
                  code3a_ajoute_instruction(jump_if_equal, op_oper2, code3a_new_constante(0), code3a_new_etiquette(etiquette_name_1), "Test operande 1 Et");
                  code3a_ajoute_instruction(assign, code3a_new_constante(-1),NULL,  op_result,"Correction Et");
                  code3a_ajoute_etiquette(etiquette_name_1);
                  break;
        default : ; //certainnement un ERREUR sinon
      }
      
    }
    
    else
    {
      if(n->u.opExp_.op == non)
      {
        op_result = code3a_new_temporaire();
        char * etiquette_name = code3a_new_etiquette_name();

        code3a_ajoute_instruction(assign, code3a_new_constante(-1), NULL, op_result, "Initialisation Negation");
        code3a_ajoute_instruction(jump_if_equal, op_oper1, code3a_new_constante(0), code3a_new_etiquette(etiquette_name), "Test Negation");
        code3a_ajoute_instruction(assign, code3a_new_constante(0), NULL, op_result, "Correction Negation");
        code3a_ajoute_etiquette(etiquette_name);
      }
      else erreur("Le type d'expression ne match pas le nombre d'opérande");
    }
    
  }
  else erreur("Nombre d'operandes invalide");

  return op_result;
}

/*-------------------------------------------------------------------------*/

operande * parcours_intExp(n_exp *n)
{
  return code3a_new_constante(n->u.entier);
}

/*-------------------------------------------------------------------------*/

operande * parcours_appelExp(n_exp *n)
{
  return parcours_appel(n->u.appel, 1);;
}

/*-------------------------------------------------------------------------*/

operande * parcours_lireExp(n_exp *n)
{
  operande * op_result = code3a_new_temporaire();
  code3a_ajoute_instruction(sys_read, NULL, NULL, op_result, "Apel de lire()");
  return op_result;
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
  char * nom_func = malloc(sizeof(char)*100);
  sprintf(nom_func, "f%s", n->nom);
  code3a_ajoute_etiquette(nom_func); 

  code3a_ajoute_instruction(func_begin, NULL, NULL, NULL,"Début de fonction");

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

  code3a_ajoute_instruction(func_end, NULL, NULL, NULL,"Fin de fonction");
}

/*-------------------------------------------------------------------------*/

void parcours_varDec(n_dec *n)
{

  if(portee == P_ARGUMENT)
  {
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseArgumentCourant, 1);
    adresseArgumentCourant += 4;
  }

  else if(portee == P_VARIABLE_LOCALE)
  {
    code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "allocation declaration variable locale");
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseLocaleCourante, 1);
    adresseLocaleCourante += 4;
  }

  else if(portee == P_VARIABLE_GLOBALE)
  {
    code3a_ajoute_instruction(alloc, code3a_new_constante(1), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "allocation declaration variable globale");
    ajouteIdentificateur(n->nom, portee, T_ENTIER, adresseGlobaleCourante, 1);
    adresseGlobaleCourante += 4;
  }

}

/*-------------------------------------------------------------------------*/

void parcours_tabDec(n_dec *n)
{

  if(portee == P_VARIABLE_GLOBALE)
  {
    code3a_ajoute_instruction(alloc, code3a_new_constante(n->u.tabDec_.taille), code3a_new_var(n->nom, portee, adresseLocaleCourante), NULL, "allocation declaration tableau globale");
    ajouteIdentificateur(n->nom, portee, T_TABLEAU_ENTIER, adresseGlobaleCourante, n->u.tabDec_.taille);
    adresseGlobaleCourante += (4 * n->u.tabDec_.taille);
  }
  else erreur("Un tableau ne peut être déclaré que comme variable globale");
}

/*-------------------------------------------------------------------------*/

operande * parcours_var(n_var *n)
{
  if(rechercheExecutable(n->nom) != -1)
  {
    if(n->type == simple)
    {
      if(!(tabsymboles.tab[rechercheExecutable(n->nom)].type == T_ENTIER))
      {
              erreur("Usage incorrect du tableau");
      }
      else
      {
        return code3a_new_var(n->nom, tabsymboles.tab[rechercheExecutable(n->nom)].portee, tabsymboles.tab[rechercheExecutable(n->nom)].adresse);
      }
      
    }
    else 
    {
      if(n->type == indicee)
      {
        if(tabsymboles.tab[rechercheExecutable(n->nom)].type == T_TABLEAU_ENTIER)
        {  
          operande * op_indice = parcours_var_indicee(n);
          operande * op_temp;

          switch(op_indice->oper_type)
          {
            case O_VARIABLE : op_temp = code3a_new_temporaire();
                              code3a_ajoute_instruction(assign, op_indice, NULL, op_temp, "Creation temporaire case tableau");
                              return code3a_new_var_indicee(n->nom, tabsymboles.tab[rechercheExecutable(n->nom)].portee, tabsymboles.tab[rechercheExecutable(n->nom)].adresse, op_temp);
                              break;
            case O_TEMPORAIRE :
            case O_CONSTANTE :  return code3a_new_var_indicee(n->nom, tabsymboles.tab[rechercheExecutable(n->nom)].portee, tabsymboles.tab[rechercheExecutable(n->nom)].adresse, op_indice);
                                break;
             
            default : ;
          }
          
        }
        else
        {
          erreur("Usage incorrect de la variable");
        }
      }
      erreur("Type de variable erroné");
    }
  }
  else erreur("Variable ou tableau non déclaré.e");
  return NULL; // Wallah c'est encore à cause des warnings!
}

/*-------------------------------------------------------------------------*/
operande * parcours_var_indicee(n_var *n)
{
  return parcours_exp(n->u.indicee_.indice);
}
/*-------------------------------------------------------------------------*/