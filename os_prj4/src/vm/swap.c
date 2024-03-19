#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "devices/block.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

const size_t PAGE_BLOCK_CNT = PGSIZE / BLOCK_SECTOR_SIZE;

void swap_init(void){
    swap_bitmap = bitmap_create(1024*8);
}

void swap_in(size_t used_index, void* kaddr){
    struct block *swap_block = block_get_role(BLOCK_SWAP);

    if(bitmap_test(swap_bitmap, used_index)) {
		for(int i=0; i<PAGE_BLOCK_CNT; i++){
			block_read(swap_block, PAGE_BLOCK_CNT * used_index + i, BLOCK_SECTOR_SIZE * i + kaddr);
		}
		bitmap_reset(swap_bitmap, used_index);
	}
	return;
}
size_t swap_out(void* kaddr){
    struct block *swap_block = block_get_role(BLOCK_SWAP);
	size_t swap_index = bitmap_scan(swap_bitmap, 0, 1, false);
	if(swap_index != BITMAP_ERROR){
		for(int i=0; i<PAGE_BLOCK_CNT; i++){
			block_read(swap_block, PAGE_BLOCK_CNT * swap_index + i, BLOCK_SECTOR_SIZE * i + kaddr);
		}
		bitmap_set(swap_bitmap, swap_index, true);
	}
	return swap_index;
}

