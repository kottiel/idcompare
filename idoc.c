//
// Created by kott on 8/13/19.
//

#include <stdlib.h>
#include <string.h>
#include "idoc.h"

/*
 * /*
 * Copyright (c) 1998, 2015 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t dsize) {
    const char *osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if ((*dst++ = *src++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0';        /* NUL-terminate dst */
        while (*src++);
    }

    return (src - osrc - 1);    /* count does not include NUL */
}

/*
 * /*
 * Adapted from strlcpy (see above). Modified for strings that are
 * space delimited.
 */
size_t strlcpy_spdl(char *dst, const char *src, size_t dsize) {
    const char *osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if (*src == ' ') {
                *dst = '\0';
                break;
            } else
                *dst++ = *src++;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0';        /* NUL-terminate dst */
        while (*src++);
    }

    return (src - osrc - 1);    /* count does not include NUL */
}

char *get_value(char *line) {

    char *rpos = line + RIGHT_MOST_EDGE;
    char *lpos, *str;
    int length;

    // start at right-most edge, look for any non-space character
    while (*rpos-- == ' ');

    // start at last char and find either the '\' or the first position
    lpos = ++rpos;
    while ((*lpos != '\\') && lpos >= line)
        lpos--;

    length = rpos - lpos++;

    str = (char *)malloc(length + 1);
    strncpy(str, lpos, length + 1);
    str[length] = '\0';
    return str;
}

int read_idoc(Idoc_row *idoc, FILE *fp) {

    char linetype[TYPE];
    char line[MAX_ROW_LEN] = {};
    int n = 0;
    int capacity = START_SIZE;
    char current_pcode[MED] = {};
    char current_label[LBL] = {};
    char *cp = current_pcode;
    char *lb = current_label;
    char *at;
    char *lt;

    idoc = (Idoc_row *)malloc((START_SIZE) * sizeof(Idoc_row));

    // read and discard first idoc line
    fgets(line, MAX_ROW_LEN - 1, fp);

    while (fgets(line, MAX_ROW_LEN - 1, fp) != NULL) {

        strlcpy(linetype, line + ID_START, TYPE);
        at = line + ATTR_NAME;
        lt = linetype;

        if (strcmp(linetype, MATNR) == 0) {
            strlcpy_spdl(current_pcode, line + ATTR_NAME, MED);

        } else if (strcmp(linetype, LABEL) == 0) {
            strlcpy_spdl(current_label, line + ATTR_NAME, LBL);

/*
        } else if ((strcmp(linetype, TDLINE) == 0)) {
            strlcpy(idoc[n].pcode, current_pcode, MED);
            strlcpy(idoc[n].label, current_label, LBL);
            strlcpy_spdl(idoc[n].attr_name, line + ATTR_NAME, MED);
*/

        } else if (strcmp(linetype, DESCR) == 0) {
            strlcpy(idoc[n].pcode, current_pcode, MED);
            strlcpy(idoc[n].label, current_label, LBL);
            strlcpy_spdl(idoc[n].attr_name, line + ATTR_NAME, MED);
            idoc[n].attr_val = get_value(line + VALUE_START);
            printf("%30s %9s %30s %s\n", current_pcode, current_label, idoc[n].attr_name, idoc[n].attr_val);
            n++;
        }

        if (n == capacity) {
            capacity *= 2;
            idoc = (Idoc_row *)realloc(idoc, (capacity) * sizeof(Idoc_row));
        }
    }
    return n;
}