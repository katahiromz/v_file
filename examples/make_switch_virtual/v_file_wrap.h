/**************************************************************************/
/* v_file_wrap.h --- wrap standard I/O with v_file I/O                    */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#ifndef KATAHIROMZ_V_FILE_H_
    #include "v_file.h"
#endif

/**************************************************************************/
/* undef */

#undef FILE
#undef tmpfile
#undef v_fclose
#undef fsetbin
#undef fsettext
#undef fopen
#undef fread
#undef fwrite
#undef fgetc
#undef fputc
#undef getc
#undef putc
#undef feof
#undef ferror
#undef clearerr
#undef ftell
#undef fgetpos
#undef fsetpos
#undef rewind
#undef fseek
#undef ungetc
#undef fgets
#undef fputs
#undef fprintf
#undef vfprintf
#undef fscanf
#undef vfscanf
#undef fflush
#undef getchar
#undef putchar
#undef puts
#undef fgetchar
#undef fputchar
#undef gets
#undef printf
#undef vprintf
#undef scanf
#undef vscanf
#undef tmpfile_s
#undef fscanf_s
#undef fprintf_s
#undef vfprintf_s
#undef clearerr_s
#undef HFILE
#undef HFILE_ERROR
#undef _lcreat
#undef _lopen
#undef _llseek
#undef _lread
#undef _lwrite
#undef _lclose
#undef OpenFile
#undef CreateFile
#undef CreateFileA
#undef CreateFileW
#undef CloseHandle
#undef ReadFile
#undef WriteFile
#undef FlushFileBuffer
#undef GetFileSize
#undef SetEndOfFile
#undef SetFilePointer
#undef _tfopen

/**************************************************************************/
/* do wrap */

#define FILE v_FILE
#define tmpfile v_tmpfile
#define fclose v_fclose
#define fsetbin v_fsetbin
#define fsettext v_fsettext
#define fopen v_fopen
#define fread v_fread
#define fwrite v_fwrite
#define fgetc v_fgetc
#define fputc v_fputc
#define getc v_getc
#define putc v_putc
#define feof v_feof
#define ferror v_ferror
#define clearerr v_clearerr
#define ftell v_ftell
#define fgetpos v_fgetpos
#define fsetpos v_fsetpos
#define rewind v_rewind
#define fseek v_fseek
#define ungetc v_ungetc
#define fgets v_fgets
#define fputs v_fputs
#define fprintf v_fprintf
#define vfprintf v_vfprintf
#define fscanf v_fscanf
#define vfscanf v_vfscanf
#define fflush v_fflush
#define getchar v_getchar
#define putchar v_putchar
#define puts v_puts
#define fgetchar v_fgetchar
#define fputchar v_fputchar
#define gets v_gets
#define printf v_printf
#define vprintf v_vprintf
#define scanf v_scanf
#define vscanf v_vscanf
#define tmpfile_s v_tmpfile_s
#define fscanf_s v_fscanf_s
#define fprintf_s v_fprintf_s
#define vfprintf_s v_vfprintf_s
#define clearerr_s v_clearerr_s
#define HFILE v_HFILE
#define HFILE_ERROR v_HFILE_ERROR
#define _lcreat v__lcreat
#define _lopen v__lopen
#define _llseek v__llseek
#define _lread v__lread
#define _lwrite v__lwrite
#define _lclose v__lclose
#define OpenFile v_OpenFile
#define CreateFile v_CreateFile
#define CreateFileA v_CreateFileA
#define CreateFileW v_CreateFileW
#define CloseHandle v_CloseHandle
#define ReadFile v_ReadFile
#define WriteFile v_WriteFile
#define FlushFileBuffer v_FlushFileBuffer
#define GetFileSize v_GetFileSize
#define SetEndOfFile v_SetEndOfFile
#define SetFilePointer v_SetFilePointer

#ifdef _UNICODE
    #define _tfopen v__wfopen
#else
    #define _tfopen v_fopen
#endif

/**************************************************************************/
