.section .rodata
.PrintIntString: .string "%d\n"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
.text
.globl main
main:
movq $0, %rax
movq $, %rdi
call printf
