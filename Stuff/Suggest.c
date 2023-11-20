#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

/*
 ******************************************************************************
 ****************************** GLOBAL VARIABLES ******************************
 ******************************************************************************
*/

struct user* user_list;
struct user* guard;
struct movie* category_array[6]; 	/* Each element is the head of an SLL */
struct new_movie* new_movies_list;	/* Head of new movies, SLL */

/*
 ******************************************************************************
 ********************************* USER LIST ********************************
 ******************************************************************************
*/

/*
 * Search in user list for a specific uid.
 * Returns 1 if the uid is already inside the list, 0 otherwise.
*/
int UserListSearch(int uid) {
    /* Insert the value inside the guard node*/
    guard->uid = uid;
    
    struct user* tmp = user_list;
    while (tmp->uid != uid) {
        tmp = tmp->next;
    }

    /* Bring guard back to initial state*/
    guard->uid = -1;

    return (tmp != guard); /* True if value was not found. */
}

/* Returns a pointer to the node of user_list with id uid, else NULL*/
struct user* FindUserList(int uid) {
    /* Insert the value inside the guard node*/
    guard->uid = uid;
    
    struct user* tmp = user_list;
    while (tmp->uid != uid) {
        tmp = tmp->next;
    }

    /* Bring guard back to initial state*/
    guard->uid = -1;

    return (tmp != guard) ? tmp : NULL;
}

/*
 * Insert inside the user list. Return 1 on success, -1 otherwise.
*/
int UserListInsert(int uid) {
    struct user* new_user = (struct user*) malloc(sizeof(struct user));
    if (new_user == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    new_user->uid = uid;
    new_user->suggestedHead = NULL;
    new_user->suggestedTail = NULL;
    new_user->watchHistory = NULL;

    /* Check if uid is already inside the list*/
    if (UserListSearch(uid)) {
        fprintf(stderr, "User %d is already in the list\n", uid);
        return -1;
    }

    /* Insert new user at the head of the list. */
    new_user->next = user_list;
    user_list = new_user;
    
    return 1;
}

/*
 ******************************************************************************
 ******************************* NEW MOVIES LIST ******************************
 ******************************************************************************
*/

/* Insert to a sorted list in ascending order.*/
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
 ******************************************************************************
 ******************************* CATEGORY TABLE *******************************
 ******************************************************************************
*/

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

    if ((*head == NULL) && (*tail == NULL)) { /* Empty list */
        *head = *tail = new_node;
    }
    else {
        (*tail)->next = new_node;
        (*tail) = new_node;
    }

    new_node->next = NULL; /* Because it is placed at the tail */

	return 1;
}

/*
 * Split new_movies_list and place it to the category array.
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
    struct movie_info errorinfo = {-1, -1}; /* error info */

    /* 
     * Scan the whole list. Category list is sorted so we
     * don't need to scan movies with id larger than mid.
    */
    while (tmp != NULL && tmp->info.mid < mid) {
        tmp = tmp->next;
    }

    /* Here tmp is NULL or has tmp->info.mid >= mid */

    if (tmp == NULL || tmp->info.mid != mid) {
        return errorinfo;
    }
    else if (tmp->info.mid == mid){
        return tmp->info; /* movie found */
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

        if (minfo.mid != -1) break; /* Movie found */
    }
    return minfo;
}

/*
 ******************************************************************************
 ********************************* WATCH STACK ********************************
 ******************************************************************************
*/

int IsEmptyWatchStack(struct movie* S) { return (S == NULL); }

/*
 * Returns the movie_info of the top node of the watch stack.
 * If the stack is empty, returns a struct movie_info with mid = year = -1.
*/
struct movie_info Top(struct movie* S) {
    if (IsEmptyWatchStack(S)) {
        struct movie_info errorinfo = {-1, -1}; /* Info to return when an error has occurred.*/
        fprintf(stderr, "Nothing on top, watch stack is empty.\n");
        return errorinfo;
    }

    return S->info;
}

/* 
 * Push a movie node with movie info minfo to watch stack.
 * Returns 1 on success, otherwise -1.
*/ 
int Push(struct movie** S, struct movie_info minfo) {
    struct movie* new_film = (struct movie*) malloc(sizeof(struct movie));
    
    if (new_film == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }

    new_film->info = minfo;
    new_film->next = (*S);

    (*S) = new_film;
    return 1;
}

