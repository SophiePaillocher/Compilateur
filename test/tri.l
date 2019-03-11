entier $tab[ 10 ];

initialiser()
{
    $tab[0] = 8;    $tab[1] = 6;    $tab[2] = 9;
    $tab[3] = 9;    $tab[4] = 4;    $tab[5] = 2;
    $tab[6] = 3;    $tab[7] = 1;    $tab[8] = 4;
    $tab[9] = 5;    
}

afficher( entier $n )
entier $i;
{
    $i = 0;
    tantque $i < $n faire {
        ecrire( $tab[ $i ] );
        $i = $i + 1;
    }    
    ecrire( 0 ); # marqueur fin de tableau         
}

echanger( entier $i, entier $j )
entier $temp;
{
    $temp = $tab[ $j ];
    $tab[ $j ] = $tab[ $i ];
    $tab[ $i ] = $temp;
}

trier( entier $n )
entier $echange, entier $j, entier $m;
{
    $m = $n;
    $echange = 1;
    tantque $echange = 1 faire
    {
        $echange = 0;
        $j = 0;
        tantque $j < $m - 1 faire 
        {
            si $tab[ $j + 1 ] < $tab[ $j ] alors {
                echanger( $j, $j + 1 );
                $echange = 1;
            }
            $j = $j + 1;
        }
        $m = $m - 1;        
    }    
}

main()
{
    initialiser();
    afficher( 10 );
    trier( 10 );
    afficher( 10 );
}
