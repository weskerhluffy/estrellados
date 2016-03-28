rm -fv estreshados
#gcc -v -g3 -fno-strict-aliasing -DONLINE_JUDGES -DCACA_COMUN_LOG -lm  -pedantic src/estreshados.c -o estreshados -m32
#gcc -v -g3 -fno-strict-aliasing -DONLINE_JUDGES -DCACA_COMUN_LOG -lm  -pedantic src/estreshados.c -o estreshados -m32 -DESTRESHADOS_CONTEO_PROBADO
#gcc -v -g3 -fno-strict-aliasing -DONLINE_JUDGE -lm  -pedantic src/estreshados.c -o estreshados -m32 -DESTRESHADOS_CONTEO_PROBADO
gcc -v -g3 -fno-strict-aliasing -DONLINE_JUDGE -lm  -pedantic src/estreshados.c -o estreshados -m32
