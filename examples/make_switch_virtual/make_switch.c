/**************************************************************************/
/* make_switch.c --- generate switch statements to identify string        */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "make_switch.h"
#include "v_file_wrap.h"

/**************************************************************************/
/* configuration */

/* #define MS_TAB_WIDTH 3 */
#define MS_TAB_WIDTH 4
/* #define MS_TAB_WIDTH 8 */
/* #define MS_TAB_WIDTH -1 */   /* "\t" */

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************/

/* make indent */
void ms_make_indent(int n)
{
    int i;
#if (MS_TAB_WIDTH < 0)
    /* print n tabs out */
    for (i = 0; i < n; ++i)
    {
        putchar('\t');
    }
#else   /* !(MS_TAB_WIDTH < 0) */
    /* print (n * MS_TAB_WIDTH) space characters out */
    int j;
    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < MS_TAB_WIDTH; ++j)
        {
            putchar(' ');
        }
    }
#endif  /* !(MS_TAB_WIDTH < 0) */
}

/* make switch statements to identify the string */
void ms_make_switch(MS_MAKE_SWITCH *pms, int depth, int ident_level)
{
    int i;
    for (i = 0; i < pms->count; ++i)
    {
        if (pms->checks[i] || memcmp(pms->entries[i], pms->str, depth))
        {
            /* ignore it */
            continue;
        }
        ms_make_indent(ident_level);
        if (pms->entries[i][depth])
        {
            /* not end of string */
            printf("case '%c':\n", pms->entries[i][depth]);
            ms_make_indent(ident_level + 1);
            printf("switch (s[%d])\n", depth + 1);
            ms_make_indent(ident_level + 1);
            puts("{");
                /* set the current character */
                pms->str[depth] = pms->entries[i][depth];
                /* recurse */
                ms_make_switch(pms, depth + 1, ident_level + 1);
                /* default: return -1; */
                ms_make_indent(ident_level + 1);
                puts("default:");
                ms_make_indent(ident_level + 2);
                puts("return -1;");     /* -1 means 'not matched' */
            ms_make_indent(ident_level + 1);
            puts("}");
        }
        else
        {
            /* end of string */
            puts("case 0:");
            ms_make_indent(ident_level + 1);
            printf("return %d;\n", i);  /* i is the index of the string */
            /* check it */
            pms->checks[i] = 1;
        }
    }
}

/* make a function code to identify the string */
int ms_make_identify_string_function(int count, char **entries)
{
    int i, len, max_length;
    MS_MAKE_SWITCH ms;

    /* initialize the v_file standard I/O */
    v_file_init_stdio(NULL, 0, "rb");

    /* calculate the maximum length */
    max_length = 0;
    for (i = 0; i < count; ++i)
    {
        len = strlen(entries[i]);
        if (max_length < len)
            max_length = len;
    }

    /* fill the structure */
    ms.count = count;
    ms.str = (char *)malloc(max_length + 1);
    ms.entries = entries;
    ms.checks = (char *)calloc(count, sizeof(char));

    if (ms.str == NULL || ms.checks == NULL)
    {
        /* destroy the v_file standard I/O */
        v_file_destroy_stdio();

        /* allocation failed */
        free(ms.str);
        free(ms.checks);
        return 0;   /* failure */
    }

    /* print out the function code */
    puts("int identify_string(const char *s)\n{");
        ms_make_indent(1);
        printf("switch (s[0])\n");
        ms_make_indent(1);
        puts("{");
        ms_make_switch(&ms, 0, 1);
        ms_make_indent(1);
        puts("default:");
            ms_make_indent(2);
            puts("return -1;");
        ms_make_indent(1);
    puts("}\n}");

    /* free up */
    free(ms.str);
    free(ms.checks);

    /* output */
    v_fsave("output.txt", v_stdout, "w");

    /* destroy the v_file standard I/O */
    v_file_destroy_stdio();

    return 1;   /* success */
}

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**************************************************************************/
