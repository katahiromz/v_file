/**************************************************************************/
/* v_file_test.c --- test program for v_file                              */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <cassert>
    using namespace std;
#else
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <assert.h>
#endif

/**************************************************************************/

#include "v_file.h"

/**************************************************************************/
/* some functions for testing */

/*
 * "r" #1
 */
void v_file_test1_ms(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char *p, buf[16];
    int n;

    v_fp = v_fopen_r("TEST\r\ntest\r\nT", 13);
    assert(v_fp);
    assert(!v_feof(v_fp));
    fp = fopen("testdata\\data1.dat", "r");
    assert(fp);
    assert(!feof(fp));

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "TEST", 5) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "TEST", 5) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "\n", 1) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "\n", 1) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "test", 4) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "test", 4) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "\n", 1) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "\n", 1) == 0);
    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "T", 2) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "T", 2) == 0);

    assert(v_feof(v_fp));
    n = v_fclose(v_fp);
    assert(n == 0);
    assert(feof(fp));
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "r" #1
 */
void v_file_test1_linux(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char *p, buf[16];
    int n;

    v_fp = v_fopen_r("TEST\r\ntest\r\nT", 13);
    assert(v_fp);
    assert(!v_feof(v_fp));
    fp = fopen("testdata\\data1.dat", "r");
    assert(fp);
    assert(!feof(fp));

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "TEST", 5) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "TEST", 5) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "\r\n", 2) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "\r\n", 2) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "test", 4) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "test", 4) == 0);

    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "\r\n", 2) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "\r\n", 2) == 0);
    p = v_fgets(buf, 5, v_fp);
    assert(p);
    assert(memcmp(buf, "T", 2) == 0);
    p = fgets(buf, 5, fp);
    assert(p);
    assert(memcmp(buf, "T", 2) == 0);

    assert(v_feof(v_fp));
    n = v_fclose(v_fp);
    assert(n == 0);
    assert(feof(fp));
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "r" #2
 */
void v_file_test2_ms(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char buf[16];
    int n;

    v_fp = v_fopen_r("TEST\r\ntest\r\nT", 13);
    assert(v_fp);
    assert(!v_feof(v_fp));
    fp = fopen("testdata\\data1.dat", "r");
    assert(fp);
    assert(!feof(fp));

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "\ntes", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "\ntes", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 3);
    assert(memcmp(buf, "t\nT", 3) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 3);
    assert(memcmp(buf, "t\nT", 3) == 0);

    assert(v_feof(v_fp));
    n = v_fclose(v_fp);
    assert(n == 0);
    assert(feof(fp));
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "r" #2
 */
