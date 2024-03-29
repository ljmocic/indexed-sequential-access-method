#include "output.h"
#include "validation.h"

// stampanja
void print_record(Record *record) {
    printf("\n================================================");

    printf("\n Identifikacioni broj: ");
    printf("%d", record->id);

    printf("\n Vrstu namestaja: ");
    printf("%s", record->furniture_type);

    printf("\n Datum proizvodnje namestaja: ");
    printf("%s", record->production_date);

    printf("\n Vreme proizvodnje namestaja(HHMM): ");
    printf("%s", record->production_time);

    printf("\n Naziv modela namestaja: ");
    printf("%s", record->model_name);

    printf("\n Tezinu modela namestaja: ");
    printf("%d", record->weight);

    printf("\n================================================");
}

void print_serial() {
    FILE *serial;
    Record *record = malloc(sizeof(Record));

    serial = fopen("serial.bin", "rb");
    if(!serial) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
    }

    while(fread(record, sizeof(Record), 1, serial)){
        print_record(record);
    }
    fclose(serial);
}

void print_sequential() {
    FILE *sequential;
    Record record;

    sequential = fopen("sequential.bin", "rb");
    if(!sequential) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
    }

    while(fread(&record, sizeof(Record), 1, sequential)){
        print_record(&record);
    }
    fclose(sequential);
}

void print_primary_zone() {
    FILE *index_sequential;
    Block_primary_zone *bpz = malloc(sizeof(Block_primary_zone));
    int i;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka! \n");
        return;
    }

    index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);

    int j, k = 0;
    for(j = 0; j < header.block_count_primary_zone; j++) {
        fread(bpz, sizeof(Block_primary_zone), 1, index_sequential);
        printf("\n Broj bloka: %d \n", ++k);
        for(i = 0; i < BLOCK_FACTOR; i++){
            if(bpz->records[i].status == 1) {
                printf("\n Slog: %d", i+1);
                print_record(&(bpz->records[i]));
            }
        }
    }
    free(bpz);
    fclose(index_sequential);
}

void print_index_zone() {
    FILE *index_sequential;
    Block_index_zone *biz = malloc(sizeof(Block_index_zone));
    int i;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka! \n");
        return;
    }

    index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);
    fseek(index_sequential, header.index_zone_address, SEEK_SET);

    int j = 0;
    for(i = 0; i < header.block_count_index_zone; i++) {
        fread(biz, sizeof(Block_index_zone), 1, index_sequential);
        printf("\n Blok indeks zone: %d \n", ++j);
        printf(" %d", biz->keys[0]);
        printf(" %d", biz->keys[1]);
    }
    printf("\n==========================================\n");
    fclose(index_sequential);
}

void print_overflow_zone() {
    FILE *index_sequential;
    Block_overflow_zone boz;
    int i;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka! \n");
        return;
    }

    index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);
    fseek(index_sequential, header.overflow_zone_address, SEEK_SET);

    int j = 0;
    for(i = 0; i < header.block_count_primary_zone; i++) {
        fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
            printf("\n Blok zone prekoracilaca %d: ", ++j);
            printf(" %d %d", boz.records[0].id, boz.status);

    }
    fclose(index_sequential);
}

void print_temp_serial() {
    FILE *serial;
    Record *record = malloc(sizeof(Record));

    serial = fopen("temp_serial.bin", "rb");
    if(serial == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }

    while(fread(record, sizeof(Record), 1, serial)){
        print_record(record);
    }

    free(record);
    fclose(serial);
}

void print_temp_sequential() {
    FILE *sequential;
    Record *record = malloc(sizeof(Record));

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka! \n");
        return;
    }

    sequential = fopen("temp_sequential.bin", "rb");
    if(!sequential) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }

    while(fread(record, sizeof(Record), 1, sequential)){
        print_record(record);
    }

    free(record);
    fclose(sequential);
}
