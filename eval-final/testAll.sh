#!/bin/bash
##############################################################################
#
# Script d'évaluation automatique du compilateur.
# Compile le programme source et compare la sortie avec la référence.
# Pour le code machine, compare la SORTIE de l'exécution
# Vérifie aussi que MIPS et INTEL donnent le même résultat (optionnel)
# Comprend 
#   * analyse lexicale, 
#   * arbre abstrait, 
#   * code trois adresses,
#   * table des symboles et
#   * code machine (Intel).
#
# ATTENTION : pour que le script marche, votre compilateur doit returner 0 en
# cas de succès, autre valeur en cas d'erreur (programme ne compile pas 
# correctement) et doit permettre d'afficher les sorties sur stdout, sauf 
# d'éventuels messages d'erreur, sur stderr. On doit pouvoir, à tout moment,
# changer le type de sortie (lex, asynt, tab, 3a, nasm) avec des options.
#
##############################################################################
###     LLL      IIIIIII    RRRRRRR     EEEEEEEEE     ||||
###     LLL        III      RRR  RRR    EEEEEEEEE     ||||
###     LLL        III      RRR    RR   EEE           ||||
###     LLL        III      RRR  RRR    EEEEEEEE      ||||
###     LLL        III      RRRRRRR     EEEEEEEE    \\\\//// 
###     LLL        III      RRR RRR     EEE          \\\/// 
###     LLLLLLLL   III      RRR   RRR   EEEEEEEE      \\// 
###     LLLLLLLL IIIIIII    RRR   RRR   EEEEEEEEE      \/  
##############################################################################

# 1) MODIFIEZ LA VARIABLE CI-DESSOUS AVEC LE CHEMIN VERS VOTRE COMPILATEUR

MYCOMPILO="../compilo"

# 2) DÉCOMMENTEZ + MODIFIEZ LES COMMANDES POUR GÉNÉRER LES DIFFÉRENTES SORTIES

EXITONFAIL=0                          # mettre à un pour s'arrêter à la première erreur
VERBOSE=1                             # mettre =0 pour cacher les détails
declare -A MYCOMPILOV                 # déclare un tableau (ne pas toucher)
MYCOMPILOV["lex"]="${MYCOMPILO} -l"   # exécuter l'analyseur lexical
MYCOMPILOV["asynt"]="${MYCOMPILO} -a" # afficher l'arbre abstrait
MYCOMPILOV["tab"]="${MYCOMPILO} -t"   # afficher les tables des symboles
MYCOMPILOV["3a"]="${MYCOMPILO} -3"    # générer code trois adresses
MYCOMPILOV["nasm"]="${MYCOMPILO} -n"  # générer code Intel
NASM="nasm"                           # Commande nasm -- changez la version si besoin
NASMOPTS="-f elf -g -F dwarf"         # Options à passer à nasm -- notamment architecture cible
LD="ld"                               # Commande ld -- changez la version si besoin
LDOPTS="-m elf_i386"                  # Options à passer à ld -- notamment architecture cible
##############################################################################
# NE PLUS LIRE À PARTIR D'ICI ;-)
##############################################################################

MYCOMPILOV["default"]="${MYCOMPILOV[nasm]}" # utilisé pour test erreur

declare -A testname
testname["lex"]="Analyse lexicale"
testname["asynt"]="Arbre abstrait"
testname["tab"]="Table des symboles"
testname["3a"]="Code trois adresses"
testname["nasm"]="Code machine Intel"
testname["default"]="Programmes contenant des erreurs"


##############################################################################
function echoverbose() {
  if [ $VERBOSE -eq 1 ]; then
    echo -e $@
  fi
}

##############################################################################
# Fonction pour faire le diff textuel régulier (lex et tab).
# Cette fonction peut être passée en paramètre à la fonction diff_prog.
# @param $1 Fichier 1
# @param $2 Fichier 2
# @return 0 si pas de différence, autre sinon

function REGDIFF() {
  diff -q -w $1 $2
}

