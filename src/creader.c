#include "creader.h"

static char *_tmp_line;

static int fskip_to_next_line(FILE *fp)
{
    while (1)
    {
        char ch = fgetc(fp);
        if (ch == EOF)
            break;
        if (ch == '\n')
            break;
    }
    return 0;
}

static int fskip_whitespace(FILE *fp)
{

    while (1)
    {
        char ch = fgetc(fp);
        switch (ch)
        {
        case EOF:
            return 0;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            return 0;
        default:
            fseek(fp, -1, SEEK_CUR);
            return 0;
        }
    }
    return 0;
}

static char *trim_whitespace(char *str)
{
    int len;
    if (str == NULL)
        return NULL;
    len = strlen(str);
    for (int i = len - 1; i >= 0; i--, len--)
    {
        switch (str[i])
        {
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            str[i] = '\0';
            break;
        default:
            goto next;
        }
    }
next:
    while (*str)
    {
        switch (*str)
        {
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            *str = '\0';
            str++;
            break;
        default:
            goto end;
        }
    }
end:
    return str;
}

char *fdump_until(FILE *fp, char end)
{
    int len = 0;
    while (feof(fp) != EOF)
    {
        _tmp_line[len] = fgetc(fp);
        if (end == '\n')
        {
            if (_tmp_line[len] == '\n' || _tmp_line[len] == EOF)
                break;
            else
                len++;
        }
        else
        {
            if (_tmp_line[len] == end)
                break;
            else if (_tmp_line[len] == '\n' || _tmp_line[len] == EOF)
                return NULL;
            else
                len++;
        }
    }
    _tmp_line[len] = '\0';

    char *tmp = trim_whitespace(_tmp_line);
    len = strlen(tmp) + 1;

    char *ret = (char *)malloc(len);
    memcpy(ret, tmp, len);
    return ret;
}

creader_t *creader_create(int sections_max, int params_max, int line_max)
{
    if (line_max < 32)
        line_max = 32;
    _tmp_line = (char *)malloc(line_max);

    creader_t *cr = (creader_t *)malloc(sizeof(creader_t));

    cr->sections_max = sections_max;
    cr->params_max = params_max;

    cr->sections = 0;
    cr->params = 0;

    cr->section_list = (char **)malloc(sizeof(char *) * sections_max);
    cr->section_index = (int *)malloc(sizeof(int) * sections_max);

    cr->name_list = (char **)malloc(sizeof(char *) * params_max);
    cr->value_list = (char **)malloc(sizeof(char *) * params_max);

    return cr;
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
    cr->params = 0;
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
                if (cr->sections >= cr->sections_max)
                    goto end;
                cr->section_list[cr->sections] = sec_tmp;
                cr->section_index[cr->sections] = cr->params;
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
                    if (cr->params >= cr->params_max)
                        goto end;
                    cr->name_list[cr->params] = name_tmp;
                    cr->value_list[cr->params] = val_tmp;
                    cr->params++;
                }
                else
                {
                    free(cr->name_list[cr->params]);
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

char *creader_get(creader_t *cr, const char *section, const char *name)
{
    if (cr == NULL)
        return NULL;
    int start_index = 0, end_index = cr->params - 1;
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
        if (strcmp(name, cr->name_list[i]) == 0)
            return cr->value_list[i];
    }
    return NULL;
}

void creader_release(creader_t *cr)
{
    free(_tmp_line);

    for (int i = 0; i < cr->sections; i++)
    {
        free(cr->section_list[i]);
    }
    for (int i = 0; i < cr->params; i++)
    {
        free(cr->name_list[i]);
        free(cr->value_list[i]);
    }

    free(cr->section_list);
    free(cr->section_index);
    free(cr->name_list);
    free(cr->value_list);

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
        printf("\t%d section [%s] start at [%d]\n", i, cr->section_list[i], cr->section_index[i]);
    }
    printf("params = %d\n", cr->params);
    for (int i = 0; i < cr->params; i++)
    {
        printf("\t%d [%s] = [%s]\n", i, cr->name_list[i], cr->value_list[i]);
    }
}

int str2long(const char *str, long *number)
{
    int i, num_tmp = 0;
    char state = 0, pos_or_neg = 1;
    if (str == NULL || number == NULL)
        return -1;
    //printf("str is [%s]\n", str);
    for (i = 0; i < strlen(str); i++)
    {
        switch (state)
        {
        case 0:
            if (str[i] == '-')
            {
                pos_or_neg = -1;
                break;
            }
            else if (isdigit(str[i]))
            {
                state = 1;
            }
            else
            {
                pos_or_neg = 1;
                break;
            }
        case 1:
            if (isdigit(str[i]))
            {
                num_tmp *= 10;
                num_tmp += str[i] - '0';
            }
            else
            {
                goto end;
            }
            break;
        default:
            break;
        }
    }
end:
    if(state == 1)
    {
        *number = pos_or_neg * num_tmp;
        return i;
    }
    return -1;
}

int str2longarray(const char *str, long *number, int n)
{
    int i, offset = 0;
    if (str == NULL || number == NULL)
        return -1;
    for (i = 0; i < n; i++)
    {
        int offset_tmp = str2long(str + offset, number + i);
        if (offset_tmp > 0)
        {
            offset += offset_tmp;
        }
        else
        {
            break;
        }
    }
    return i;
}

int str2double(const char *str, double *number)
{
    int i, num_int_tmp = 0;
    double num_frac_tmp = 0, factor_tmp = 0.1;
    char state = 0, pos_or_neg = 1;
    if (str == NULL || number == NULL)
        return -1;
    //printf("str is [%s]\n", str);
    for (i = 0; i < strlen(str); i++)
    {
        switch (state)
        {
        case 0:
            if(str[i] == '-')
            {
                pos_or_neg = -1;
                break;
            }
            else if (isdigit(str[i]))
            {
                state = 1;
            }
            else
            {
                pos_or_neg = 1;
                break;
            }
        case 1:
            if (isdigit(str[i]))
            {
                num_int_tmp *= 10;
                num_int_tmp += str[i] - '0';
            }
            else if (str[i] == '.')
            {
                state = 2;
            }
            else
            {
                goto end;
            }
            break;
        case 2:
            if (isdigit(str[i]))
            {
                num_frac_tmp += (str[i] - '0') * factor_tmp;
                factor_tmp *= 0.1;
            }
            else
            {
                goto end;
            }
            break;
        default:
            break;
        }
        //printf("para [%d][%d][%d][%ld][%lf]\n", i ,state, pos_or_neg, num_int_tmp,num_frac_tmp);
    }
end:
    
    if(state > 0)
    {
        *number = pos_or_neg * (num_int_tmp + num_frac_tmp);
        return i;
    }
    return -1;
}

int str2doublearray(const char *str, double *number, int n)
{
    int i, offset = 0;
    if (str == NULL || number == NULL)
        return -1;
    for (i = 0; i < n; i++)
    {
        int offset_tmp = str2double(str + offset, number + i);
        if (offset_tmp > 0)
        {
            offset += offset_tmp;
        }
        else
        {
            break;
        }
    }
    return i;
}