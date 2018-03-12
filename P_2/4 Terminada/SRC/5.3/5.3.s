.section .data						# En esta zona se adjuntan los datos del proceso
	.macro linea					  # Ejemplos de numeros para comprobar

	# .int 1,2,-3,-4
	# .int 0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF
	# .int 0xF0000000,0xE0000000,0xE0000000,0xD0000000
	# .int -1,-1,-1,-1
	# .int 0,-1,-1,-1
	# .int 0,-2,-1,-1
	# .int 0x10000000,0x20000000,0x40000000,0x80000000
	# .int 0xffffffff,0xffffffff,0xffffffff,0xffffffff
	# .int 0x08000000,0x08000000,0x08000000,0x08000000
	# .int 1,1,1,1

	.endm

lista:
	.irpc i,01234567 		    # Numero de ejecuciones, que en esta ocasión son 8
		linea
	.endr

longlista:	.int (.-lista)/4
media:		  .int 0x01234567
resto:  	  .int 0x12345678
formato:
	.ascii "media = %d, resto = %d\n\0"

.section .text
#_start:	.global _start  # Inicio el proceso en ensamblador
main:		.global main  	  # Inicio del main, donde usaremos printf

	mov  $lista, %ebx
	call suma		            # Llamamos a la subrutina "suma"
	mov  longlista, %ecx
	mov  %eax, media
	mov  %edx, resto


	push resto		          # Meto "resto" en la pila
	push media
	push $formato 		      # Guarda en pila lo que está dentro de "formato"
	call printf 		        # Muestra todo lo que hay en la pila
	add  $12, %esp	        # Esto es lo mismo que hacer 3 pop %ebx

	mov  $1, %eax 		      # Instrucciones necesarias
	mov  $0, %ebx
	int  $0x80

suma:
	push %esi
	push %edi
	mov  $0, %eax
	mov  $0, %edx
	mov  $0, %esi
bucle:                    # Entramos en el bucle
	mov  (%ebx,%esi,4), %edi
	cmp  $0, %edi			      # Comparamos con 0
	jl   negativo			      # Hace un salto si es negativo
	add  %edi, %eax
	adc  $0, %edx 			    # Sumamos el acarreo

seguir:
	inc  %esi 			        # Incrementamos en 1
	cmp  %esi,%ecx 			  	# Comparamos los registros
	jne  bucle 			        # Hace un salto si != bucle
	idiv %ecx
	pop  %esi
	pop  %edi
	ret 				          	# Regresamos a donde se llama a "seguir"
negativo:
	add  %edi, %eax
	jmp  seguir              # Hace un salto a "seguir"
	adc  $0xffffffff, %edx
