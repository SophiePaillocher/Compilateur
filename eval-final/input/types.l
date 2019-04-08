entier $a[10], entier $b;
main() {
  $a[0] = 10;
  $b = 5;
  ecrire( $a );      # erreur de type ou ecrit 10?
  ecrire( $a + 1 );  # erreur de type ou ecrit 11?
  ecrire( $b[0] );   # erreur de type ou ecrit 5?
  ecrire( $a[10] );  # erreur de type ou ecrit 5?
  retour 0;
  ecrire( $b );      # ecrit 5 ou pas?
}
