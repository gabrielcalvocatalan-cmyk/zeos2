/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

char initial_stack[KERNEL_STACK_SIZE]; // Space for the initial system stack
struct task_struct * init_task;
void cpu_idle(void)
{
	__sti();
	while(1)
	{
	;
	}
}

void init_idle (void)
{

}

void init_task1(void)
{
	int Dir = alloc_frame();
	page_table_entry *DirAddress = (page_table_entry *) (Dir << 12);
	clear_page_table(DirAddress);
	int SPT = alloc_frame();
	page_table_entry *SPTAddress = (page_table_entry *) (SPT << 12);
	clear_page_table(SPTAddress);
	set_kernel_pages(SPTAddress);
	int UPT = alloc_frame();
	page_table_entry *UPTAddress = (page_table_entry *) (UPT << 12);
	clear_page_table(UPTAddress);
	set_user_pages(UPTAddress);

	set_ss_pag(SPTAddress, Dir, Dir,0);
	set_ss_pag(SPTAddress, SPT,SPT,0);
	set_ss_pag(SPTAddress, UPT,UPT,0);
	
	DirAddress[0].bits.pbase_addr = SPT;
	DirAddress[0].bits.present = 1;
	DirAddress[0].bits.rw = 1;
	DirAddress[0].bits.user = 0;

	DirAddress[1].bits.pbase_addr = UPT;
	DirAddress[1].bits.present = 1;
	DirAddress[1].bits.rw = 1;
	DirAddress[1].bits.user = 1;



	int task_frame = alloc_frame();
	union task_union *task = (union task_union *) (task_frame << 12);
	set_ss_pag(SPTAddress, task_frame, task_frame,0);
	task->task.PID = 1;
	task->task.dir_pages_baseAddr = DirAddress;
	tss.esp0 = (unsigned long)&task->stack[KERNEL_STACK_SIZE];

	set_cr3(DirAddress);
	init_task = &task->task;


}


void init_sched()
{

}

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t)
{
       return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t)
{
       return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

