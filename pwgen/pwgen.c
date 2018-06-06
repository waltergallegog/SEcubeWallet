/*
 * pwgen.c --- generate secure passwords
 *
 * Copyright (C) 2001,2002 by Theodore Ts'o
 *
 * This file may be distributed under the terms of the GNU Public
 * License.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pwgen.h"

/* Globals variables */
int (*pw_number)(int max_num);

int	pw_length = 8;
int	pwgen_flags = 0;

int main_pwgen(int num_options, const char *options, int pw_length, char* buf){
    int	j;
    char *remove=NULL;
    void (*pwgen)(char *inbuf, int size, int pw_flags, char *remove);

    pwgen = pw_phonemes;
    pw_number = pw_random_number;
    pwgen_flags = PW_DIGITS | PW_UPPERS; //default flags, include digits and uppercases

    for  (j=0; j<num_options;j++){
        switch (options[j]) {
        case '0':
            pwgen_flags &= ~PW_DIGITS;
            break;
        case 'A':
            pwgen_flags &= ~PW_UPPERS;
            break;
        case 'B':
            pwgen_flags |= PW_AMBIGUOUS;
            break;
        case 'c':
            pwgen_flags |= PW_UPPERS;
            break;
        case 'n':
            pwgen_flags |= PW_DIGITS;
            break;
        case 's':
            pwgen = pw_rand;
            break;
        case 'y':
            pwgen_flags |= PW_SYMBOLS;
            break;
        case 'v':
            pwgen = pw_rand;
            pwgen_flags |= PW_NO_VOWELS;
            break;
        default:
            break;
        }
    }


    if (pw_length < 5)
        pwgen = pw_rand;
    if (pwgen != pw_rand) {
        if (pw_length <= 2)
            pwgen_flags &= ~PW_UPPERS;
        if (pw_length <= 1)
            pwgen_flags &= ~PW_DIGITS;
    }

    pwgen(buf, pw_length, pwgen_flags, remove);

    return 0;
}
