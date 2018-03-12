.section .data
lista:		.int 1, 2, 3, 4, -1000, -1
longlista:	.int (.-lista)/4
resultado:	.quad -1 	     # Declaro resultado como un quad para que tenga 64 bits

.section .text
_start:	.global _start

	mov    $lista, %ebx
	mov longlista, %ecx
	call suma

	mov %edx, (resultado+4)  # Movemos primero a la parte alta a resultado+4
													 # porque es little endian
	mov %eax, resultado	     # Movemos la parte  baja de resultado

	mov $1, %eax
	mov $0, %ebx
	int $0x80

suma:
	push %esi     		       # esi es el indice
	push %edi
	push %ebp
	mov $0, %eax  		       # Inicio a 0 los acumuladores y registros
	mov $0, %edx
	mov $0, %esi
	mov $0, %ebp
	mov $0, %edi
bucle:

	add (%ebx,%esi,4),%eax   # cargo en edx:eax el número de la lista
	CDQ			             # Extensión de signo
	add %eax, %ebp	          # Sumamos la parte baja del número de 64 bits en %ebp
	adc %edx, %edi		       # Sumamos la parte alta del número de 64 bits
				    				 # y el acarreo en %edi
	mov $0, %eax		       # Limpiamos %eax
	mov $0, %edx             # Limpiamos %edx
	inc %esi

	cmp %esi,%ecx
	jne bucle

	mov %ebp, %eax		       # Trasladamos el resultado a %eax
	mov %edi, %edx		       # y a %edx, que es dónde el programa espera que estén
	cdq			             # Extensión de signo
	idiv %ecx                # Media aritmética

	pop %ebp
	pop %edi
	pop %esi
	ret