##############################################################################
# Fonction pour faire le diff entre deux fichiers XML (asyn).
# Cette fonction peut être passée en paramètre à la fonction diff_prog.
# @param $1 Fichier XML 1
# @param $2 Fichier XML 2
# @return 0 si pas de différence, autre sinon

function XMLDIFF() {
  ./compare_arbres_xml $1 $2
}

##############################################################################
# Fonction qui exécute un programme nasm passé en paramètre
# La sortie est affichée sur stdout
# @param $1 Fichier nasm à exécuter
# @param $2 Entrée à donner à l'exécution du programme (optionnel)

function nasm_exec(){
  nasmfile="$1"
  input="$2"
  ${NASM} ${NASMOPTS} "${nasmfile}" -o "${nasmfile}.o"
  ${LD} ${LDOPTS} -o "${nasmfile}.exe" "${nasmfile}.o"
  echo -e "${input}" | 
  ./"${nasmfile}".exe |
  sed ':a;N;$!ba;s/[ \n]/_/g' # Homogenize whitespace
  # Comment the line below out if you want to keep .o and executable
  rm "${nasmfile}.o" "${nasmfile}.exe" 
}

##############################################################################
# Fonction pour faire le diff entre deux sorties d'exécution nasm (Intel).
# Cette fonction peut être passée en paramètre à la fonction diff_prog.
# @param $1 Fichier nasm 1
# @param $2 Fichier nasm 2
# @param $3 Entrée à donner à l'exécution du programme (optionnel)
# @return 0 si pas de différence, autre sinon

function NASMOUTDIFF() {
  diff <(nasm_exec "$1" "$3") <(nasm_exec "$2" "$3")
}

##############################################################################
# Fonction pour faire le diff entre deux fichiers, vérifier et afficher le 
# résultat avec de belles couleurs.
# @param $1 Nom de la fonction utilisée pour faire le diff (parmi 3 ci-dessus)
# @param $2 Nom du fichier d'entrée à tester sans extension (p. ex. affect)
# @param $3 Extension du fichier à tester (p. ex. synt)
# @param $4 Entrée à donner à l'exécution du programme (optionnel)

function diff_prog() {
  diffprog=$1
  input=$2
  suffix=$3
  values=$4
  (( nombretotal[$suffix]++ ))
  refoutput="ref-${suffix}/${input}.${suffix}"
  sysoutput="output/${input}.${suffix}"
  echoverbose "\033[35m > ${testname[${suffix}]} (.${suffix})\033[0m"
  echoverbose "`wc -l output/${input}.${suffix} | awk '{print $1}'` lignes"
  if [ -f ref-${suffix}/$input.${suffix} ]; then
    ${diffprog} output/${input}.${suffix} ref-${suffix}/${input}.${suffix} ${values} 2>&1 2> /dev/null # TODO: UNCOMMENT
    if [ ! $? -eq 0 ]; then 
      echoverbose "\033[31mTEST ${testname[${suffix}]} ÉCHOUÉ\033[0m"
      echoverbose "Différences entre output/${input}.${suffix} ref-${suffix}/${input}.${suffix} en utilisant $diffprog:"
      if [ $VERBOSE -eq 1 ]; then
        ${diffprog} output/${input}.${suffix} ref-${suffix}/${input}.${suffix} ${values} 
      fi
      if [ $EXITONFAIL -eq 1 ]; then exit 1; fi
    else
      echoverbose "\033[32mTEST ${testname[${suffix}]} OK\033[0m"
      (( nombreok[$suffix]++ ))
    fi
  else
    echoverbose "\033[34mRéférence ref-${suffix}/${input}.${suffix} absente\033[0m"
  fi
}


##############################################################################
# Exécute tous les tests (reco, lex, asynt, tab, 3a, nasm) pour un exemple
# donné qui doit être correctement compilé à tous les niveaux.
# @param $1 Nom du fichier d'entrée à tester sans extension (p. ex. affect)
# @param $2 Entrée à donner à l'exécution du programme (optionnel)

