section .text
 
global exit
global string_length
global string_equals
global print_string
global print_newline
global print_char
global print_int 
global print_uint
global parse_uint
global parse_int
global read_char
global read_word
global print_err


; Принимает код возврата и завершает текущий процесс
;Parameters: rdi - return code
;Out - !exit
exit: 
    mov rax, 60
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
;Parameters: rdi - pointer to null-terminator
;Out: rax - string length
string_length:
     xor rax, rax
    .loop:
        cmp byte [rdi+rax], 0 ; Compare if the current symbol is 0
        je .end ; If its 0 then exit
        inc rax ; Go to next symbol and increase counter
        jmp .loop ; Start .loop again
    .end:
        ret

;Принимает указатель на нуль-терминированную строку и выводит ее в stderr
;Parameters: rdi - pointer to null-terminated string
;Out: !stderr
print_err:
    push rdi ; Save caller-saved register
    call string_length ; Call string_length
    pop rdi
    mov rsi, rdi ; Save data in rsi
    mov rdx, rax ; Save the length of the string 
    mov rax, 1  ; Write syscall identifier
    mov rdi, 2 ; Stderr descriptor
    syscall 
    ret



; Принимает указатель на нуль-терминированную строку, выводит её в stdout
;Parameters: rdi - pointer to null-terminator
;Out: !stdout
print_string:
    push rdi ; Save caller-saved register
    call string_length ; Call string_length
    mov rdx, rax ; Save the length of the string 
    mov rax, 1  ; Write syscall identifier
    mov rsi, rdi ; Save data in rsi
    mov rdi, 1 ; Stdout descriptor
    syscall 
    pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
;Parameters: none
;Out: !stdout
print_newline:
    mov rdi, 0xA ; 

; Принимает код символа и выводит его в stdout
;Parameters: rdi - symbol code
;Out: !stdout
print_char:
    push rdi ; Save caller-saved register
    mov rdi, 1 ; Stdout descriptor
    mov rax, 1 ; Write syscall identifier
    mov rdx, 1 ; The amount of bytes to write
    mov rsi, rsp ; Save data in rsi
    syscall
    pop rdi
    ret


; Выводит знаковое 8-байтовое число в десятичном формате 
;Parameters: rdi - signed 8-byte number in decimal format 
;Out: !stdout
print_int:
    cmp rdi, 0 ; Check the sign of the number
    jnl print_uint ; If its positive - print it
    push rdi ; Save caller-saved register
    mov rdi, '-'
    call print_char ; Print "-" symbol
    pop rdi
    neg rdi ; Get a negative value



; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
;Parameters: rdi - unsigned 8-byte number in decimal format 
;Out: !stdout
print_uint:
    mov rax, rdi ; Save the number in rax
    mov rdi, 10 ; Use rdi as a divider
    mov rsi, rsp ; Put the address of stack in rsi
    dec rsp
    mov [rsp], byte 0
    .loop:
        xor rdx, rdx ; Clear rdx in the beginning of cycle
        div rdi ; Division by rdi(10)
        add rdx, 48 ; Getting the ASCII code
        dec rsp
        mov byte [rsp], dl ; Save code to the stack
        cmp byte rax, 0 ; Check if the its an end of loop
        jnz .loop
        mov rdi, rsp 
        push rsi ;Save caller-saved register
        call print_string
        pop rsp
        ret


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
;Parameters: 
;   rdi - pointer to 1st string with null-terminator; rsi - pointer to 2nd string with null-terminator
;Out: rax - result
string_equals:
   .loop:
        mov al, byte[rdi]  ; rdi contains pointer to 1st string, rsi to 2nd
        cmp al, byte[rsi] ; Comparing symbols
        jne .error
        cmp al, 0 ; Checking on null-terminator
        je .end
        cmp byte[rsi], 0 ; Checking on null-terminator
        je .end
        inc rsi ; Moving to next symbol
        inc rdi
        jmp .loop

    .error:
        mov rax, 0 ; Returning '0' if not
        ret
    
    .end:
        mov rax, 1 ; Returning '1' if success
        ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
