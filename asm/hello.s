.section .rodata
.PrintIntString: .string "%d\n"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
s0: .string "Hello World"
.text
.globl main
main:
movq $0, %rax
movq $s0, %rdi
call printf
movq $0, %rax
movq $.PrintNewLineString, %rdi
call printf
