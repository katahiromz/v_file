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
v_fopen_intern(v_LPCVOID data, size_t index, size_t siz, int modes)
{
    v_LPFILE fp;

#ifndef V_FILE_NEED_SPEED
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

#ifndef V_FILE_NEED_SPEED
    v_LPFILE v_fopen_r(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READ | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_a(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_rb(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READ | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_ab(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_rp(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READWRITE | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_ap(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_TEXT);
    }

    v_LPFILE v_fopen_rpb(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, 0, siz, v_FMODE_READWRITE | v_FMODE_BINARY);
    }

    v_LPFILE v_fopen_apb(v_LPCVOID data, size_t siz)
    {
        return v_fopen_intern(data, siz, siz, v_FMODE_APPEND | v_FMODE_BINARY);
    }
#endif  /* ndef V_FILE_NEED_SPEED */

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
    if (fp && fp != v_HFILE_ERROR)
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
    assert(fp && fp != v_HFILE_ERROR);
    if (fp && fp != v_HFILE_ERROR)
    {
        data = fp->data;
        free(fp);
    }
    /* NOTE: the library user should free return value of v_fclose_detach */
    return data;
}

/**************************************************************************/
/* set binary mode / set text mode */

void v_fsetbin(v_FILE *fp)
{
#ifndef V_FILE_NEED_SPEED
    assert(fp && fp != v_HFILE_ERROR);
    if (fp && fp != v_HFILE_ERROR)
#endif
    {
        fp->modes &= ~v_FMODE_TEXT;
        fp->modes |= v_FMODE_BINARY;
    }
}

void v_fsettext(v_FILE *fp)
{
#ifndef V_FILE_NEED_SPEED
    assert(fp && fp != v_HFILE_ERROR);
    if (fp && fp != v_HFILE_ERROR)
#endif
    {
        fp->modes &= ~v_FMODE_BINARY;
        fp->modes |= v_FMODE_TEXT;
    }
}

/**************************************************************************/
/* loading from real file / saving to real file */

v_LPFILE v_fopen(v_LPCSTR fname, v_LPCSTR modes)
{
    FILE *fp;
    size_t n;
    char buf[v_FILE_MAX_BUFFER];
    v_LPFILE v_fp = NULL;
    int m;

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fname && modes);
    if (fname == NULL || modes == NULL)
        return NULL;
#endif

    m = 0;
    if (strchr(modes, 'r'))
        m |= v_FMODE_READ;
    if (strchr(modes, 'w'))
        m |= v_FMODE_WRITE;
    if (strchr(modes, 'a'))
        m |= v_FMODE_APPEND;
    if (strchr(modes, '+'))
        m |= v_FMODE_READWRITE;
    if (strchr(modes, 'b'))
        m |= v_FMODE_BINARY;

    if ((m & v_FMODE_READWRITE) == v_FMODE_WRITE)
        return v_fopen_w();

    if (m & v_FMODE_BINARY)
    {
        if ((m & v_FMODE_READWRITE) == v_FMODE_READWRITE)
            fp = fopen(fname, "r+b");
        else
            fp = fopen(fname, "rb");
    }
    else
    {
        if ((m & v_FMODE_READWRITE) == v_FMODE_READWRITE)
            fp = fopen(fname, "r+");
        else
            fp = fopen(fname, "r");
    }
    assert(fp);
    if (fp)
    {
        v_fp = v_fopen_wb();
        assert(v_fp);
        if (v_fp)
        {
            for (;;)
            {
                n = fread(buf, 1, v_FILE_MAX_BUFFER, fp);
                if (n == 0)
                    break;

                if (v_fwrite_raw(buf, 1, n, v_fp) != n)
                {
                    fclose(fp);
                    v_fclose(v_fp);
                    return NULL;
                }
            }
            if ((m & v_FMODE_APPEND) == v_FMODE_APPEND)
            {
                v_fp->index = v_fp->size;
            }
            else
            {
                v_fp->index = 0;
            }
        }
        fclose(fp);
    }
    return v_fp;
}

int v_fsave(v_LPCSTR fname, v_LPFILE v_fp, v_LPCSTR modes)
{
    FILE *fp;
    int ret = v_EOF;

    #ifndef V_FILE_NEED_SPEED
        /* check paramters */
        assert(v_fp);
        if (v_fp == NULL || v_fp == v_HFILE_ERROR)
            return ret;
        assert(fname);
        assert(strcmp(modes, "w") == 0 || strcmp(modes, "wb") == 0);
    #endif

    fp = fopen(fname, modes);
    assert(fp);
    if (fp)
    {
        if (fwrite(v_fp->data, (size_t)v_fp->size, 1, fp))
            ret = 0;
        ret |= fclose(fp);
    }
    return ret;
}

#ifdef _WIN32
    v_LPFILE v__wfopen(v_LPCWSTR fname, v_LPCWSTR modes)
    {
        FILE *fp;
        size_t n;
        char buf[v_FILE_MAX_BUFFER];
        v_LPFILE v_fp = NULL;
        int m;

#ifndef V_FILE_NEED_SPEED
        /* check parameters */
        assert(fname && modes);
        if (fname == NULL || modes == NULL)
            return NULL;
#endif

        m = 0;
        if (wcschr(modes, L'r'))
            m |= v_FMODE_READ;
        if (wcschr(modes, L'w'))
            m |= v_FMODE_WRITE;
        if (wcschr(modes, L'a'))
            m |= v_FMODE_APPEND;
        if (wcschr(modes, L'+'))
            m |= v_FMODE_READWRITE;
        if (wcschr(modes, L'b'))
            m |= v_FMODE_BINARY;

        if ((m & v_FMODE_READWRITE) == v_FMODE_WRITE)
            return v_fopen_w();

        if (m & v_FMODE_BINARY)
        {
            if ((m & v_FMODE_READWRITE) == v_FMODE_READWRITE)
                fp = _wfopen(fname, L"r+b");
            else
                fp = _wfopen(fname, L"rb");
        }
        else
        {
            if ((m & v_FMODE_READWRITE) == v_FMODE_READWRITE)
                fp = _wfopen(fname, L"r+");
            else
                fp = _wfopen(fname, L"r");
        }
        assert(fp);
        if (fp)
        {
            v_fp = v_fopen_wb();
            assert(v_fp);
            if (v_fp)
            {
                for (;;)
                {
                    n = fread(buf, 1, v_FILE_MAX_BUFFER, fp);
                    if (n == 0)
                        break;

                    if (v_fwrite_raw(buf, 1, n, v_fp) != n)
                    {
                        fclose(fp);
                        v_fclose(v_fp);
                        return NULL;
                    }
                }
                if ((m & v_FMODE_APPEND) == v_FMODE_APPEND)
                {
                    v_fp->index = v_fp->size;
                }
                else
                {
                    v_fp->index = 0;
                }
            }
            fclose(fp);
        }
        return v_fp;
    }

    int v__wfsave(v_LPCWSTR fname, v_LPFILE v_fp, v_LPCWSTR modes)
    {
        FILE *fp;
        int ret = v_EOF;

        #ifndef V_FILE_NEED_SPEED
            /* check paramters */
            assert(v_fp);
            if (v_fp == NULL || v_fp == v_HFILE_ERROR)
                return ret;
            assert(fname);
            assert(wcscmp(modes, L"w") == 0 || wcscmp(modes, L"wb") == 0);
        #endif

        fp = _wfopen(fname, modes);
        assert(fp);
        if (fp)
        {
            if (fwrite(v_fp->data, (size_t)v_fp->size, 1, fp))
                ret = 0;
            ret |= fclose(fp);
        }
        return ret;
    }
#endif  /* def _WIN32 */

/**************************************************************************/
/* Windows file operation */

#if (defined(WIN16) || defined(_WIN32))
    v_HFILE WINAPI v__lcreat(LPCSTR fname, int attr)
    {
        v_HFILE hf;
        if (attr & 1)
            hf = v_fopen(fname, "rb");
        else
            hf = v_fopen(fname, "wb");
        if (hf == NULL)
            hf = v_HFILE_ERROR;
        return hf;
    }

    v_HFILE WINAPI v__lopen(LPCSTR fname, int attr)
    {
        v_HFILE hf;
        switch (attr & 3)
        {
        case OF_READ:
            hf = v_fopen(fname, "rb");
            break;
        case OF_WRITE:
            hf = v_fopen(fname, "wb");
            break;
        case OF_READWRITE:
            hf = v_fopen(fname, "r+b");
            break;
        default:
            hf = NULL;
            assert(0);
        }
        if (hf == NULL)
            hf = v_HFILE_ERROR;
        return hf;
    }

    LONG WINAPI v__llseek(v_HFILE hf, LONG offset, int origin)
    {
        if (hf != NULL && hf != v_HFILE_ERROR)
        {
            switch (origin)
            {
            case FILE_BEGIN:
                if (v_fseek(hf, offset, v_SEEK_SET) == 0)
                {
                    return (LONG)hf->index;
                }
                break;
            case FILE_CURRENT:
                if (v_fseek(hf, offset, v_SEEK_CUR) == 0)
                {
                    return (LONG)hf->index;
                }
                break;
            case FILE_END:
                if (v_fseek(hf, offset, v_SEEK_END) == 0)
                {
                    return (LONG)hf->index;
                }
                break;
            default:
                assert(0);
            }
        }
        return 0xFFFFFFFFL;
    }

    UINT WINAPI v__lread(v_HFILE hf, LPVOID buffer, UINT cb)
    {
        UINT read;
        if (hf && hf != v_HFILE_ERROR)
        {
            read = (UINT)v_fread(buffer, 1, cb, hf);
            if (read)
                return read;
        }
        #ifdef WIN16
            return 0xFFFF;
        #else
            return 0xFFFFFFFFU;
        #endif
    }

    UINT WINAPI v__lwrite(v_HFILE hf, LPCSTR buffer, UINT cb)
    {
        UINT written;
        if (hf && hf != v_HFILE_ERROR)
        {
            written = (UINT)v_fwrite(buffer, 1, cb, hf);
            if (written)
                return written;
        }
        #ifdef WIN16
            return 0xFFFF;
        #else
            return 0xFFFFFFFFU;
        #endif
    }

    v_HFILE WINAPI v__lclose(v_HFILE hf)
    {
        if (hf != NULL && hf != v_HFILE_ERROR)
        {
            v_CloseHandle(hf);
            return NULL;
        }
        return v_HFILE_ERROR;
    }

    v_HFILE WINAPI v_OpenFile(LPCSTR fname, LPOFSTRUCT pofs, UINT style)
    {
        v_LPFILE fp;

        assert(fname && pofs);
        if (fname == NULL || pofs == NULL)
            return v_HFILE_ERROR;

        if (style & OF_DELETE)
            return (v_HFILE)DeleteFileA(fname);
        if (style & OF_CREATE)
            return v_fopen_wb();
        if (style & OF_PARSE)
        {
            ZeroMemory(pofs, sizeof(*pofs));
            pofs->cBytes = sizeof(*pofs);
            lstrcpynA(pofs->szPathName, fname, OFS_MAXPATHNAME);
            return NULL;
        }
        if (style & OF_EXIST)
        {
            return (v_HFILE)(~GetFileAttributesA(fname) ? 1 : -1);
        }

        if (style & OF_PROMPT)
        {
            while (!~GetFileAttributesA(fname))
            {
                LPSTR p, q;
                CHAR title[256], text[256];
                GetModuleFileNameA(NULL, text, 256);
                p = q = text;
                while (*q)
                {
                    if (*q == '\\' || *q == '/')
                        p = q + 1;
                    q++;
                }
                #ifdef NO_JAP_ALLOWED
                    lstrcpyA(title, p);
                    lstrcatA(title, " - The file is not found");
                    lstrcpyA(text, "File ");
                    lstrcatA(text, fname);
                    lstrcpyA(text, " doesn't exist.");
                #else
                    lstrcpyA(title, p);
                    lstrcatA(title, " - ファイルが見つかりません");
                    lstrcpyA(text, "ファイル ");
                    lstrcatA(text, fname);
                    lstrcpyA(text, " は存在しません。");
                #endif
                if (MessageBoxA(NULL, text, title,
                                MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
                {
                    return v_HFILE_ERROR;
                }
            }
        }

        if (style & OF_REOPEN)
        {
            /* TODO & FIXME */
            assert(0);
            return v_HFILE_ERROR;
        }

        if (style & OF_VERIFY)
        {
            /* TODO & FIXME */
        }

        fp = NULL;
        switch (style & 3)
        {
        case OF_READ:
            fp = v_fopen(fname, "rb");
            break;
        case OF_WRITE:
            fp = v_fopen_wb();
            break;
        case OF_READWRITE:
            fp = v_fopen(fname, "r+b");
            break;
        }
        if (fp == NULL)
            fp = v_HFILE_ERROR;
        return fp;
    }
#endif  /* (defined(WIN16) || defined(_WIN32)) */

#ifdef WIN16
    HANDLE WINAPI v_CreateFile16(LPCSTR fname, DWORD access, DWORD share,
                                 LPVOID sa, DWORD creation, DWORD flags,
                                 HANDLE hTempFile)
    {
        v_LPFILE fp;
        #ifdef V_FILE_USE_STDIO
            if (lstrcmpiA(fname, "CON") == 0)
            {
                if (access & GENERIC_READ)
                    return v_stdin;
                if (access & GENERIC_WRITE)
                    return v_stdout;
                return INVALID_HANDLE_VALUE;
            }
            if (lstrcmpiA(fname, "CONIN$") == 0)
            {
                return v_stdin;
            }
            if (lstrcmpiA(fname, "CONOUT$") == 0)
            {
                return v_stdout;
            }
        #endif  /* def V_FILE_USE_STDIO */
        fp = NULL;
        if ((access & GENERIC_READ) && (access & GENERIC_WRITE))
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v_fopen(fname, "w+b");
            }
            else if (creation == OPEN_EXISTING)
            {
                fp = v_fopen(fname, "r+b");
            }
            else if (creation == OPEN_ALWAYS)
            {
                fp = v_fopen(fname, "r+b");
                if (fp == NULL)
                    fp = v_fopen_wb();
            }
        }
        else if (access & GENERIC_WRITE)
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v_fopen(fname, "wb");
            }
        }
        else if (access & GENERIC_READ)
        {
            if (creation == OPEN_EXISTING)
            {
                fp = v_fopen(fname, "rb");
            }
        }
        if (fp == NULL)
            fp = INVALID_HANDLE_VALUE;
        return fp;
    }
