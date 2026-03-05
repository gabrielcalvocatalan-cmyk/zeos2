#include <asm.h>

.extern errno


ENTRY(write)
    pushl %ebp            
    movl %esp, %ebp       

    movl $4, %eax        
    pushl %ebx

    movl 8(%ebp), %edx   //fd
    movl 12(%ebp), %ecx  //buffer
    movl 16(%ebp), %ebx  // size

    pushl %ecx;
    pushl %edx;       //registros que machaca la llamda
    pushl $write_return  //link dinamico
    pushl %ebp
    movl %esp, %ebp     //Siempre

    SYSENTER           

write_return:
   
    popl %ebp
    addl $4, %esp
    popl %edx;
    popl %ecx;
    popl %ebx

    
    cmpl $0, %eax   //comprueba erro      
    jge write_no_error        
write_error:
    
    negl %eax            
    movl %eax, errno     //Valor errno
    movl $-1, %eax       //return de la llamada write
write_no_error:
    movl %ebp, %esp      
    popl %ebp            
    ret
    
ENTRY(gettime)
    pushl %ebp            
    movl %esp, %ebp       

    movl $10, %eax    //Cada llamad a sistema tiene un numero diferente
                    //Se debe actualizar la sysTable para que funcione    
  	
    pushl %ecx;
    pushl %edx;       
    pushl $time_return
    pushl %ebp
    movl %esp, %ebp

    SYSENTER           
time_return:
    popl %ebp
    addl $4, %esp
    popl %edx;
    popl %ecx;
time_end:
    movl %ebp, %esp
    popl %ebp
    ret
