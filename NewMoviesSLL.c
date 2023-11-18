// Insert to New movies list which must must sorted.
#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

struct new_movie* new_movies_list;	/* Head of new movies SLL */

// Insert to a sorted list in ascending order.
int NewMoviesInsertSorted(unsigned mid, movieCategory_t cat, unsigned year) {
    struct new_movie *prev = NULL;              /* parent of new node */
    struct new_movie *tmp = new_movies_list;    /* node used for scanning */

    /* Create and initialize new node*/
    struct new_movie* new_film = (struct new_movie*)malloc(sizeof(struct new_movie));
    if (new_film == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    new_film->info.mid = mid;
    new_film->info.year = year;
    new_film->category = cat;

    /* Scan the list to find the right place*/
    while ((tmp!= NULL) && (tmp->info.mid < mid)) {
        prev = tmp;
        tmp = tmp->next;
    }

    /* 
     * If tmp != NULL then tmp's mid > mid.
     * If tmp == NULL (empty list or end of list found) then mid is the largest.
    */
    new_film->next = tmp;
    
    /* (mid < head's mid) OR (empty list)*/
    if (prev == NULL) {
        new_movies_list = new_film; /* Replace list's head*/
    }
    else {
        /* Insert new node between prev and tmp */
        prev->next = new_film;
    }

    return 1;
}

void print_list() {
    struct new_movie* tmp = new_movies_list;
    
    printf("[");

    while(tmp != NULL) {
        printf("%d, ", tmp->info.mid);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

int main(void) {
    new_movies_list = NULL;

    NewMoviesInsertSorted(10, DRAMA, 1976);
    NewMoviesInsertSorted(18, DRAMA, 1976);
    NewMoviesInsertSorted(147, DRAMA, 1976);
    NewMoviesInsertSorted(4, DRAMA, 1976);
    NewMoviesInsertSorted(711, DRAMA, 1976);
    NewMoviesInsertSorted(235, DRAMA, 1976);

    print_list();

    return 0;
}