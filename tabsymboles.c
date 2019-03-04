#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tabsymboles.h"
#include "util.h"

/* Table des symboles (globale ET locale) */
tabsymboles_ tabsymboles;

/*-------------------------------------------------------------------------*/

/** 
  * Fonction qui bascule table globale -> table locale. À appeler lors qu'on
  * parcourt une déclaration de fonction, juste avant la liste d'arguments.
  * ATTENTION : après avoir traité les arguments, votre programme doit modifier
  * __excplicitement___ la valeu de `portee` pour stocker les var. locales avec
  * la portée P_VARIABLE_LOCALE
  */
void entreeFonction(void){
  tabsymboles.base = tabsymboles.sommet;
  portee = P_ARGUMENT;
  adresseLocaleCourante = 0;
  adresseArgumentCourant = 0;
}

/*-------------------------------------------------------------------------*/
/** 
  * Fonction qui bascule table locale -> table globale. À appeler lors qu'on
  * a fini de parcourir une déclaration de fonction, après le bloc 
  * d'instructions qui constitue le corps de la fonction.
  * @param trace_tabsymb Booléen, si 0 n'affiche pas la tab. de symboles avant
  *                      de changer de contexte, si != 0 affiche tab. symboles
  */
void sortieFonction(int trace_tabsymb){
  if(trace_tabsymb) { 
    afficheTabsymboles();
  }
  tabsymboles.sommet = tabsymboles.base;
  tabsymboles.base = 0;
  portee = P_VARIABLE_GLOBALE;  
}

/*-------------------------------------------------------------------------*/

/**
  * Ajoute un nouvel identificateur à la table des symboles courante.
  * @param identif Nom du nouvel identificateur (variable ou fonction)
  * @param portee Constante parmi P_VARIABLE_GLOBALE, P_ARGUMENT ou 
  *               P_VARIABLE_LOCALE
  * @param type Constante parmi T_ENTIER, T_TABLEAU_ENTIER et T_FONCTION
  * @param adresse Nombre d'octets de décalage par rapport à la base de la zone
  *                mémoire des variables ($fp pour locales/arguments, .data pour 
  *                globales)
  * @param complement Nombre de paramètres d'une fonction OU nombre de cases 
  *                   d'un tableau. Indéfini (0) quand type=T_ENTIER.
  * @return Numéro de ligne de l'entrée qui vient d'être rajoutée
  */
int ajouteIdentificateur(char *identif, int portee, int type, 
                         int adresse, int complement)
{
  tabsymboles.tab[tabsymboles.sommet].identif = strdup(identif);
  tabsymboles.tab[tabsymboles.sommet].portee = portee;
  tabsymboles.tab[tabsymboles.sommet].type = type;
  tabsymboles.tab[tabsymboles.sommet].adresse = adresse;
  tabsymboles.tab[tabsymboles.sommet].complement = complement;
  tabsymboles.sommet++;

  if(tabsymboles.sommet == MAX_IDENTIF){
    erreur("Plus de place dans la table des symboles, augmenter MAX_IDENTIF\n");
  }
  return tabsymboles.sommet - 1;
}

/*-------------------------------------------------------------------------*/

/**
  * Recherche si un identificateur est présent dans la table LOCALE ou GLOBALE
  * Cette fonction doit être utilisée pour s'assurer que tout identificateur
  * utilisé a été déclaré auparavant. Elle doit être utilisée lors de 
  * l'UTILISATION d'un identificateur, soit dans un appel à fonction, une
  * partie gauche d'affectation ou une variable dans une expression. Si deux
  * identificateurs ont le même nom dans des portées différentes (un global et
  * un local), la fonction renvoie le plus proche, c-à-d le local.
  * @param identif Le nom de variable ou fonction que l'on cherche
  * @return Si oui, renvoie le numéro de ligne dans tabsymboles, si non -1  
  */
int rechercheExecutable(char *identif)
{
  int i;
  // Parcours dans l'ordre : var. locales, arguments, var. globales
  for(i = tabsymboles.sommet - 1; i >= 0; i--){
    if(!strcmp(identif, tabsymboles.tab[i].identif))
      return i;
  }
  return -1;
}

/*-------------------------------------------------------------------------*/

/**
  * Recherche si un identificateur est présent dans la table LOCALE
  * Cette fonction doit être utilisée pour s'assurer qu'il n'y a pas deux
  * identificateurs avec le même lors d'une DÉCLARATION d'un identificateur.
  * @param identif Le nom de variable ou fonction que l'on cherche
  * @return Si oui, renvoie le numéro de ligne dans tabsymboles, si non -1  
  */
int rechercheDeclarative(char *identif) {
  int i;
  for(i = tabsymboles.base; i < tabsymboles.sommet; i++){
    if(!strcmp(identif, tabsymboles.tab[i].identif))
      return i;
  }
  return -1; // Valeur absente
}

/*-------------------------------------------------------------------------*/

/**
  * Fonction auxiliaire qui permet d'afficher le contenu actuel de la table
  * des symboles. Pour obtenir un résultat identique aux fichiers de 
  * référence, appelez cette fonction juste avant sortieFonction(), une fois
  * que vous avez parcouru toutes les sous-parties d'une déclaration de 
  * fonction. Cet affichage doit être conditionné sur une variable booléenne
  * qui contrôle si on veut ou pas afficher la table des symboles en fonction
  * des options passées au compilateur.
  */
void afficheTabsymboles(void) {
  int i;
  printf("------------------------------------------\n");
  printf("base = %d\n", tabsymboles.base);
  printf("sommet = %d\n", tabsymboles.sommet);
  for(i=0; i < tabsymboles.sommet; i++){
    printf("%d ", i);
    printf("%s ", tabsymboles.tab[i].identif);
    
    if(tabsymboles.tab[i].portee == P_VARIABLE_GLOBALE)
      printf("GLOBALE ");
    else
    if(tabsymboles.tab[i].portee == P_VARIABLE_LOCALE)
      printf("LOCALE ");
    else
    if(tabsymboles.tab[i].portee == P_ARGUMENT)
      printf("ARGUMENT ");

    if(tabsymboles.tab[i].type == T_ENTIER)
      printf("ENTIER ");
    else if(tabsymboles.tab[i].type == T_TABLEAU_ENTIER)
      printf("TABLEAU ");
    else if(tabsymboles.tab[i].type == T_FONCTION)
      printf("FONCTION ");
    
    printf("%d ", tabsymboles.tab[i].adresse);
    printf("%d\n", tabsymboles.tab[i].complement);
  }
  printf("------------------------------------------\n");
}
