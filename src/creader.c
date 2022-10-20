#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "creader.h"

static char tmp_line[CR_FILELINE_MAX];

static void fskip_to_next_line(FILE *fp)
{
    char ch;
    while (1)
    {
        ch = fgetc(fp);
        switch (ch)
        {
        case EOF:
        case '\n':
            return;
        case '\r':
            ch = fgetc(fp);
            if (ch == '\n')
                return;
            else
                fseek(fp, -1, SEEK_CUR);
        default:
            break;
        }
    }
}

static void fskip_whitespace(FILE *fp)
{
    char ch;
    while (1)
    {
        ch = fgetc(fp);
        if (ch == EOF)
            break;
        else if (isgraph(ch))
        {
            fseek(fp, -1, SEEK_CUR);
            break;
        }
    }
}

static char *trim_whitespace(char *str)
{
    int len;
    if (str == NULL)
        return NULL;
    len = strlen(str);
    for (int i = len - 1; i >= 0; i--, len--)
    {
        if (isgraph(str[i]))
            break;
        else
            str[i] = '\0';
    }
    for (int i = 0; i < len; i++, str++)
    {
        if (isgraph(str[i]))
            break;
        else
            str[i] = '\0';
    }
    return str;
}

char *fdump_until(FILE *fp, char end)
{
    int len = 0;
    while (feof(fp) != EOF)
    {
        tmp_line[len] = fgetc(fp);
        if (end == '\n')
        {
            if (tmp_line[len] == '\n' || tmp_line[len] == EOF)
                break;
            else
                len++;
        }
        else
        {
            if (tmp_line[len] == end)
                break;
            else if (tmp_line[len] == '\n' || tmp_line[len] == EOF)
                return NULL;
            else
                len++;
        }
    }
    tmp_line[len] = '\0';

    char *tmp = trim_whitespace(tmp_line);
    len = strlen(tmp) + 1;

    char *ret = (char *)malloc(len);
    memcpy(ret, tmp, len);
    return ret;
}

int creader_load(creader_t *cr, const char *filename)
{
    char *sec_tmp, *name_tmp, *val_tmp;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return -1;
    }

    cr->sections = 0;
    cr->key_values = 0;
    while (feof(fp) != EOF)
    {
        fskip_whitespace(fp);

        char ch = fgetc(fp);
        if (ch == EOF)
            break;
        switch (ch)
        {
        case '#':
        case ';':
            fskip_to_next_line(fp);
            break;
        case '[':
            sec_tmp = fdump_until(fp, ']');
            if (sec_tmp)
            {
                if (cr->sections >= CR_SECTION_MAX)
                    goto end;
                cr->section_list[cr->sections] = sec_tmp;
                cr->section_index[cr->sections] = cr->key_values;
                cr->sections++;
                fskip_to_next_line(fp);
            }
            break;
        case EOF:
            goto end;
        default:
            fseek(fp, -1, SEEK_CUR);
            name_tmp = fdump_until(fp, '=');
            if (name_tmp)
            {
                val_tmp = fdump_until(fp, '\n');
                if (val_tmp)
                {
                    if (cr->key_values >= CR_KEY_VALUE_MAX)
                        goto end;
                    cr->key_list[cr->key_values] = name_tmp;
                    cr->value_list[cr->key_values] = val_tmp;
                    cr->key_values++;
                }
                else
                {
                    free(cr->key_list[cr->key_values]);
                }
            }
            break;
        }
        // creader_dump(cr);
    }
end:
    fclose(fp);
    return 0;
}

creader_t *creader_create(const char *filename)
{
    creader_t *cr = (creader_t *)malloc(sizeof(creader_t));
    cr->sections = 0;
    cr->key_values = 0;
    creader_load(cr, filename);
    return cr;
}

char *creader_get_str(creader_t *cr, const char *section, const char *key)
{
    if (cr == NULL)
        return NULL;
    int start_index = 0, end_index = cr->key_values - 1;
    if (section != NULL)
    {
        for (int i = 0; i < cr->sections; i++)
        {
            if (strcmp(section, cr->section_list[i]) == 0)
            {
                start_index = cr->section_index[i];
                if (i < cr->sections - 1)
                    end_index = cr->section_index[i + 1] - 1;
                break;
            }
        }
    }
    for (int i = end_index; i >= start_index; i--)
    {
        if (strcmp(key, cr->key_list[i]) == 0)
            return cr->value_list[i];
    }
    return NULL;
}

