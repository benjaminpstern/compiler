.section .rodata
.PrintIntString: .string "%d\n"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
s0: .string "Hello World"
.text
.globl main
main:
movq %rsp, %rbx
movq $s0, %rsi
movq $0, %rax
movq $.PrintStringString, %rdi
call printf
movq $0, %rax
movq $.PrintNewLineString, %rdi
call printf
