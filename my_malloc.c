//Shizhe Chen
#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the 
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you will receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif


/* make sure this always points to the beginning of your current
 * heap space! if it does not, then the grader will not be able
 * to run correctly and you will receive 0 credit. remember that
 * only the _first_ call to my_malloc() returns the beginning of
 * the heap. sequential calls will return a pointer to the newly
 * added space!
 * Technically this should be declared static because we do not
 * want any program outside of this file to be able to access it
 * however, DO NOT CHANGE the way this variable is declared or
 * it will break the autograder.
 */
void* heap;

/* our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * Technically this should be declared static for the same reasons
 * as above, but DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist[8];
/**** SIZES FOR THE FREE LIST ****
 * freelist[0] -> 16
 * freelist[1] -> 32
 * freelist[2] -> 64
 * freelist[3] -> 128
 * freelist[4] -> 256
 * freelist[5] -> 512
 * freelist[6] -> 1024
 * freelist[7] -> 2048
 */

//prototype
 metadata_t* setRemove(metadata_t* node);

int getIndex(size_t size);

metadata_t* get(metadata_t *arr);


void* my_malloc(size_t size)
{
    if ((int)size < 0) return NULL;
    //initialize the heap
    if (heap == NULL) {
        heap = my_sbrk(SBRK_SIZE);
        if (heap == (void*) -1) {
            ERRNO = OUT_OF_MEMORY;
            return NULL;
        }
        metadata_t* Bigblock = (metadata_t*) heap;
        Bigblock->prev = NULL;
        Bigblock->next = NULL;
        freelist[7] = Bigblock;
        freelist[7] -> size = SBRK_SIZE;
        freelist[7] -> in_use = 0;
    }
    //total size we want
    int s = (int)size + sizeof(metadata_t);
    //if data too large
    if (s > SBRK_SIZE) {
        ERRNO = SINGLE_REQUEST_TOO_LARGE;
        return NULL;
    }
    int index = getIndex(s);
    metadata_t* block = freelist[index];
    //test if space avaliable
    if (freelist[index] != NULL) {
        freelist[index] = freelist[index]->next;
        block->next = NULL;
        block->in_use = 1;
        ERRNO = NO_ERROR;
        return ((char*) block) + sizeof(metadata_t);
    } else {
        //else we need to search the nearest avaliable block and split it
        int nextIndex = index + 1;
        while (nextIndex < 8 && freelist[nextIndex] == NULL) {
            nextIndex++;
        }
        //out of memory request more
        if (nextIndex == 8) {
            freelist[7] = my_sbrk(SBRK_SIZE);
            if (freelist[7] == (void*) -1) {
                ERRNO = OUT_OF_MEMORY;
                return NULL;
            }
            freelist[7]->prev = NULL;
            freelist[7]->next = NULL;
            freelist[7]->size = SBRK_SIZE;
            freelist[7]->in_use = 0;
            nextIndex = 7;
        }
        block = freelist[nextIndex];
        metadata_t* half;
        //chop the memory half and half until get the size we want
        while (index != nextIndex) {
            freelist[nextIndex] = freelist[nextIndex]->next;
            block->size = block->size / 2;
            half = (metadata_t*) ((char*)block + block->size);
            //set prev and next
            block->next = half;
            half->prev = block;
            half->size = block->size;
            nextIndex--;
            freelist[nextIndex] = block;
        }
        block = freelist[index];
        freelist[index] = freelist[index]->next;
        block->next = NULL;
        block->in_use = 1;
        ERRNO = NO_ERROR;
        return ((char*) block) + sizeof(metadata_t);
    }
}
void* my_calloc(size_t num, size_t size)
{
    /* check size */
    if (num * size > SBRK_SIZE) {
        ERRNO = SINGLE_REQUEST_TOO_LARGE;
        return NULL;
    }
    char *m = my_malloc(num * size);
    if (m == NULL) {
        ERRNO = OUT_OF_MEMORY;
        return NULL;
    }
    /* initialize to 0 */
    char *temp = m + sizeof(metadata_t);
    for (int i = 0; i < num; i++) {
        *temp = 0;
        temp += size;
    }
    ERRNO = NO_ERROR;
    return (void *)m;
}
void my_free(void* ptr)
{
    //get the address of the block
    metadata_t* block = (metadata_t*)((char*)ptr - sizeof(metadata_t));
    metadata_t* buddy;
    int sizeBig = 0;
    while (sizeBig == 0) {
        //get the buddy address
        buddy = get(block);
        block->in_use = 0;
        //if is freed, not max size, then merge
        if (buddy != NULL && buddy->size != SBRK_SIZE) {
            if (block->next == buddy) {
                block->next = buddy->next;
            }
            if (block->prev == buddy) {
                block->prev = buddy->prev;
            }
            if (buddy->next == block) {
                buddy->next = block->next;
            }
            if (buddy->prev == block) {
                buddy->prev = block->prev;
            }
            setRemove(buddy);
            setRemove(block);
            metadata_t* first;
            if (block > buddy) {
                first = buddy;
            } else {
                first = block;
            }
            first->size *=2;
            if (first->size == SBRK_SIZE) {
                sizeBig = 1;
            } else {
                block = first;
            }
        } else {
            ERRNO = NO_ERROR;
            return;
        }
    }
}
//wrong
void* my_memmove(void* dest, const void* src, size_t num_bytes)
{
    // dest == src
    if (dest == src) {
        ERRNO = NO_ERROR;
        return dest;
    }
    char *d = (char *)dest;
    char *s = (char *)src;
    char *temp = my_malloc(num_bytes);
    for (int i = 0; i < num_bytes; i++) {
        temp[i] = s[i];
    }
    //dest > src
    if ((long)dest > (long)src) {
        for (int i = (int)num_bytes - 1; i >= 0; i--) {
            d[i] = temp[i];
        }
    } else {
        // dest < src
        for (int i = 0; i < num_bytes; i++) {
            d[i] = temp[i];
        }
    }
    ERRNO = NO_ERROR;
    return dest;
}

//calculate which index in the freelist will look into
int getIndex(size_t size) {
    int b = 1;
    int count = 0;
    while (b < size) {
        b = b * 2;
        count++;
    }
    count = count - 4;
    if (count < 0) {
        count = 0;
    }
    return count;
}
//if we want to remove the node, set pre and next
metadata_t* setRemove(metadata_t* node) {
    metadata_t *p = node->prev;
    metadata_t *n = node->next;
    int index = getIndex(node->size);
    if (n == NULL) {
        if (p == NULL) {
            freelist[index] = NULL;
        } else {
            p->next = NULL;
        }
    } else {
        if (p == NULL) {
            freelist[index] = n;
            n->prev = NULL;
        } else {
            p->next = n;
            n->prev = p;
        }
    }
    node->next = NULL;
    node->prev = NULL;
    node->in_use = 1;
    return node;
}

metadata_t* get(metadata_t *arr) {
    //use xor to get the buddy
    metadata_t *buddy = (metadata_t *)((long)arr ^ (long)arr->size);
    if (arr->size == buddy->size) {
        return buddy;
    }
    return NULL;
}
