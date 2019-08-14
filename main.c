#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"
#include <time.h>

int main(int argc, char *argv[]) {

    // elapsed time
    clock_t start = clock();

    FILE *fpSAP, *fpMaster, *fpout;

    int sap_numlines = 0;
    int master_numlines = 0;

    Idoc_row *sap;
    Idoc_row *master;

    if (argc != 3) {
        printf("usage: %s verified_idoc.txt sap_extract.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((fpSAP = fopen(argv[1], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    sap = read_idoc(&sap_numlines, fpSAP);
    qsort(sap, sap_numlines, sizeof(Idoc_row), comparator);
    printf("Processed %d lines in the SAP extract...\n", sap_numlines);

    fpout = fopen("sap_out.txt", "w");
    for (int i = 0; i < sap_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", sap[i].pcode, sap[i].label, sap[i].attr_name, sap[i].attr_val);
    fclose(fpout);



    if ((fpMaster = fopen(argv[2], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    fpout = fopen("master_out.txt", "w");
    master = read_idoc(&master_numlines, fpMaster);
    qsort(master, master_numlines, sizeof(Idoc_row), comparator);

    printf("Processed %d lines in the Master idoc...\n", master_numlines);
    for (int i = 0; i < master_numlines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", master[i].pcode, master[i].label, master[i].attr_name,
                master[i].attr_val);
    fclose(fpout);

    for (int i = 0; i < sap_numlines; i++) {
        free(sap[i].attr_val);
    }

    for (int i = 0; i < master_numlines; i++) {
        free(master[i].attr_val);
    }

    free(sap);
    free(master);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);

    /* report
     	read mas
	read sap
while not EOF MAS and not EOF SAP

	if mas <> sap
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
    return 0;
}