void v_file_test2_linux(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char buf[16];
    int n;

    v_fp = v_fopen_r("TEST\r\ntest\r\nT", 13);
    assert(v_fp);
    assert(!v_feof(v_fp));
    fp = fopen("testdata\\data1.dat", "r");
    assert(fp);
    assert(!feof(fp));

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "\r\nte", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "\r\nte", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "st\r\n", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "st\r\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 1);
    assert(memcmp(buf, "T", 1) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 1);
    assert(memcmp(buf, "T", 1) == 0);

    assert(v_feof(v_fp));
    n = v_fclose(v_fp);
    assert(n == 0);
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "rb" #1
 */
void v_file_test3(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char buf[16];
    int n;

    v_fp = v_fopen_rb("TEST\r\ntest\r\nT", 13);
    assert(v_fp);
    assert(!v_feof(v_fp));
    fp = fopen("testdata\\data1.dat", "rb");
    assert(fp);
    assert(!feof(fp));

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "\r\nte", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "\r\nte", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "st\r\n", 4) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 4);
    assert(memcmp(buf, "st\r\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 1);
    assert(memcmp(buf, "T", 1) == 0);
    n = fread(buf, 1, 4, fp);
    assert(n == 1);
    assert(memcmp(buf, "T", 1) == 0);

    assert(v_feof(v_fp));
    n = v_fclose(v_fp);
    assert(n == 0);
    assert(feof(fp));
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "w" #1
 */
void v_file_test4_ms(void)
{
    FILE *fp;
    v_FILE *v_fp;
    int n;

    v_fp = v_fopen_w();
    assert(v_fp);
    fp = fopen("testdata\\data2.dat", "w");
    assert(fp);

    n = v_fwrite("TEST\ntest\nTE", 1, 12, v_fp);
    assert(n == 12);
    assert(memcmp(v_fp->data, "TEST\r\ntest\r\nTE", 14) == 0);
    n = fwrite("TEST\ntest\nTE", 1, 12, fp);
    assert(n == 12);

    n = v_fclose(v_fp);
    assert(n == 0);
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "w" #1
 */
void v_file_test4_linux(void)
{
    FILE *fp;
    v_FILE *v_fp;
    int n;

    v_fp = v_fopen_w();
    assert(v_fp);
    fp = fopen("testdata\\data2.dat", "w");
    assert(fp);

    n = v_fwrite("TEST\ntest\nTE", 1, 12, v_fp);
    assert(n == 12);
    assert(memcmp(v_fp->data, "TEST\ntest\nTE", 12) == 0);
    n = fwrite("TEST\ntest\nTE", 1, 12, fp);
    assert(n == 12);

    n = v_fclose(v_fp);
    assert(n == 0);
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "wb" #1
 */
void v_file_test5(void)
{
    FILE *fp;
    v_FILE *v_fp;
    int n;

    v_fp = v_fopen_wb();
    assert(v_fp);
    fp = fopen("testdata\\data3.dat", "wb");
    assert(fp);

    n = v_fwrite("TESTtestTE", 5, 2, v_fp);
    assert(n == 2);
    assert(memcmp(v_fp->data, "TESTtestTE", 10) == 0);
    n = fwrite("TESTtestTE", 5, 2, fp);
    assert(n == 2);
    assert(memcmp(v_fp->data, "TESTtestTE", 10) == 0);

    n = v_fclose(v_fp);
    assert(n == 0);
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "r" #2
 */
void v_file_test6(void)
{
    FILE *fp;
    v_FILE *v_fp;
    char buf[32];
    int n;

    v_fp = v_fopen_rb("TESTtestTE", 10);
    assert(v_fp);
    fp = fopen("testdata\\data3.dat", "rb");
    assert(fp);

    n = v_fread(buf, 1, 32, v_fp);
    assert(n == 10);
    n = fread(buf, 1, 32, fp);
    assert(n == 10);

    n = v_fclose(v_fp);
    assert(n == 0);
    n = fclose(fp);
    assert(n == 0);
}

/*
 * "a+" #1
 */
void v_file_test7_ms(void)
{
    v_FILE *v_fp;
    char *buf[16];
    int n;

    v_fp = v_fopen_ap("TESTtestTE", 10);
    assert(v_fp);

    n = v_fwrite("ABC\nabc\n123", 1, 11, v_fp);
    assert(n == 11);
    assert(memcmp(v_fp->data, "TESTtestTEABC\r\nabc\r\n123", 23) == 0);

    v_fseek(v_fp, 10, v_SEEK_SET);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "ABC\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "abc\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 3);
    assert(memcmp(buf, "123", 3) == 0);

    n = v_fwrite("------", 6, 1, v_fp);
    assert(n == 1);
    assert(memcmp(v_fp->data, "TESTtestTEABC\r\nabc\r\n123------", 29) == 0);

    n = v_fclose(v_fp);
    assert(n == 0);
}

/*
 * "a+" #1
 */
void v_file_test7_linux(void)
{
    v_FILE *v_fp;
    char buf[16];
    int n;

    v_fp = v_fopen_ap("TESTtestTE", 10);
    assert(v_fp);

    n = v_fwrite("ABC\nabc\n123", 1, 11, v_fp);
    assert(n == 11);
    assert(memcmp(v_fp->data, "TESTtestTEABC\nabc\n123", 21) == 0);

    v_fseek(v_fp, 10, v_SEEK_SET);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "ABC\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "abc\n", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 3);
    assert(memcmp(buf, "123", 3) == 0);

    n = v_fwrite("------", 6, 1, v_fp);
    assert(n == 1);
    assert(memcmp(v_fp->data, "TESTtestTEABC\nabc\n123------", 27) == 0);

    n = v_fclose(v_fp);
    assert(n == 0);
}

/*
 * "a+b"
 */
void v_file_test8(void)
{
    v_FILE *v_fp;
    char buf[16];
    int n;

    v_fp = v_fopen_apb("TESTtestTE", 10);
    assert(v_fp);

    n = v_fwrite("ABCabc123", 3, 3, v_fp);
    assert(n == 3);
    assert(memcmp(v_fp->data, "TESTtestTEABCabc123", 19) == 0);

    v_fseek(v_fp, 0, v_SEEK_SET);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "TEST", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "test", 4) == 0);

    n = v_fread(buf, 1, 4, v_fp);
    assert(n == 4);
    assert(memcmp(buf, "TEAB", 4) == 0);

    n = v_fwrite("------", 6, 1, v_fp);
    assert(n == 1);
    assert(memcmp(v_fp->data, "TESTtestTEAB------3", 19) == 0);

    n = v_fclose(v_fp);
    assert(n == 0);
}

/**************************************************************************/
/* the main function for testing */

int main(void)
{
    #if defined(_WIN32) || defined(MSDOS)
        v_file_test1_ms();
        v_file_test2_ms();
        v_file_test3();
        v_file_test4_ms();
        v_file_test5();
        v_file_test6();
        v_file_test7_ms();
        v_file_test8();
    #else
        v_file_test1_linux();
        v_file_test2_linux();
        v_file_test3();
        v_file_test4_linux();
        v_file_test5();
        v_file_test6();
        v_file_test7_linux();
        v_file_test8();
    #endif
    return 0;
}

/**************************************************************************/
