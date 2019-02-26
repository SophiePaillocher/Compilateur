add (entier $a, entier $b)
{
 retour $a + $b;
}

mult (entier $a, entier $b)
{
 retour $a * $b;
}

main()
entier $x, entier $y, entier $op;
{
 $x = lire();
 $y = lire();
 $op = lire();
 si $op = 1  alors {
  ecrire(add($x, $y));
 }
 sinon {
   ecrire(mult($x, $y));
 }
}