void creader_destroy(creader_t *cr)
{
    for (int i = 0; i < cr->sections; i++)
        free(cr->section_list[i]);
    for (int i = 0; i < cr->key_values; i++)
    {
        free(cr->key_list[i]);
        free(cr->value_list[i]);
    }
    free(cr);
}

void creader_dump(creader_t *cr)
{
    printf("creader_dump\n");
    if (cr == NULL)
    {
        printf("\tcreader_t is null\n");
        return;
    }
    printf("sections = %d\n", cr->sections);
    for (int i = 0; i < cr->sections; i++)
    {
        printf("\t%3d section [ %s ] start at [ %d ]\n", i, cr->section_list[i], cr->section_index[i]);
    }
    printf("key_values = %d\n", cr->key_values);
    for (int i = 0; i < cr->key_values; i++)
    {
        printf("\t%3d [ %s ] = [ %s ]\n", i, cr->key_list[i], cr->value_list[i]);
    }
}

static char *_str2long(const char *str, long *num)
{
    long number = 0;
    const char *cur_ptr = str;
    char *end_ptr;
    if (str == NULL || num == NULL)
        return NULL;
    while (str[0] != '\0')
    {
        cur_ptr = str;
        if (isdigit(str[0]) || (str[0] == '-' && ('0' < str[1]) && str[1] <= '9'))
        {
            number = strtol(str, &end_ptr, 0);
            if (number == 0 && end_ptr == cur_ptr)
                return NULL;
            *num = number;
            return end_ptr;
        }
        str++;
    }
    return NULL;
}

bool str2long(const char *str, long *num)
{
    return (_str2long(str, num) != NULL) ? true : false;
}

int str2longs(const char *str, long *num, int count_max)
{
    int num_count = 0;
    const char *ptr = str;
    do
    {
        ptr = _str2long(ptr, num);
        if (ptr == NULL)
            break;
        num++;
        num_count++;
    } while (num_count < count_max);
    return num_count;
}

static char *_str2double(const char *str, double *num)
{
    double number = 0;
    const char *cur_ptr = str;
    char *end_ptr;
    if (str == NULL || num == NULL)
        return 0;
    while (str[0] != '\0')
    {
        cur_ptr = str;
        if (isdigit(str[0]) || (str[0] == '-' && ('0' < str[1]) && str[1] <= '9'))
        {
            number = strtod(str, &end_ptr);
            if (number == 0 && end_ptr == cur_ptr)
                return NULL;
            *num = number;
            return end_ptr;
        }
        str++;
    }
    return NULL;
}

bool str2double(const char *str, double *num)
{
    return (_str2double(str, num) != NULL) ? true : false;
}

int str2doubles(const char *str, double *num, int count_max)
{
    int num_count = 0;
    const char *ptr = str;
    do
    {
        ptr = _str2double(ptr, num);
        if (ptr == NULL)
            break;
        num++;
        num_count++;
    } while (num_count < count_max);
    return num_count;
}

bool creader_get_int(creader_t *cr, const char *section, const char *key, int *val)
{
    long num;
    if (str2long(creader_get_str(cr, section, key), &num))
    {
        *val = (int)num;
        return true;
    }
    else
    {
        return false;
    }
}
int creader_get_int_array(creader_t *cr, const char *section, const char *key, int *val, int count_max)
{
    long num[count_max];
    int count = str2longs(creader_get_str(cr, section, key), num, count_max);
    for (int i = 0; i < count; i++)
        val[i] = (int)num[i];
    return count;
}
bool creader_get_long(creader_t *cr, const char *section, const char *key, long *val)
{
    return str2long(creader_get_str(cr, section, key), val);
}
int creader_get_long_array(creader_t *cr, const char *section, const char *key, long *val, int count_max)
{
    return str2longs(creader_get_str(cr, section, key), val, count_max);
}
bool creader_get_double(creader_t *cr, const char *section, const char *key, double *val)
{
    return str2double(creader_get_str(cr, section, key), val);
}
int creader_get_double_array(creader_t *cr, const char *section, const char *key, double *val, int count_max)
{
    return str2doubles(creader_get_str(cr, section, key), val, count_max);
}
