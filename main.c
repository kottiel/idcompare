#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"

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

    int lines = read_idoc(sap, fpSAP);

    for (int i = 0; i < lines; i++) {
        free(sap[lines].attr_val);
    }
    free(sap);

    /*if ((fpMaster = fopen(argv[2], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    read_idoc(master, fpMaster);
    free(master);*/

    return 0;
}