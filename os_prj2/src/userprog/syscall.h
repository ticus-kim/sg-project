#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "process.h"

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

#endif /* userprog/syscall.h */