#endif  /* def WIN16 */

#ifdef _WIN32
    HANDLE WINAPI v_CreateFileA(LPCSTR fname, DWORD access, DWORD share,
                                LPVOID sa, DWORD creation, DWORD flags,
                                HANDLE hTempFile)
    {
        v_LPFILE fp;
        #ifdef V_FILE_USE_STDIO
            if (lstrcmpiA(fname, "CON") == 0)
            {
                if (access & GENERIC_READ)
                    return v_stdin;
                if (access & GENERIC_WRITE)
                    return v_stdout;
                return INVALID_HANDLE_VALUE;
            }
            if (lstrcmpiA(fname, "CONIN$") == 0)
            {
                return v_stdin;
            }
            if (lstrcmpiA(fname, "CONOUT$") == 0)
            {
                return v_stdout;
            }
        #endif  /* def V_FILE_USE_STDIO */
        fp = NULL;
        if ((access & GENERIC_READ) && (access & GENERIC_WRITE))
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v_fopen(fname, "w+b");
            }
            else if (creation == OPEN_EXISTING)
            {
                fp = v_fopen(fname, "r+b");
            }
            else if (creation == OPEN_ALWAYS)
            {
                fp = v_fopen(fname, "r+b");
                if (fp == NULL)
                    fp = v_fopen_wb();
            }
        }
        else if (access & GENERIC_WRITE)
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v_fopen(fname, "wb");
            }
        }
        else if (access & GENERIC_READ)
        {
            if (creation == OPEN_EXISTING)
            {
                fp = v_fopen(fname, "rb");
            }
        }
        if (fp == NULL)
            fp = INVALID_HANDLE_VALUE;
        return fp;
    }

    HANDLE WINAPI v_CreateFileW(LPCWSTR fname, DWORD access, DWORD share,
                                LPVOID sa, DWORD creation, DWORD flags,
                                HANDLE hTempFile)
    {
        v_LPFILE fp;
        #ifdef V_FILE_USE_STDIO
            /* is it console I/O? */
            if (lstrcmpiW(fname, L"CON") == 0)
            {
                if (access & GENERIC_WRITE)
                    return v_stdout;
                if (access & GENERIC_READ)
                    return v_stdin;
                return INVALID_HANDLE_VALUE;
            }
            if (lstrcmpiW(fname, L"CONIN$") == 0)
            {
                return v_stdin;
            }
            if (lstrcmpiW(fname, L"CONOUT$") == 0)
            {
                return v_stdout;
            }
        #endif  /* def V_FILE_USE_STDIO */
        fp = NULL;
        if ((access & GENERIC_READ) && (access & GENERIC_WRITE))
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v__wfopen(fname, L"w+b");
            }
            else if (creation == OPEN_EXISTING)
            {
                fp = v__wfopen(fname, L"r+b");
            }
            else if (creation == OPEN_ALWAYS)
            {
                fp = v__wfopen(fname, L"r+b");
                if (fp == NULL)
                    fp = v_fopen_wb();
            }
        }
        else if (access & GENERIC_WRITE)
        {
            if (creation == CREATE_NEW || creation == CREATE_ALWAYS ||
                creation == TRUNCATE_EXISTING)
            {
                fp = v__wfopen(fname, L"wb");
            }
        }
        else if (access & GENERIC_READ)
        {
            if (creation == OPEN_EXISTING)
            {
                fp = v__wfopen(fname, L"rb");
            }
        }
        if (fp == NULL)
            fp = INVALID_HANDLE_VALUE;
        return fp;
    }

    BOOL WINAPI v_CloseHandle(HANDLE hFile)
    {
        v_FILE *fp = (v_FILE *)hFile;
        assert(fp);
        return v_fclose(fp) == 0;
    }

    BOOL WINAPI v_ReadFile(HANDLE hFile, LPVOID pBuf, DWORD to_read,
                           LPDWORD read, LPVOID o)
    {
        v_FILE *fp = (v_FILE *)hFile;
#ifndef V_FILE_NEED_SPEED
        assert(fp && fp != v_HFILE_ERROR && read);
        if (fp == NULL || fp == v_HFILE_ERROR || read == NULL)
            return FALSE;
#endif
        *read = (DWORD)v_fread_raw(pBuf, 1, to_read, fp);
        return *read > 0;
    }

    BOOL WINAPI v_WriteFile(HANDLE hFile, LPCVOID pBuf, DWORD to_write,
                            LPDWORD written, LPVOID o)
    {
        v_FILE *fp = (v_FILE *)hFile;
#ifndef V_FILE_NEED_SPEED
        assert(fp && fp != v_HFILE_ERROR && written);
        if (fp == NULL || fp == v_HFILE_ERROR  || written == NULL)
            return FALSE;
#endif
        *written = (DWORD)v_fwrite_raw(pBuf, 1, to_write, fp);
        return *written > 0;
    }

    BOOL WINAPI v_FlushFileBuffer(HANDLE hFile)
    {
        v_FILE *fp = (v_FILE *)hFile;
        return v_fflush(fp) == 0;
    }

    DWORD WINAPI v_GetFileSize(HANDLE hFile, LPDWORD hi)
    {
        v_FILE *fp = (v_FILE *)hFile;
        DWORDLONG size;

#ifndef V_FILE_NEED_SPEED
        assert(fp || fp != v_HFILE_ERROR);
        if (fp == NULL || fp == v_HFILE_ERROR)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return 0xFFFFFFFFUL;
        }
