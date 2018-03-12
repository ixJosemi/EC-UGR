#include <stdio.h>	    // para printf()
#include <stdlib.h>	    // para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<16)    // tamaño suficiente para tiempo apreciable
// unsigned lista[SIZE]={0x80000000, 0x00100000, 0x00000800, 0x00000001};
// unsigned lista[SIZE]={0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe, 0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
// unsigned lista[SIZE]={0x0	   , 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff 0x12345678, 0x9abcdef0, 0xcafebeef};

unsigned lista[SIZE];
#define WSIZE (8*sizeof(int))

// Primera version de parity, donde recorremos el array con un bucle for,
// y posteriormente recorremos los bits con un segundo bucle for
unsigned parity_1(unsigned *array, int len){

    unsigned i;
    int j;
    unsigned result = 0;
    unsigned val = 0;
    unsigned x;

    for(i = 0; i < len; i++){            // Recorremos el vector
        x = array[i];                    // Seleccionamos un numero

        for(j = 0; j < WSIZE; j++){      // Recorremos los bits
            unsigned mascara = 0x1 << j; // Aplicamos la mascara y el desplazamiento
            val ^= (x & mascara) != 0;   // Acumulamos en val los bits con XOR
        }

        result += val;                   // Acumulamos en result lo que tenemos en val
        val = 0;                         // Ponemos val a 0
    }
    return result;
}

// Segunda version de parity, donde recorremos el array con un bucle for,
// y tras ello recorreremos los bits mediante un ciclo while
unsigned parity_2(unsigned *array, int len){

    int i;
    unsigned result = 0;
    unsigned val = 0;
    unsigned x;

    for(i = 0; i < len; i++){  // Recorremos el vector
        x = array[i];          // Seleccionamos un numero del vector

        while(x >>= 1){        // Recorremos los bits mediante el ciclo while
            val ^= x&0x1;      // Acumulamos dichos bits en la variable val con XOR
                               // Hemos utilizado aqui la máscara 0x1 y el desplazamiento
        }

        result += val;         // Acumulamos en result lo que tenemos en val
        val = 0;               // Ponemos val a 0
    }
    return result;
}

// Tercera version del parity, implementada a partir del ejercicio 3.22 del libro
// de clase, adaptada para un array completo. En esta version acumulamos la mascara
// en el resultado final
unsigned parity_3(unsigned *array, int len){

    int i;
    unsigned result = 0;
    unsigned val = 0;
    unsigned x;

    for(i = 0; i < len; i++){   // Recorremos el vector
        x = array[i];           // Seleccionamos un numero del vector

        while(x){
            val ^= x;           // Aplicamos XOR
            x >>= 0x1;          // Aplicamos el desplazamiento y la máscara
        }

        result += (val & 0x1);  // Acumulamos en result lo anterior
        val = 0;                // Ponemos val a 0
    }
    return result;
}

// Cuarta version del parity, donde sustituimos el ciclo while utilizado anteriormente
// implementando en su lugar varias instrucciones de ensamblador
unsigned parity_4(unsigned *array, int len){

    int i;
    unsigned result = 0;
    unsigned val = 0;
    unsigned x;

    for(i = 0; i < len; i++){       // Recorremos el vector
        x = array[i];               // Escogemos un numero del vector
        val = 0;                    // Ponemos la variable val a 0

        asm("\n"
        "ini3:			   \n\t"	// Seguir mientras que x!=0
        "xor  %[x], %[val] \n\t"    // Realmente solo nos interesa LSB
        "shr  %[x]		   \n\t"    // LSB en CF
        "test %[x], %[x]   \n\t"    // Hacemos una comparacion si x != x
        "jnz  ini3		   \n\t"    // Saltamos a ini3 si quedan bits a 1
        "and  $1,   %[val] \n\t"
        : [val] "+r" (val)			// E/S: entrada 0, salida paridad elemento
        : [x] "r" (x)				// Entrada: valor elemento
        );

        result += val;              // Acumulamos en result lo que teniamos en val
    }
    return result;
}

// Quinta version del parity donde recurrimos a la suma en arbol, al igual
// que se hizo en el ejercicio pop_count4, utilizando XOR en el array y haciendo
// desplazamientos sucesivos a mitad de distancia hasta finalizar en x ^= x >> 1
unsigned parity_5(unsigned *array, int len){

    int i, j;
    unsigned x;
    unsigned result = 0;

    for (i = 0; i < len; i++){           // Recorremos el vector
        x = array[i];                    // Seleccionamos un numero del vector

        for (j = 16; j >= 1; j = j / 2)
            x ^= x >> j;                 // Aplicamos XOR y desplazamiento

        result += (x&0x1);               // Acumulamos en result lo anterior
    }
    return result;
}

// Sexta version del parity, sustituyendo de nuevo, el ciclo for interno
// por instrucciones en ensamblador, incluyendo XOR y SHR, ademas de SETcc y
// MOVZx
unsigned parity_6(unsigned *array, int len){

    int i;
    unsigned result = 0;
    unsigned x;

    for (i = 0; i < len; i++){
        x = array[i];

        asm("\n"
        "mov   %[x], %%edx  \n\t" 	//sacar copia para XOR. Controlar el registro edx
        "shr   $16,  %[x]   \n\t"
        "xor   %[x], %%edx  \n\t"   // Aplicamos XOR
        "xor   %%dh, %%dl	\n\t"   // Aplicamos XOR
        "setpo %%dl 		\n\t"
        "movzx %%dl, %[x]	\n\t"   // devolvemos en 32b
        : [x]"+r" (x)			  	//e/s: entrada valor elemento, salida paridad
        :
        :"edx"						//clobber
        );

        result += x;
    }
    return result;
}

// Funcion cronometro para calcular los tiempos
void cronometro(unsigned (*func)(), char* msg){

	unsigned resultado;
    struct timeval tv1,tv2;		// gettimeofday() secs-usecs
    long tv_usecs;			    // y sus cuentas

    gettimeofday(&tv1, NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2, NULL);

    tv_usecs = (tv2.tv_sec - tv1.tv_sec )*1E6 + (tv2.tv_usec - tv1.tv_usec);

	 	printf("resultado = %d\t", resultado);
	 	printf("%s:%9ld us\n", msg, tv_usecs);
}

int main(){

    int i;

    for(i = 0; i < SIZE; i++)
        lista[i] = i;

    cronometro(parity_1, "parity1, con doble ciclo for");
    cronometro(parity_2, "parity2, con ciclo for y while");
    cronometro(parity_3, "parity3, con ciclo for y while (libro)");
    cronometro(parity_4, "parity4, con ciclo for y asm");
    cronometro(parity_5, "parity5, con doble ciclo for y suma en arbol");
    cronometro(parity_6, "parity6, con suma en arbol en asm");

    printf("N * (N + 1) / 2 = %d\n", (SIZE-1)*(SIZE/2));  /*OF*/

    exit(0);
}
