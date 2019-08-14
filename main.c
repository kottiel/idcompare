#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idoc.h"
#include <time.h>

int main(int argc, char *argv[]) {

    // elapsed time
    clock_t start = clock();

    FILE *fpSAP, *fpMaster, *fpout;

    int num_lines = 0;

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

    sap = read_idoc(&num_lines, fpSAP);
    qsort(sap, num_lines, sizeof(Idoc_row), comparator);
    printf("Processed %d lines in the SAP extract...\n", num_lines);

    fpout = fopen("sap_out.txt", "w");
    for (int i = 0; i < num_lines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", sap[i].pcode, sap[i].label, sap[i].attr_name, sap[i].attr_val);
    fclose(fpout);

    for (int i = 0; i < num_lines; i++) {
        free(sap[i].attr_val);
    }

    free(sap);

    if ((fpMaster = fopen(argv[2], "r")) == NULL) {
        printf("File not found.\n");
        return EXIT_FAILURE;
    }

    fpout = fopen("master_out.txt", "w");
    master = read_idoc(&num_lines, fpMaster);
    qsort(master, num_lines, sizeof(Idoc_row), comparator);

    printf("Processed %d lines in the Master idoc...\n", num_lines);
    for (int i = 0; i < num_lines; i++)
        fprintf(fpout, "%-15s %9s %-28s %s\n", master[i].pcode, master[i].label, master[i].attr_name,
                master[i].attr_val);
    fclose(fpout);

    for (int i = 0; i < num_lines; i++) {
        free(master[i].attr_val);
    }

    free(master);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);

    /* report
     * MAS    SAP
a      a
b      b
c      d
g      e

while not EOF MAS and not EOF SAP
	read mas
	read sap
	if mas <> sap and mas < sap:
	   report missing mas record current record "xyz"
           read next mas

        else mas <> sap and mas > sap
           report sap record not found current record
           read next sap

end while
if EOF MAS
    while not EOF SAP
        report sap record not found

else if EOF SAP
    while not EOF MAS
        report mas record not found current record
     *
     *
     * */
    return 0;
}