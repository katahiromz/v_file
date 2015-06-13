/**************************************************************************/
/* v_file.c --- virtual file operation on memory                          */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include "v_file.h"

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
/* configuration */

/*
 * Specify the sufficient buffer size for the insecure functions
 */
#define v_FILE_MAX_BUFFER   1024
/* #define v_FILE_MAX_BUFFER   32 */
/* #define v_FILE_MAX_BUFFER   256 */

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/**************************************************************************/
/* private constants */

/* virtual file modes */
#define v_FMODE_ERROR       1   /* is there any error? */
#define v_FMODE_READ        2   /* read */
#define v_FMODE_WRITE       4   /* write */
#define v_FMODE_READWRITE   6   /* read+write */
#define v_FMODE_APPEND      14  /* append (includes v_FMODE_READWRITE) */
#define v_FMODE_BINARY      16  /* binary mode */
#if defined(_WIN32) || defined(MSDOS)
    #define v_FMODE_TEXT        0   /* text mode */
#else
    #define v_FMODE_TEXT        v_FMODE_BINARY
#endif

/**************************************************************************/
/* opening / closing */

static v_FILE *
v_fopen_internal(const void *data, v_fpos_t index, v_fpos_t siz, int modes)
{
    v_FILE *fp;

    /* check parameters */
    assert(data || siz == 0);
    if (data == NULL && siz != 0)
        return NULL;

    /* allocate and store */
    fp = (v_FILE *)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        fp->data = (char *)malloc(siz + 1);
        if (fp->data)
        {
            /* success */
            memcpy(fp->data, data, siz);
            fp->data[siz] = 0;
            fp->modes = modes;
            fp->index = index;
            fp->size = siz;
        }
        else
        {
            /* failure */
            free(fp);
            fp = NULL;
        }
    }

    /* return it */
    return fp;
}

v_FILE *v_fopen_r(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, 0, siz, v_FMODE_READ | v_FMODE_TEXT);
}

v_FILE *v_fopen_a(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
}

v_FILE *v_fopen_rb(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, 0, siz, v_FMODE_READ | v_FMODE_BINARY);
}

v_FILE *v_fopen_ab(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
}

v_FILE *v_fopen_rp(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, 0, siz, v_FMODE_READWRITE | v_FMODE_TEXT);
}

v_FILE *v_fopen_ap(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
}

v_FILE *v_fopen_rpb(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, 0, siz, v_FMODE_READWRITE | v_FMODE_BINARY);
}

v_FILE *v_fopen_apb(const void *data, v_fpos_t siz)
{
    return v_fopen_internal(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
}

v_FILE *v_fopen_w(void)
{
    v_FILE *fp;
    fp = (v_FILE *)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        fp->data = NULL;
        fp->modes = v_FMODE_WRITE | v_FMODE_TEXT;
        fp->index = 0;
        fp->size = 0;
    }
    return fp;
}

v_FILE *v_fopen_wb(void)
{
    v_FILE *fp;
    fp = (v_FILE *)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        fp->data = NULL;
        fp->modes = v_FMODE_WRITE | v_FMODE_BINARY;
        fp->index = 0;
        fp->size = 0;
    }
    return fp;
}

int v_fclose(v_FILE *fp)
{
    assert(fp);
    if (fp)
    {
        free(fp->data);
        free(fp);
        return 0;
    }
    return v_EOF;
}

char *v_fclose_detach(v_FILE *fp)
{
    char *data = NULL;
    assert(fp);
    if (fp)
    {
        data = fp->data;
        free(fp);
    }
    return data;    /* the library user should free this */
}

/**************************************************************************/
/* binary transfer */

