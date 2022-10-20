#ifndef __CREADER_H__
#define __CREADER_H__

#include <stdbool.h>

#define CR_SECTION_MAX 64
#define CR_KEY_VALUE_MAX 512
#define CR_FILELINE_MAX 256

typedef struct creader
{
    int sections;
    char *section_list[CR_SECTION_MAX];
    int section_index[CR_SECTION_MAX];

    int key_values;
    char *key_list[CR_KEY_VALUE_MAX];
    char *value_list[CR_KEY_VALUE_MAX];
}creader_t;

creader_t *creader_create(const char *filename);
void creader_destroy(creader_t *cr);
void creader_dump(creader_t *cr);

bool str2long(const char *str, long *num);
int str2longs(const char *str, long *num, int count_max);
bool str2double(const char *str, double *num);
int str2doubles(const char *str, double *num, int count_max);

char *creader_get_str(creader_t *cr, const char *section, const char *key);
bool creader_get_int(creader_t *cr, const char *section, const char *key, int *val);
int creader_get_int_array(creader_t *cr, const char *section, const char *key, int *val, int count_max);
bool creader_get_long(creader_t *cr, const char *section, const char *key, long *val);
int creader_get_long_array(creader_t *cr, const char *section, const char *key, long *val, int count_max);
bool creader_get_double(creader_t *cr, const char *section, const char *key, double *val);
int creader_get_double_array(creader_t *cr, const char *section, const char *key, double *val, int count_max);


#endif