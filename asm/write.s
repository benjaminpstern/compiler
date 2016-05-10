.section .rodata
.PrintIntString: .string "%d "
.ReadIntString: .string "%d"
.PrintFloatString: .string "%f "
.PrintStringString: .string "%s"
.PrintNewLineString: .string "\n"
.text
.globl main
main:
movq %rsp, %rbx
subq $0, %rsp
push %rbx
movq $0, %rax
movq $.PrintNewLineString, %rdi
call printf
pop %rbx
addq $0, %rsp
ret
