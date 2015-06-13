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

/* virtual "SEEK_*" values */
#define v_SEEK_CUR  1
#define v_SEEK_END  2
#define v_SEEK_SET  0

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

/* virtual end of file (EOF) */
#define v_EOF (-1)

/**************************************************************************/
/* opening / closing */

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

int v_fflush(v_FILE *fp);

/**************************************************************************/
/* Windows .exe resource */

#ifdef _WIN32
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif
    /* opening Windows .exe resource as virtual file */
    v_FILE *v_fopen_res(HMODULE hMod, LPCTSTR res_id, LPCTSTR type);
#endif

/**************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif  /* __cplusplus */

#endif  /* ndef V_FILE_H_ */