;Parameters: !stdin
;Out: rax - entered symbol
read_char:
    push 0 ; Create space in stack
    mov rdi, 0 ; Stdout descriptor
    mov rax, 0 ; Write syscall identifier
    mov rdx, 1 ; The amount of bytes to write
    mov rsi, rsp ; Save data in rsi
    syscall
    pop rax
    ret
 

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
;Parameters: 
;   rdi - address of buffer's start
;   rsi - size of buffer
;   !stdin
;Out: rax - address of buffer; rsi - size of buffer
read_word:
    push r12
    push r13
    push r14
    mov r12, rdi ; Save caller-saved registers(address)
    mov r13, rsi ; Size of buffer
    xor r14, r14  ; Counter
    .check1:
        call read_char ; Read symbol
        cmp rax, 0 ; Checking on null-terminator
        je .end
        cmp r14, r13 ;
        je .error
        cmp r14, 0
        jne .check2

        cmp rax, 0x20 ; Checking on space symbol
        je .check1
        cmp rax, 0x9 ; Checking on tabs
        je .check1
        cmp rax, 0xA ; Checking on line break
        je .check1
    
    .check2:
        cmp rax, 0 ; Checking on null-terminator
        je .end
        cmp rax, 0x20 ; Checking on space symbol
        je .end
        cmp rax, 0x9 ; Checking on tabs
        je .end
        cmp rax, 0xA ; Checking on line break
        je .end
        mov [r12+r14], al ; Put new symbol in buffer
        inc r14
        jmp .check1 

    .error:
        xor rax, rax
        pop r14
        pop r13 ; Return caller-saved registers
        pop r12
        ret

    .end:  
        mov byte[r12+r14], 0 
        mov rdx, r14
        mov rax, r12
        pop r14
        pop r13 ; Return caller-saved registers
        pop r12
        ret

 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
;Parameters: rdi - pointer to the string
;Out: rax - number from the string; rdx - length of the number
parse_uint:
    xor rax, rax ; Erase needed registeres
    xor rdx, rdx
    xor rcx, rcx
    xor r11, r11
    mov r9, 10 ; Put '10' in r9 register

    .loop:
        mov r11b, byte[rcx+rdi]
        sub r11, 48 ; Getting the ASCII code
        cmp r11, 0 ; Checking 
        jb .end
        cmp r11, 9 ; Checking 
        ja .end
        mul r9
        add rax, r11 
        inc rcx ; Increment counter
        jmp .loop
    
    .end:
        mov rdx, rcx
        ret

    

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
;Parameters: rdi - pointer to the string
;Out: rax - number from the string; rdx - length of the number
parse_int:
    xor rax, rax ; Erase needed registeres
    xor rdx, rdx

    cmp byte[rdi], '-' ; Check the sign of the number
    jne parse_uint ; If its positive - parse it
    inc rdi
    call parse_uint
    cmp rax, 0 ; Checking if its not a digit
    jz .error
    neg rax ; Get a negative value
    inc rdx
    ret
    
    .error:
        xor rax, rax
        ret

   
; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
;Parameters: rdi - pointer to the string
;   rsi - pointer to the buffer
;   rdx - length of the buffer
;Out: rax - length of the string
string_copy:
    xor rax, rax ; Erase needed register
    xor r11, r11  
    push rdi ; Save caller-saved registers
    push rsi
    push rdx
    call string_length ; Getting length of string
    pop rdx ; Returning caller-saved registers
    pop rsi
    pop rdi
    inc rax
    cmp rax, rdx ; Checking if buffer(rdx) equals string(rax)
    jnz .error

    .loop:
        mov cl, byte[rdi+r11]
        mov byte[rsi+r11], cl
        inc r11
        cmp r11, rax ; Checking buffer
        jnz .loop
        dec rax
        ret

    .error:
        mov rax, 0 ; Returning '0'
        ret


