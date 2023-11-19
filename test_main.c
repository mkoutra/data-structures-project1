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
    // UserListInsert(5);
    // UserListInsert(10);
    // UserListInsert(4);
    // UserListInsert(100);
    // UserListInsert(3);

    register_user(5);
    register_user(10);
    register_user(4);
    register_user(100);
    register_user(3);
    // print_user_list();

    // unregister_user(3);
    // unregister_user(5);
    // unregister_user(4);
    // register_user(3);
    // register_user(3);


    // /* Fill new movies list */
    add_new_movie(10, DRAMA, 1976);
    add_new_movie(18, DRAMA, 1976);
    add_new_movie(147, COMEDY, 2014);
    add_new_movie(4, DRAMA, 1976);
    add_new_movie(711, ROMANCE, 1976);
    add_new_movie(235, SCIFI, 1976);
	add_new_movie(123, COMEDY, 1976);
    add_new_movie(3, DRAMA, 1976);
    add_new_movie(71, ROMANCE, 1976);
    add_new_movie(25, SCIFI, 1976);
    print_new_movie_list();

    /* Distribute new movies to category table */
    // split_list();
    // print_table();
    distribute_new_movies();

    watch_movie(3, 147);
    watch_movie(100, 10);
    watch_movie(4, 123);
    watch_movie(5, 25);
    watch_movie(10, 235);

    watch_movie(10, 711);
    // print_watch_stack(user_list->watchHistory);
    // print_watch_stack(user_list->next->watchHistory);
    suggest_movies(100);
    watch_movie(10, 4);

    print_movies();
    print_users();
    // print_sug_list(user_list->next->suggestedHead);

    // watch(3, 711);
    // watch(4, 18);
    // watch(5, 71);
    // watch(10, 4);
    // suggest(100);
    // print_sug_list(user_list->next->suggestedHead);

    return 0;
}