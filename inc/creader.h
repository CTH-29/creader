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
    char **name_list;
    char **value_list;
}creader_t;

creader_t *creader_create(int sections_max, int params_max, int line_max);
int creader_load(creader_t *cr, const char *filename);

void creader_release(creader_t *cr);
void creader_dump(creader_t *cr);

char *creader_get(creader_t *cr, const char *section, const char *name);

int str2long(const char *str, long *number);
int str2longarray(const char *str, long *number, int n);

int str2double(const char *str, double *number);
int str2doublearray(const char *str, double *number, int n);

#endif