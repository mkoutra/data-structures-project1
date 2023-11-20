// Functions for global Single Linked List
// Similar to Exercise 1 of Set 2 .
#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

struct movie* category_array[6]; 	/* Each element is the head of an SLL */
struct new_movie* new_movies_list;	/* Head of new movies, SLL */

// Insert to a sorted list in ascending order.
int NewMoviesInsertSorted(unsigned mid, movieCategory_t cat, unsigned year) {
    struct new_movie *prev = NULL;              /* parent of new node */
    struct new_movie *tmp = new_movies_list;    /* node used for scanning */

    /* Scan the list to find the right place*/
    while ((tmp!= NULL) && (tmp->info.mid < mid)) {
        prev = tmp;
        tmp = tmp->next;
    }

    /* We don't allow duplicate movies*/ 
    if ((tmp!= NULL) && (tmp->info.mid == mid)) {
        fprintf(stderr, "Movie %d is already inside.\n", mid);
        return -1;
    }

    /* Create and initialize new node*/
    struct new_movie* new_film = (struct new_movie*)malloc(sizeof(struct new_movie));
    if (new_film == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    new_film->info.mid = mid;
    new_film->info.year = year;
    new_film->category = cat;


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


/* 
 * Given the head and the tail of a struct movie SLL, creates a node 
 * with mid mid and year year and place it at the end of the SLL.
 * Time complexity: O(1)
*/
int insert_end(struct movie** head, struct movie** tail, unsigned mid, unsigned year) {
	/* Node of category list*/
	struct movie* new_node = (struct movie*)malloc(sizeof(struct movie));
	if(new_node == NULL) {
		fprintf(stderr, "Malloc error\n");
		return -1;
	}
	new_node->info.mid = mid;
	new_node->info.year = year;

    if ((*head == NULL) && (*tail == NULL)) { // Empty list
        *head = *tail = new_node;
    }
    else {
        (*tail)->next = new_node;
        (*tail) = new_node;
    }

    new_node->next = NULL; // Because it is placed at the tail

	return 1;
}

/*
 * Split new_movies_list and place it to the category array.
 * SL_array is an array of SLL heads.
 * Time complexity: O(N)
*/
void split_list() {
    struct new_movie* tmp = new_movies_list;
    struct new_movie* cur = NULL;   /* Used to deallocate new_movies_list*/
    struct movie* SL_tails[6]; 	    /* Contains the tails of each category list */
	int cat; 					    /* Movie Category*/
	int i = 0;

	/* Initialize tails */
    for (i = 0; i < 6; ++i) SL_tails[i] = NULL;

    while (tmp != NULL) {
        cur = tmp;
        tmp = tmp->next;

        cat = cur->category;

        /* Add to the proper category table element*/
        insert_end(&category_array[cat], &SL_tails[cat], cur->info.mid, cur->info.year);

        free(cur); /* Deallocate node from new_movies_list*/
    }

    new_movies_list = NULL;
}

/*
 * Given the head of a category list, search for a movie with id mid.
 * Returns the movie_info of the movie found.
 * If there is no movie with id mid, retuns a
 * struct movie_info with mid = year = -1.  
*/
struct movie_info CategoryListSearch(struct movie* head, unsigned mid) {
    struct movie* tmp = head;
    struct movie_info errorinfo = {-1, -1}; // error info

    /* 
     * Scan the whole list. Category list is sorted so we
     * don't need to scan movies with id larger than mid.
    */
    while (tmp != NULL && tmp->info.mid < mid) {
        tmp = tmp->next;
    }

    // Here tmp is NULL or has tmp->info.mid >= mid

    if (tmp == NULL || tmp->info.mid != mid) {
        return errorinfo;
    }
    else if (tmp->info.mid == mid){
        return tmp->info; // movie found
    }
}

/*
 * Scan every category list from the category table to find
 * the movie with the movie id mid.
 * 
 * Returns the movie_info of the movie with movie Id mid.
 * If there is no movie with the given mid, it returns 
 * a struct movie_info with mid = year = -1.
*/
struct movie_info CategoryArraySearch(unsigned mid) {
    struct movie_info minfo; /* Movie info to return */ 
    int i = 0;
    for (i = 0; i < 6; ++i) {
        minfo = CategoryListSearch(category_array[i], mid);

        if (minfo.mid != -1) break; // Movie found
    }
    return minfo;
}

/*********************************
 ************ PRINTS *************
 *********************************
*/

void print_new_movie_list() {
    struct new_movie* tmp = new_movies_list;
	
    printf("[");

    while(tmp != NULL) {
		printf("%d, ", tmp->info.mid);
		tmp = tmp->next;
    }
    
    printf("]\n");
}

void print_category_list(struct movie* head) {
    struct movie* tmp = head;
    
    printf("[");

    while(tmp != NULL) {
        printf("%d, ", tmp->info.mid);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

void print_table() {
	int i = 0;
	for (i = 0; i < 6; ++i) {
		printf("Category: %d: ", i);
		print_category_list(category_array[i]);
	}
	putchar('\n');
}


int main(void) {
    int i = 0;
    for (i = 0; i < 6; ++i) {
        category_array[i] = NULL;
    }

    new_movies_list = NULL;

    NewMoviesInsertSorted(10, DRAMA, 1976);
    NewMoviesInsertSorted(18, DRAMA, 1976);
    NewMoviesInsertSorted(147, COMEDY, 2014);
    NewMoviesInsertSorted(4, DRAMA, 1976);
    NewMoviesInsertSorted(711, ROMANCE, 1976);
    NewMoviesInsertSorted(235, SCIFI, 1976);
	NewMoviesInsertSorted(123, COMEDY, 1976);
    NewMoviesInsertSorted(3, DRAMA, 1976);
    NewMoviesInsertSorted(71, ROMANCE, 1976);
    NewMoviesInsertSorted(25, SCIFI, 1976);

	print_table();
	print_new_movie_list();

	split_list();

	print_table();
    print_new_movie_list();

    /*Search for a movie id*/
    struct movie_info minfo = CategoryArraySearch(1);
    printf("Mid = %d, Year = %d\n", minfo.mid, minfo.year);


	return 0;
}