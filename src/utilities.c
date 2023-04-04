#include "./functions.h"

char *IntToString(int num)
{
    char *str = (char *)malloc(10 * sizeof(char));
    sprintf(str, "%d", num);
    return str;
}

char *FloatToString(float num)
{
    char *str = (char *)malloc(10 * sizeof(char));
    sprintf(str, "%f", num);
    return str;
}