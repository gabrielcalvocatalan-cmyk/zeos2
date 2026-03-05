/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

void keyboard_handler();
void syscall_handler_sysenter();
void clock_handler();
void my_page_fault_handler();
int zeos_ticks = 0;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}
void writeMSR(unsigned long msr,unsigned long val);

void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();
  setInterruptHandler(33,keyboard_handler,0);
  setInterruptHandler(32, clock_handler,0);
  setInterruptHandler(14,my_page_fault_handler,0);
  

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  writeMSR(0x174, __KERNEL_CS);
  writeMSR(0x175, INITIAL_ESP);
  writeMSR(0x176,(unsigned long)syscall_handler_sysenter);
  set_idt_reg(&idtR);
}

void keyboard_routine(){

	unsigned char pv = inb(0x60);
	int isbreak = pv & 0x80;
	if(isbreak == 0){
		char print = char_map[pv & 0x7f];
		if(print == 0){
			print = 'C';
		}
		printc_xy(0, 18, print);


	}
}
void clock_routine(){
	zeos_ticks++;
	zeos_show_clock();
	

}
void print_string_xy(int x, int y, char *s) {
    int i = 0;
    while(s[i] != '\0') {
        printc_xy(x + i, y, s[i]);
        i++;
    }
}


void print_hex_xy(int x, int y, unsigned int n) {
    char hex[] = "0123456789ABCDEF";
    int i;

    for(i = 0; i < 8; i++) {
        int shift = (7 - i) * 4;
        char c = hex[(n >> shift) & 0xF];
        printc_xy(x + i, y, c);
    }
}




void my_page_fault_routine(unsigned int eip) {

    print_string_xy(0, 10, "Process generates a PAGE FAULT exception at EIP: 0x");
    print_hex_xy(52, 10, eip);

    while(1);
}