#endif
        size = fp->size;
        if (hi)
        {
            *hi = (DWORD)(size >> 32);
            SetLastError(NO_ERROR);
            return (DWORD)size;
        }
        else if (size > 0xFFFFFFFFUL)
        {
            SetLastError(ERROR_MORE_DATA);
            return 0xFFFFFFFFUL;
        }
        else
        {
            SetLastError(NO_ERROR);
            return (DWORD)size;
        }
    }

    BOOL WINAPI v_SetEndOfFile(HANDLE hFile)
    {
        v_FILE *fp = (v_FILE *)hFile;

#ifndef V_FILE_NEED_SPEED
        assert(fp && fp != v_HFILE_ERROR);
        if (fp == NULL || fp == v_HFILE_ERROR)
            return FALSE;
#endif
        fp->size = fp->index;
        return TRUE;
    }

    DWORD WINAPI v_SetFilePointer(HANDLE hFile, LONG lo, PLONG hi,
                                  DWORD method)
    {
        LONGLONG offset;
        v_FILE *fp = (v_FILE *)hFile;

#ifndef V_FILE_NEED_SPEED
        /* check paramters */
        assert(fp && fp != v_HFILE_ERROR);
        if (fp == NULL || fp == v_HFILE_ERROR)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return 0xFFFFFFFFUL;
        }
