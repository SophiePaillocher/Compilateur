fact( entier $f ) 
{
  si $f = 1 | $f = 0 alors {
    retour 1;
  }
  sinon {
    retour fact( $f - 1 ) * $f ;
  }
}

main() 
entier $a;
{
  $a = lire();
  ecrire( fact( $a ) );
}
