////////////////////////////////////////////////////////////////////////////
// make_switch
// Copyright (C) 2015 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////

[CONCEPT]

    This software generates C switch statements or a function code from 
strings to identify a string.

Tested on Visual C++ 2013, Borland C++ Compiler 5.5.1,
MinGW gcc, Ubuntu gcc and Ubuntu clang.

For example, the three strings:

    "abb",
    "abc",
    "ad"

will generate the following C code:

    int identify_string(const char *s)
    {
        switch (s[0])
        {
        case 'a':
            switch (s[1])
            {
            case 'b':
                switch (s[2])
                {
                case 'b':
                    switch (s[3])
                    {
                    case 0:
                        return 0;
                    default:
                        return -1;
                    }
                case 'c':
                    switch (s[3])
                    {
                    case 0:
                        return 1;
                    default:
                        return -1;
                    }
                default:
                    return -1;
                }
            case 'd':
                switch (s[2])
                {
                case 0:
                    return 2;
                default:
                    return -1;
                }
            default:
                return -1;
            }
        default:
            return -1;
        }
    }

The generated "identify_string" function returns:

    the index   if the string was matched;
    -1          otherwise.

[REQUIREMENTS]

    C compiler

/////////////////////////////////////////////////////
// Katayama Hirofumi MZ (katahiromz) [ARMYANT]
// Homepage     http://katahiromz.web.fc2.com/eindex.html
// BBS          http://katahiromz.bbs.fc2.com/
// E-Mail       katayama.hirofumi.mz@gmail.com
/////////////////////////////////////////////////////
