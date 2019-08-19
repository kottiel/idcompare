#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"
#include <time.h>

int main(int argc, char *argv[]) {

    // elapsed time
    clock_t start = clock();

    FILE *fpMaster, *fpSAP, *fpout;
    char dconame[11];
    char errorfile[33];

    size_t master_numlines = 0;
    size_t sap_numlines = 0;

    Idoc_row *master;
    Idoc_row *sap;

    if (argc != 3) {
        printf("usage: %s verified_idoc.txt sap_extract.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* process the master (verified IDoc file)                                       */
    if ((fpMaster = fopen(argv[1], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    } else {
        strlcpy(dconame, argv[1], 11);
        printf("Opened verified IDoc file \"%-35s\" ", argv[1]);
    }
    if ((master = read_idoc(&master_numlines, fpMaster)) == NULL) {
        return EXIT_FAILURE;
    }
    qsort(master, master_numlines, sizeof(Idoc_row), comparator);
    printf("and processed %d lines.\n", (int)master_numlines);

    if ((fpout = fopen("master_out.txt", "w")) == NULL) {
        printf("Problem opening master_out.txt.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < master_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", master[i].pcode,
                                               master[i].label,
                                               master[i].attr_name,
                                               master[i].attr_val);
    fclose(fpout);

    /* process the SAP extract(s)                                                    */
    if ((fpSAP = fopen(argv[2], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    } else
        printf("Opened SAP extract file   \"%-35s\" ", argv[2]);

    if ((sap = read_idoc(&sap_numlines, fpSAP)) == NULL) {
        printf("Failed to read label extract.\n");
        return EXIT_FAILURE;
    }
    qsort(sap, sap_numlines, sizeof(Idoc_row), comparator);
    printf("and processed %d lines.\n", (int)sap_numlines);

    if ((fpout = fopen("sap_out.txt", "w")) == NULL) {
        printf("Problem opening sap_out.txt.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < sap_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", sap[i].pcode,
                                               sap[i].label,
                                               sap[i].attr_name,
                                               sap[i].attr_val);
    fclose(fpout);

    /* print IDoc Error File                                                         */
    strlcpy(errorfile, "IDoc Error File - ", 18);
    strncat(errorfile, dconame, 11);
    strncat(errorfile, ".txt", 5);
    if ((fpout = fopen(errorfile, "w")) == NULL) {
        printf("Problem opening Idoc Error File.\n");
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