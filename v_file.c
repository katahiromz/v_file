/**************************************************************************/
/* v_file.c --- virtual file operation on memory                          */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <cerrno>
    #include <cassert>
    using namespace std;
#else
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
    #include <assert.h>
#endif

/**************************************************************************/

#include "v_file.h"

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/**************************************************************************/
/* opening / closing */

v_LPFILE 
v_fopen_intern(v_LPCVOID data, v_fpos_t index, v_fpos_t siz, int modes)
{
    v_LPFILE fp;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(data || siz == 0);
    if (data == NULL && siz != 0)
        return NULL;
#endif

    /* allocate and store */
    fp = (v_LPFILE)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        fp->data = (v_LPCHAR)malloc(siz + 1);
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

#ifndef V_FILE_SPEED
    v_LPFILE v_fopen_r(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READ | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_a(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_rb(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READ | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_ab(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_rp(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READWRITE | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_ap(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_rpb(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READWRITE | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_apb(v_LPCVOID data, v_fpos_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
    }
#endif  /* ndef V_FILE_SPEED */

v_LPFILE v_fopen_w(void)
{
    v_LPFILE fp;
    fp = (v_LPFILE)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        /* fp->data = NULL; */
        fp->modes = v_FMODE_WRITE | v_FMODE_TEXT;
        /* fp->index = 0; */
        /* fp->size = 0; */
    }
    return fp;
}

v_LPFILE v_fopen_wb(void)
{
    v_LPFILE fp;
    fp = (v_LPFILE)calloc(1, sizeof(v_FILE));
    if (fp)
    {
        /* fp->data = NULL; */
        fp->modes = v_FMODE_WRITE | v_FMODE_BINARY;
        /* fp->index = 0; */
        /* fp->size = 0; */
    }
    return fp;
}

int v_fclose(v_LPFILE fp)
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

v_LPCHAR v_fclose_detach(v_LPFILE fp)
{
    v_LPCHAR data = NULL;
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

int v_fread_raw(v_LPVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp)
{
    v_fpos_t count, read_size;
    v_LPCHAR pch;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp);
    if (v_ferror(fp))
        return 0;
    if ((fp->modes & v_FMODE_READ) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;
#endif

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
    pch = (v_LPCHAR)fp->data + fp->index;
    memcpy(ptr, pch, read_size);

    /* proceed the position */
    fp->index += read_size;

    /* return the count of blocks read */
    return count;
}

int v_fwrite_raw(v_LPCVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp)
{
    v_fpos_t increment, end;
    v_LPCHAR pch;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp);
    if (v_ferror(fp))
        return 0;
    if ((fp->modes & v_FMODE_WRITE) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;
#endif

    /* calculate increment and the ending point */
    increment = siz * nelem;
    end = fp->index + increment;
    if (end > fp->size)
    {
        v_LPVOID data = realloc(fp->data, end + 1);
        if (data == NULL)
        {
            fp->modes |= v_FMODE_ERROR;
            return 0;
        }

        fp->data = (v_LPCHAR)data;
        fp->size = end;
        fp->data[end] = 0;
    }

    /* retrieve the current position */
    /* copy from buffer */
    pch = (v_LPCHAR)fp->data + fp->index;
    memcpy(pch, ptr, increment);

    /* proceed the position */
    fp->index += increment;

    /* return the count of blocks written */
    return nelem;
}

/**************************************************************************/
/* read / write a character */

int v_fgetc(v_LPFILE fp)
{
    int ch;

#ifndef V_FILE_SPEED
    /* check parameter */
    assert(fp);
    /* check status */
    if (v_ferror(fp))
        return v_EOF;
#endif

    if (fp->size < fp->index + sizeof(char))
        return v_EOF;

#if (v_FMODE_TEXT == 0)
    if (fp->modes & v_FMODE_BINARY)
#endif
    {
        /* binary mode */
        ch = *(fp->data + fp->index);
        fp->index += sizeof(char);
        return ch;
    }

#if (v_FMODE_TEXT == 0)
    /* text mode: eat every '\r' */
    do
    {
        ch = *(fp->data + fp->index);
        fp->index += sizeof(char);
    } while (ch == '\r' && fp->index + sizeof(char) <= fp->size);
    return ch;
#endif
}

int v_fputc(char c, v_LPFILE fp)
{
#ifndef V_FILE_SPEED
    /* check parameter */
    assert(fp);
    if (fp == NULL)
        return v_EOF;
#endif

#if (v_FMODE_TEXT == 0)
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
    if (v_fwrite_raw(&c, sizeof(char), 1, fp))
        return c;
    else
        return v_EOF;
}

/**************************************************************************/
/* read / write buffer */

int v_fread(v_LPVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp)
{
#if (v_FMODE_TEXT == 0)
    v_LPCHAR pch;
    int ch;
    v_fpos_t i, count;
#endif

#ifndef V_FILE_SPEED
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
#endif

#if (v_FMODE_TEXT == 0)
    if (fp->modes & v_FMODE_BINARY)
#endif
    {
        /* binary mode */
        return v_fread_raw(ptr, siz, nelem, fp);
    }

#if (v_FMODE_TEXT == 0)
    /* text mode */
    pch = (v_LPCHAR)ptr;
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
#endif  /* (v_FMODE_TEXT == 0) */
}

int v_fwrite(v_LPCVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp)
{
#if (v_FMODE_TEXT == 0)
    v_LPCSTR pch;
    int ret;
    v_fpos_t i, count;
#endif

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp);
    if (fp == NULL)
        return 0;
    if ((fp->modes & v_FMODE_WRITE) == 0)
        return 0;
    if (ptr == NULL || siz <= 0 || nelem <= 0)
        return 0;
#endif

#if (v_FMODE_TEXT == 0)
    if (fp->modes & v_FMODE_BINARY)
#endif
    {
        /* binary mode */
        return v_fwrite_raw(ptr, siz, nelem, fp);
    }

#if (v_FMODE_TEXT == 0)
    /* text mode */
    pch = (v_LPCSTR)ptr;
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
#endif  /* (v_FMODE_TEXT == 0) */
}

/**************************************************************************/
/* file status */

int v_feof(v_LPFILE fp)
{
    assert(fp);
    return (fp && fp->size <= fp->index);
}

int v_ferror(v_LPFILE fp)
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

void v_clearerr(v_LPFILE fp)
{
    assert(fp);
#ifndef V_FILE_SPEED
    if (fp == NULL)
        return;
#endif
    fp->modes &= ~v_FMODE_ERROR;
}

/**************************************************************************/
/* file position */

long v_ftell(v_LPFILE fp)
{
#ifndef V_FILE_SPEED
    assert(fp);
    if (fp)
    {
        return (long)(fp->index);
    }
    return 0;
#else
    return (long)(fp->index);
#endif
}

int v_fgetpos(v_LPFILE fp, v_fpos_t *pos)
{
#ifndef V_FILE_SPEED
    assert(fp);
    assert(pos);
    if (fp && pos)
    {
        *pos = fp->index;
        return 0;
    }
    return v_EOF;
#else
    *pos = fp->index;
    return 0;
#endif
}

int v_fsetpos(v_LPFILE fp, const v_fpos_t *pos)
{
#ifndef V_FILE_SPEED
    assert(fp);
    assert(pos);
    if (fp && pos)
    {
        fp->index = *pos;
        return 0;
    }
    return v_EOF;
#else
    fp->index = *pos;
    return 0;
#endif
}

void v_rewind(v_LPFILE fp)
{
#ifndef V_FILE_SPEED
    assert(fp);
    if (fp)
    {
        fp->index = 0;
    }
#else
    fp->index = 0;
#endif
}

int v_fseek(v_LPFILE fp, long offset, int type)
{
#ifndef V_FILE_SPEED
    assert(fp);
    if (fp == NULL)
        return v_EOF;
#endif

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

int v_ungetc(char c, v_LPFILE fp)
{
#ifndef V_FILE_SPEED
    assert(fp);
    if (fp == NULL)
        return v_EOF;
    if (v_ferror(fp))
        return v_EOF;
    if ((fp->modes & v_FMODE_READ) == 0)
        return v_EOF;
#endif

    if (fp->index < sizeof(char))
        return v_EOF;

#if (v_FMODE_TEXT == 0)
    if ((fp->modes & v_FMODE_BINARY) == 0)
    {
        /* text mode */
        if (c == '\r')
        {
            /* don't undo */
            return c;
        }
    }
#endif

    /* undo */
    fp->index -= sizeof(char);
    *((v_LPCHAR)fp->data + fp->index) = c;
    return c;
}

/**************************************************************************/
/* read / write string */

v_LPSTR v_fgets(v_LPSTR s, int n, v_LPFILE fp)
{
    int i;
    char ch;

#ifndef V_FILE_SPEED
    /* check parameters */
    if (n == 0)
        return NULL;
    if (fp == NULL)
        return NULL;
#endif

    if (v_feof(fp))
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

#if (v_FMODE_TEXT == 0)
        if ((fp->modes & v_FMODE_BINARY) == 0) 
        {
            assert(s[i] != '\r');
        }
#endif

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

int v_fputs(v_LPCSTR s, v_LPFILE fp)
{
    size_t len;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(s && fp);
    if (s == NULL || fp == NULL)
        return v_EOF;
#endif

    len = strlen(s);
    if (len == 0)
        return v_EOF;

#if (v_FMODE_TEXT == 0)
    if (fp->modes & v_FMODE_BINARY)
#endif
    {
        /* binary mode */
        if (v_fwrite_raw(s, len * sizeof(char), 1, fp))
            return 0;
        else
            return v_EOF;
    }

#if (v_FMODE_TEXT == 0)
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
#endif
}

/**************************************************************************/
/* scan */

int v_fscanf(v_LPFILE fp, v_LPCSTR format, ...)
{
    va_list va;
    int ret;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;
#endif

    va_start(va, format);
    ret = v_vfscanf(fp, format, va);
    va_end(va);

    return ret;
}

int v_vfscanf(v_LPFILE fp, v_LPCSTR format, va_list arg)
{
    char buf[v_FILE_MAX_BUFFER];

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;
#endif

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

int v_fprintf(v_LPFILE fp, v_LPCSTR format, ...)
{
    va_list va;
    int n;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;
#endif

    va_start(va, format);
    n = v_vfprintf(fp, format, va);
    va_end(va);

    return n;
}

int v_vfprintf(v_LPFILE fp, v_LPCSTR format, va_list arg)
{
    int n;
    char buf[v_FILE_MAX_BUFFER];
    size_t len;

#ifndef V_FILE_SPEED
    /* check parameters */
    assert(fp && format);
    if (fp == NULL || format == NULL)
        return v_EOF;
#endif

    n = vsprintf(buf, format, arg);
    len = strlen(buf);
    assert(len < v_FILE_MAX_BUFFER);

#if (v_FMODE_TEXT == 0)
    if (fp->modes & v_FMODE_BINARY)
#endif
    {
        if (v_fwrite_raw(buf, len * sizeof(char), 1, fp))
            return n;
        else
            return 0;
    }
#if (v_FMODE_TEXT == 0)
    return v_fputs(buf, fp);
#endif
}

/**************************************************************************/
/* flush */

#ifndef V_FILE_SPEED
    int v_fflush(v_LPFILE fp)
    {
        /* NOTE: v_fflush does nothing */
        assert(fp);
        return 0;
    }
#endif

/**************************************************************************/
/* Windows .exe resource */

#ifdef WIN16
    /* opening 16-bit Windows .exe resource as virtual file */
    v_LPFILE v_fopen_res16(HMODULE hMod, v_LPCSTR res_id, v_LPCSTR type)
    {
        HRSRC       hRsrc;
        HGLOBAL     hGlobal;
        LPVOID      data;
        DWORD       size;
        v_LPFILE    fp;

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

        UnlockResource(hGlobal);
        FreeResource(hGlobal);
        return fp;
    }
#endif  /* WIN16 */

#ifdef _WIN32
    /* opening Windows .exe resource as virtual file */
    v_LPFILE v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type)
    {
        HRSRC       hRsrc;
        HGLOBAL     hGlobal;
        LPVOID      data;
        DWORD       size;
        v_LPFILE    fp;

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
#endif  /* _WIN32 */

/**************************************************************************/
/* the v_file standard I/O */

#ifdef V_FILE_USE_STDIO
    v_LPFILE v_stdin = NULL;
    v_LPFILE v_stdout = NULL;
    v_LPFILE v_stderr = NULL;

    void v_file_init_stdio(v_LPCVOID input_data, v_fpos_t input_size)
    {
        v_stdin = v_fopen_r(input_data, input_size);
        v_stdout = v_fopen_w();
        v_stderr = v_fopen_w();
    }

    void v_file_destroy_stdio(void)
    {
        v_fclose(v_stdin);
        v_fclose(v_stdout);
        v_fclose(v_stderr);
        v_stdin = NULL;
        v_stdout = NULL;
        v_stderr = NULL;
    }

    #ifndef V_FILE_SPEED
        int v_getchar(void)
        {
            assert(v_stdin);
            return v_fgetc(v_stdin);
        }

        int v_putchar(char c)
        {
            assert(v_stdout);
            return v_fputc(c, v_stdout);
        }

        v_LPSTR v_gets(v_LPSTR s)
        {
            assert(v_stdin);
            return v_fgets(s, v_FILE_MAX_BUFFER, v_stdin);
        }

        int v_puts(v_LPCSTR s)
        {
            assert(v_stdout);
            v_fputs(s, v_stdout);
            v_putchar('\n');
			return (v_stdout ? 0 : -1);
        }
    #endif  /* ndef V_FILE_SPEED */

    int v_printf(v_LPCSTR format, ...)
    {
        va_list va;
        int n;

#ifndef V_FILE_SPEED
        /* check parameters */
        assert(v_stdout && format);
        if (v_stdout == NULL || format == NULL)
            return v_EOF;
#endif

        va_start(va, format);
        n = v_vfprintf(v_stdout, format, va);
        va_end(va);

        return n;
    }

    int v_vprintf(v_LPCSTR format, va_list arg)
    {
        int n;

#ifndef V_FILE_SPEED
        /* check parameters */
        assert(v_stdout && format);
        if (v_stdout == NULL || format == NULL)
            return v_EOF;
#endif

        n = v_vfprintf(v_stdout, format, arg);
        return n;
    }

    int v_scanf(v_LPFILE fp, v_LPCSTR format, ...)
    {
        va_list va;
        int ret;

#ifndef V_FILE_SPEED
        /* check parameters */
        assert(v_stdin && format);
        if (v_stdin == NULL || format == NULL)
            return v_EOF;
#endif

        va_start(va, format);
        ret = v_vfscanf(v_stdin, format, va);
        va_end(va);

        return ret;
    }

    int v_vscanf(v_LPFILE fp, v_LPCSTR format, va_list arg)
    {
        char buf[v_FILE_MAX_BUFFER];

#ifndef V_FILE_SPEED
        /* check parameters */
        assert(v_stdin && format);
        if (v_stdin == NULL || format == NULL)
            return v_EOF;
#endif

        /* read one line */
        if (v_fgets(buf, v_FILE_MAX_BUFFER, v_stdin))
        {
            /* FIXME: one line only? */
            return vsscanf(buf, format, arg);
        }
        return v_EOF;
    }
#endif  /* def V_FILE_USE_STDIO */

/**************************************************************************/
/* secure functions */

#ifdef __V_FILE_WANT_SECURE_LIB__
    v_errno_t v_fopen_r_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_r(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_w_s(v_FILE **pfp)
    {
        assert(pfp);
        if (pfp)
        {
            *pfp = v_fopen_w();
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_a_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_a(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_rp_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_rp(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_ap_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_ap(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_rb_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_rb(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_wb_s(v_FILE **pfp)
    {
        assert(pfp);
        if (pfp)
        {
            *pfp = v_fopen_wb();
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_ab_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_ab(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_rpb_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_rpb(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_fopen_apb_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz)
    {
        assert(pfp);
        assert(data || siz == 0);
        if (pfp && (data || siz == 0))
        {
            *pfp = v_fopen_apb(data, siz);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    v_errno_t v_clearerr_s(v_FILE *fp)
    {
        assert(fp);
        if (fp)
        {
            v_clearerr(fp);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
        }
        return errno;
    }

    int v_fscanf_s(v_FILE *fp, v_LPCSTR format, ...)
    {
        va_list va;
        int ret;

        assert(fp && format);
        if (fp && format)
        {
            va_start(va, format);
            ret = v_vfscanf(fp, format, va);
            va_end(va);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
            ret = v_EOF;
        }
        return ret;
    }

    int v_fprintf_s(v_FILE *fp, v_LPCSTR format, ...)
    {
        va_list va;
        int n;

        assert(fp);
        assert(format);
        if (fp && format)
        {
            va_start(va, format);
            n = v_vfprintf(fp, format, va);
            va_end(va);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
            n = v_EOF;
        }
        return n;
    }

    int v_vfprintf_s(v_FILE *fp, v_LPCSTR format, va_list va)
    {
        int n;

        assert(fp);
        assert(format);
        if (fp && format)
        {
            n = v_vfprintf(fp, format, va);
            errno = 0;
        }
        else
        {
            errno = EINVAL;
            n = v_EOF;
        }
        return n;
    }
#endif  /* def __V_FILE_WANT_SECURE_LIB__ */

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

/**************************************************************************/
