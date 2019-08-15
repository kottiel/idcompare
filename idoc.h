//
// Created by kottiel on 8/12/2019.
//

#ifndef IDCOMPARE_IDOC_H
#define IDCOMPARE_IDOC_H

#include <stdio.h>

#define MED                31
#define LBL                10
#define MAX_ROW_LEN      1000
#define RIGHT_MOST_EDGE    99
#define TDLINE_RIGHT_EDGE  70
#define VALUE_START       123
#define TDLINE_START      152
#define TYPE                3
#define ATTR_NAME          63
#define START_SIZE         10
#define ID_START            4
#define MATNR            "MH"
#define LABEL            "LH"
#define TDLINE           "TX"
#define DESCR            "LC"

typedef struct {
    char pcode[MED];
    char label[LBL];
    char attr_name[MED];
    char *attr_val;
} Idoc_row;

size_t strlcpy(char *dst, const char *src, size_t dsize);

size_t strlcpy_spdl(char *dst, const char *src, size_t dsize);

Idoc_row *read_idoc(int *n, FILE *fp);

int comparator(const void *p, const void *q);

int compare(FILE *fpout, Idoc_row *master, int mrows, Idoc_row *sap, int srows);


#endif //IDCOMPARE_IDOC_H
