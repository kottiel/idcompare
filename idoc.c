//
// Created by kottiel on 8/13/19.
//

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
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
 * Returns strlen(src); if return value >= dsize, truncation occurred.
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

int compare(FILE *fpout, Idoc_row *master, size_t mrows, Idoc_row *sap, size_t srows) {

    int mpos = 0;
    int spos = 0;

    char *master_attr;
    char *sap_attr;

    bool errors = false;

    char curr_master_label[LBL];
    char curr_sap_label[LBL];
    char prevMasterLabel[LBL];
    char prevSAPLabel[LBL];
    int result;

    fprintf(fpout, "PCODE\tLabelNumber\tAttributeName\tDisplayValue\tErrorMessage\n");

    if (mrows == 0)
        printf("Verified Idoc file is empty.\n");
    else {
        strcpy(curr_master_label, (master + mpos)->label);
        strcpy(prevMasterLabel, (master + mpos)->label);
    }
    if (srows == 0)
        printf("SAP extract file is empty.\n");
    else {
        strcpy(curr_sap_label, (sap + spos)->label);
        strcpy(prevSAPLabel, (sap + spos)->label);
    }
    while ((mpos < mrows) && (spos < srows)) {

        if (strcmp(curr_master_label, (master + mpos)->label) != 0) {
            strcpy(prevMasterLabel, curr_master_label);
            strcpy(curr_master_label, (master + mpos)->label);
        }
        if (strcmp(curr_sap_label, (sap + spos)->label) != 0) {
            strcpy(prevSAPLabel, curr_sap_label);
            strcpy(curr_sap_label, (sap + spos)->label);
        }

        result = compare_attributes(master + mpos, sap + spos);
        if (result < 0) {
            if (strcmp((master + mpos)->label, (sap + spos)->label) == 0) {
                fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Missing Data\n",
                        (master + mpos)->pcode,
                        (master + mpos)->label,
                        (master + mpos)->attr_name,
                        (master + mpos)->attr_val);
                errors = true;
                mpos++;
            } else {
                while (strcmp(prevSAPLabel, (master + mpos)->label) == 0) {
                    fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Missing Data\n",
                            (master + mpos)->pcode,
                            (master + mpos)->label,
                            (master + mpos)->attr_name,
                            (master + mpos)->attr_val);
                    errors = true;
                    mpos++;
                }

                while (strcmp(prevMasterLabel, (master + mpos)->label) == 0) {
                    if (strcmp((master + mpos)->attr_name, "TEMPLATENUMBER") == 0)
                        fprintf(fpout, "%s\t%s\t%s\t%s\tLabel not in Extract File!\n",
                                (master + mpos)->pcode,
                                (master + mpos)->label,
                                (master + mpos)->attr_name,
                                (master + mpos)->attr_val);

                    errors = true;
                    mpos++;
                }
                strcpy(prevMasterLabel, (master + mpos)->label);
            }
        } else if (result > 0) {
            if ((strcmp((master + mpos)->label, (sap + spos)->label) == 0) ||
                (strcmp(prevMasterLabel, (sap + spos)->label) == 0)) {
                fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Wrong data Added\n",
                        (sap + spos)->pcode,
                        (sap + spos)->label,
                        (sap + spos)->attr_name,
                        (sap + spos)->attr_val);
                errors = true;
                spos++;
            } else {
                while (strcmp(prevSAPLabel, (sap + spos)->label) == 0) {
                    if (strcmp((sap + spos)->attr_name, "TEMPLATENUMBER") == 0)
                        fprintf(fpout, "%s\t%s\t%s\t%s\tLabel not in verified file!\n",
                                (sap + spos)->pcode,
                                (sap + spos)->label,
                                (sap + spos)->attr_name,
                                (sap + spos)->attr_val);
                    errors = true;
                    spos++;
                }
                strcpy(prevSAPLabel, (sap + spos)->label);
            }
        } else {
            mpos++;
            spos++;
        }
    }
    while (spos < srows) {
        if (strcmp(curr_master_label, (sap + spos)->label) == 0) {
            fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Wrong data Added\n",
                    (sap + spos)->pcode,
                    (sap + spos)->label,
                    (sap + spos)->attr_name,
                    (sap + spos)->attr_val);
            errors = true;
            spos++;
        } else {
            while (strcmp(prevSAPLabel, (sap + spos)->label) == 0) {
                if (strcmp((sap + spos)->attr_name, "TEMPLATENUMBER") == 0)
                    fprintf(fpout, "%s\t%s\t%s\t%s\tLabel not in verified file!\n",
                            (sap + spos)->pcode,
                            (sap + spos)->label,
                            (sap + spos)->attr_name,
                            (sap + spos)->attr_val);
                errors = true;
                spos++;
            }
            strcpy(prevSAPLabel, (sap + spos)->label);
        }
    }
    while (mpos < mrows) {
        if (strcmp((master + mpos)->label, curr_sap_label) == 0) {
            fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Missing Data\n",
                    (master + mpos)->pcode,
                    (master + mpos)->label,
                    (master + mpos)->attr_name,
                    (master + mpos)->attr_val);
            errors = true;
            mpos++;
        } else {

            while (strcmp(prevSAPLabel, (master + mpos)->label) == 0) {
                fprintf(fpout, "%s\t%s\t%s\t%s\tSAP - Missing Data\n",
                        (master + mpos)->pcode,
                        (master + mpos)->label,
                        (master + mpos)->attr_name,
                        (master + mpos)->attr_val);
                errors = true;
                mpos++;
            }


            while (strcmp(prevMasterLabel, (master + mpos)->label) == 0) {
                if (strcmp((master + mpos)->attr_name, "TEMPLATENUMBER") == 0)
                    fprintf(fpout, "%s\t%s\t%s\t%s\tLabel not in Extract File!\n",
                            (master + mpos)->pcode,
                            (master + mpos)->label,
                            (master + mpos)->attr_name,
                            (master + mpos)->attr_val);

                errors = true;
                mpos++;
            }
            strcpy(prevMasterLabel, (master + mpos)->label);
        }
    }
    if (!errors) {
        printf("No errors found.\n");
        fprintf(fpout, "No Errors Found\tNo Errors Found\tNo Errors Found\tNo Errors Found\tNo Errors Found\n");
    } else
        printf("Errors found.\n");
}

