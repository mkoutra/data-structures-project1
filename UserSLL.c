// User SLL wit guard node
#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

struct user* user_list;
struct user* guard;

/*
 * Returns 1 if uid is already inside the list, 0 otherwise.
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

    return (tmp != guard); // True if value was not found.
}

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

    // Insert new user at the head of the list.
    new_user->next = user_list;
    user_list = new_user;
    
    return 1;
}

void print_list() {
    struct user* tmp = user_list;
    
    printf("[");

    while(tmp != NULL) {
        printf("%d, ", tmp->uid);
        tmp = tmp->next;
    }
    
    printf("]\n");
}

int main(void) {
    // Initialize guard node
    guard = (struct user*)malloc(sizeof(struct user));
    guard->uid = -1;
    guard->suggestedHead = NULL;
    guard->suggestedTail = NULL;
    guard->watchHistory = NULL;
    guard->next = NULL;

    /* Empty SLL contains only the guard node*/
    user_list = guard;

    printf("%d\n", UserListInsert(5));
    printf("%d\n", UserListInsert(10));
    print_list();

    printf("%d\n", UserListInsert(4));
    printf("%d\n", UserListInsert(4));
    printf("%d\n", UserListInsert(100));

    print_list();

    return 0;
}