function test_fichier_ok() {
    input=$1
    echoverbose "\n\033[4m ---- Test input/$input.l ----\033[0m"
    if [ -n "$2" ]; then
      echoverbose "Input : $2"
    fi
    if [ -f input/$input.l ]; then
      # Reconnaissance : programme correct doit être accepté
      if [ -n "${MYCOMPILODEFAULT}" ]; then
        echoverbose "\033[35m > Reconnaissance (accepte l'entrée)\033[0m"        
        ${MYCOMPILODEFAULT} input/$input.l > output/$input.synt
        if [ $? != 0 ]; then 
          echoverbose "\033[31mTEST Reconnaissance ÉCHOUÉ\033[0m"
          echoverbose "Le programme $input.l n'a pas été compilé correctement"	
          if [ $EXITONFAIL = 1 ]; then exit 1; fi
        else
          echoverbose "\033[32mTEST Reconnaissance OK\033[0m"
        fi
      fi     
      # Teste analyseur lexical
      if [ -n "${MYCOMPILOV[lex]}" ]; then
        if [ $VERBOSE -eq 1 ]; then
          ${MYCOMPILOV["lex"]} input/$input.l > output/$input.lex
        else
          ${MYCOMPILOV["lex"]} input/$input.l > output/$input.lex 2> /dev/null
        fi        
        diff_prog REGDIFF $input lex
      fi 
      # Teste création de l'arbre abstrait
      if [ -n "${MYCOMPILOV[asynt]}" ]; then
        if [ $VERBOSE -eq 1 ]; then
          ${MYCOMPILOV["asynt"]} input/$input.l > output/$input.asynt 
        else
          ${MYCOMPILOV["asynt"]} input/$input.l > output/$input.asynt   2> /dev/null
        fi
        diff_prog XMLDIFF $input asynt
      fi  
      
      # Teste replissage de la table des symboles
      if [ -n "${MYCOMPILOV[tab]}" ]; then
        if [ $VERBOSE -eq 1 ]; then
          ${MYCOMPILOV["tab"]} input/$input.l > output/$input.tab
        else
          ${MYCOMPILOV["tab"]} input/$input.l > output/$input.tab 2> /dev/null
        fi        
        diff_prog REGDIFF $input tab
      fi 
      # Teste code trois adresses
      if [ -n "${MYCOMPILOV[3a]}" ]; then
        if [ $VERBOSE -eq 1 ]; then
          ${MYCOMPILOV["3a"]} input/$input.l > output/$input.3a 
        else
          ${MYCOMPILOV["3a"]} input/$input.l > output/$input.3a   2> /dev/null
        fi
        diff_prog REGDIFF $input 3a
      fi        
      # Teste génération de code Intel      
      if [ -n "${MYCOMPILOV[nasm]}" ]; then
        if [ $VERBOSE -eq 1 ]; then
          ${MYCOMPILOV["nasm"]} input/$input.l > output/$input.nasm 
        else
          ${MYCOMPILOV["nasm"]} input/$input.l > output/$input.nasm   2> /dev/null
        fi
        diff_prog NASMOUTDIFF $input nasm "$2"
      fi
      # Sanity check: MIPS et Intel génèrent la même sortie      
    else
      echo -e "\033[31minput/$input.l non trouvé\033[0m"
      echo -e "\033[31mTest impossible\033[0m"     
    fi
}

##############################################################################
# Vérifie qu'un programme contenant des erreurs n'est pas reconnu/compilé.
# @param $1 Nom du fichier d'entrée à tester sans extension (p. ex. affect)

function test_fichier_fail() {
    input=$1
    (( nombretotal["default"]++ ))          
    echoverbose "\n\033[4m ---- Test input/$input.l ----\033[0m"
    if [ $VERBOSE -eq 1 ]; then
      ${MYCOMPILOV["default"]} input/$input.l > /dev/null
      res=$?
    else
      ${MYCOMPILOV["default"]} input/$input.l > /dev/null 2> /dev/null
      res=$?
    fi
    if [ $res -eq 0 ]; then 
    echoverbose "\033[31mTEST REJET ÉCHOUÉ\033[0m"
      echoverbose "Le programme $input.l est accepté mais il devrait être rejeté"
      if [ $EXITONFAIL -eq 1 ]; then exit 1; fi
    else
      echoverbose "\033[32mTEST REJET OK\033[0m"
      (( nombreok["default"]++ ))      
    fi
}

