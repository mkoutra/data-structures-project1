#ifndef CLEANING_FUNCTIONS_H
#define CLEANING_FUNCTIONS_H

#include "streaming_service.h"

/* Deallocate all nodes from the suggested movie list given*/
void CleanSuggestedMovies(struct suggested_movie** head, struct suggested_movie** tail);

/* Deallocate all stack nodes and the stack. */
void CleanStack(struct movie** S);

/* Deallocate all nodes of the category list given*/
void CleanCategoryList(struct movie** L);

/* Deallocate all nodes of the new movies list given*/
void CleanNewMoviesList(struct new_movie** L);

#endif /* CLEANING_FUNCTIONS_H*/