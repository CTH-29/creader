#ifndef __CREADER_H__
#define __CREADER_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct _creader_t
{
    int sections_max;
    int params_max;

    int sections;
    char **section_list;
    int *section_index;

    int params;
    char **key_list;
    char **value_list;
}creader_t;

creader_t *creader_create(void);
int creader_load(creader_t *cr, const char *filename);

void creader_destroy(creader_t *cr);

void creader_dump(creader_t *cr);

char *creader_get_str(creader_t *cr, const char *section, const char *key);
int creader_get_int(creader_t *cr, const char *section, const char *key, int *val);
int creader_get_int_array(creader_t *cr, const char *section, const char *key, int *val, int max_count);
int creader_get_double(creader_t *cr, const char *section, const char *key, double *val);
int creader_get_double_array(creader_t *cr, const char *section, const char *key, double *val, int max_count);


#endif