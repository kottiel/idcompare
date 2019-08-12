//
// Created by jkottiel on 8/12/2019.
//

#ifndef IDCOMPARE_IDOC_H
#define IDCOMPARE_IDOC_H

#define MED            31
#define LBL            10
#define MAX_ROW_LEN  1000
#define TYPE            7
#define ATTR_NAME      64



typedef struct {
    char pcode[MED];
    char label[LBL];
    char attr_name[MED];
    char attr_val[MED];
} Idoc_row;

#endif //IDCOMPARE_IDOC_H
