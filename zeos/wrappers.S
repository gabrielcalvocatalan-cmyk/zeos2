#include <asm.h>

.extern errno


ENTRY(write)
    pushl %ebp            
    movl %esp, %ebp       

    movl $4, %eax        
    pushl %ebx

    movl 8(%ebp), %edx   
    movl 12(%ebp), %ecx  
    movl 16(%ebp), %ebx  

    pushl %ecx;
    pushl %edx;       
    pushl $write_return
    pushl %ebp
    movl %esp, %ebp

    SYSENTER           

write_return:
   
    popl %ebp
    addl $4, %esp
    popl %edx;
    popl %ecx;
    popl %ebx

    
    cmpl $0, %eax        
    jge write_no_error        
write_error:
    
    negl %eax            
    movl %eax, errno     
    movl $-1, %eax       
write_no_error:
    movl %ebp, %esp      
    popl %ebp            
    ret
    
ENTRY(gettime)
    pushl %ebp            
    movl %esp, %ebp       

    movl $10, %eax        
  	

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