#endif

        /* calculcate practical offset */
        if (hi)
            offset = lo | (((LONGLONG)*hi) << 32);
        else
            offset = lo;

        switch (method)
        {
        case FILE_BEGIN:
            if (offset >= 0)
            {
                if (fp->index + offset <= fp->size)
                    fp->index += (long)offset;
                else
                    fp->index = fp->size;
            }
            else
            {
                if ((v_fpos_t)-offset <= fp->index)
                    fp->index += (long)offset;
                else
                    fp->index = 0;
            }
            break;
        case FILE_CURRENT:
            assert(fp->size <= 0x7FFFFFFF);
            if ((v_fpos_t)offset <= fp->size)
                fp->index = (v_fpos_t)(fp->size - offset);
            else
                fp->index = 0;
            break;
        case FILE_END:
            assert(fp->size <= 0x7FFFFFFF);
            if ((v_fpos_t)offset <= fp->size)
                fp->index = (v_fpos_t)offset;
            else
                fp->index = fp->size;
            break;
        default:
            assert(0);
            return 0xFFFFFFFFUL;
        }

        if (hi)
        {
            /* store high */
            *hi = (DWORD)(fp->index >> 32);
            SetLastError(NO_ERROR);
        }
        else if (fp->index > 0xFFFFFFFFUL)
        {
            /* cannot store high */
            SetLastError(ERROR_MORE_DATA);
        }
        else
        {
            /* no high required */
            SetLastError(NO_ERROR);
        }
        return (DWORD)(fp->index & 0xFFFFFFFFUL);
    }
