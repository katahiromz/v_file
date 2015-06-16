/**************************************************************************/
/* v_file --- virtual file operation on memory                            */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

This library "v_file" virtualizes file I/O operations of <stdio.h>.

Tested on Visual C++ 2013, Borland C++ Compiler 5.5.1, MinGW g++, 
Linux g++, and Linux clang.

By replacing <stdio.h> items with v_file's items, file operations 
become virtual file operations on memory, as follows:

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
    fgets              | v_fgets
    fputs              | v_fputs
    fprintf            | v_fprintf
    vfprintf           | v_vfprintf
    fflush             | v_fflush
    ...                | ...
    -------------------+-----------------------

Just add "v_" at the beginning of each name. It's simple.

The v_fopen_* functions store the specified data into 
the v_FILE structure, if any; and set the file position.

If you wanna use "the v_file standard I/O" (v_stdio or 
v_printf etc.), you have to call v_file_init_stdio and 
v_file_destroy_stdio.

See source for details.

/////////////////////////////////////////////////////
// Katayama Hirofumi MZ (katahiromz) [ARMYANT]
// Homepage     http://katahiromz.web.fc2.com/eindex.html
// BBS          http://katahiromz.bbs.fc2.com/
// E-Mail       katayama.hirofumi.mz@gmail.com
/////////////////////////////////////////////////////
