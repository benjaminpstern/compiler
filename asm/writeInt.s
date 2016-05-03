.section .rodata
.PrintIntString: .string "%d\n"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
.text
.globl main
main:
movq $5, %rax
movq %rax, %rsi
movq $0, %rax
movq $.PrintIntString, %rdi
call printf
