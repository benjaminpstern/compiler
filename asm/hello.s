.section .rodata
.PrintIntString: .string "%d\n"
.ReadIntString: .string "%d"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
s0: .string "Hello World"
.text
.globl main
main:
movq %rsp, %rbx
subq $0, %rsp
movq $s0, %rsi
push %rbx
movq $0, %rax
movq $.PrintStringString, %rdi
call printf
pop %rbx
push %rbx
movq $0, %rax
movq $.PrintNewLineString, %rdi
call printf
pop %rbx
addq $0, %rsp
ret
