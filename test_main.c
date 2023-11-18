#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"
#include "my_funcs.h"

struct user* user_list;
struct user* guard;
struct movie* category_array[6]; 	/* Each element is the head of an SLL */
struct new_movie* new_movies_list;	/* Head of new movies, SLL */

/*
 ******************************************************************************
 ********************************* MAIN ***************************************
 ******************************************************************************
*/

int main(void) {
    /* Initialize global variables */
    int i = 0;
    for (i = 0; i < 6; ++i) category_array[i] = NULL;
    
    new_movies_list = NULL;
    
    /* Initialization of Guard Node*/ 
    guard = (struct user*) malloc(sizeof(struct user));
    if (guard == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    guard->next = NULL;
    guard->suggestedHead = guard->suggestedTail = NULL;
    guard->watchHistory = NULL;
    guard->uid = -1;

    user_list = guard;

    /* Fill the user list*/
    UserListInsert(5);
    UserListInsert(10);
    UserListInsert(4);
    UserListInsert(100);
    UserListInsert(3);
    print_user_list();

    /* Fill new movies list */
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
    print_new_movie_list();

    /* Distribute new movies to category table */
    split_list();
    print_table();

    watch(3, 147);
    // watch(100, 10);
    // watch(4, 123);
    // watch(5, 25);
    // watch(10, 235);

    print_watch_stack(user_list->watchHistory);
    print_watch_stack(user_list->next->watchHistory);
    suggest(100);
    print_sug_list(user_list->next->suggestedHead);

    watch(3, 711);
    watch(4, 18);
    watch(5, 71);
    watch(10, 4);
    suggest(100);
    print_sug_list(user_list->next->suggestedHead);

    return 0;
}