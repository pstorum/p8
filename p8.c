#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "p8.h"

#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - (((x) - 1) & (ALIGNMENT - 1)))

#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))

struct block *head = NULL;

int main(){
	
	void *p, *q;

	p = myalloc(10); print_data();
	q = myalloc(20); print_data();

	myfree(p); print_data();
	myfree(q); print_data();
}

void * myalloc(int allo){
	int padded_block_size;
	int alloed = allo + GET_PAD(allo);
	
	if (head == NULL) {
	    head = sbrk(1024);
	    head->next = NULL;
	    head->size = 1024 - PADDED_SIZE(sizeof(struct block));
	    head->in_use = 0;
	}
	struct block *holding = head;

	while(holding != NULL){
		if(holding->size >= alloed && !holding->in_use){
			padded_block_size = PADDED_SIZE(sizeof(struct block));
			int required_space = alloed + padded_block_size + 16;

			if (holding->size >= required_space){
				struct block *new_block = PTR_OFFSET(holding, padded_block_size + alloed);
				new_block->size = holding->size - (alloed + padded_block_size);
				new_block->in_use = 0;
				new_block->next =  holding->next;
				
				holding->size = alloed;
				holding->next = new_block;
			}
			holding->in_use = 1;
			return PTR_OFFSET(holding, padded_block_size);
		}
		holding = holding->next;
	}
	return NULL;
}

void print_data(void){
	struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

void myfree(void *p){
	struct block *temp = PTR_OFFSET(p, -PADDED_SIZE(sizeof *temp));
	temp->in_use = 0;

	struct block *cur = head;

	while (cur->next != NULL){
		if (cur->in_use == 0 && cur->next->in_use == 0){
			cur->size = cur->size + PADDED_SIZE(sizeof *cur->next) + cur->next->size;
			if(cur->next->next == NULL){
				cur->next = NULL;
			}else{
				cur->next = cur->next->next;
			}
		}else{
			cur = cur->next;
		}
	}
}

