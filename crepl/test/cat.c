#include "animal.h"
#include <stdio.h>
int fun(void);

void print_name(const char* type)
{
    printf("Tama is a %s%d.\n", type,fun());
}
