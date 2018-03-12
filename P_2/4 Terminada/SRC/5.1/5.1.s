.section .data			    # En esta zona se adjuntan los datos del proceso
	.macro linea		      # Ejemplos de numeros para comprobar
	# .int 1,1,1,1
	# .int 2,2,2,2
	# .int 1,2,3,4
	# .int -1,-1,-1,-1
	# .int 0xffffffff,0xffffffff,0xffffffff,0xffffffff
	# .int 0x08000000,0x08000000,0x08000000,0x08000000
	# .int 0x10000000,0x20000000,0x40000000,0x80000000
	.endm
lista:
	.irpc i,01234567 	     # Numero de ejecuciones, que en esta ocasión son 8
		linea
	.endr

longlista:	.int (.-lista)/4
resultado:	.quad 0x0123456701234567 #2x8 = 16
formato:
	.ascii "suma = %llu = %llx hex\n\0"

.section .text
_start:	.global _start     # Inicio el proceso en ensamblador
main:		.global main   # Inicio del main, donde usaremos printf

	mov  $lista, %ebx
	mov  longlista, %ecx
	call suma              # Llamamos a la subrutina "suma"
	mov  %eax, resultado
	mov  %edx, resultado+4 # Añado 4 bytes

	push resultado+4
	push resultado         # Meto el resultado en la pila
	push resultado+4
	push resultado
	push $formato          # Guarda en pila lo que está dentro de "formato"
	call printf            # Muestra todo lo que hay en la pila
	add  $20, %esp 	       # Esto es lo mismo que hacer 5 pop %ebx

 	mov  $1, %eax 	       # Instrucciones necesarias
	mov  $0, %ebx
	int  $0x80

suma:
	push %esi
	mov  $0, %eax
	mov  $0, %edx
	mov  $0, %esi
bucle:                   	   # Entramos en el bucle
	add  (%ebx,%esi,4), %eax
	adc  $0, %edx 			   # Sumamos el acarreo
	inc  %esi 			       # Incremententamos en 1
	cmp  %esi, %ecx 		   # Comparamos los registros
	jne  bucle 			       # salta si != bucle

	pop  %esi
	ret 				       # regresa al punto donde llamamos a "bucle"
