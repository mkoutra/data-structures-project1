#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

/*
 ******************************************************************************
 ****************************** GLOBAL VARIABLES ******************************
 ******************************************************************************
*/

extern struct user* user_list;
extern struct user* guard;
extern struct movie* category_array[6]; 	/* Each element is the head of an SLL */
extern struct new_movie* new_movies_list;	/* Head of new movies, SLL */

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
    
    return 0;
}

void CleanSuggestedMovies(struct suggested_movie** head, struct suggested_movie** tail) {
    struct suggested_movie* n; /* Next */
    struct suggested_movie* tmp = (*head);

    while (tmp != NULL) {
        n = tmp->next;
        free(tmp);
        tmp = n;
    }

    (*head) = NULL;
    (*tail) = NULL;
}

/* Deallocate all stack nodes and the stack. */
void CleanStack(struct movie** S) {
    struct movie* n;            /* next node */
    struct movie* tmp = (*S);   /* save the head */

    while (tmp != NULL) {
        n = tmp->next;  /* save next node */
        free(tmp);      /* deallocate */
        tmp = n;
    }

    (*S) = NULL;
}

/* Remove a user from the user_list and deallocate suggested DLL and stack */
void DeleteUser(int uid) {
    struct user* tmp = user_list;
    struct user* prev = NULL;
    
    /* Search for uid */
    guard->uid = uid;
    while (tmp->uid != uid) {
        prev = tmp;
        tmp = tmp->next;
    }

    guard->uid = -1; /* Restore guard value*/

    /* User does not exist*/
    if (tmp == guard) {
        fprintf(stderr, "User %d does not exist\n", uid);   
        return;
    }
    
    /* Clean suggested movies DLL and watchHistory */
    CleanSuggestedMovies(&tmp->suggestedHead, &tmp->suggestedTail);
    CleanStack(&tmp->watchHistory);

    if (tmp == user_list) {     /* uid is the head node */
        user_list = tmp->next;  /* Update list head*/
    }
    else {                      /* uid is a regular node*/
        prev->next = tmp->next;
    }
    
    free(tmp);
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

    return 0;
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
 * Returns 0 on success, otherwise -1.
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
    return 0;
}

/*
 * Returns the movie info of the top node and removes it from the watch stack.
 * If the stack is empty, returns a struct movie_info with mid = year = -1.
*/
struct movie_info Pop(struct movie** S) {
    if (IsEmptyWatchStack(*S)) {
        struct movie_info errorinfo = {-1, -1}; /* Info to return when an error has occurred. */
        /* fprintf(stderr, "Nothing to pop, watch stack is empty.\n"); */
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

    printf("Users = ");
    
    while(tmp != guard) {
        printf("<%d>", tmp->uid);
        tmp = tmp->next;
        if (tmp != guard) printf(", ");
    }

    putchar('\n');
}

/* Print New movies list*/
void print_new_movie_list() {
    struct new_movie* tmp = new_movies_list;
	
    printf("New movies = ");

    while(tmp != NULL) {
		printf("<%d, %d, %d>", tmp->info.mid, tmp->category, tmp->info.year);
		tmp = tmp->next;
        if (tmp != NULL) printf(", ");
    }
    
    putchar('\n');
}

/* Print a single category list with head given*/
void print_category_list(struct movie* head) {
    struct movie* tmp = head;
    
    while(tmp != NULL) {
        printf("<%d>", tmp->info.mid);
        tmp = tmp->next;
        if (tmp != NULL) printf(", ");
    }

    putchar('\n');
}

/* Print the whole category table*/
void print_table() {
    char* cat_names[6] = {"Horror", "Sci-fi", "Drama",\
                          "Romance", "Documentary", "Comedy"};
	int i = 0;
	for (i = 0; i < 6; ++i) {
		printf("  %s: ", cat_names[i]);
		print_category_list(category_array[i]);
	}
}

/* Print the watch stack given*/
void print_watch_stack (struct movie* S) {
    struct movie* tmp = S;

    printf("Watch History = ");

    while(tmp != NULL) {
        printf("<%d>", tmp->info.mid);
        tmp = tmp->next;
        if (tmp != NULL) printf(", ");
    }
    
    putchar('\n');
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
    return 0;
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

    /* Initially Empty DLL */
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

    (*curr) = new_sug_mov;  /* Move curr to the right */

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

    (*curr) = new_sug_mov;  /* Move curr to the left */

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
            
            /* Insert to the right */
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

    return 0;
}

void print_sug_list(struct suggested_movie* head) {
    struct suggested_movie* tmp = head;

    printf("Suggested Movies = ");

    while (tmp != NULL) {
        printf("<%d>", tmp->info.mid);
        tmp = tmp->next;
        if (tmp != NULL) printf(", ");
    }

    putchar('\n');
}

/*
 ******************************************************************************
 ****************************** FILTERING *************************************
 ******************************************************************************
*/

/*
 * Insert to the tail of a doubly linked list a node with movie info minfo.
 * Returns 0 on success, -1 otherwise.
*/
int InsertDLLTail(struct movie_info minfo, struct suggested_movie** head,\
                    struct suggested_movie** tail) {
    struct suggested_movie* new_node = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
    if (new_node == NULL) {
        fprintf(stderr, "Malloc Error\n");
        return -1;
    }
    new_node->info = minfo;
    
