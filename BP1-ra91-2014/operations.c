#include <stdio.h>
#include <stdlib.h>
#include "operations.h"

char active_file_name[FILE_NAME_LIMIT];
FILE *index_seq;

void create_empty_file() {
    FILE* file;
    char file_name[FILE_NAME_LIMIT];

    printf(" Unesite ime datoteke: ");
    scanf("%s", file_name);

    file = fopen(file_name, "w");

    if(file != NULL) {
        printf(" Datoteka uspesno kreirana!");
    }
    else {
        printf(" Doslo je do greske pri kreiranju datoteke!");
    }

    fclose(file);
}

void choose_active_file() {
    FILE* file;
    char file_name[FILE_NAME_LIMIT];

    printf("Unesite naziv zaljene aktivne datoteke: ");
    scanf("%s", file_name);

    file = fopen(file_name, "r");

    if(file != NULL) {
        printf("Datoteka je uspesno otvorena.");
    }
    else {
        printf("Doslo je do greske pri otvaranju datoteke!");
        return;
    }

    if(index_seq != NULL) {
        fclose(index_seq);
    }
    index_seq = file;
    strcpy(active_file_name, file_name);
    fclose(file);
}

void show_active_file_name() {
    if(strcmp(active_file_name, "") != 0) {
        printf("Aktivna datoteka: %s", active_file_name);
    }
    else {
        printf("Nema aktivne datoteke!");
    }
}

void create_serial_file() {
    Furniture *furniture = malloc(sizeof(Furniture));

    int ret;
    fflush(stdin);

    {
        do {
            printf("Unesite identifikacioni broj: ");
            gets(furniture->id);
            ret = validate_id(furniture->id);
        } while(ret != 1);

        do {
            printf("Unesite tip namestaja: ");
            gets(furniture->futniture_type);
            ret = validate_furniture_type(furniture->futniture_type);
        } while(ret != 1);

        do {
            printf("Unesite datum proizvodnje: ");
            gets(furniture->manufacture_date);
            ret = validate_date(furniture->manufacture_date);
        } while(ret != 1);

        do {
            printf("Unesite vreme proizvodnje: ");
            gets(furniture->manufacture_time);
            ret = validate_time(furniture->manufacture_time);
        } while(ret != 1);

        do {
            printf("Unesite ime modela: ");
            gets(furniture->model_name);
            ret = validate_model_name(furniture->model_name);
        } while(ret != 1);

        do {
            printf("Unesite tezinu namestaja: ");
            gets(furniture->weight);
            ret = validate_weight(furniture->weight);
        } while(ret != 1);

        FILE *serial_file = fopen("serial.bin", "ab+");
        if(serial_file == NULL) {
            printf("Doslo je do greske pri otvaranju datoteke!\n");
            return;
        }

        if(fwrite(furniture, sizeof(Furniture), 1, serial_file)) {
            printf("\nSlog:");
            printRecord(furniture);
            printf(" uspesno zapisan u datoteku!");
        }

        free(furniture);
        fclose(serial_file);
    }
}

void create_sequential_file() {
    FILE *serial = fopen("serial.bin", "rb");
    if(serial == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    FILE *sequential = fopen("sequential.bin", "wb");
    if(sequential == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    // ucitaj velicinu, i vrati pokazivac na pocetak fajla
    fseek(serial, 0, SEEK_END);
    int size = ftell(serial);
    //rewind(serial);
    fseek(serial, 0, SEEK_END);

    // inicijalizuj niz potrebne velicine
    Furniture *data = malloc(sizeof(Furniture) * size);;

    // citaj iz serijske datoteke i popuni niz
    int i = 0, ret = 1;
    while(ret) {
        ret = fread(&data[i], sizeof(Furniture), 1, serial);
        i++;
    }

    // implement sorting
    //quick_sort(&data, 0, size);

}

void quick_sort(Furniture* array, int left, int right){
    if (left < right) {
        int i = left;
        int j = right;
        int pivot = atoi((array + (i+j)/2 * sizeof(Furniture))->id);
        printf("test");
        while (i<=j)
        {
            while (atoi(array[i].id) < pivot) i++;
            while (atoi(array[j].id) > pivot) j--;
            if (i<=j)
            {
                Furniture f = array[i];
                array[i] = array[j];
                array[j] = f;
                i++;
                j--;
            }
        }
        quick_sort(array, left, j);
        quick_sort(array, i, right);
    }
}

void create_active_file() {

}

void write_to_active_file() {

}

void search_active_file() {

}

void reorganization_active_file() {

}

void printRecord(Furniture *furniture) {
    printf(" %s", furniture->id);
    printf(" %s", furniture->futniture_type);
    printf(" %s", furniture->manufacture_date);
    printf(" %s", furniture->manufacture_time);
    printf(" %s", furniture->model_name);
    printf(" %s", furniture->weight);
}
