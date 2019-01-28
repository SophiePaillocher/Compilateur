#ifndef __UTIL__
#define __UTIL__

char *duplique_chaine(char *s);
void warning(char *message);
void erreur(char *message);
void warning_1s(char *message, char *s);
void erreur_1s(char *message, char *s);
void affiche_balise_ouvrante(const char *fct_, int trace_xml);
void affiche_balise_fermante(const char *fct_, int trace_xml);
void affiche_element(char *fct_, char *texte_, int trace_xml);
void affiche_feuille(int uc, int trace_xml);
void affiche_xml_texte(char *texte_, int trace_xml);
void consommer( int c, int *uc, int trace_xml );
void test_yylex_internal(FILE *yyin);

#endif
