#include "main.h"

int main(int argc, char *argv[])
{
    int ret;
    char str[256];
    printf("***************  main start ***************\n");

    creader_t *cr = creader_create(10, 50, 32);

    if (creader_load(cr, "/home/taihua.chen/projs/mytest/creader/doc/test.ini"))
        printf("creader_load err");

    creader_dump(cr);

    long iarray[4];

    if (str2long(creader_get(cr, NULL, "integer1"), iarray) > 0)
        printf("str2long = [%ld]\n", iarray[0]);
    else
        printf("str2long err\n");

    if ((ret = str2longarray(creader_get(cr, NULL, "integer2"), iarray, 4)) == 4)
        printf("str2longarray = [%ld %ld %ld %ld]\n", iarray[0], iarray[1], iarray[2], iarray[3]);
    else
        printf("str2longarray err\n");

    double darray[4];

    if (str2double(creader_get(cr, NULL, "double1"), darray) > 0)
        printf("str2double = [%lf]\n", darray[0]);
    else
        printf("str2double err\n");

    if ((ret = str2doublearray(creader_get(cr, NULL, "double2"), darray, 4)) == 4)
        printf("creader_get = [%lf %lf %lf %lf]\n", darray[0], darray[1], darray[2], darray[3]);
    else
        printf("str2doublearray err\n");

    creader_release(cr);

    printf("**************** main end ****************\n");
    return 0;
}
