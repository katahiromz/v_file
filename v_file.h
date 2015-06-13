/**************************************************************************/
/* v_file.h --- virtual file operation on memory                          */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#ifndef KATAHIROMZ_V_FILE_H_
#define KATAHIROMZ_V_FILE_H_  /* version */ 1 

#ifdef __cplusplus
    #include <cstdarg>
#else
    #include <stdarg.h>
#endif

#if defined(_WIN32) || defined(WIN16)
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/**************************************************************************/
/* configuration */

/*
 * 1. Please specify the sufficient buffer size for the insecure functions.
 */
#ifndef v_FILE_MAX_BUFFER
    #if defined(MSDOS) || defined(WIN16)
        #define v_FILE_MAX_BUFFER   256
    #else
        #define v_FILE_MAX_BUFFER   1024
    #endif
#endif

/*
 * 2. Which would you choose, speed or safety?
 */
#define V_FILE_SPEED                /* speed */
/* #undef V_FILE_SPEED */           /* safety */

/**************************************************************************/
/* constants */

/* virtual "SEEK_*" values */
#define v_SEEK_CUR  1
#define v_SEEK_END  2
#define v_SEEK_SET  0

/* virtual file modes */
#define v_FMODE_ERROR       1   /* is there any error? */
#define v_FMODE_READ        2   /* read */
#define v_FMODE_WRITE       4   /* write */
#define v_FMODE_READWRITE   6   /* read+write */
#define v_FMODE_APPEND      14  /* append (includes v_FMODE_READWRITE) */
#define v_FMODE_BINARY      16  /* binary mode */

#if defined(MSDOS) || defined(WIN16) || defined(_WIN32)
    /* NOTE: Windows and MS-DOS can use "text mode" (not binary mode). */
    #define v_FMODE_TEXT    0   /* text mode */
#else
    /* There is no text mode on the other platforms */
    #define v_FMODE_TEXT    v_FMODE_BINARY
#endif

/* virtual end of file (EOF) */
#define v_EOF (-1)

/**************************************************************************/
/* types */

#if defined(MSDOS) || defined(WIN16)
    #ifndef FAR
        #define FAR far
    #endif
#endif

/* virtual "fpos_t" type */
typedef unsigned long v_fpos_t;

/* virtual pointer types */
#if defined(MSDOS) || defined(WIN16)
    typedef char FAR *          v_LPCHAR;
    typedef char FAR *          v_LPSTR;
    typedef const char FAR *    v_LPCSTR;
    typedef void FAR *          v_LPVOID;
    typedef const void FAR *    v_LPCVOID;
#else
    typedef char *              v_LPCHAR;
    typedef char *              v_LPSTR;
    typedef const char *        v_LPCSTR;
    typedef void *              v_LPVOID;
    typedef const void *        v_LPCVOID;
#endif

/* virtual "FILE" structure */
typedef struct v_FILE
{
    v_LPCHAR    data;       /* malloc'ed and free'd */
    v_fpos_t    index;      /* file position */
    v_fpos_t    size;       /* file size */
    int         modes;      /* virtual file modes */
} v_FILE;

/* pointer to v_FILE */
#if defined(MSDOS) || defined(WIN16)
    typedef v_FILE FAR *        v_LPFILE;
#else
    typedef v_FILE *            v_LPFILE;
#endif

/**************************************************************************/
/* opening / closing */

v_LPFILE 
v_fopen_intern(v_LPCVOID data, v_fpos_t index, v_fpos_t siz, int modes);

/*
 * text mode
 */
v_LPFILE v_fopen_r  (v_LPCVOID data, v_fpos_t siz);      /* mode "r"  */
v_LPFILE v_fopen_w  (void);                              /* mode "w"  */
v_LPFILE v_fopen_a  (v_LPCVOID data, v_fpos_t siz);      /* mode "a"  */

v_LPFILE v_fopen_rp (v_LPCVOID data, v_fpos_t siz);      /* mode "r+" */
#define  v_fopen_wp v_fopen_rp                           /* mode "w+" */
v_LPFILE v_fopen_ap (v_LPCVOID data, v_fpos_t siz);      /* mode "a+" */

/*
 * binary mode
 */
v_LPFILE v_fopen_rb (v_LPCVOID data, v_fpos_t siz);      /* mode "rb" */
v_LPFILE v_fopen_wb (void);                              /* mode "wb" */
v_LPFILE v_fopen_ab (v_LPCVOID data, v_fpos_t siz);      /* mode "ab" */