    new_node->next = NULL;
    new_node->prev = (*tail);

    if (((*head) == NULL) && ((*tail) == NULL)) {   /* Empty list */
        (*head) = new_node;
    }
    else {                                          /* Not empty list */
        (*tail)->next = new_node;
    }

    (*tail) = new_node;

    return 0;
}

/*
 ******************************************************************************
 *************************** TAKE OFF MOVIE ***********************************
 ******************************************************************************
*/

/* Remove movie from category table*/
void RemoveFromTable(unsigned mid) {
    struct movie* cat_list_tmp = NULL;
    struct movie* cat_list_prev = NULL;
    int i = 0;
    for (i = 0; i < 6; ++i) {
        cat_list_tmp = category_array[i];
        
        /* Scan the category list*/
        while ((cat_list_tmp != NULL) && (cat_list_tmp->info.mid < mid)) {
            cat_list_prev = cat_list_tmp;
            cat_list_tmp = cat_list_tmp->next;
        }

        /* mid found */
        if ((cat_list_tmp != NULL) && (cat_list_tmp->info.mid == mid)) {
            /* mid is the head node */
            if (cat_list_tmp == category_array[i]) {
                category_array[i] = cat_list_tmp->next; /* Update list head*/
            }
            /* mid is a regular node*/
            else cat_list_prev->next = cat_list_tmp->next;

            free (cat_list_tmp);
            printf("  Category list = ");
            print_category_list(category_array[i]);
            break; /* for loop*/
        }
    }
}

/*
 * Remove movie with mid from suggested list DLL described by head and tail.
 * Returns 0 on success, -1 otherwise.
*/
int RemoveFromSuggList(unsigned mid, struct suggested_movie** head,\
                       struct suggested_movie** tail) {
    struct suggested_movie* tmp = (*head);

    /* Search for mid*/
    while (tmp != NULL && (tmp->info.mid != mid)) tmp = tmp->next;

    if (tmp == NULL) return -1; /* mid not found*/

    if ((tmp == (*head)) && (tmp == (*tail))) { /* mid is the only node */
        (*head) = (*tail) = NULL;               /* Update head and tail*/
    }
    else if (tmp == (*head)) {                  /* mid is the head */
        (*head) = tmp->next;                    /* Update head */
        (*head)->prev = NULL;
    }
    else if (tmp == (*tail)) {                  /* mid is the tail*/
        (*tail) = tmp->prev;                    /* Update tail */
        (*tail)->next = NULL;
    }
    else {                                      /* mid is a regular node*/
        (tmp->prev)->next = tmp->next;
        (tmp->next)->prev = tmp->prev;
    }

