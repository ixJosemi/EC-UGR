#include <stdio.h>		// para printf()
#include <stdlib.h>		// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<16) 	// tamaño suficiente para tiempo apreciable
// unsigned lista[SIZE]={0x80000000, 0x00100000, 0x00000800, 0x00000001};
// unsigned lista[SIZE]={0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe, 0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
// unsigned lista[SIZE]={0x0	   , 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff 0x12345678, 0x9abcdef0, 0xcafebeef};

unsigned lista[SIZE];
#define WSIZE (8*sizeof(int))

// Primera version del pop_count, donde recorremos el array con un bucle for
// y recorremos los bits con otro bucle for
unsigned pop_count1(int* array, int len){

	int i;
	int j;
	unsigned result = 0;
	unsigned x;

	for(i = 0; i < len; i++) {	      		    // Para todos los números del vector:
		x = array[i];					      	// Extraemos un número

		for(j = 0; j < WSIZE; j++){    	    	// Recorremos todos los bits del número.
			unsigned mascara = 1 << j; 			// Extraemos los bits con una máscara 0x1
			result += (x & mascara) != 0; 		// Vamos acumulando el resultado
		}
	}
	return result;
}

// Segunda version del pop_count, donde recorremos el array con un bucle for
// y recorremos los bits con un bucle while
unsigned pop_count2(int* array, int len){

	int i;
	unsigned x;
	unsigned result = 0;

	for(i = 0; i < len; i++) {    // Recorremos el vector
		x = array[i];			  // Extraemos un número

		while (x) {			  // Recorremos todos los bits de dicho número
			result += x&0x1; 	  // Extraemos los bits con una máscara 0x1 y
								  // los acumulamos en el contador "resultado"
			x >>= 1;			  // Realizamos el desplazamiento de bits a la dcha
		}
	}
	return result;
}

// Tercera version del pop_count, donde recorremos el array con un bucle for
// y para recorrer los bits no utilizaremos un ciclo while, sino recurriremos
// al ensamblador, aplicando la orden adc
unsigned pop_count3(unsigned *array, int len){

	int i;
  	unsigned result = 0;
  	unsigned x;

  	for(i = 0; i < len; i++){
   		x = array[i];
    	asm( 				   "\n"
		"ini3:				  \n\t"	   	// Seguiremos mientras num != 0
		"shr  %[x]			  \n\t"	   	// LSB en CF
      	"adc  $0, 	%[result] \n\t"	   	// Acumulamos el acarreo
		"test %[x], %[x]	  \n\t"	   	// Comprobamos si num != 0
		"jnz  ini3			  \n\t"	   	// Hacemos el salto si quedan bits a 1
      	: [result]"+r" (result)			// E/S: añadir lo acumulado por el momento
      	: [x] "r" (x)  			   		// entrada: valor elemento
		);
    }
  	return result;
}

// Cuarta versión del pop_count, donde recurrimos a un bucle for anidado,
// y realizamos una suma en arbol
unsigned pop_count4(unsigned *array, int len){

	int i;
	int j;
	unsigned val = 0;
  	unsigned result = 0;
  	unsigned x;

  	for(i = 0; i < len; i++){     // Recorremos el vector
	  	x = array[i];			  // Escogemos un numero del vector

    	for(j = 0; j < 8; j++){	  // Recorremos los bytes
      	val += x & 0x01010101;	  // Acumulamos los bits de cada byte recorrido
      	x >> 1;					  // Realizamos el desplazamiento a la dcha
    	}

    	val += (val >> 16);		  // Volvemos a acumular en "valor" mientras desplazamos
    	val += (val >> 8);		  // Volvemos a acumular en "valor" mientras desplazamos

    	result += (val & 0xFF);   // Acumulamos todo en "resultado"
    	val = 0; 			 	  // Volvemos a poner a cero la variable valor
								  // y volvemos a empezar el bucle
   }
	return result;
}

// Quinta versión del pop_count, implementando la instruccion SSS3
unsigned pop_count5 (unsigned *array, int len){

	int i;
  	unsigned val;
	unsigned result = 0;
  	int SSE_mascara[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
  	int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};

  	if (len & 0x3)
   	printf("leyendo 128b pero len no multiplo de 4?\n");

    	for (i = 0; i < len; i += 4){
      	asm(
			"movdqu 	%[x], 	%%xmm0 	\n\t"
        	"movdqa		%%xmm0, %%xmm1	\n\t"	// Creamos dos copias de x
        	"movdqu 	%[m], 	%%xmm6	\n\t"	// Creamos la máscara
        	"psrlw		$4, 	%%xmm1 	\n\t"
        	"pand 		%%xmm6, %%xmm0	\n\t"	// xmm0 - nibbles inferiores
        	"pand		%%xmm6, %%xmm1	\n\t"	// xmm1 - nibbles superiores

        	"movdqu		%[l],	%%xmm2	\n\t"	// ...como pshufb sobrescribe LUT
        	"movdqa		%%xmm2, %%xmm3	\n\t"	// ...queremos 2 copias
        	"pshufb		%%xmm0,	%%xmm2	\n\t"	// xmm2 = vector popcount inferiores
        	"pshufb		%%xmm1,	%%xmm3	\n\t"	// xmm3 = vector popcount superiores

        	"paddb		%%xmm2, %%xmm3	\n\t"	// xmm2 += xmm3 - vector popcount bytes
        	"pxor		%%xmm0, %%xmm0	\n\t"	// xmm0 = 0,0,0,0
        	"psadbw		%%xmm0, %%xmm3	\n\t"	// xmm3 = [pcnt bytes0..7|pcnt bytes8..15]
        	"movhlps	%%xmm3,	%%xmm0	\n\t"	// xmm0 = [	0 		  |pcnt bytes0..7]
        	"paddd		%%xmm3,	%%xmm0	\n\t"	// xmm0 = [	no usado  |pcnt bytes0..15]
        	"movd 		%%xmm0, %[val]\n\t"
        	: [val]"=r" (val)
        	: [x]  "m"	(array[i]),
        	[m]  "m"	(SSE_mascara[0]),
        	[l]  "m"	(SSE_LUTb[0])
        	);
        	result += val;
      }
      return result;
}

// Funcion cronometro para calcular los tiempos
void cronometro(unsigned (*func)(), char* msg){

    struct timeval tv1,tv2;		// gettimeofday() secs-usecs
    long tv_usecs;			    // y sus cuentas

    gettimeofday(&tv1, NULL);
    unsigned resultado = func(lista, SIZE);
    gettimeofday(&tv2, NULL);

    tv_usecs = (tv2.tv_sec - tv1.tv_sec )*1E6 + (tv2.tv_usec - tv1.tv_usec);

	 	printf("resultado = %d\t", resultado);
	 	printf("%s:%9ld us\n", msg, tv_usecs);
}

int main(){

	int i;

	for (int i = 0; i < SIZE; i++)
		lista[i] = i;

	cronometro(pop_count1, "pop_count1, con doble ciclo for");
	cronometro(pop_count2, "pop_count2, con ciclo for y while");
	cronometro(pop_count3, "pop_count3, con ciclo for y asm");
	cronometro(pop_count4, "pop_count4, ejercicio del libro, suma en arbol");
	cronometro(pop_count5, "pop_count5, aplicando la instruccion SSS3");

   exit(0);
}