int equals_blanktif(char *str) {

    char blank[] = "blank-01.tif";

    if ((str == NULL) || strlen(str) == 0)
        return 0;

    int length = (int) (strlen(blank) < strlen(str) ? strlen(blank) : strlen(str));

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
    Idoc_row *l = (Idoc_row *) p;
    Idoc_row *r = (Idoc_row *) q;

    if (strcasecmp(l->label, r->label) < 0)
        return -1;
    else if (strcasecmp(l->label, r->label) > 0)
        return 1;
    else if (strcasecmp(l->attr_val, r->attr_val) < 0)
        return -1;
    else if (strcasecmp(l->attr_val, r->attr_val) > 0)
        return 1;
    else if (strcasecmp(l->attr_name, r->attr_name) < 0)
        return -1;
    else if (strcasecmp(l->attr_name, r->attr_name) > 0)
        return 1;
    else
        return 0;
}

int compare_attributes(const void *p, const void *q) {
    Idoc_row *l = (Idoc_row *) p;
    Idoc_row *r = (Idoc_row *) q;

    char lname[8];
    char rname[8];
    strlcpy(lname, l->attr_name, 8);
    strlcpy(rname, r->attr_name, 8);

    if (strcasecmp(l->label, r->label) < 0)
        return -1;
    else if (strcasecmp(l->label, r->label) > 0)
        return 1;
    if (strcasecmp(l->attr_val, r->attr_val) < 0)
        return -1;
    else if (strcasecmp(l->attr_val, r->attr_val) > 0)
        return 1;
    else if (strcasecmp(l->attr_name, r->attr_name) < 0) {
        if ((strcasecmp(lname, "GRAPHIC") == 0) &&
            (strcasecmp(rname, "GRAPHIC") == 0))
            return 0;
        else
            return -1;
    } else if (strcasecmp(l->attr_name, r->attr_name) > 0) {
        if ((strcasecmp(lname, "GRAPHIC") == 0) &&
            (strcasecmp(rname, "GRAPHIC") == 0))
            return 0;
        else
            return 1;
    } else
        return 0;
}

