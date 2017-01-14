#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME_LIMIT 100

char activeFileName[FILE_NAME_LIMIT + 1];
FILE *indexSeq;

void createEmptyFile();
void chooseActiveFile();
void showActiveFileName();
void createSerialFile();
void createSequentialFile();
void createActiveFile();
void writeToActiveFile();
void searchActiveFile();
void reorganizationActiveFile();

int main() {

    int chosenMenu;

    do {
        printf("==========================================\n");
        printf(" 1. Formiranje prazne datoteke \n");
        printf(" 2. Izbor aktivne datoteke \n");
        printf(" 3. Prikaz naziva aktivne datoteke \n");
        printf(" 4. Formiranje serijske datoteke \n");
        printf(" 5. Formiranje sekvencijalne datoteke \n");
        printf(" 6. Formiranje aktivne datoteke \n");
        printf(" 7. Upis novog sloga u aktivnu datoteku \n");
        printf(" 8. Trazenje sloga u aktivnoj datoteci \n");
        printf(" 9. Reorganizacija aktivne datoteke \n");
        printf(" 10. Izlaz \n");
        printf("==========================================\n");

        printf("\n >> ");
        scanf("%d",&chosenMenu);
        printf("\n");

        switch(chosenMenu) {
            case 1:
                createEmptyFile();
                break;
            case 2:
                chooseActiveFile();
                break;
            case 3:
                showActiveFileName();
                break;
            case 4:
                createSerialFile();
                break;
            case 5:
                createSequentialFile();
                break;
            case 6:
                createActiveFile();
                break;
            case 7:
                writeToActiveFile();
                break;
            case 8:
                searchActiveFile();
                break;
            case 9:
                reorganizationActiveFile();
                break;
            case 10:
                printf("Izlaz...");
                return 0;
            default:
                break;
        }

        printf("\n\n");
        // system("cls");

    } while(chosenMenu != 10);

    return 0;
}

void createEmptyFile() {
    FILE* file;
    char fileName[FILE_NAME_LIMIT];

    printf(" Unesite ime datoteke: ");
    scanf("%s", fileName);

    file = fopen(fileName, "w");

    if(file != NULL) {
        printf(" Datoteka uspesno kreirana!");
    }
    else {
        printf(" Doslo je do greske pri kreiranju datoteke!");
    }

    fclose(file);
}

void chooseActiveFile() {
    FILE* file;
    char fileName[FILE_NAME_LIMIT];

    printf("Unesite naziv zaljene aktivne datoteke: ");
    scanf("%s", fileName);

    file = fopen(fileName, "r");

    if(file != NULL) {
        printf("Datoteka je uspesno otvorena.");
    }
    else {
        printf("Doslo je do greske pri otvaranju datoteke!");
        return;
    }

    if(indexSeq != NULL) {
        fclose(indexSeq);
    }
    indexSeq = file;
    strcpy(activeFileName, fileName);
    fclose(file);
}

void showActiveFileName() {
    if(strcmp(activeFileName, "") != 0) {
        printf("Aktivna datoteka: %s", activeFileName);
    }
    else {
        printf("Nema aktivne datoteke!");
    }
}

void createSerialFile() {

}

void createSequentialFile() {

}

void createActiveFile() {

}

void writeToActiveFile() {

}

void searchActiveFile() {

}

void reorganizationActiveFile() {

}
