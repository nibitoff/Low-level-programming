extern string_equals
global find_word

;Функция проходит по всему словарю в поисках 
;подходящего ключа. Если подходящий ключ найден, то
;возвращает адрес начала вхождения в словарь. Иначе возвращает ноль.
;Parameters: rdi - pointer to pointer to null-terminated string
;            rsi - pointer to the start of dictionary
;Out: rax - 0
find_word:
	xor rax, rax ; Erase rax register
	.loop:
		cmp rsi, 0 ; Checking if the dictionary ended
		je .error
		push rdi ; Save caller-saved registers
		push rsi
		add rsi, 8 ; Increasing pointer by 8 to get pointer to key
		call string_equals
		pop rsi ; Returning registers
		pop rdi
		cmp rax, 1  ; Checking result from string_equals
		je .end ; If strings are equal go to end
		mov rax, [rsi]
		cmp rax, 0; If strings arent equal put to rax address of key element
		je .error
		mov rsi, [rsi] ; Move to next element in dictionary
		jmp .loop
		
	.error:
		xor rax, rax
		ret
		
	.end:
		mov rax, rsi
		ret
		
		
