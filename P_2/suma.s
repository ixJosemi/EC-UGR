.section .data
.macro linea
#	.int 1,1,1,1
#	.int 2,2,2,2
#	.int 1,2,3,4
#	.int -1,-1,-1,-1
#	.int 0xffffffff,0xffffffff,0xffffffff,0xffffffff
#	.int 0x08000000,0x08000000,0x08000000,0x08000000
#	.int 0x10000000,0x10000000,0x10000000,0x10000000
.endm
lista:
	.irpc i,12345678
		linea
	.endr
longlista:
	.int (.-lista)/4
resultado:
	.quad -1 # Declaro el resultado como quad para que tenga 64B

formato:
	.ascii "suma = %u = %x hex\n\0"

.section .text
_start: .global _start
# main: .global main

	mov  $lista, %ebx
	mov  longlista, %ecx
	call suma
	mov  %eax, resultado+4

#	push resultado
#	push resultado
#	push $formato
#	call printf
#	add  $12, %esp

	mov  $1, %eax
	mov  $0, %ebx
	int  $0x80

suma:
	push     %esi 	# esi es el indice
	mov  $0, %eax 	# pongo a cero los registros y el acumulador
	mov  $0, %esi
	mov  $0, %edx
bucle:
	add  (%ebx,%esi,4), %eax
	inc  %esi
	jc   carry
	cmp  %esi, %ecx
	jne  bucle

	pop  %esi
	ret

carry:
	inc  %edx
	cmp  %esi, %ecx # no es necesario utilizarlo aqui
	jne  bucle

	pop  %esi
	ret
