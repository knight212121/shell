#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

char **input_completion(const char *text, int start, int end);
void create_autocomplete_cache();

#endif