/*
 * Returns the movie info of the top node and removes it from the watch stack.
 * If the stack is empty, returns a struct movie_info with mid = year = -1.
*/
struct movie_info Pop(struct movie** S) {
    if (IsEmptyWatchStack(*S)) {
        struct movie_info errorinfo = {-1, -1}; /* Info to return when an error has occurred. */
        fprintf(stderr, "Nothing to pop, watch stack is empty.\n");
        return errorinfo;
    }
    
    struct movie* tmp = (*S);           /* Node to delete. */
    struct movie_info minfo = Top(*S);  /* Data to return. */

    (*S) = (*S)->next;
    
    free(tmp);                          /* Deallocate node. */

    return minfo;
}

/*
 ******************************************************************************
 ******************************* PRINTS ***************************************
 ******************************************************************************
*/

/* Print User list*/
void print_user_list() {
    struct user* tmp = user_list;
    
    printf("[");

    while(tmp != NULL) {
        printf("%d, ", tmp->uid);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

/* Print New movies list*/
void print_new_movie_list() {
    struct new_movie* tmp = new_movies_list;
	
    printf("[");

    while(tmp != NULL) {
		printf("%d, ", tmp->info.mid);
		tmp = tmp->next;
    }
    
    printf("]\n");
}

/* Print a single category list with head given*/
void print_category_list(struct movie* head) {
    struct movie* tmp = head;
    
    printf("[");

    while(tmp != NULL) {
        printf("%d, ", tmp->info.mid);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

/* Print the whole category table*/
void print_table() {
	int i = 0;
	for (i = 0; i < 6; ++i) {
		printf("Category: %d: ", i);
		print_category_list(category_array[i]);
	}
	putchar('\n');
}

/* Print the watch stack given*/
void print_watch_stack (struct movie* S) {
    struct movie* tmp = S;

    printf("[");

    while(tmp != NULL) {
        printf("(%d, %d), ", tmp->info.mid, tmp->info.year);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

/*
 ******************************************************************************
 ******************************** WATCH ***************************************
 ******************************************************************************
*/

/* Look for uid and mid. Add mid to uid's watch stack.*/
int watch(int uid, unsigned mid) {
    struct user* user_node;
    struct movie_info minfo;

    /* Find user node from user_list */
    user_node = FindUserList(uid);
    if (user_node == NULL) {
        fprintf(stderr, "User %d was not found\n", uid);
        return -1;
    }

    /* Find movie info from category table */
    minfo = CategoryArraySearch(mid);
    if (minfo.mid == -1) {
        fprintf(stderr, "Movie %d was not found\n", mid);
        return -1;
    }

    /* Create a movie node and push it to user's watch stack*/
    struct movie* new_film = (struct movie*) malloc(sizeof(struct movie));
    Push(&(user_node->watchHistory), minfo);
    return 1;
}

/*
 ******************************************************************************
 ******************************* SUGGEST **************************************
 ******************************************************************************
*/

/*
 * Insert a new node with info minfo to the right(next) of curr node.
 * Head and tail might need to change, so we pass them as index-to-index.
 * Also, it updates curr. 
 */
int InsertRight(struct suggested_movie** curr, struct movie_info minfo,\
                 struct suggested_movie** head, struct suggested_movie** tail) {
    
    /* Create suggested_movie node */
    struct suggested_movie* new_sug_mov = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
    if (new_sug_mov == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    new_sug_mov->info = minfo;
    new_sug_mov->next = NULL;
    new_sug_mov->prev = NULL;

    // Initially Empty DLL
    if ((*curr) == NULL) {
        (*head) = new_sug_mov;
        (*curr) = new_sug_mov;
        (*tail) = new_sug_mov;
        return 1;
    }

    /* curr's next before insertion of new node */
    struct suggested_movie* q = (*curr)->next;
    
    new_sug_mov->prev = (*curr);
    new_sug_mov->next = q;

    (*curr)->next = new_sug_mov;
    
    if (q == NULL) (*tail) = new_sug_mov;
    else q->prev = new_sug_mov;

    (*curr) = new_sug_mov;  // Move curr to the right

    return 1;
}

/*
 * Insert a new node with info minfo to the left(prev) of curr node.
 * Head and tail might need to change, so we pass them as index-to-index.
 * Also, it updates curr. 
 */
int InsertLeft(struct suggested_movie** curr, struct movie_info minfo,\
                 struct suggested_movie** head, struct suggested_movie** tail) {
    
    /* Create suggested_movie node */
    struct suggested_movie* new_sug_mov = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
    if (new_sug_mov == NULL) {
        fprintf(stderr, "Malloc error\n");
        return -1;
    }
    new_sug_mov->info = minfo;

    /* The second element of an initially empty list 
       Here, head and tail point to te same node */
    if ((*curr) == NULL) {
        (*head)->next = new_sug_mov;
        new_sug_mov->prev = (*head);
        
        new_sug_mov->next = NULL;

        (*curr) = new_sug_mov;
        (*tail) = new_sug_mov;
        return 1;
    }

    /* curr's prev before insertion of new node */
    struct suggested_movie* q = (*curr)->prev;
    
    new_sug_mov->prev = q;
    new_sug_mov->next = (*curr);

    (*curr)->prev = new_sug_mov;
    
    if (q == NULL) (*head) = new_sug_mov;
    else q->next = new_sug_mov;

    (*curr) = new_sug_mov;  // Move curr to the left

    return 1;
}

/* 
 * Function for event S. Suggest movie to user uid 
 * based an the watch history of others.
 */
int suggest(int uid) {
    struct user* target_user;
    struct user* tmp_user = user_list;
    struct movie_info minfo;
    int u_counter = 0;  /* count users whose pop was valid */
    int check; /* Check the output of InsertRight and Left below*/

    /* The new node will be added to the right(next) of this node*/
    struct suggested_movie* to_right;

    /* The new node will be added to the left(prev) of this node*/
    struct suggested_movie* to_left;

    /* Find target user */
    target_user = FindUserList(uid);
    if (target_user == NULL) {
        fprintf(stderr, "User %d was not found\n", uid);
        return -1;
    }

    to_right = target_user->suggestedHead;
    to_left  = target_user->suggestedTail;

    /*  Scan user_list */
    while(tmp_user->uid != -1) {
        if (tmp_user->uid != uid) {
            minfo = Pop(&tmp_user->watchHistory);
            
            /* This user has nothing on his watch history */
            if (minfo.mid == -1) {
                tmp_user = tmp_user->next; /* go to next user */
                continue; /* go back to the while-loop */
            }
            
            /* Increase user counter */
            u_counter++;
            
            // Insert to the right
            if ((u_counter % 2 == 1)) {
                check = InsertRight(&to_right, minfo, &target_user->suggestedHead, \
                            &target_user->suggestedTail);
                
                if (check == -1) {
                    fprintf(stderr, "Problem with InsertRight\n");   
                    return -1;
                }
            }
            else {
                check = InsertLeft(&to_left, minfo, &target_user->suggestedHead,\
                           &target_user->suggestedTail);
                
                if (check == -1) {
                    fprintf(stderr, "Problem with InsertLeft\n");   
                    return -1;
                }
            }

            tmp_user = tmp_user->next;
        }
        /* tmp_user is uid */
        else {
            tmp_user = tmp_user->next;
            continue; /* go back to the while-loop */
        }
    }    

    return 1;
}

void print_sug_list(struct suggested_movie* head) {
    struct suggested_movie* tmp = head;
    while (tmp != NULL) {
        printf("%d, ", tmp->info.mid);
        tmp = tmp->next;
    }
    putchar('\n');
}

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
    /*
    Stack tests

    struct movie* stack = NULL;
    
    for (int i = 1; i < 500; i += 100) {
        struct movie_info info = {i + 10, 1990 + i/100};
        Push(&stack, info);
    }

    print_watch_stack(stack);

    struct movie_info pop_info = Pop(&stack);

    printf("Pop info = %d, %d\n", pop_info.mid, pop_info.year);
    print_watch_stack(stack);

    struct movie_info top_info = Top(stack);

    printf("Top info = %d, %d\n", top_info.mid, top_info.year);
    print_watch_stack(stack);

    for (int i = 10; i >= 1; --i){
        struct movie_info a = Pop(&stack);
        printf("%d, %d\n", a.mid, a.year);
        print_watch_stack(stack);
    }
    */
}