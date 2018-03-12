void crono(unsigned (*func)(), char* msg){
  unsigned result;
  struct timeval tv1, tv2;  // gettimeofday() secs-usecs
  long           tv_usecs;  // y sus cuentas

  gettimeofday(&tv1, NULL);
  result = func(lista, SIZE);
  gettimeofday(&tv2, NULL);

  tv_usecs = (tv2.tv_sec -tv1.tv_sec)*1E6+
  (tv2.tv_usec-tv1.tv_usec);
  printf("resultado = %d\t", result);
  printf("%s:%9ld us\n", msg, tv_usecs);
}
