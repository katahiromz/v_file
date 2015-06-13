/**************************************************************************/
/* v_file --- virtual file operation on memory                            */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

This library "v_file" virtualizes file I/O operations of <stdio.h>.

Tested on Visual C++ 2013, MinGW g++, and clang.

By replacing <stdio.h> items with v_file's items, file operations 
become virtual file operations on memory.

    -------------------+-----------------------
    <stdio.h>          | "v_file.h"
    -------------------+-----------------------
    FILE structure     | v_FILE structure
    fopen              | v_fopen_*
    fclose             | v_fclose
    fread              | v_fread
    fwrite             | v_fwrite
    fgetc              | v_fgetc
    fputc              | v_fputc
    feof               | v_feof
    ferror             | v_ferror
    clearerr           | v_clearerr
    ftell              | v_ftell
    fgetpos            | v_fgetpos
    fsetpos            | v_fsetpos
    rewind             | v_rewind
    fseek              | v_fseek
    ungetc             | v_ungetc
    fgets              | v_fgets
    fputs              | v_fputs
    fscanf             | v_fscanf
    vfscanf            | v_vfscanf
    fprintf            | v_fprintf
    vfprintf           | v_vfprintf
    fflush             | v_fflush
    -------------------+-----------------------

Just add "v_" at the beginning of each name. It's simple.

The v_fopen_* functions store the specified data into 
the v_FILE structure, and set the file position.

/////////////////////////////////////////////////////
// Katayama Hirofumi MZ (katahiromz) [ARMYANT]
// Homepage     http://katahiromz.web.fc2.com/eindex.html
// BBS          http://katahiromz.bbs.fc2.com/
// E-Mail       katayama.hirofumi.mz@gmail.com
/////////////////////////////////////////////////////
