entier v[20], entier i;
main(){  
  v[0]=5;
  pour i=0; i<20; i = i+1; faire {
   v[i] = v[i-1] + 5;
  }
  pour i=0; i<20; i = i+1; faire {
   ecrire(v[i]);
  }
}
