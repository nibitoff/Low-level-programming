%include "colon.inc"
%include "words.inc"
%define BUFFER 256

extern print_string             
extern exit
extern find_word
extern read_word
extern print_err
extern print_newline
extern string_length

global _start

section .rodata
start_msg: db "Enter the key:", 10,0
err_msg1: db "Cant find this key", 0
err_msg2: db "Key must be shorter than 256 symbols!",10, 0

section .text
_start:
	sub rsp, BUFFER
	mov rdi, start_msg ; Putting starting message to rdi
	call print_string ; Printing message
	mov rdi, rsp ; Entering key to buffer
	mov rsi, BUFFER
	call read_word ; Reading entered key
	cmp rax, 0 ; Checking if there was an error
	je .length_error
	mov rdi, rax ; Putting rax to rdi
	mov rsi, address ; Putting pointer to rsi
	call find_word ; Checking if the key in our dictionary
	cmp rax, 0 ; Checking if there was an error	
	je .key_error
	add rax, 8 ; Adding '8' to rax to make offset
	push rax ; Saving rax in stack
	mov rdi, rax ; Putting rax to rdi
	call string_length ; Getting length of the string
	pop rdi ; Getting value from stack to rdi
	add rax, rdi 
	inc rax ; Increasing rax by '1' for null-terminator
	mov rdi, rax 
	call print_string ; Printing the value of the key
	jmp .end
	
	.key_error:
		mov rdi, err_msg1 ; Putting error message to rdi
		call print_err ; Printing error
		jmp .end
	

	.length_error:
		mov rdi, err_msg2 ; Putting error message to rdi
		call print_err ; Printing error
		jmp .end
	
	.end: 
		call print_newline ; Printing line break
		add rsp, BUFFER ; Resetting the buffer
		xor rax, rax
		call exit
		
		 
