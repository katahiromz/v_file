/**************************************************************************/
/* v_file.h --- virtual file operation on memory                          */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#ifndef KATAHIROMZ_V_FILE_H_
#define KATAHIROMZ_V_FILE_H_  /* version */ 2

#ifdef __cplusplus
    #include <cstdarg>
#else
    #include <stdarg.h>
#endif

#if defined(WIN16) || defined(_WIN32)
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
/* #define V_FILE_NEED_SPEED */             /* speed */
#undef V_FILE_NEED_SPEED                    /* safety */

/*
 * 3. Would you like the secure functions?
 */
/* #define V_FILE_WANT_SECURE_LIB */        /* Yes */
#undef V_FILE_WANT_SECURE_LIB               /* No */

/*
 * 4. Would you use the v_file standard I/O?
 */
#define V_FILE_USE_STDIO                    /* Yes */
/* #undef V_FILE_USE_STDIO */               /* No */

/**************************************************************************/
/* validation */

#if defined(V_FILE_NEED_SPEED) && defined(V_FILE_WANT_SECURE_LIB)
    #error Choose either V_FILE_NEED_SPEED or V_FILE_WANT_SECURE_LIB. You lose.
#endif

/**************************************************************************/
/* constants */

/* virtual "SEEK_*" values */
#define v_SEEK_CUR          1
#define v_SEEK_END          2
#define v_SEEK_SET          0

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
#define v_EOF               (-1)

/**************************************************************************/
/* types */

#if defined(MSDOS) || defined(WIN16)
    #ifndef FAR
        #define FAR far
    #endif
#endif

/* virtual "fpos_t" type */
typedef size_t                  v_fpos_t;
#if 0
    #if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
        typedef unsigned long long  v_fpos_t;
    #elif defined(MSDOS) || defined(WIN16)
        typedef unsigned int        v_fpos_t;
    #else
        typedef unsigned long       v_fpos_t;
    #endif
#endif

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

#ifdef _WIN32
    typedef       wchar_t *     v_LPWSTR;
    typedef const wchar_t *     v_LPCWSTR;
#endif

/* virtual "FILE" structure */
typedef struct v_FILE
{
    v_LPCHAR        data;       /* malloc'ed and free'd */
    v_fpos_t        index;      /* file position */
    v_fpos_t        size;       /* file size */
    int             modes;      /* virtual file modes */
    union user_data
    {
        v_LPVOID    p;
        size_t      i;
    } user;
} v_FILE;

/* pointer to v_FILE */
#if defined(MSDOS) || defined(WIN16)
    typedef v_FILE FAR *        v_LPFILE;
#else
    typedef v_FILE *            v_LPFILE;
#endif

/**************************************************************************/
/* opening / closing */

/*
 * the internal function (don't use directly)
 */
v_LPFILE 
v_fopen_intern(v_LPCVOID data, size_t index, size_t siz, int modes);

/*
 * text mode
 */
v_LPFILE v_fopen_r  (v_LPCVOID data, size_t siz);       /* mode "r"  */
v_LPFILE v_fopen_w  (void);                             /* mode "w"  */
v_LPFILE v_fopen_a  (v_LPCVOID data, size_t siz);       /* mode "a"  */

v_LPFILE v_fopen_rp (v_LPCVOID data, size_t siz);       /* mode "r+" */
#define  v_fopen_wp v_fopen_rp                          /* mode "w+" */
v_LPFILE v_fopen_ap (v_LPCVOID data, size_t siz);       /* mode "a+" */

/*
 * binary mode
 */
v_LPFILE v_fopen_rb (v_LPCVOID data, size_t siz);       /* mode "rb" */
v_LPFILE v_fopen_wb (void);                             /* mode "w"  */
v_LPFILE v_fopen_ab (v_LPCVOID data, size_t siz);       /* mode "ab" */

v_LPFILE v_fopen_rpb(v_LPCVOID data, size_t siz);       /* mode "r+b" */
#define  v_fopen_wpb v_fopen_wb                         /* mode "w+b" */
v_LPFILE v_fopen_apb(v_LPCVOID data, size_t siz);       /* mode "a+b" */

#define v_tmpfile v_fopen_wpb

/*
 * closing
 */
int         v_fclose(v_LPFILE fp);              /* will free fp->data  */
v_LPCHAR    v_fclose_detach(v_LPFILE fp);       /* won't free fp->data */

/**************************************************************************/
/* set binary mode / set text mode */

void v_fsetbin (v_FILE *fp);
void v_fsettext(v_FILE *fp);

/**************************************************************************/
/* loading from real file / saving to real file */

v_LPFILE        v_fopen (v_LPCSTR fname, v_LPCSTR modes);
int             v_fsave (v_LPCSTR fname, v_LPFILE v_fp);

#ifdef _WIN32
    v_LPFILE    v_wfopen(v_LPCWSTR fname, v_LPCWSTR modes);
    int         v_wfsave(v_LPCWSTR fname, v_LPFILE v_fp);
#endif

/**************************************************************************/
/* binary transfer */

size_t v_fread_raw (v_LPVOID ptr,  size_t siz, size_t nelem, v_LPFILE fp);
size_t v_fwrite_raw(v_LPCVOID ptr, size_t siz, size_t nelem, v_LPFILE fp);

/**************************************************************************/
/* read / write buffer */