#endif  /* def _WIN32 */

/**************************************************************************/
/* binary transfer */

size_t v_fread_raw(v_LPVOID ptr, size_t siz, size_t nelem, v_LPFILE fp)
{
    v_fpos_t count, read_size;
    v_LPCHAR pch;

#ifndef V_FILE_NEED_SPEED
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
    pch = (v_LPCHAR)fp->data + (size_t)fp->index;
    memcpy(ptr, pch, (size_t)read_size);

    /* proceed the position */
    fp->index += read_size;

    /* return the count of blocks read */
    return (size_t)count;
}

size_t v_fwrite_raw(v_LPCVOID ptr, size_t siz, size_t nelem, v_LPFILE fp)
{
    size_t increment;
    v_fpos_t end;
    v_LPCHAR pch;

#ifndef V_FILE_NEED_SPEED
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
        v_LPVOID data = realloc(fp->data, (size_t)(end + 1));
        if (data == NULL)
        {
            fp->modes |= v_FMODE_ERROR;
            return 0;
        }

        fp->data = (v_LPCHAR)data;
        fp->size = end;
        fp->data[(size_t)end] = 0;
    }

    /* retrieve the current position */
    /* copy from buffer */
    pch = (v_LPCHAR)fp->data + (size_t)fp->index;
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

