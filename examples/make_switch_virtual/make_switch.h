/**************************************************************************/
/* make_switch.h --- generate switch statements to identify string        */
/* Copyright (C) 2015 Katayama Hirofumi MZ.  All Rights Reserved.         */
/**************************************************************************/

#ifndef KATAHIROMZ_MS_MAKE_SWITCH_H_
#define KATAHIROMZ_MS_MAKE_SWITCH_H_ /* version */ 1

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************/

/* MS_MAKE_SWITCH structure */
typedef struct MS_MAKE_SWITCH
{
    int count;
    char *str;
    char **entries;
    char *checks;
} MS_MAKE_SWITCH;

/* make indent */
void ms_make_indent(int n);

/* make switch statements to identify the string */
void ms_make_switch(MS_MAKE_SWITCH *pms, int depth, int ident_level);

/* make a function code to identify the string */
int ms_make_identify_string_function(int count, char **entries);

/**************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* ndef KATAHIROMZ_MS_MAKE_SWITCH_H_ */