int chkdups(Idoc_row *sap, int sap_numlines) {

    int i = 0;
    while (i < sap_numlines - 1) {
        if ((strcasecmp((sap + i)->pcode, (sap + i + 1)->pcode) == 0) &&
            (strcasecmp((sap + i)->label, (sap + i + 1)->label) == 0) &&
            (strcasecmp((sap + i)->attr_name, (sap + i + 1)->attr_name) == 0) &&
            (strcasecmp((sap + i)->attr_val, (sap + i + 1)->attr_val) == 0))
            return 1;
        else
            i++;
    }
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

    if ((str = (char *) malloc(length + 1)) == NULL) {
        return NULL;
    }
    strlcpy(str, lpos, length + 1);
    str[length] = '\0';

    return str;

}

Idoc_row *read_idoc(size_t *num_lines, FILE *fp) {

    char linetype[TYPE] = {};
    char prev_linetype[TYPE] = {};

    char line[MAX_ROW_LEN] = {};
    size_t n = 0;
    size_t actual_linenums = 1;
    int capacity = START_SIZE;
    char current_pcode[MED] = {};
    char current_label[LBL] = {};
    char current_attname[MED] = {};

    char *pvalue;
    char *tmp;
    char *tdline;
    if ((tdline = (char *) calloc(1, sizeof(char))) == NULL) {
        fprintf(stderr, "Memory error in initial allocation of tdline.\n");
        return NULL;
    }
    size_t total_len = 0;

    Idoc_row *idoc;
    if ((idoc = (Idoc_row *) malloc((START_SIZE) * sizeof(Idoc_row))) == NULL) {
        fprintf(stderr, "Error in initial allocation of memory for idoc file.\n");
        return NULL;
    }

    while (fgets(line, MAX_ROW_LEN, fp) != NULL) {

        strlcpy(linetype, line + ID_START, TYPE);

        // process a batch of TDLINE rows into a single TDLINE row
        if ((strcmp(prev_linetype, TDLINE) == 0) && (strcmp(linetype, prev_linetype) != 0)) {
            strlcpy(idoc[n].pcode, current_pcode, MED);
            strlcpy(idoc[n].label, current_label, LBL);
            strcpy(idoc[n].attr_name, "TDLINE");
            idoc[n].attr_val = tdline;

            total_len = 0;
            tdline = (char *) calloc(1, sizeof(char));
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
            if ((tmp = get_value(line + TDLINE_START)) == NULL) {
                fprintf(stderr, "Error allocating memory for TDLINE attribute value.\n");
                return NULL;
            }
            total_len += strlen(tmp);
            tdline = (char *) realloc(tdline, total_len + 2);
            strncat(tdline, tmp, total_len + 1);
            tdline[total_len] = '\0';
            char *t = tdline;

        } else if (strcmp(linetype, DESCR) == 0) {
            line[VALUE_START + RIGHT_MOST_EDGE] = '\0';
            if ((pvalue = get_value(line + VALUE_START)) == NULL) {
                fprintf(stderr, "Error allocating memory for regular (non-TDLINE) attribute value.\n");
                return NULL;
            }
            strlcpy_spdl(current_attname, line + ATTR_NAME, MED);

            if ((strcmp(current_attname, "STERILITYTYPE") == 0) ||
                (!equals_blanktif(pvalue) &&
                 (!equals_no(pvalue)) &&
                 (strcmp(current_attname, "BOMLEVEL") != 0) &&
                 (strcmp(current_attname, "SIZELOGO") != 0) &&
                 (strcmp(current_attname, "PLANT")    != 0))) {
                strlcpy(idoc[n].pcode, current_pcode, MED);
                strlcpy(idoc[n].label, current_label, LBL);
                strlcpy(idoc[n].attr_name, current_attname, MED);

                char *attr_val = pvalue;
                if (attr_val == NULL) {
                    fprintf(stderr, "Error allocating memory for STERILITYTYPE attribute value.\n");
                    return NULL;
                } else {
                    idoc[n].attr_val = attr_val;
                }
                n++;
            }
        }

        if (n >= capacity - 1) {
            capacity *= 2;
            if ((idoc = (Idoc_row *) realloc(idoc, (capacity) * sizeof(Idoc_row))) == NULL) {
                fprintf(stderr, "Error allocating memory for idoc file.\n");
                return NULL;
            }

        }
        actual_linenums++;
    }

    *num_lines = n;
    return idoc;
}