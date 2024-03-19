#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "process.h"
#include "vm/page.h"

typedef int tid_t;

void syscall_init (void);
void check_user_vaddr(const void *);
void halt(void);
void exit(int);
bool create(const char *, unsigned);
bool remove(const char *);
int open(const char *);
int filesize(int fd);
int write(int,const void *, unsigned );
tid_t exec(const char *);
int wait(tid_t );
int read(int , void *, unsigned);
void seek(int , unsigned );
unsigned tell (int );
void close(int );
int max_of_four(int , int , int , int );
int fibonacci(int );

struct vm_entry * check_address (void *addr, void *esp);
void check_valid_buffer (void *buffer, unsigned size, void *esp, bool to_write);
void check_valid_str (void *str, void *esp);
void check_valid_str_size (void *str, unsigned size, void *esp);

#endif /* userprog/syscall.h */