int v_fread_raw(void *ptr, v_fpos_t siz, v_fpos_t nelem, v_FILE *fp)
{
    v_fpos_t count, read_size;
    char *pch;

    /* check parameters */
    assert(fp);
    if (v_ferror(fp))
        return 0;
    if ((fp->modes & v_FMODE_READ) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;

    /* check exceed */
    if (fp->size < fp->index)
        fp->index = fp->size;

    /* count the read blocks */
    count = (fp->size - fp->index) / siz;
    if (count > nelem)
        count = nelem;

    /* calculate the size to read */
    /* retrieve the current position */
    /* copy to buffer */
    read_size = count * siz;
    pch = (char *)fp->data + fp->index;
    memcpy(ptr, pch, read_size);

    /* proceed the position */
    fp->index += read_size;

    /* return the count of blocks read */
    return count;
}

int v_fwrite_raw(const void *ptr, v_fpos_t siz, v_fpos_t nelem,
                 v_FILE *fp)
{
    v_fpos_t increment, end;
    char *pch;

    /* check parameters */
    assert(fp);
    if (v_ferror(fp))
        return 0;
    if ((fp->modes & v_FMODE_WRITE) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;

    /* calculate increment and the ending point */
    increment = siz * nelem;
    end = fp->index + increment;
    if (end > fp->size)
    {
        void *data = realloc(fp->data, end + 1);
        if (data == NULL)
        {
            fp->modes |= v_FMODE_ERROR;
            return 0;
        }

        fp->data = (char *)data;
        fp->size = end;
        fp->data[end] = 0;
    }

    /* retrieve the current position */
    /* copy from buffer */
    pch = (char *)fp->data + fp->index;
    memcpy(pch, ptr, increment);

    /* proceed the position */
    fp->index += increment;

    /* return the count of blocks written */
    return nelem;
}

/**************************************************************************/
/* read / write a character */

int v_fgetc(v_FILE *fp)
{
    int ch;

    /* check parameter */
    assert(fp);
    /* check status */
    if (v_ferror(fp))
        return v_EOF;
    if (fp->size < fp->index + sizeof(char))
        return v_EOF;

    if (fp->modes & v_FMODE_BINARY)
    {
        /* binary mode */
        ch = *(fp->data + fp->index);
        fp->index += sizeof(char);
    }
    else
    {
        /* text mode: eat every '\r' */
        do
        {
            ch = *(fp->data + fp->index);
            fp->index += sizeof(char);
        } while (ch == '\r' && fp->index + sizeof(char) <= fp->size);
    }

    return ch;
}

int v_fputc(char c, v_FILE *fp)
{
    /* check parameter */
    assert(fp);
    if (fp == NULL)
        return v_EOF;

#if defined(_WIN32) || defined(MSDOS)
    if ((fp->modes & v_FMODE_BINARY) == 0)
    {
        /* text mode */
        /* add '\r' before '\n' */
        char cr = '\r';
        if (c == '\n' && v_fwrite_raw(&cr, sizeof(char), 1, fp) == 0)
        {
            return v_EOF;
        }
    }
#endif
    return (v_fwrite_raw(&c, sizeof(char), 1, fp) ? c : v_EOF);
}

/**************************************************************************/
/* read / write buffer */

int v_fread(void *ptr, v_fpos_t siz, v_fpos_t nelem, v_FILE *fp)
{
    char *pch;
    int ch;
    v_fpos_t i, count;

    /* check parameters */
    assert(fp);
    if (fp == NULL)
        return 0;
    if ((fp->modes & v_FMODE_READ) == 0)
        return 0;
    if (v_feof(fp))
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;
    if (fp->modes & v_FMODE_BINARY)
    {
        /* binary mode */
        return v_fread_raw(ptr, siz, nelem, fp);
    }

    /* text mode */
    pch = (char *)ptr;
    for (count = 0; count < nelem; ++count)
    {
        for (i = 0; i < siz; ++i)
        {
            ch = v_fgetc(fp);
            if (ch == v_EOF)
                goto hell;
            *pch++ = ch;
        }
    }
hell:
    return count;
}

int v_fwrite(const void *ptr, v_fpos_t siz, v_fpos_t nelem,
             v_FILE *fp)
{
#if defined(_WIN32) || defined(MSDOS)
    const char *pch;
    int ret;
    v_fpos_t i, count;
#endif

    /* check parameters */
    assert(fp);
    if (fp == NULL)
        return 0;
    if ((fp->modes & v_FMODE_WRITE) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;
    if (fp->modes & v_FMODE_BINARY)
    {
        /* binary mode */
        return v_fwrite_raw(ptr, siz, nelem, fp);
    }

#if defined(_WIN32) || defined(MSDOS)
    /* text mode */
    pch = (const char *)ptr;
    for (count = 0; count < nelem; ++count)
    {
        for (i = 0; i < siz; ++i)
        {
            ret = v_fputc(*pch++, fp);
            if (ret == v_EOF)
                goto hell;
        }
    }
hell:
    return count;
#else
    return v_fwrite_raw(ptr, siz, nelem, fp);
#endif
}

/**************************************************************************/
/* file status */

int v_feof(v_FILE *fp)
{
    assert(fp);
    return (fp && fp->size <= fp->index);
}

int v_ferror(v_FILE *fp)
{
    assert(fp);
    if (fp == NULL)
        return v_EOF;
    if (fp->size < fp->index)
        return v_EOF;
    if (fp->modes & v_FMODE_ERROR)
        return v_EOF;
    return 0;
}

void v_clearerr(v_FILE *fp)
{
    assert(fp);
    if (fp == NULL)
        return;
    fp->modes &= ~v_FMODE_ERROR;
}

/**************************************************************************/
/* file position */

long v_ftell(v_FILE *fp)
{
    assert(fp);
    return (fp ? (long)(fp->index) : 0);
}

int v_fgetpos(v_FILE *fp, v_fpos_t *pos)
{
    assert(fp);
    if (fp && pos)
    {
        *pos = fp->index;
        return 0;
    }
    return v_EOF;
}

int v_fsetpos(v_FILE *fp, const v_fpos_t *pos)
{
    assert(fp);
    if (fp && pos)
    {
        fp->index = *pos;
        return 0;
    }
    return v_EOF;
}

void v_rewind(v_FILE *fp)
{
    assert(fp);
    if (fp)
    {
        fp->index = 0;
    }
}

int v_fseek(v_FILE *fp, long offset, int type)
{
    assert(fp);
    if (fp == NULL)
        return v_EOF;

    switch (type)
    {
    case v_SEEK_CUR:
        if (offset >= 0)
        {
            if (fp->index + offset <= fp->size)
                fp->index += offset;
            else
                fp->index = fp->size;
        }
        else
        {
            if ((v_fpos_t)-offset <= fp->index)
                fp->index += offset;
            else
                fp->index = 0;
        }
        break;

    case v_SEEK_END:
        assert(fp->size <= 0x7FFFFFFF);
        if (offset <= (long)fp->size)
            fp->index = fp->size - offset;
        else
            fp->index = 0;
        break;

    case v_SEEK_SET:
        assert(fp->size <= 0x7FFFFFFF);
        if (offset <= (long)fp->size)
            fp->index = offset;
        else
            fp->index = fp->size;
        break;

    default:
        assert(0);
        return v_EOF;
    }

    return 0;
}

/**************************************************************************/
/* unget a character */

int v_ungetc(char c, v_FILE *fp)
{
    if (fp == NULL || fp->index < sizeof(char))
        return v_EOF;
    if (v_ferror(fp))
        return v_EOF;
    if ((fp->modes & v_FMODE_READ) == 0)
        return v_EOF;

    if ((fp->modes & v_FMODE_BINARY) == 0)
    {
        /* text mode */
        if (c == '\r')
        {
            /* don't undo */
            return c;
        }
    }

    /* undo */
    fp->index -= sizeof(char);
    *((char *)fp->data + fp->index) = c;
    return c;
}

/**************************************************************************/
/* read / write string */

char *v_fgets(char *s, int n, v_FILE *fp)
{
    int i;
    char ch;

    /* check parameters */
    if (v_feof(fp) || n == 0)
        return NULL;

    /* ignore NUL */
    --n;

    /* loop while there is space */
    for (i = 0; i < n; ++i)
    {
        /* get a character */
        ch = (char)v_fgetc(fp);
        /* store a character */
        s[i] = ch;

        if ((fp->modes & v_FMODE_BINARY) == 0) 
        {
            assert(s[i] != '\r');
        }

        if (s[i] == v_EOF)
        {
            /* error */
            break;
        }

        if (s[i] == '\n')
        {
            /* newline */
            ++i;
            break;
        }
    }

    /* set NUL */
    s[i] = 0;
    return s;
}

int v_fputs(const char *s, v_FILE *fp)
{
    size_t len;

    /* check parameters */
    assert(s);
    assert(fp);
    if (fp == NULL)
        return v_EOF;
    len = strlen(s);
    if (len == 0)
        return v_EOF;

    if (fp->modes & v_FMODE_BINARY)
    {
        /* binary mode */
        return (v_fwrite_raw(s, len * sizeof(char), 1, fp) ? 0 : v_EOF);
    }

    /* text mode */
    while (len--)
    {
        /* put a character */
        if (v_fputc(*s, fp) == v_EOF)
        {
            /* failure */
            return v_EOF;
        }
        ++s;
    }
    return 0;
}

/**************************************************************************/
/* scan */

int v_fscanf(v_FILE *fp, const char *format, ...)
{
    va_list va;
    int ret;

    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;

    va_start(va, format);
    ret = v_vfscanf(fp, format, va);
    va_end(va);

    return ret;
}

int v_vfscanf(v_FILE *fp, const char *format, va_list arg)
{
    char buf[v_FILE_MAX_BUFFER];

    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;

    /* read one line */
    if (v_fgets(buf, v_FILE_MAX_BUFFER, fp))
    {
        /* FIXME: one line only? */
        return vsscanf(buf, format, arg);
    }
    return v_EOF;
}

/**************************************************************************/
/* print */

int v_fprintf(v_FILE *fp, const char *format, ...)
{
    va_list va;
    int n;

    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;

    va_start(va, format);
    n = v_vfprintf(fp, format, va);
    va_end(va);

    return n;
}

int v_vfprintf(v_FILE *fp, const char *format, va_list arg)
{
    int n;
    char buf[v_FILE_MAX_BUFFER];
    size_t len;

    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;

    n = vsprintf(buf, format, arg);
    len = strlen(buf);
    assert(len < v_FILE_MAX_BUFFER);
    if (fp->modes & v_FMODE_BINARY)
    {
        return (v_fwrite_raw(buf, len * sizeof(char), 1, fp) ? n : 0);
    }
    return v_fputs(buf, fp);
}

/**************************************************************************/
/* flush */

int v_fflush(v_FILE *fp)
{
    /* NOTE: v_fflush does nothing */
    assert(fp);
    return 0;
}

/**************************************************************************/
/* Windows .exe resource */

#ifdef _WIN32
    /* opening Windows .exe resource as virtual file */
    v_FILE *v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type)
    {
        HRSRC       hRsrc;
        HGLOBAL     hGlobal;
        void *      data;
        DWORD       size;
        v_FILE *    fp;

        if (hMod == NULL)
            hMod = GetModuleHandle(NULL);

        hRsrc = FindResource(hMod, res_id, type);
        if (hRsrc == NULL)
            return NULL;

        hGlobal = LoadResource(hMod, hRsrc);
        if (hGlobal == NULL)
            return NULL;

        data = LockResource(hGlobal);
        size = SizeofResource(hMod, hRsrc);
        fp = v_fopen_rb(data, size);
        return fp;
    }
#endif

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */
