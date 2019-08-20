#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"
#include <time.h>

int main(int argc, char *argv[]) {

    // elapsed time
    clock_t start = clock();

    FILE *fpMaster, *fpSAP, *fpSAPvar, *fpout;
    char dconame[11];
    char *fn;
    char errorfile[33] = {};

    size_t master_numlines = 0;
    size_t sap_numlines = 0;

    Idoc_row *master;
    Idoc_row *sap;

    char line[MAX_ROW_LEN];

    /* open the master (verified IDoc file)                                       */
    if ((fpMaster = fopen(argv[1], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    } else {
        fn = strstr(argv[1], "DCO");
        strncpy(dconame, fn, 10);
    }

    if (argc < 3) {
        printf("usage: %s verified_idoc.txt sap_extract1.txt sap_extract2.txt ... sap_extractN.txt\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc > 3) {

        // we need to combine the SAP extract files into 1 file.
        remove("tempSAP.txt");
        if ((fpSAP = fopen("tempSAP.txt", "a")) == NULL) {
            printf("File 'tempSAP.txt' could not be created.\n");
            return EXIT_FAILURE;
        } else {
            for (int i = 2; i < argc; i++) {
                if ((fpSAPvar = fopen(argv[i], "r")) == NULL) {
                    printf("File 'argv[i]' could not be opened.\n");
                    return EXIT_FAILURE;
                } else {
                    while (fgets(line, MAX_ROW_LEN, fpSAPvar) != NULL)
                        fprintf(fpSAP, "%s", line);
                }
                fclose(fpSAPvar);

            }
            fclose(fpSAP);
            if ((fpSAP = fopen("tempSAP.txt", "r")) == NULL) {
                printf("File not found.\n");
                return EXIT_FAILURE;
            }
        }
    } else {
        if ((fpSAP = fopen(argv[2], "r")) == NULL) {
            printf("File not found.\n");
            return EXIT_FAILURE;
        }
    }


    if ((master = read_idoc(&master_numlines, fpMaster)) == NULL) {
        return EXIT_FAILURE;
    }
    fclose(fpMaster);
    qsort(master, master_numlines, sizeof(Idoc_row), comparator);
    printf("Opened verified IDoc file \"%s\" ", argv[1]);
    printf("...and processed %d lines.\n", (int)master_numlines);

    if ((fpout = fopen("verified_idoc_out.txt", "w")) == NULL) {
        printf("Problem opening master_out.txt.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < master_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", master[i].pcode,
                master[i].label,
                master[i].attr_name,
                master[i].attr_val);
    fclose(fpout);

    if ((sap = read_idoc(&sap_numlines, fpSAP)) == NULL) {
        printf("Failed to read label extract.\n");
        return EXIT_FAILURE;
    }
    fclose(fpSAP);
    qsort(sap, sap_numlines, sizeof(Idoc_row), comparator);
    for (int i = 2; i < argc; i++) {
        printf("Opened SAP extract file \"%s\"%c", argv[i], (argc > 3 ? '\n' : ' '));
    }

    printf("...and processed %d lines.\n", (int)sap_numlines);

    if ((fpout = fopen("sap extract(s)_out.txt", "wb")) == NULL) {
        printf("Problem opening sap_out.txt.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < sap_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", sap[i].pcode,
                sap[i].label,
                sap[i].attr_name,
                sap[i].attr_val);
    fclose(fpout);
    remove("tempSAP.txt");

    /* print IDoc Error File                                                         */
    strlcpy(errorfile, "IDoc Error File - ", 20);
    strncat(errorfile, dconame, 11);
    strncat(errorfile, ".txt", 5);
    printf("Writing error file to %s\n", errorfile);
    if ((fpout = fopen(errorfile, "wb")) == NULL) {
        printf("Problem opening %s\n", errorfile);
        return EXIT_FAILURE;
    }

    compare(fpout, master, master_numlines, sap, sap_numlines);
    fclose(fpout);

    /* free dynamically allocated memory                                             */
    for (int i = 0; i < master_numlines; i++) {
        free(master[i].attr_val);
    }
    for (int i = 0; i < sap_numlines; i++) {
        if (sap[i].attr_val != NULL)
            free(sap[i].attr_val);
    }

    free(sap);
    free(master);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);

    return 0;
}