#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"

int main(int argc, char *argv[]) {

    FILE *fpSAP;
    FILE *fpMaster;
    int num_lines = 0;

    Idoc_row *sap;
    Idoc_row *master;

    if (argc != 3) {
        printf("usage: %s filename.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((fpSAP = fopen(argv[1], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    sap = read_idoc(&num_lines, fpSAP);
    printf("number of lines is %d\n", num_lines);
    for (int i = 0; i < num_lines; i++)
        printf("%-30s %9s %-30s %s\n", sap[i].pcode, sap[i].label, sap[i].attr_name, sap[i].attr_val);

    for (int i = 0; i < num_lines; i++) {
        free(sap[i].attr_val);
    }

    free(sap);

    if ((fpMaster = fopen(argv[2], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    master = read_idoc(&num_lines, fpMaster);
//    for (int i = 0; i < num_lines; i++)
//        printf("%-30s %9s %-30s %s\n", master[i].pcode, master[i].label, master[i].attr_name, master[i].attr_val);

    for (int i = 0; i < num_lines; i++) {
        free(master[i].attr_val);
    }

    free(master);

    return 0;
}