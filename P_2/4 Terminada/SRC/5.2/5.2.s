.section .data					# En esta zona se adjuntan los datos del proceso
	.macro linea				  # Ejemplos de numeros para comprobar
	 .int -1,-1,-1,-1
	# .int 1,-2,1,-2
	# .int 1,2,-3,-4
	# .int 0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF
	# .int 0x80000000,0x80000000,0x80000000,0x80000000
	# .int 0x04000000,0x04000000,0x04000000,0x04000000
	# .int 0x08000000,0x08000000,0x08000000,0x08000000
	# .int 0xFC000000,0xFC000000,0xFC000000,0xFC000000
	# .int 0xF8000000,0xF8000000,0xF8000000,0xF8000000
	# .int 0xF0000000,0xE0000000,0xE0000000,0xD0000000

	.endm

lista:
	.irpc i,01234567 		# Numero de ejecuciones, que en esta ocasión son 8
		linea
	.endr

longlista:	.int (.-lista)/4
resultado:	.quad 0x0123456701234567 #2x8 = 16
formato:
	.ascii "suma = %lld = %llx hex\n\0"

.section .text
_start:	.global _start  		# Inicio el proceso en ensamblador
main:		.global main  			# Inicio del main, donde usaremos printf

	mov  $lista, %ebx
	mov  longlista, %ecx
	call suma                 # Llamamos a la subrutina "suma"
	mov  %eax, resultado
	mov  %edx, resultado+4    # Añado 4 bytes

	push resultado+4
	push resultado            # Meto el resultado en la pila
	push resultado+4
	push resultado
	push $formato 					  # Guarda en pila lo que está dentro de "formato"
	call printf 					    # Muestra todo lo que hay en la pila
	add  $20, %esp 					  # Esto es lo mismo que hacer 5 pop %ebx

	mov  $1, %eax 					  # Instrucciones necesarias
	mov  $0, %ebx
	int  $0x80

suma:
	push %esi
	push %edi
	mov  $0, %eax
	mov  $0, %edx
	mov  $0, %esi
bucle:                     # Entramos en el bucle
	mov  (%ebx,%esi,4), %edi
	cmp  $0, %edi			       # Compara con 0
	jl   negativo			       # Hace un salto si es negativo
	add  %edi, %eax
	adc  $0, %edx 			     # Sumamos el acarreo

seguir:
	inc       %esi 			     # Incrementamos en 1
	cmp  %esi,%ecx 			     # Comparamos los registros
	jne  bucle 			         # Salta si != bucle

	pop  %esi
	pop  %edi
	ret 				             # Regresamos al punto en el que se llama a "seguir"
negativo:
	add  %edi, %eax
	adc  $0xffffffff, %edx
	jmp  seguir			         # Saltamos a "seguir"
