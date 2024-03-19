#include "vm/frame.h"
#include "vm/swap.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"
#include "userprog/syscall.h"
#include <threads/malloc.h>
#include <stdio.h>

void lru_list_init(void){
    list_init(&lru_list);
	lock_init(&lru_lock);
	lru_clock = NULL;
}

void add_page_to_lru_list(struct page *page){
    list_push_back(&lru_list, &page->lru);
}

void del_page_from_lru_list(struct page *page){
    if(lru_clock == page) lru_clock = list_entry(list_remove(&page->lru), struct page, lru);
    else list_remove(&page->lru);
}

struct page *alloc_page(enum palloc_flags flags UNUSED){
    lock_acquire(&lru_lock);

    uint8_t *kaddr = palloc_get_page(flags);
    while(!kaddr){
		try_to_free_pages(flags);
		kaddr = palloc_get_page(flags);
	}

    struct page *p = malloc(sizeof(struct page));

    p->kaddr = kaddr;
    p->thread = thread_current();

    add_page_to_lru_list(p);
    lock_release(&lru_lock);

    return p;
}

void free_page(void *kaddr){
    lock_acquire(&lru_lock);
    struct list_elem *e = list_begin(&lru_list);
    struct page *lru_page = NULL;
    
    while(e != list_end(&lru_list)){
        lru_page = list_entry(e, struct page, lru);
        if(lru_page->kaddr == kaddr) break;
        e = list_next(e);
    }
    if(lru_page) __free_page(lru_page);
    lock_release(&lru_lock);
}
void __free_page(struct page *page){
    del_page_from_lru_list(page);
    pagedir_clear_page(page->thread->pagedir, pg_round_down(page->vme->vaddr));
    palloc_free_page(page->kaddr);
    free(page);
}
struct list_elem* get_next_lru_clock(void){
    if(list_empty(&lru_list)) return NULL;

    if(!lru_clock || lru_clock == list_end(&lru_list)) return list_begin(&lru_list);

    if (list_next(lru_clock) == list_end(&lru_list)) return list_begin (&lru_list);
    else return list_next (lru_clock);

    return lru_clock;
}
void try_to_free_pages(enum palloc_flags flags){
    struct page *p;

    lru_clock = get_next_lru_clock();

    p = list_entry(lru_clock, struct page, lru);
    while(p->vme->pinned || pagedir_is_accessed(p->thread->pagedir, p->vme->vaddr)){
        bool flag = p->vme->pinned;
        pagedir_set_accessed(p->thread->pagedir, p->vme->vaddr, false);
        if(flag == true) flag = false;
        lru_clock = get_next_lru_clock();
        p = list_entry(lru_clock, struct page, lru);
    }

    struct page *found = p;
    switch(found->vme->type)
	{
		case VM_BIN:
			if(pagedir_is_dirty(found->thread->pagedir, found->vme->vaddr)){
				found->vme->swap_slot = swap_out(found->kaddr);
				found->vme->type = VM_SWAP;
			}
			break;
		case VM_SWAP:
			found->vme->swap_slot = swap_out(found->kaddr);
			break;
	}
	found->vme->is_loaded = false;

	__free_page(found);
	return;
}