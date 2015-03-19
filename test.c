//Shizhe Chen
#include <stdlib.h>
#include "my_malloc.h"
#include "list.h"

extern metadata_t* freelist[8];

int main() {
	#ifdef DEBUG
    printf("Debug run\n");
    #endif
    
    int tests = 0;
    int passed = 0;

    //Test 1
    printf("TEST ONE\n");
    tests++;
    printf("check for correct error codes\n");
    char* a = my_malloc(10000); //should throw error
    printf("checking single request for basic case\n");
    if (ERRNO == SINGLE_REQUEST_TOO_LARGE && a == NULL) {
        printf("\tSINGLE REQUEST WAS TOO LARGE (working as intended)\n");
        passed++;
    } else {
        printf("\tSINGLE REQUEST WAS TOO LARGE (not working as intended)\n");
    }

    //Test 2
    printf("TEST TWO\n");
    tests++;
    a = my_malloc(2040); //should throw error
    printf("checking single request for case where the request is smaller, but\nthe request and the metadata is too large\n");
    if (ERRNO == SINGLE_REQUEST_TOO_LARGE && a == NULL) {
        printf("\tSINGLE REQUEST WAS TOO LARGE (working as intended)\n");
        passed++;
    } else {
        printf("\tSINGLE REQUEST WAS TOO LARGE (not working as intended)\n");
    }

    //Test 3
    printf("TEST THREE\n");
    tests++;
    a = my_malloc(1); //should not throw error
    printf("checking single when the request is correctly sized\n");
    if (ERRNO == NO_ERROR && a != NULL) {
        printf("\tREQUEST WAS CORRECTLY SIZED (working as intended)\n");
        passed++;
    } else {
        printf("\tMALLOC THOUGHT THERE WAS AN ERROR (not working as intended)\n");
    }

    //now for some dark magic
    //test 4
    printf("TEST FOUR\n");
    tests++;
    printf("checking to make sure the size of the chunk is correct\n");
    metadata_t* position = (metadata_t*) ((char*) a - sizeof(metadata_t));

    if (position->in_use == 1 && position->size == 16) {
        printf("\tREQUEST WAS CORRECTLY ALLOCATED (working as intended)\n");
        passed++;
    } else {
        printf("\tREQUEST WAS INCORRECTLY ALLOCATED (not working as intended)\n");
        printf("%i\n", position->size);
    }

    //more dark magic
    //test 5
    printf("TEST FIVE\n");
    tests++;
    printf("checking to make sure the freelist is correct\n");
    int truth = 1;
    if (freelist[7] != NULL) {
        truth = 0;
    }
    for (int i = 6; i >= 0; i--) {
        if (!(freelist[i] != NULL && freelist[i]->size == (1 << (i+4)) && freelist[i]->next == NULL)) {
            truth = 0;
        }
    }
    if (truth == 1 && ERRNO == NO_ERROR) {
        printf("\tFREELIST IS CHANGED CORRECTLY (working as intended)\n");
        passed++;
    } else {
        printf("\tFREELIST HAS ISSUES (not working as intended)\n");
    }


    //summary
    printf("---------------------SUMMARY-----------------------\n");
    printf("%i of %i cases passed\n", passed, tests);
    if (passed == tests) {
        printf("IT WORKS PERFECTLY!!!!!!\n");
    }

	return 0;
}