#ifndef V_FILE_NEED_SPEED
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
        ch = *(fp->data + (size_t)fp->index);
        fp->index += sizeof(char);
        return ch;
    }

#if (v_FMODE_TEXT == 0)
    /* text mode: eat every '\r' */
    do
    {
        ch = *(fp->data + (size_t)fp->index);
        fp->index += sizeof(char);
    } while (ch == '\r' && fp->index + sizeof(char) <= fp->size);
    return ch;
#endif
}

int v_fputc(char c, v_LPFILE fp)
{
#ifndef V_FILE_NEED_SPEED
    /* check parameter */
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
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

size_t v_fread(v_LPVOID ptr, size_t siz, size_t nelem, v_LPFILE fp)
{
#if (v_FMODE_TEXT == 0)
    v_LPCHAR pch;
    int ch;
    size_t i, count;
#endif

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
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

size_t v_fwrite(v_LPCVOID ptr, size_t siz, size_t nelem, v_LPFILE fp)
{
#if (v_FMODE_TEXT == 0)
    v_LPCSTR pch;
    int ret;
    size_t i, count;
#endif

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
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
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
        return v_EOF;
    if (fp->size < fp->index)
        return v_EOF;
    if (fp->modes & v_FMODE_ERROR)
        return v_EOF;
    return 0;
}

void v_clearerr(v_LPFILE fp)
{
    assert(fp && fp != v_HFILE_ERROR);
#ifndef V_FILE_NEED_SPEED
    if (fp == NULL || fp == v_HFILE_ERROR)
        return;
#endif
    fp->modes &= ~v_FMODE_ERROR;
}

/**************************************************************************/
/* file position */

long v_ftell(v_LPFILE fp)
{
#ifndef V_FILE_NEED_SPEED
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
#ifndef V_FILE_NEED_SPEED
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
#ifndef V_FILE_NEED_SPEED
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
#ifndef V_FILE_NEED_SPEED
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
#ifndef V_FILE_NEED_SPEED
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
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
#ifndef V_FILE_NEED_SPEED
    assert(fp && fp != v_HFILE_ERROR);
    if (fp == NULL || fp == v_HFILE_ERROR)
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
    *((v_LPCHAR)fp->data + (size_t)fp->index) = c;
    return c;
}

/**************************************************************************/
/* read / write string */

v_LPSTR v_fgets(v_LPSTR s, int n, v_LPFILE fp)
{
    int i;
    char ch;

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    if (n == 0 || fp == NULL || fp == v_HFILE_ERROR)
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

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(s && fp && fp != v_HFILE_ERROR);
    if (s == NULL || fp == NULL || fp == v_HFILE_ERROR)
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

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR && format);
    if (fp == NULL || fp == v_HFILE_ERROR || format == NULL)
        return v_EOF;
#endif

    va_start(va, format);
    ret = v_vfscanf(fp, format, va);
    va_end(va);

    return ret;
}

int v_vfscanf(v_LPFILE fp, v_LPCSTR format, va_list va)
{
    char buf[v_FILE_MAX_BUFFER];

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR && format);
    if (fp == NULL || fp == v_HFILE_ERROR || format == NULL)
        return v_EOF;
#endif

    /* read one line */
    if (v_fgets(buf, v_FILE_MAX_BUFFER, fp))
    {
        /* FIXME: one line only? */
        return vsscanf(buf, format, va);
    }
    return v_EOF;
}

