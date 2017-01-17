#include <stdio.h>
#include <stdlib.h>
#include "operations.h"
#include "list.h"

char active_file_name[FILE_NAME_LIMIT];
FILE *index_seq;

void create_empty_file() {
    FILE* file;
    char file_name[FILE_NAME_LIMIT];

    printf(" Unesite ime datoteke: ");
    scanf("%s", file_name);

    file = fopen(file_name, "wb");

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

    file = fopen(file_name, "rb");

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
    Record *furniture = malloc(sizeof(Record));

    int ret;
    fflush(stdin);

    {
        do {
            printf("Unesite identifikacioni broj: ");
            char temp[50];
            gets(temp);
            furniture->id = atoi(temp);
            ret = validate_id(furniture->id);
        } while(ret != 1);

        do {
            printf("Unesite tip namestaja: ");
            gets(furniture->furniture_type);
            ret = validate_furniture_type(furniture->furniture_type);
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

        if(fwrite(furniture, sizeof(Record), 1, serial_file)) {
            printf("\nSlog:");
            // print_record(furniture);
            printf(" uspesno zapisan u datoteku!");
        }

        free(furniture);
        fclose(serial_file);

        print_file("serial.bin");
    }
}

void create_sequential_file() {
    FILE *serial, *sequential;
    Record furniture;
    Block block;
    record_list *head = NULL, *iterator;
    int i = 0;

    serial = fopen("serial.bin", "rb");
    if(serial == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    sequential = fopen("sequential.bin", "wb+");
    if(sequential == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    // procitaj i serijske datoteke i dodaj u listu
    while(fread(&furniture, sizeof(Record), 1, serial)){
        add_record(&head, &furniture);
    }

    // prodji kroz listu i zapisi blokove u serijsku
    iterator = head;
    while(iterator != NULL) {
        if(i == BLOCK_FACTOR){
            fwrite(&block, sizeof(Block), 1, sequential);
            i = 0;
        }
        block.records[i++] = iterator->record;
        iterator = iterator->next;
    }

    // zapisi poslednji blok
    if(i > 0) {
        fwrite(&block,sizeof(Block), 1, sequential);
    }

    // oslobodi resurse
    delete_list(&head);
    free(iterator);
    fclose(serial);
    fclose(sequential);

    print_file("sequential.bin");
}

void quick_sort(Record* array, int left, int right){
    if (left < right) {
        int i = left;
        int j = right;
        int pivot = atoi(array[(i+j)/2].id);
        while (i <= j) {
            while (atoi(array[i].id) < pivot) i++;
            while (atoi(array[j].id) > pivot) j--;
            if (i <= j) {
                Record furniture = array[i];
                array[i] = array[j];
                array[j] = furniture;
                i++;
                j--;
            }
        }
        quick_sort(array, left, j);
        quick_sort(array, i, right);
    }
}

void create_active_file() {
    FILE *sequential, *index_sequential;
    Record record;
    Block block;
    int i = 0;

    sequential = fopen("sequential.bin", "rb");
    if(sequential == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    // trebalo bi biti active, ali to cemo kasnije
    index_sequential = fopen("index_sequential.bin", "wb+");
    if(index_sequential == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    // citaj iz sekvencijalne i pisi u primarnu zonu indeks sekvencijalne
    while(fread(&record, sizeof(Record), 1, sequential)){
        if(i == BLOCK_FACTOR){
            fwrite(&block, sizeof(Block), 1, index_sequential);
            i = 0;
        }
        block.records[i++] = record;
    }
    // kopiraj ostatak poslenjeg bloka
    if(i > 0) {
        fwrite(&block, sizeof(Block), 1, index_sequential);
    }

    // zatvori datoteke
    fclose(sequential);
    fclose(index_sequential);

    print_file("index_sequential.bin");
}

void write_to_active_file() {

}

void search_active_file() {

}

void reorganization_active_file() {

}

void print_record(Record *furniture) {
    printf(" %lu", furniture->id);
    printf(" %s", furniture->furniture_type);
    printf(" %s", furniture->manufacture_date);
    printf(" %s", furniture->manufacture_time);
    printf(" %s", furniture->model_name);
    printf(" %s", furniture->weight);
}

void print_file(char file_name[]) {
    FILE *file;
    Record furniture;

    file = fopen(file_name, "rb");
    int i = 0;

    printf("Prikaz datoteke: %s\n", file_name);
    while(fread(&furniture, sizeof(Record), 1, file)) {
        printf("\n\nSlog %d: ", ++i);
        print_record(&furniture);
    }
}
