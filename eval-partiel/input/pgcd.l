entier $a, entier $b;

pgcd( entier $a, entier $b )
entier $c;
{
  $c = $a - ( $a / $b ) * $b;
  si $c = 0 alors{ retour $b; }
  sinon{ retour pgcd( $b, $c ); }
}

main() {
  $a = lire();
  $b = lire();
  si $b < $a alors{ ecrire( pgcd( $a, $b ) ); }
  sinon{ ecrire( pgcd( $b, $a ) ); }
}