/**************************************************************************/
/* print */

int v_fprintf(v_LPFILE fp, v_LPCSTR format, ...)
{
    va_list va;
    int n;

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR && format);
    if (fp == NULL || fp == v_HFILE_ERROR || format == NULL)
        return v_EOF;
#endif

    va_start(va, format);
    n = v_vfprintf(fp, format, va);
    va_end(va);

    return n;
}

int v_vfprintf(v_LPFILE fp, v_LPCSTR format, va_list va)
{
    int n;
    char buf[v_FILE_MAX_BUFFER];
    size_t len;

#ifndef V_FILE_NEED_SPEED
    /* check parameters */
    assert(fp && fp != v_HFILE_ERROR && format);
    if (fp == NULL || fp == v_HFILE_ERROR || format == NULL)
        return v_EOF;
#endif

    n = vsprintf(buf, format, va);
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

#ifndef V_FILE_NEED_SPEED
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
    /* the v_file standard I/O objects */
    v_LPFILE v_stdin = NULL;
    v_LPFILE v_stdout = NULL;
    v_LPFILE v_stderr = NULL;

    /* initialize the v_file standard I/O */
    void v_file_init_stdio(
        v_LPCVOID input_data, size_t input_size, v_LPCSTR modes)
    {
#ifndef V_FILE_NEED_SPEED
        assert(modes);
        assert(input_data || input_size == 0);
        assert(strcmp(modes, "r") == 0 || strcmp(modes, "rb") == 0);
        v_file_destroy_stdio();
#endif
        if (strchr(modes, 'b'))
            v_stdin = v_fopen_rb(input_data, input_size);
        else
            v_stdin = v_fopen_r(input_data, input_size);
        v_stdout = v_fopen_wb();
        v_stderr = v_fopen_wb();
    }

    void v_file_init_stdio_2(v_LPCSTR input_file_name, v_LPCSTR modes)
    {
#ifndef V_FILE_NEED_SPEED
        assert(modes);
        assert(strcmp(modes, "r") == 0 || strcmp(modes, "rb") == 0);
        v_file_destroy_stdio();
#endif
        if (input_file_name)
            v_stdin = v_fopen(input_file_name, modes);
        else
            v_stdin = v_fopen_r(NULL, 0);
        v_stdout = v_fopen_wb();
        v_stderr = v_fopen_wb();
    }

    /* destroy the v_file standard I/O */
    void v_file_destroy_stdio(void)
    {
        if (v_stdin)
        {
            v_fclose(v_stdin);
            v_stdin = NULL;
        }
        if (v_stdout)
        {
            v_fclose(v_stdout);
            v_stdout = NULL;
        }
        if (v_stderr)
        {
            v_fclose(v_stderr);
            v_stderr = NULL;
        }
    }

    #ifndef V_FILE_NEED_SPEED
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

        int v_puts(v_LPCSTR s)
        {
            assert(v_stdout && s);
            if (v_stdout == NULL || s == NULL)
                return v_EOF;
            v_fputs(s, v_stdout);
            v_putchar('\n');
            return 0;
        }
    #endif  /* ndef V_FILE_NEED_SPEED */

    v_LPSTR v_gets(v_LPSTR s)
    {
        size_t len;

#ifndef V_FILE_NEED_SPEED
        assert(v_stdin && s);
        if (v_stdin == NULL || s == NULL)
        {
            return NULL;
        }
#endif

        s = v_fgets(s, v_FILE_MAX_BUFFER, v_stdin);
        len = strlen(s);
        if (len > 0 && s[len - 1] == '\n')
        {
            s[len - 1] = 0;
        }
        return s;
    }

    int v_printf(v_LPCSTR format, ...)
    {
        va_list va;
        int n;

#ifndef V_FILE_NEED_SPEED
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

    int v_vprintf(v_LPCSTR format, va_list va)
    {
        int n;

#ifndef V_FILE_NEED_SPEED
        /* check parameters */
        assert(v_stdout && format);
        if (v_stdout == NULL || format == NULL)
            return v_EOF;
#endif

        n = v_vfprintf(v_stdout, format, va);
        return n;
    }

    int v_scanf(v_LPCSTR format, ...)
    {
        va_list va;
        int ret;

#ifndef V_FILE_NEED_SPEED
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

    int v_vscanf(v_LPCSTR format, va_list va)
    {
        char buf[v_FILE_MAX_BUFFER];

#ifndef V_FILE_NEED_SPEED
        /* check parameters */
        assert(v_stdin && format);
        if (v_stdin == NULL || format == NULL)
            return v_EOF;
#endif

        /* read one line */
        if (v_fgets(buf, v_FILE_MAX_BUFFER, v_stdin))
        {
            /* FIXME: one line only? */
            return vsscanf(buf, format, va);
        }
        return v_EOF;
    }
#endif  /* def V_FILE_USE_STDIO */

/**************************************************************************/
/* secure functions */

#ifdef V_FILE_WANT_SECURE_LIB
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
#endif  /* def V_FILE_WANT_SECURE_LIB */

/**************************************************************************/
/* C/C++ switching */

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

/**************************************************************************/