v_LPFILE v_fopen_rpb(v_LPCVOID data, v_fpos_t siz);      /* mode "r+b" */
#define  v_fopen_wpb v_fopen_rpb                         /* mode "w+b" */
v_LPFILE v_fopen_apb(v_LPCVOID data, v_fpos_t siz);      /* mode "a+b" */

#define v_tmpfile v_fopen_wpb

/*
 * closing
 */
int         v_fclose(v_LPFILE fp);              /* will free fp->data  */
v_LPCHAR    v_fclose_detach(v_LPFILE fp);       /* won't free fp->data */

/**************************************************************************/
/* binary transfer */

int v_fread_raw(v_LPVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp);
int v_fwrite_raw(v_LPCVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp);

/**************************************************************************/
/* read / write buffer */

int v_fread(v_LPVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp);
int v_fwrite(v_LPCVOID ptr, v_fpos_t siz, v_fpos_t nelem, v_LPFILE fp);

/**************************************************************************/
/* read / write a character */

int v_fgetc(v_LPFILE fp);
int v_fputc(char c, v_LPFILE fp);

#define v_getc v_fgetc
#define v_putc v_fputc

/**************************************************************************/
/* file status */

int v_feof(v_LPFILE fp);
int v_ferror(v_LPFILE fp);
void v_clearerr(v_LPFILE fp);

/**************************************************************************/
/* file position */

long v_ftell(v_LPFILE fp);
int v_fgetpos(v_LPFILE fp, v_fpos_t *pos);
int v_fsetpos(v_LPFILE fp, const v_fpos_t *pos);
void v_rewind(v_LPFILE fp);
int v_fseek(v_LPFILE fp, long offset, int type);

/**************************************************************************/
/* unget a character */

int v_ungetc(char c, v_LPFILE fp);

/**************************************************************************/
/* read / write string */

v_LPSTR v_fgets(v_LPSTR s, int n, v_LPFILE fp);
int v_fputs(v_LPCSTR s, v_LPFILE fp);

/**************************************************************************/
/* print */

int v_fprintf(v_LPFILE fp, v_LPCSTR format, ...);
int v_vfprintf(v_LPFILE fp, v_LPCSTR format, va_list arg);

/**************************************************************************/
/* scan */

int v_fscanf(v_LPFILE fp, v_LPCSTR format, ...);
int v_vfscanf(v_LPFILE fp, v_LPCSTR format, va_list arg);

/**************************************************************************/
/* flush */

#ifndef V_FILE_SPEED
    int v_fflush(v_LPFILE fp);
#else
    #define v_fflush(fp)
#endif

/**************************************************************************/
/* Windows .exe resource */

#ifdef WIN16
    /* opening old 16-bit Windows .exe resource as virtual file */
    v_LPFILE v_fopen_res16(HMODULE hMod, v_LPCSTR res_id, v_LPCSTR type);
#endif

#ifdef _WIN32
    /* opening Windows .exe resource as virtual file */
    v_LPFILE v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type);
#endif

/**************************************************************************/

#ifdef V_FILE_SPEED
    #define v_fopen_r(data, siz) \
        v_fopen_intern((data), 0, (siz), v_FMODE_READ | v_FMODE_TEXT)
    #define v_fopen_a(data, siz) \
        v_fopen_intern((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_TEXT)
    #define v_fopen_rb(data, siz) \
        v_fopen_intern((data), 0, (siz), v_FMODE_READ | v_FMODE_BINARY)
    #define v_fopen_ab(data, siz) \
        v_fopen_intern((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_BINARY)
    #define v_fopen_rp(data, siz) \
        v_fopen_intern((data), 0, (siz), v_FMODE_READWRITE | v_FMODE_TEXT)
    #define v_fopen_ap(data, siz) \
        v_fopen_intern((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_TEXT)
    #define v_fopen_rpb(data, siz) \
        v_fopen_intern((data), 0, (siz), v_FMODE_READWRITE | v_FMODE_BINARY)
    #define v_fopen_apb(data, siz) \
        v_fopen_intern((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_BINARY)
#endif  /* def V_FILE_SPEED */

/**************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

#endif  /* ndef KATAHIROMZ_V_FILE_H_ */
