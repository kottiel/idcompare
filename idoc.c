//
// Created by kottiel on 8/13/19.
//

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

int compare(FILE *fpout, Idoc_row *master, int mrows, Idoc_row *sap, int srows) {

    int mpos = 0;
    int spos = 0;

    /* report
     	read mas
	read sap
while not EOF MAS and not EOF SAP */
    while ((mpos < mrows) && (spos < srows)) {

        if (comparator(master + mpos, sap + spos) != 0)
            printf("compared %s and %s with result %d\n", (master + mpos)->attr_name, (sap + spos)->attr_name, comparator(master + mpos, sap + spos));
        else
            printf("compared %s and %s with result %d\n", (master + mpos)->attr_name, (sap + spos)->attr_name, comparator(master + mpos, sap + spos));

        mpos++;
        spos++;
    }
/*	if mas <> sap
           if mas < sap:
	      print record # not in SAP
              read next mas

           else (mas > sap)
              report wrong data sap record added to SAP
              read next sap
	else
	   read mas
	   read sap

end while

if EOF MAS
    while not EOF SAP
        report wrong data sap record added to SAP

else if EOF SAP
    while not EOF MAS
        rprint record # not in SAP
     * */
}

int equals_blanktif(char *str) {

    char blank[] = "blank-01.tif";

    if ((str == NULL) || strlen(str) == 0)
        return 0;

    int length = (strlen(blank) < strlen(str) ? strlen(blank) : strlen(str));

    for (int i = 0; i < length; i++) {
        if (blank[i] != tolower(str[i]))
            return 0;
    }

    return 1;
}

int equals_no(char *field) {
    return ((strcasecmp(field, "N") == 0) || (strcasecmp(field, "NO") == 0));
}

int comparator(const void *p, const void *q) {
    Idoc_row *l = (Idoc_row *)p;
    Idoc_row *r = (Idoc_row *)q;

    if (strcmp(l->pcode, r->pcode) < 0)
        return -1;
    else if (strcmp(l->pcode, r->pcode) > 0)
        return 1;
    else if (strcmp(l->label, r->label) < 0)
        return -1;
    else if (strcmp(l->label, r->label) > 0)
        return 1;
    else if (strcmp(l->attr_name, r->attr_name) < 0)
        return -1;
    else if (strcmp(l->attr_name, r->attr_name) > 0)
        return 1;
    else if (strcmp(l->attr_val, r->attr_val) < 0)
        return -1;
    else if (strcmp(l->attr_val, r->attr_val) > 0)
        return 1;
    else
        return 0;
}

char *get_value(char *line) {

    char *lpos, *str;
    size_t length = strlen(line);
    char *rpos = line + length - 1;

    // start at right-most edge, look for any non-space character
    while (*rpos-- == ' ');

    // start at last char and find either the '\' or the first position
    lpos = ++rpos;
    while ((*lpos != '\\') && lpos >= line)
        lpos--;

    length = rpos - lpos++;

    str = (char *)malloc(length + 1);
    strlcpy(str, lpos, length + 1);
    str[length] = '\0';
    return str;
}

Idoc_row *read_idoc(int *num_lines, FILE *fp) {

    char linetype[TYPE]      = {};
    char prev_linetype[TYPE] = {};

    char line[MAX_ROW_LEN] = {};
    int n = 0;
    int capacity = START_SIZE;
    char current_pcode[MED] = {};
    char current_label[LBL] = {};
    char current_attname[MED] = {};

    char *pvalue;
    char *tmp;
    char *tdline = (char *)calloc(1, sizeof(char));
    size_t total_len = 0;

    Idoc_row *idoc = (Idoc_row *)malloc((START_SIZE) * sizeof(Idoc_row));

    // read and discard first idoc line
    fgets(line, MAX_ROW_LEN - 1, fp);

    while (fgets(line, MAX_ROW_LEN - 1, fp) != NULL) {

        strlcpy(linetype, line + ID_START, TYPE);

        if ((strcmp(prev_linetype, TDLINE) == 0) && (strcmp(linetype, prev_linetype) != 0)) {
            strlcpy(idoc[n].pcode, current_pcode, MED);
            strlcpy(idoc[n].label, current_label, LBL);
            strcpy(idoc[n].attr_name, "TDLINE");
            idoc[n].attr_val = tdline;
            //free(tdline);
            total_len = 0;
            tdline = (char *)calloc(1, sizeof(char));
            n++;
            strcpy(prev_linetype, linetype);
        } else
            strcpy(prev_linetype, linetype);

        if (strcmp(linetype, MATNR) == 0) {
            strlcpy_spdl(current_pcode, line + ATTR_NAME, MED);

        } else if (strcmp(linetype, LABEL) == 0) {
            strlcpy_spdl(current_label, line + ATTR_NAME, LBL);

        } else if (strcmp(linetype, TDLINE) == 0) {
            line[TDLINE_START + TDLINE_RIGHT_EDGE] = '\0';
            tmp = get_value(line + TDLINE_START);
            total_len += strlen(tmp);
            tdline = (char *) realloc(tdline, total_len + 1);
            strcat(tdline, tmp);

        } else if (strcmp(linetype, DESCR) == 0) {
            line[VALUE_START + RIGHT_MOST_EDGE] = '\0';
            pvalue = get_value(line + VALUE_START);
            strlcpy_spdl(current_attname, line + ATTR_NAME, MED);

            if ((strcmp(current_attname, "STERILITYTYPE") == 0) ||
                (!equals_blanktif(pvalue) && (!equals_no(pvalue)))) {
                strlcpy(idoc[n].pcode, current_pcode, MED);
                strlcpy(idoc[n].label, current_label, LBL);
                strlcpy(idoc[n].attr_name, current_attname, MED);

                idoc[n].attr_val = get_value(line + VALUE_START);

                n++;
            }
        }

        if (n == capacity) {
            capacity *= 2;
            idoc = (Idoc_row *)realloc(idoc, (capacity) * sizeof(Idoc_row));
        }
    }

    *num_lines = n;
    return idoc;
}