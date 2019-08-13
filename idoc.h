//
// Created by jkottiel on 8/12/2019.
//

#ifndef IDCOMPARE_IDOC_H
#define IDCOMPARE_IDOC_H

#include <stdio.h>

#define MED            31
#define LBL            10
#define MAX_ROW_LEN  1000
#define TYPE            3
#define ATTR_NAME      63
#define START_SIZE     10
#define ID_START        4
#define MATNR          "MH"
#define LABEL          "LH"
#define TDLINE         "TX"
#define DESCR          "LC"

typedef struct {
    char pcode[MED];
    char label[LBL];
    char attr_name[MED];
    char attr_val[MED];
} Idoc_row;

size_t strlcpy(char *dst, const char *src, size_t dsize);

size_t strlcpy_spdl(char *dst, const char *src, size_t dsize);

int read_idoc(Idoc_row *idoc, FILE *fp);

#endif //IDCOMPARE_IDOC_H