    free(tmp);
    return 0;
}

/*
 ******************************************************************************
 *************************** EVENT FUNCTIONS **********************************
 ******************************************************************************
*/

/*
 * Register User - Event R
 * 
 * Adds user with ID uid to
 * users list, as long as no
 * other user with the same uid
 * already exists.
 *
 * Returns 0 on success, -1 on
 * failure (user ID already exists,
 * malloc or other error)
 */
int register_user(int uid) {
    int code = UserListInsert(uid);
    
    printf("R <%d>\n  ", uid);
    print_user_list();
    printf("DONE\n");

    return code;
}

/*
 * Unregister User - Event U
 *
 * Removes user with ID uid from
 * users list, as long as such a
 * user exists, after clearing the
 * user's suggested movie list and
 * watch history stack
 */
void unregister_user(int uid) {
    DeleteUser(uid);

    printf("U <%d>\n  ", uid);
    print_user_list();
    printf("DONE\n");
}

/*
 * Add new movie - Event A
 *
 * Adds movie with ID mid, category
 * category and release year year
 * to new movies list. The new movies
 * list must remain sorted (increasing
 * order based on movie ID) after every
 * insertion.
 *
 * Returns 0 on success, -1 on failure
 */
int add_new_movie(unsigned mid, movieCategory_t category, unsigned year) {
    int code = NewMoviesInsertSorted(mid, category, year);
    
    if (code == 0) {
        printf("A <%d> <%d> <%d>\n  ", mid, category, year);
        print_new_movie_list();
        printf("DONE\n");
    }

    return code;
}

/*
 * Distribute new movies - Event D
 *
 * Distributes movies from the new movies
 * list to the per-category sorted movie
 * lists of the category list array. The new
 * movies list should be empty after this
 * event. This event must be implemented in
 * O(n) time complexity, where n is the size
 * of the new movies list
 */
void distribute_new_movies(void) {
    split_list();

    printf("D\nCategorized Movies:\n");
    print_table();
    printf("DONE\n");
}

/*
 * User watches movie - Event W
 *
 * Adds a new struct movie with information
 * corresponding to those of the movie with ID
 * mid to the top of the watch history stack
 * of user uid.
 *
 * Returns 0 on success, -1 on failure
 * (user/movie does not exist, malloc error)
 */
int watch_movie(int uid, unsigned mid) {
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

    printf("W <%d>, <%d>\n  ", uid, mid);
    printf("User <%d> ", uid);
    print_watch_stack(user_node->watchHistory);
    printf("DONE\n");

    return 0;
}

/*
 * Suggest movies to user - Event S
 *
 * For each user in the users list with
 * id != uid, pops a struct movie from the
 * user's watch history stack, and adds a
 * struct suggested_movie to user uid's
 * suggested movies list in alternating
 * fashion, once from user uid's suggestedHead
 * pointer and following next pointers, and
 * once from user uid's suggestedTail pointer
 * and following prev pointers. This event
 * should be implemented with time complexity
 * O(n), where n is the size of the users list
 *
 * Returns 0 on success, -1 on failure
 */
int suggest_movies(int uid) {
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
    while(tmp_user != guard) {
        if (tmp_user->uid != uid) {
            minfo = Pop(&tmp_user->watchHistory);
            
            /* This user has nothing on his watch history */
            if (minfo.mid == -1) {
                tmp_user = tmp_user->next; /* go to next user */
                continue; /* go back to the while-loop */
            }
            
            /* Increase user counter */
            u_counter++;
            
            /* Insert to the right */
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

    printf("S <%d>\n  ", uid);
    printf("User <%d> ", uid);
    print_sug_list(target_user->suggestedHead);
    printf("DONE\n");

    return 0;
}

/*
 * Filtered movie search - Event F
 *
 * User uid asks to be suggested movies
 * belonging to either category1 or category2
 * and with release year >= year. The resulting
 * suggested movies list must be sorted with
 * increasing order based on movie ID (as the
 * two category lists). This event should be
 * implemented with time complexity O(n + m),
 * where n, m are the sizes of the two category lists
 *
 * Returns 0 on success, -1 on failure
 */
int filtered_movie_search(int uid, movieCategory_t category1,
		movieCategory_t category2, unsigned year) {
    struct user* target_user;

    /* Pointers to the head of each category list*/
    struct movie* cat1 = category_array[category1];
    struct movie* cat2 = category_array[category2];
    
    /* Pointers for the DLL we will create*/
    struct suggested_movie* new_head = NULL;
    struct suggested_movie* new_tail = NULL;

    /* Check the exit code of InsertDLLTail()*/
    int code = 0;
    
    /* Find the user with id uid*/
    target_user = FindUserList(uid);
    if (target_user == NULL) {
        fprintf(stderr, "User %d does not exist.\n", uid);
        return -1;
    }
    
    while ((cat1 != NULL) && (cat2 != NULL)) {
        /* Check if years are valid*/
        if (cat1->info.year < year) {
            cat1 = cat1->next;
            continue;
        }
        if (cat2->info.year < year) {
            cat2 = cat2->next;
            continue;
        }

        /* Here both cat1 and cat2 point to movies with valid year*/

        if (cat1->info.mid < cat2->info.mid) { /* mid_1 < mid_2 */
            /* Add to the tail of new DLL */
            code = InsertDLLTail(cat1->info, &new_head, &new_tail);
            if (code == -1) return code;
            
            cat1 = cat1->next;
        }
        else { /*mid_2 < mid_1*/
            /* Add to the tail of new DLL */
            code = InsertDLLTail(cat2->info, &new_head, &new_tail);
            if (code == -1) return code;

            cat2 = cat2->next;
        }
    }
    /* 
     * Here one of the cat pointers points to NULL.
     * This is the cat pointer of the list whose 
     * max(mid value) < max(mid value) of the other list .
    */

    if (cat1 == NULL) {
        /* Insert the remaining movies with valid year from cat2 */
        while(cat2 != NULL) {
            if (cat2->info.year > year) {
                code = InsertDLLTail(cat2->info, &new_head, &new_tail);
                if (code == -1) return code;
            }
            cat2 = cat2->next;
        }
    }
    else { /*cat2 == NULL*/
        /* Insert the remaining movies with valid year from cat1 */
        while(cat1 != NULL) {
            if (cat1->info.year > year) {
                code = InsertDLLTail(cat1->info, &new_head, &new_tail);
                if (code == -1) return code;
            }
            cat1 = cat1->next;
        }
    }
    /* Connect new DLL to the suggested DLL of the user*/

    /* Suggested list was empty*/
    if ((target_user->suggestedHead == NULL) && (target_user->suggestedTail == NULL)) {
        /* The suggested DLL is the same as the new DLL*/
        target_user->suggestedHead = new_head;
        target_user->suggestedTail = new_tail;
    }
    else {
        /* Connect the tail of existing suggested DLL to the head of the new one*/
        target_user->suggestedTail->next = new_head;
        new_head->prev = target_user->suggestedTail;

        /* Update the tail of the target user*/
        target_user->suggestedTail = new_tail;
    }

    printf("F <%d> <%d> <%d> <%d>\n", uid, category1, category2, year);
    printf("   User <%d> ", uid);
    print_sug_list(target_user->suggestedHead);
    printf("DONE\n");
    return 0;
}

/*
 * Take off movie - Event T
 *
 * Movie mid is taken off the service. It is removed
 * from every user's suggested list -if present- and
 * from the corresponding category list.
 */
void take_off_movie(unsigned mid) {
    struct user* user_tmp = user_list;

    printf("T <%d>\n", mid);
    
    /* Remove from suggested lists*/
    while (user_tmp != guard) {
        if (RemoveFromSuggList(mid, &user_tmp->suggestedHead,\
                                &user_tmp->suggestedTail) == 0) {
            printf("   <%d> removed from <%d> suggested list.\n", mid, user_tmp->uid);
        }
        user_tmp = user_tmp->next;
    }

    /* Remove from category list*/
    RemoveFromTable(mid);
    printf("DONE\n");
}

/*
 * Print movies - Event M
 *
 * Prints information on movies in
 * per-category lists
 */
void print_movies(void) {
    printf("M\nCategorized Movies:\n");
    print_table();
    printf("DONE\n");
}

/*
 * Print users - Event P
 *
 * Prints information on users in
 * users list
 */
void print_users(void) {
    struct user* tmp = user_list;
    struct suggested_movie* sug_tmp;

    printf("P\nUsers:\n");

    while (tmp != guard) {
        /* Print Suggested movies */
        sug_tmp = tmp->suggestedHead;
        printf("  <%d>:\n", tmp->uid);
        printf("   Suggested: ");

        while (sug_tmp != NULL) {
            printf("<%d>", sug_tmp->info.mid);
            sug_tmp = sug_tmp->next;
            if (sug_tmp != NULL) printf(", ");
        }
        putchar('\n');

        /* Print Watch History*/
        printf("   ");
        print_watch_stack(tmp->watchHistory);

        tmp = tmp->next;
    }
    printf("DONE\n");
}