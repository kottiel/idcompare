#include <stdio.h>
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
 * Adapted from strlcpy:
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
size_t strlcpy_alt(char *dst, const char *src, size_t dsize) {
    const char *osrc = src;
    size_t nleft = dsize;

    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if (*src == ' ') {
                *dst++ = '\0';
                break;
            } else if ((*dst++ = *src++) == '\0')
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

int read_idoc(Idoc_row *idoc, FILE *fp) {

    char linetype[TYPE];
    char line[MAX_ROW_LEN] = {};
    int i = 0;
    idoc = (Idoc_row *)malloc(sizeof(Idoc_row));

    // read and discard first idoc line
    fgets(line, MAX_ROW_LEN - 1, fp);

    while (fgets(line, MAX_ROW_LEN - 1, fp) != NULL) {
        strlcpy(linetype, line, TYPE);

        if (strcmp(linetype, "Z2BTMH") == 0) {
            strlcpy_alt(idoc[i].pcode, line + ATTR_NAME, MED);
            printf("%s\n",idoc[i].pcode);

        }
    }
    return 1;
}

int main(int argc, char *argv[]) {

    FILE *fpSAP, *fpMaster;

    Idoc_row *sap, *master;

    if (argc != 3) {
        printf("usage: %s filename.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((fpSAP = fopen(argv[1], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    //fpMaster = fopen(argv[2], "r");

    read_idoc(sap, fpSAP);

    return 0;
}