/**************************************************************************/
/* program.c --- generate switch statements to identify string            */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "make_switch.h"

/**************************************************************************/
/* find file title */

const char *find_file_title(const char *s)
{
    const char *p;
    for (;;)
    {
        p = s + strcspn(s, "\\/");
        if (*p == 0)
            break;
        s = p + 1;
    }
    return s;
}

/**************************************************************************/
/* the main function */

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        const char *program = find_file_title(argv[0]);
        printf("Usage: %s string1 string2 ...", program);
        return 0;
    }

    ms_make_identify_string_function(argc - 1, &argv[1]);

    return 0;
}

/**************************************************************************/
