#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "../lib/user/syscall.h"
#include "userprog/exception.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"
#include "threads/synch.h"
#define WORD 4

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void
check_user_vaddr(const void *vaddr){
   if(!is_user_vaddr(vaddr)) exit(-1);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
   switch(*(uint32_t*)f->esp) {
      case SYS_HALT:
	     halt();
		 break;
	  case SYS_EXIT:
	     check_user_vaddr(f->esp+4);
         exit(*(uint32_t *)(f->esp+4));
	  case SYS_EXEC:
	     check_user_vaddr(f->esp+4);
		 f->eax = exec((const char *)*(uint32_t *)(f->esp+4));
		 break;
	  case SYS_WAIT:
	     check_user_vaddr(f->esp+4);
		 f->eax = (uint32_t)wait((tid_t)*(uint32_t *)(f->esp+4));
		 break;
	  case SYS_CREATE:
	     break;
	  case SYS_REMOVE:
	     break;
	  case SYS_OPEN:
	     break;
	  case SYS_FILESIZE:
	     break;
	  case SYS_READ:
	     check_user_vaddr(f->esp+4);
		 check_user_vaddr(f->esp+8);
		 check_user_vaddr(f->esp+12);
		 f->eax = (uint32_t)read((int)*(uint32_t*)(f->esp+4), (const void*)*(uint32_t*)(f->esp+8), (unsigned)*(uint32_t*)(f->esp+12));
		 break;
	  case SYS_WRITE:
	     check_user_vaddr(f->esp+4);
		 check_user_vaddr(f->esp+8);
		 check_user_vaddr(f->esp+12);
		 f->eax = (uint32_t)write((int)*(uint32_t*)(f->esp+4), (const void*)*(uint32_t*)(f->esp+8), (unsigned)*(uint32_t*)(f->esp+12));
		 break;
	  case SYS_SEEK:
	     break;
	  case SYS_TELL:
	     break;
	  case SYS_CLOSE:
	     break;
	  case SYS_FIBO:
	     check_user_vaddr(f->esp+4);
		 f->eax = (uint32_t)fibonacci((int)*(uint32_t*)(f->esp+4));
		 break;
	  case SYS_MAXNUM:
	     check_user_vaddr(f->esp+4);
		 check_user_vaddr(f->esp+8);
		 check_user_vaddr(f->esp+12);
		 check_user_vaddr(f->esp+16);
		 f->eax = (uint32_t)max_of_four_int((int)*(uint32_t*)(f->esp+4),(int)*(uint32_t*)(f->esp+8),(int)*(uint32_t*)(f->esp+12),(int)*(uint32_t*)(f->esp+16));
		 break;
   }
/*  printf ("system call!\n");
  thread_exit ();*/
}

void halt(void){
   shutdown_power_off();
}

void exit(int status){
   printf("%s: exit(%d)\n", thread_name(), status);
   thread_current()->exit_status = status;
   thread_exit();
}

tid_t exec (const char *cmd_line){
   return (tid_t)process_execute(cmd_line);
}

int wait (tid_t tid){
   return process_wait(tid);
}

int read (int fd, void *buffer, unsigned size){
   unsigned i;
   char *tmpbuf;
   tmpbuf = (char *)buffer;

   if(fd == 0){
      for(i = 0; i<size ;i++){
	     if((tmpbuf[i] = (char)input_getc()) == '\0') break;
      }
	  return i;
   }
   return -1;
}

int write (int fd, const void *buffer, unsigned size){
   if(fd == 1){
      putbuf(buffer, size);
	  return size;
   }
   return -1;
}

int max_of_four_int(int a, int b, int c, int d){
   int max = a;
   if(b > max) max = b;
   if(c > max) max = c;
   if(d > max) max = d;
   return max;
}

int fibonacci(int n){
   int a = 0, b = 1, c;
   
   if(n == 0) return 0;
   else if (n == 1) return 1;
   else if (n == 2) return 1;
   else{
      for(int i = 1; i < n ; i++){
	     c = a+b;
		 a = b;
		 b = c;
	  } 
	  return b;
   }
}
