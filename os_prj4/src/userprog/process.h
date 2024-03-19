#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "vm/page.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
void argument_stack(char** argv, int argc, void **esp);

bool handle_mm_fault (struct vm_entry *vme);
bool expand_stack (void *vaddr);
bool verify_stack (void *fault_addr, void *esp);
#endif /* userprog/process.h */
