#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include "streaming_service.h"

int UserListSearch(int uid);
struct user* FindUserList(int uid);
int UserListInsert(int uid);

/*
 ******************************************************************************
 ******************************* CATEGORY TABLE *******************************
 ******************************************************************************
*/

int NewMoviesInsertSorted(unsigned mid, movieCategory_t cat, unsigned year);
int insert_end(struct movie** head, struct movie** tail, unsigned mid, unsigned year);
void split_list();
struct movie_info CategoryListSearch(struct movie* head, unsigned mid);
struct movie_info CategoryArraySearch(unsigned mid);

/*
 ******************************************************************************
 ********************************* WATCH STACK ********************************
 ******************************************************************************
*/
int IsEmptyWatchStack(struct movie* S);
struct movie_info Top(struct movie* S);
int Push(struct movie** S, struct movie_info minfo);
struct movie_info Pop(struct movie** S);

/*
 ******************************************************************************
 ******************************* PRINTS ***************************************
 ******************************************************************************
*/
void print_user_list();
void print_new_movie_list();
void print_category_list(struct movie* head);
void print_table();
void print_watch_stack (struct movie* S);

/*
 ******************************************************************************
 ******************************** WATCH ***************************************
 ******************************************************************************
*/
int watch(int uid, unsigned mid);
int InsertRight(struct suggested_movie** curr, struct movie_info minfo,\
                 struct suggested_movie** head, struct suggested_movie** tail);
int InsertLeft(struct suggested_movie** curr, struct movie_info minfo,\
                 struct suggested_movie** head, struct suggested_movie** tail);
int suggest(int uid);
void print_sug_list(struct suggested_movie* head);
#endif /* MY_FUNCTIONS_H*/