##############################################################################
# Initialise les compteurs des cas de test

declare -A nombreok
declare -A nombretotal

function init_compteurs() {
  for a in lex asynt tab 3a nasm default; do
    nombreok[$a]=0
    nombretotal[$a]=0
  done
}

##############################################################################
# Affiche les compteurs des cas de test

function affiche_compteurs() {
  echo -e "\n\n\n\n=====================================================================\n"
  echo "Résultat des tests automatiques (réussi/total)"
  for a in lex asynt tab 3a nasm default; do
    if [ -n "${MYCOMPILOV[$a]}" ]; then
      if [ ${nombreok[${a}]} -ne ${nombretotal[${a}]} ]; then
        color="\033[31m"
      else
        color="\033[32m"
      fi
      echo -e "${testname[${a}]}: $color ${nombreok[${a}]}/${nombretotal[${a}]}\033[0m"
    fi
  done
  echo -e "\n=====================================================================\n\n\n\n"
}

##############################################################################
#######  #####  ######   ######    ###  #####  ###############################
#######   ###   #####  #  ######  ####    ###  ###############################
#######    #    ####  ###  #####  ####  ##  #  ###############################
#######  #   #  ###         ####  ####  ###    ###############################
#######  ## ##  ##  #######  ##    ###  ####   ###############################
##############################################################################

make clean
make
mkdir -p output

################################################################################
# Vérifications initiales : MYCOMPILO et MARS bien configurés?

echo -e "Votre compilateur : ${MYCOMPILO}"
if [ ! -f  ${MYCOMPILO} ]; then
  echo -e "\033[31mCompilateur introuvable"
  echo -e "Modifiez la variable MYCOMPILO avant de lancer l'éval\033[0m"
  exit 1
fi
if [ -n "${MYCOMPILONASM}" ]; then
  [ -n "${NASM}" ] && ${NASM} -v > /dev/null
  if [ ! $? -eq 0 ]; then
    echo -e "\033[31mNASM introuvable"
    echo -e "Modifiez la variable NASM avant de lancer l'éval\033[0m"
    exit 1
  fi
  if [ ! -f "io.asm" ]; then
    echo -e "\033[31mFichier io.asm introuvable"
    echo -e "Copiez-le dans le répertoire courant\033[0m"
    exit 1
  fi
fi

################################################################################

echo -e "\033[1m\n>> Tests connus\033[0m"
init_compteurs
# fichiers connus OK
test_fichier_ok affect
test_fichier_ok boucle
test_fichier_ok expression "5\n2\n"
test_fichier_ok max "3\n10\n"
test_fichier_ok max "10\n10\n"
test_fichier_ok max "-3\n-10\n"
test_fichier_ok tri
#fichiers connus FAIL
test_fichier_fail affect-err
affiche_compteurs

################################################################################

echo -e "\033[1m\n>>Tests nouveaux \033[0m"
init_compteurs
# fichiers nouveaux OK
test_fichier_ok associativite
test_fichier_ok procedure_arg
test_fichier_ok procedure_retour
test_fichier_ok procedure_varloc
test_fichier_ok si
test_fichier_ok sinon "1"
test_fichier_ok tableau
test_fichier_ok tantque
#fichiers nouveaux FAIL
test_fichier_fail lex-err     # fails at lex
test_fichier_fail synt-err    # fails at synt
test_fichier_fail semantique1 # fails at tab
test_fichier_fail semantique2 # fails at tab
test_fichier_fail semantique3 # fails at tab
test_fichier_fail semantique4 # fails at tab 
test_fichier_fail semantique5 # fails at tab 
test_fichier_fail types       # fails at tab
test_fichier_fail vide        # fails at tab
affiche_compteurs

################################################################################

echo -e "\033[1m\n>> Tests nouvelle fonctionnalité\033[0m"
init_compteurs
#fichiers nouvelle fonc. OK
test_fichier_ok pour
test_fichier_ok pourien
test_fichier_ok pourtab
#fichiers nouvelle fonc. FAIL
test_fichier_fail pourri
affiche_compteurs
