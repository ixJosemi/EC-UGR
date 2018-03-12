.section .data
saludo: 
	.ascii "Hola a todos! \n Hello, World!\n" # \n Salto de linea

longsaludo:
	.int .-saludo # . = contador posic. Aritmetica de etiquetas

# SECCIÓN DE CÓDIGO

.section .text # CAMBIAMOS DE SECCIÓN, EMITIMOS CÓDIGO
.global _start # MUESTRA PUNTO DE ENTRADA A LD (COMO MAIN EN C)

_start: # PUNTO DE ENTRADA ASM (COMO MAIN EN C)

	mov $4, %eax # write: servicio 4 kernel linux
	mov $1, %ebx #    fd: descriptor de fichero para stdout
	mov     $saludo, %ecx #   buf: dirección del texto a escribir
	mov  longsaludo, %edx # count: numero de bytes a escribir
	int $0x80 # llamar write(stdout, &saludo, longsaludo);

	# LLAMADA AL SISTEMA WRITE
	mov $1, %eax #   exit: servicio 1 kernel linux
	mov $0, %ebx # status: código a retornar (0 = 0k)
	int $0x80    # llamar exit(0);
