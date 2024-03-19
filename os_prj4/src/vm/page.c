#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads/malloc.h>
#include <threads/palloc.h>
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "lib/kernel/list.h"

static unsigned vm_hash_func (const struct hash_elem *e, void *aux UNUSED);
static bool vm_less_func (const struct hash_elem *e1, const struct hash_elem *e2, void *aux UNUSED);

void vm_init (struct hash *vm){
    hash_init(vm, vm_hash_func, vm_less_func, NULL);
}

static unsigned vm_hash_func (const struct hash_elem *e, void *aux UNUSED){
	return hash_int(hash_entry(e, struct vm_entry, elem)->vaddr);
}

static bool vm_less_func (const struct hash_elem *e1, const struct hash_elem *e2, void *aux UNUSED){
    struct vm_entry *vm1 = hash_entry(e1, struct vm_entry, elem);
    struct vm_entry *vm2 = hash_entry(e2, struct vm_entry, elem);

    return vm1->vaddr < vm2->vaddr;
}

void vm_destroy (struct hash *vm){
    hash_destroy(vm, NULL);
}

struct vm_entry *find_vme (void *vaddr){
    struct vm_entry vme;
	struct hash_elem *e;
	vme.vaddr = pg_round_down(vaddr);
	e = hash_find(&thread_current()->vm, &vme.elem);
	if(!e) return NULL;
    return hash_entry(e, struct vm_entry, elem);
}

bool insert_vme (struct hash *vm, struct vm_entry *vme){
    if(hash_insert(vm, &vme->elem)) return false;
    return true;
}

bool delete_vme (struct hash *vm, struct vm_entry *vme){
    if(hash_delete(vm, &vme->elem)){
        free(vme);
        return true;
    }
    free(vme);
    return false;
}

bool load_file (void *kaddr, struct vm_entry *vme){
    struct file *file = filesys_open(vme->filename);
    file_seek(file, vme->offset);
    if(file_read (file, kaddr, vme->read_bytes) != (int) vme->read_bytes) return false;
    memset (kaddr + vme->read_bytes, 0, vme->zero_bytes);
    return true;
}
