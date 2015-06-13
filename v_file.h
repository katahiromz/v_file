/**************************************************************************/
/* v_file.h --- virtual file operation on memory                          */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#ifndef V_FILE_H_
#define V_FILE_H_  /* version */ 1 

#ifdef __cplusplus
    #include <cstdarg>
#else
    #include <stdarg.h>
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
    #ifdef MSDOS
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

/* NOTE: Windows and MS-DOS can use "text mode" (not binary mode). */
#if defined(_WIN32) || defined(MSDOS)
    #define v_FMODE_TEXT        0   /* text mode */
#else
    #define v_FMODE_TEXT        v_FMODE_BINARY
#endif

/* virtual end of file (EOF) */
#define v_EOF (-1)

/**************************************************************************/
/* types */

/* virtual "fpos_t" type */
typedef unsigned long v_fpos_t;

/* virtual "FILE" structure */
typedef struct v_FILE
{
    char *      data;       /* malloc'ed and free'd */
    v_fpos_t    index;      /* file position */
    v_fpos_t    size;       /* file size */
    int         modes;      /* virtual file modes */
} v_FILE;

/**************************************************************************/
/* opening / closing */

v_FILE *
v_fopen_internal(const void *data, v_fpos_t index, v_fpos_t siz, int modes);

/*
 * text mode
 */
v_FILE *v_fopen_r  (const void *data, v_fpos_t siz);    /* mode "r"  */
v_FILE *v_fopen_w  (void);                              /* mode "w"  */
v_FILE *v_fopen_a  (const void *data, v_fpos_t siz);    /* mode "a"  */

v_FILE *v_fopen_rp (const void *data, v_fpos_t siz);    /* mode "r+" */
#define v_fopen_wp v_fopen_rp                           /* mode "w+" */
v_FILE *v_fopen_ap (const void *data, v_fpos_t siz);    /* mode "a+" */

/*
 * binary mode
 */
v_FILE *v_fopen_rb (const void *data, v_fpos_t siz);    /* mode "rb" */
v_FILE *v_fopen_wb (void);                              /* mode "wb" */
v_FILE *v_fopen_ab (const void *data, v_fpos_t siz);    /* mode "ab" */

v_FILE *v_fopen_rpb(const void *data, v_fpos_t siz);    /* mode "r+b" */
#define v_fopen_wpb v_fopen_rpb                         /* mode "w+b" */
v_FILE *v_fopen_apb(const void *data, v_fpos_t siz);    /* mode "a+b" */

#define v_tmpfile v_fopen_wpb

/*
 * closing
 */
int v_fclose(v_FILE *fp);           /* will free fp->data  */
char *v_fclose_detach(v_FILE *fp);  /* won't free fp->data */

/**************************************************************************/
/* binary transfer */

int v_fread_raw(void *ptr, v_fpos_t siz, v_fpos_t nelem, v_FILE *fp);
int v_fwrite_raw(const void *ptr, v_fpos_t siz, v_fpos_t nelem,
                 v_FILE *fp);

/**************************************************************************/
/* read / write buffer */

int v_fread(void *ptr, v_fpos_t siz, v_fpos_t nelem, v_FILE *fp);
int v_fwrite(const void *ptr, v_fpos_t siz, v_fpos_t nelem,
             v_FILE *fp);

/**************************************************************************/
/* read / write a character */

int v_fgetc(v_FILE *fp);
int v_fputc(char c, v_FILE *fp);

#define v_getc v_fgetc
#define v_putc v_fputc

/**************************************************************************/
/* file status */

int v_feof(v_FILE *fp);
int v_ferror(v_FILE *fp);
void v_clearerr(v_FILE *fp);

/**************************************************************************/
/* file position */

long v_ftell(v_FILE *fp);
int v_fgetpos(v_FILE *fp, v_fpos_t *pos);
int v_fsetpos(v_FILE *fp, const v_fpos_t *pos);
void v_rewind(v_FILE *fp);
int v_fseek(v_FILE *fp, long offset, int type);

/**************************************************************************/
/* unget a character */

int v_ungetc(char c, v_FILE *fp);

/**************************************************************************/
/* read / write string */

char *v_fgets(char *s, int n, v_FILE *fp);
int v_fputs(const char *s, v_FILE *fp);

/**************************************************************************/
/* print */

int v_fprintf(v_FILE *fp, const char *format, ...);
int v_vfprintf(v_FILE *fp, const char *format, va_list arg);

/**************************************************************************/
/* scan */

int v_fscanf(v_FILE *fp, const char *format, ...);
int v_vfscanf(v_FILE *fp, const char *format, va_list arg);

/**************************************************************************/
/* flush */

#ifndef V_FILE_SPEED
    int v_fflush(v_FILE *fp);
#else
    #define v_fflush(fp)
#endif

/**************************************************************************/
/* Windows .exe resource */

#if defined(_WIN32) || defined(WIN16)
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif
#endif

#ifdef WIN16
    /* opening old 16-bit Windows .exe resource as virtual file */
    v_FILE *v_fopen_res16(HMODULE hMod, LPCSTR res_id, LPCSTR type);
#endif

#ifdef _WIN32
    /* opening Windows .exe resource as virtual file */
    v_FILE *v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type);
#endif

/**************************************************************************/

#ifdef V_FILE_SPEED
    #define v_fopen_r(data, siz) \
        v_fopen_internal((data), 0, (siz), v_FMODE_READ | v_FMODE_TEXT)
    #define v_fopen_a(data, siz) \
        v_fopen_internal((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_TEXT)
    #define v_fopen_rb(data, siz) \
        v_fopen_internal((data), 0, (siz), v_FMODE_READ | v_FMODE_BINARY)
    #define v_fopen_ab(data, siz) \
        v_fopen_internal((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_BINARY)
    #define v_fopen_rp(data, siz) \
        v_fopen_internal((data), 0, (siz), v_FMODE_READWRITE | v_FMODE_TEXT)
    #define v_fopen_ap(data, siz) \
        v_fopen_internal((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_TEXT)
    #define v_fopen_rpb(data, siz) \
        v_fopen_internal((data), 0, (siz), v_FMODE_READWRITE | v_FMODE_BINARY)
    #define v_fopen_apb(data, siz) \
        v_fopen_internal((data), (siz), (siz), v_FMODE_APPEND | v_FMODE_BINARY)
#endif

/**************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

#endif  /* ndef V_FILE_H_ */