size_t v_fread (v_LPVOID ptr,  size_t siz, size_t nelem, v_LPFILE fp);
size_t v_fwrite(v_LPCVOID ptr, size_t siz, size_t nelem, v_LPFILE fp);

/**************************************************************************/
/* read / write a character */

int v_fgetc(v_LPFILE fp);
int v_fputc(char c, v_LPFILE fp);

#define v_getc v_fgetc
#define v_putc v_fputc

/**************************************************************************/
/* file status */

int     v_feof(v_LPFILE fp);
int     v_ferror(v_LPFILE fp);
void    v_clearerr(v_LPFILE fp);

/**************************************************************************/
/* file position */

long    v_ftell(v_LPFILE fp);
int     v_fgetpos(v_LPFILE fp, v_fpos_t *pos);
int     v_fsetpos(v_LPFILE fp, const v_fpos_t *pos);
void    v_rewind(v_LPFILE fp);
int     v_fseek(v_LPFILE fp, long offset, int type);

/**************************************************************************/
/* unget a character */

int     v_ungetc(char c, v_LPFILE fp);

/**************************************************************************/
/* read / write string */

v_LPSTR v_fgets(v_LPSTR s, int n, v_LPFILE fp);
int     v_fputs(v_LPCSTR s, v_LPFILE fp);

/**************************************************************************/
/* print */

int     v_fprintf(v_LPFILE fp, v_LPCSTR format, ...);
int     v_vfprintf(v_LPFILE fp, v_LPCSTR format, va_list va);

/**************************************************************************/
/* scan */

int     v_fscanf(v_LPFILE fp, v_LPCSTR format, ...);
int     v_vfscanf(v_LPFILE fp, v_LPCSTR format, va_list va);

/**************************************************************************/
/* flush */

#ifndef V_FILE_NEED_SPEED
    int v_fflush(v_LPFILE fp);
#else
    #define v_fflush(fp)
#endif

/**************************************************************************/
/* Windows .exe resource */

#ifdef WIN16
    /* opening old 16-bit Windows .exe resource as virtual file */
    v_LPFILE    v_fopen_res16(HMODULE hMod, v_LPCSTR res_id, v_LPCSTR type);
#endif

#ifdef _WIN32
    /* opening Windows .exe resource as virtual file */
    v_LPFILE    v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type);
#endif

/**************************************************************************/
/* the v_file standard I/O */

#ifdef V_FILE_USE_STDIO
    /* the v_file standard I/O objects */
    extern v_LPFILE v_stdin;
    extern v_LPFILE v_stdout;
    extern v_LPFILE v_stderr;

    /* initialize the v_file standard I/O */
    void v_file_init_stdio(
        v_LPCVOID input_data, size_t input_size, v_LPCSTR modes);
    void v_file_init_stdio_2(v_LPCSTR input_file_name, v_LPCSTR modes);

    /* destroy the v_file standard I/O */
    void v_file_destroy_stdio(void);

    #ifdef V_FILE_NEED_SPEED
        #define v_getchar()     v_fgetc(v_stdin)
        #define v_putchar(c)    v_fputc((c), v_stdout);
        #define v_puts(s)       (v_fputs((s), v_stdout), v_putchar('\n'), 0)
    #else
        int     v_getchar(void);
        int     v_putchar(char c);
        int     v_puts(v_LPCSTR s);
    #endif

    #define v_fgetchar v_getchar
    #define v_fputchar v_putchar

    v_LPSTR v_gets(v_LPSTR s);

    int v_printf    (v_LPCSTR format, ...);
    int v_vprintf   (v_LPCSTR format, va_list va);

    int v_scanf     (v_LPCSTR format, ...);
    int v_vscanf    (v_LPCSTR format, va_list va);
#endif  /* def V_FILE_USE_STDIO */

/**************************************************************************/

#ifdef V_FILE_NEED_SPEED
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
#endif  /* def V_FILE_NEED_SPEED */

/**************************************************************************/
/* secure functions */

typedef int v_errno_t;

#ifdef V_FILE_WANT_SECURE_LIB
    v_errno_t v_fopen_r_s  (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);
    v_errno_t v_fopen_w_s  (v_FILE **pfp);
    v_errno_t v_fopen_a_s  (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);

    v_errno_t v_fopen_rp_s (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);
    #define   v_fopen_wp_s v_fopen_rp_s
    v_errno_t v_fopen_ap_s (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);

    v_errno_t v_fopen_rb_s (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);
    v_errno_t v_fopen_wb_s (v_FILE **pfp);
    v_errno_t v_fopen_ab_s (v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);

    v_errno_t v_fopen_rpb_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);
    #define   v_fopen_wpb_s v_fopen_rp_s
    v_errno_t v_fopen_apb_s(v_FILE **pfp, v_LPCVOID data, v_fpos_t siz);

    #define   v_tmpfile_s v_fopen_wpb_s

    int       v_fscanf_s   (v_FILE *fp, v_LPCSTR format, ...);
    int       v_fprintf_s  (v_FILE *fp, v_LPCSTR format, ...);
    int       v_vfprintf_s (v_FILE *fp, v_LPCSTR format, va_list va);

    v_errno_t v_clearerr_s (v_FILE *fp);
#endif  /* def V_FILE_WANT_SECURE_LIB */

/**************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

#endif  /* ndef KATAHIROMZ_V_FILE_H_ */
