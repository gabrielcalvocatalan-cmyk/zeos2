/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define BUFFER_SIZE 256
char buffer_k[256];
extern int zeos_ticks;
int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_gettime(){

	return zeos_ticks;
}

int sys_write(int fd, char * buffer,int size){
	int error = check_fd(fd, ESCRIPTURA);
	if(error) return error; 
	if(buffer == NULL) return -EFAULT; 
	
	if(size < 0) return -EINVAL; 
	if(!access_ok(ESCRIPTURA,buffer,size)) return -EFAULT;
	int bytes = size;
	int bytes_escritos;

	while(bytes > BUFFER_SIZE){
		copy_from_user(buffer+(size-bytes), buffer_k, BUFFER_SIZE);
		bytes_escritos = sys_write_console(buffer_k, BUFFER_SIZE);

		buffer = buffer+BUFFER_SIZE;
		bytes = bytes-bytes_escritos;
	}

	
	copy_from_user(buffer+(size-bytes), buffer_k, bytes);
	bytes_escritos = sys_write_console(buffer_k, bytes);
	bytes = bytes-bytes_escritos;

	return size-bytes;
}


