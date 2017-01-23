#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "operations.h"
#include "list.h"

char active_file_name[FILE_NAME_LIMIT];
FILE *active_file;

void create_empty_file() {
    FILE* file;
    char file_name[FILE_NAME_LIMIT], answer[3];

    printf(" Unesite ime datoteke: ");
    fflush(stdin);
    gets(file_name);

    file = fopen(file_name, "wb");
    if(file != NULL) {
        printf(" Datoteka uspesno kreirana!");
    }
    else {
        printf(" Doslo je do greske pri kreiranju datoteke!");
    }

    printf("\n Da li zelite da datoteka postane aktivna? (DA / NE) >> ");
    fflush(stdin);
    gets(answer);

    if(strcmp(answer, "DA") == 0 || strcmp(answer, "Da") == 0 || strcmp(answer, "da") == 0) {
        printf(" Datoteka je postvaljena kao aktivna!");
        strcpy(active_file_name, file_name);
    }

    fclose(file);
}

void choose_active_file() {
    FILE* file;
    char file_name[FILE_NAME_LIMIT];

    printf("Unesite naziv zeljene aktivne datoteke: ");
    fflush(stdin);
    gets(file_name);

    strcpy(active_file_name, file_name);
    active_file = fopen(active_file_name, "rb");

    if(active_file != NULL) {
        printf("Datoteka je uspesno otvorena.");
    }
    else {
        printf("Doslo je do greske pri otvaranju datoteke!");
    }

    fclose(active_file);
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
    char answer[3];
    int ret, i = 0;

    FILE *serial_file = fopen("serial.bin", "ab+");
    if(serial_file == NULL) {
        printf("Doslo je do greske pri otvaranju datoteke!\n");
        return;
    }

    do{
        do {
            printf("Unesite identifikacioni broj: ");
            scanf("%lu", &furniture->id);
            fflush(stdin);
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


        if(fwrite(furniture, sizeof(Record), 1, serial_file)) {
            printf("\nSlog:");
            // print_record(furniture);
            printf(" uspesno zapisan u datoteku!\n");
        }

        i++;

        fflush(stdin);

    } while(i < BLOCK_FACTOR);

    fclose(serial_file);
    print_file("serial.bin");
}

void create_sequential_file() {
    FILE *serial, *sequential;
    Record furniture;
    Block block;
    Record_list *head = NULL, *iterator;
    int i = 0;

    serial = fopen("serial.bin", "rb");
    if(serial == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    sequential = fopen("sequential.bin", "wb");
    if(sequential == NULL) {
        printf("Greska prilikom otvaranja datoteke!");
        return;
    }

    // procitaj iz serijske datoteke i dodaj u listu
    while(fread(&furniture, sizeof(Record), 1, serial)){
        add_record(&head, &furniture);
    }

    // prodji kroz listu i zapisi blokove u serijsku
    iterator = head;
    do {
        block.records[i++] = iterator->record;
        if(i == BLOCK_FACTOR){
            fwrite(&block, sizeof(Block), 1, sequential);
            i = 0;
        }
        iterator = iterator->next;
    } while(iterator != NULL);

    // oslobodi resurse
    delete_list(&head);
    free(iterator);
    fclose(serial);
    fclose(sequential);

    print_file("sequential.bin");
}

void create_index_sequential_file() {

    create_primary_zone();

    // formiranje zone indeksa
    FILE *is_index = fopen("is_index.bin", "wb");
    FILE *is_primary = fopen("is_primary.bin", "rb");

    // izracunaj koliko postoji blokova u primarnoj zoni
    int primary_blocks_count = count_blocks(PRIMARY_ZONE);

    // izracunaj visinu stabla
    int tree_height = ceil(log2(primary_blocks_count));

    // Nadji broj cvorova
    int nodes_count = 0;
    int i = 0, j = 0;
    for(i; i < tree_height; i++) {
        nodes_count += ceil( (float) primary_blocks_count / pow(BLOCK_FACTOR_INDEX_ZONE, tree_height - i));
    }

    // izracunaj broj listova
    int leaf_nodes_count = ceil( (float) primary_blocks_count /(float) BLOCK_FACTOR_INDEX_ZONE );


    printf("\nbroj cvorova: %d", nodes_count);
    printf("\nbroj listova: %d", leaf_nodes_count);

    Block_index_zone biz[leaf_nodes_count];
    unsigned long biz_addresses[leaf_nodes_count];
    Block_primary_zone bpz;


    for(i = 0; i < leaf_nodes_count; i++) {
        for(j = 0; j < BLOCK_FACTOR_INDEX_ZONE; j++) {
            biz[i].records[j].address = ftell(is_primary);
            fread(&bpz, sizeof(Block_primary_zone),1, is_primary);

            unsigned long max_id = 0;
            int index;
            int k;
            for(k = 0; k < BLOCK_FACTOR; k++) {
                if(max_id < bpz.records[k].id && bpz.records[k].id != 999999999)
                    index = k;
            }

            biz[i].records[j].id = bpz.records[index].id;
        }
    }

    for(i = 0; i < leaf_nodes_count; i++) {
        printf("\nListovi indeks zone: 1: %lu %lu \n",
        biz[i].records[0].id,
        biz[i].records[0].address);
        printf("\nListovi indeks zone: 2: %lu %lu \n",
        biz[i].records[1].id,
        biz[i].records[1].address);

        // zapisi ih u indeks
        biz_addresses[i] = ftell(is_index);
        fwrite(&biz[i], sizeof(Block_index_zone), 1, is_index);
    }


    for(i = 0; i < 2; i++) {

        Block_index_zone biz_node;

        biz_node.records[i*2].id = biz[i*2].records[1].id;
        biz_node.records[i*2].address = biz_addresses[i*2];

        biz_node.records[i*2 + 1].id = biz[i*2 + 1].records[1].id;
        biz_node.records[i*2 + 1].address = biz_addresses[i*2 + 1];

        fwrite(&biz_node, sizeof(Block_index_zone), 1, is_index);

    }


    fclose(is_primary);
    fclose(is_index);

    print_index_zone();

}

void create_primary_zone() {
    // kreiranje primarne zone
    FILE *sequential, *is_primary;
    Record record;
    Block block;
    int i = 0;

    sequential = fopen("sequential.bin", "rb");
    is_primary = fopen("is_primary.bin", "wb");

    if(sequential == NULL || is_primary == NULL) {
        printf("Greska prilikom otvaranja datoteka!");
        return;
    }

    Block_primary_zone bpz;
    // citaj iz sekvencijalne i pisi u primarnu zonu indeks sekvencijalne
    while(fread(&block, sizeof(Block), 1, sequential)){
        // kopiranje slogova
        memcpy(&bpz, &block, sizeof(Block));

        // zapis bloka primarne zone u fajl
        fwrite(&bpz, sizeof(Block_primary_zone), 1, is_primary);
    }

    // zatvori datoteke
    fclose(sequential);
    fclose(is_primary);

    print_primary_zone();
}

void create_index_zone() {

}
int count_blocks(int type) {
    FILE *file;
    unsigned number_of_blocks = -1;

    if(type == SERIAL) {
        file = fopen("serial.bin", "rb");
        fseek(file, 0, SEEK_END);
        number_of_blocks = ftell(file) / sizeof(Block);
    }
    if(type == SEQUENTIAL) {
        file = fopen("sequential.bin", "rb");
        fseek(file, 0, SEEK_END);
        number_of_blocks = ftell(file) / sizeof(Block);
    }
    else if(type == PRIMARY_ZONE){
        file = fopen("is_primary.bin", "rb");
        fseek(file, 0, SEEK_END);
        number_of_blocks = ftell(file) / sizeof(Block_primary_zone);
    }

    printf("\n\nNumber of blocks: %d\n", number_of_blocks);
    fclose(file);
    return number_of_blocks;
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

    printf("\n\nPrikaz datoteke: %s\n", file_name);
    while(fread(&furniture, sizeof(Record), 1, file)) {
        printf("\nSlog %d: ", ++i);
        print_record(&furniture);
    }
}

void print_primary_zone() {
    FILE *file;
    Block_primary_zone bpz;

    file = fopen("is_primary.bin", "rb");
    int i = 0;

    printf("\n\nPrikaz primarne zone\n");
    while(fread(&bpz, sizeof(Block_primary_zone), 1, file)) {
        printf("\n\nBlok %d: ", ++i);
        print_block_primary_zone(&bpz);
    }
}

void print_index_zone() {
    FILE *file;
    Block_index_zone biz;

    file = fopen("is_index.bin", "rb");
    int i = 0;

    printf("\n\nPrikaz indeks zone\n");
    while(fread(&biz, sizeof(Block_index_zone), 1, file)) {
        printf("\n\nBlok %d: ", ++i);
        print_block_index_zone(&biz);
    }
}

void print_block_primary_zone(Block_primary_zone *bpz) {
    int i = 0;
    for(i; i < BLOCK_FACTOR; i++) {
        Record record = bpz->records[i];
        print_record(&record);
    }
}

void print_block_index_zone(Block_index_zone *biz) {
    int i = 0;
    for(i; i < BLOCK_FACTOR_INDEX_ZONE; i++) {
        Record_index_zone record = biz->records[i];
        printf("\nListovi indeks zone: 1: %lu %lu \n",
        record.id,
        record.address);
    }
}

