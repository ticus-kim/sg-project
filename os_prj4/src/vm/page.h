#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>

#define VM_BIN 0
#define VM_SWAP 1

struct vm_entry{
    uint8_t type;
    void *vaddr;
    bool is_loaded;
    bool writable;
    bool pinned;
    char *filename;
    size_t offset;
    size_t read_bytes;
    size_t zero_bytes;
    size_t swap_slot;
    struct hash_elem elem;
};

struct page{
    void *kaddr;
    struct vm_entry *vme;
    struct thread *thread;
    struct list_elem lru;    
};

void vm_init (struct hash *vm);
void vm_destroy (struct hash *vm);
struct vm_entry *find_vme (void *vaddr);
bool insert_vme (struct hash *vm, struct vm_entry *vme);
bool delete_vme (struct hash *vm, struct vm_entry *vme);
bool load_file (void *kaddr, struct vm_entry *vme);

#endif