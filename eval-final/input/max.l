max( entier $a, entier $b ) 
{
    si $a < $b alors {
        retour $b;
    }
    retour $a;
}

main()
entier $v_1, entier $v_2;
{
    $v_1 = lire();
    $v_2 = lire();
    si max( $v_1, $v_2 ) = $v_1 alors
    {
        ecrire( $v_1 );
    }
    sinon
    {
        ecrire( $v_2 );
    }

}
