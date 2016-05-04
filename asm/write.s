.section .rodata
.PrintIntString: .string "%d\n"
.ReadIntString: .string "%d"
.PrintFloatString: .string "%f\n"
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
.text
.globl main
main:
movq %rsp, %rbx
subq $8, %rsp
push %rbx
movq $0, %rax
movq $.PrintNewLineString, %rdi
call printf
pop %rbx
addq $8, %rsp
ret
