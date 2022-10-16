#include "main.h"

int main(int argc, char *argv[])
{
    printf("***************  main start ***************\n");

    creader_t *cr = creader_create(10, 50, 32);
    if (creader_load(cr, "./doc/test.ini"))
        printf("creader_load err\n");

    creader_dump(cr);

    int iarray[4];

    if (creader_get_int(cr, NULL, "integer1", iarray) == 0)
        printf("creader_get_int = [%d]\n", iarray[0]);
    else
        printf("creader_get_int err\n");

    if(creader_get_int_array(cr, NULL, "integer2",iarray,4) == 4)
        printf("creader_get_int_array = [%d %d %d %d]\n", iarray[0], iarray[1], iarray[2], iarray[3]);
    else
        printf("creader_get_int_array err\n");

    double darray[4];

    if (creader_get_double(cr, NULL, "double1", darray) == 0)
        printf("creader_get_double = [%lf]\n", darray[0]);
    else
        printf("creader_get_double err\n");

    if (creader_get_double_array(cr, NULL, "double2", darray, 4) == 4)
        printf("creader_get_double_array = [%lf %lf %lf %lf]\n", darray[0], darray[1], darray[2], darray[3]);
    else
        printf("creader_get_double_array err\n");

    creader_destroy(cr);

    printf("**************** main end ****************\n");
    return 0;
}
