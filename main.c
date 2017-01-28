#include "defs.h"
#include "output.h"
#include "list.h"

int main() {
    int chosen_menu;
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
        printf(" 11. Prikazi serijsku \n");
        printf(" 12. Prikazi sekvencijalnu \n");
        printf(" 13. Prikazi indeks sekvencijalnu \n");
        printf(" 14. Prikazi temp_serial za reorganizaciju \n");
        printf("==========================================\n");

        printf("\n >> ");
        fflush(stdin);
        scanf("%d", &chosen_menu);
        printf("\n");

        switch(chosen_menu) {
            case 1:
                create_empty_file();
                break;
            case 2:
                choose_active_file();
                break;
            case 3:
                show_active_file_name();
                break;
            case 4:
                create_serial_file();
                break;
            case 5:
                // obicni mod 0, mod za reorganizaciju 1
                create_sequential_file(0);
                break;
            case 6:
                // obicni mod 0, mod za reorganizaciju 1
                create_index_sequential_file(0);
                break;
            case 7:
                write_to_active_file();
                break;
            case 8:
                search_active_file();
                break;
            case 9:
                reorganization_active_file();
                break;
            case 10:
                printf("Izlaz...");
                return 0;
            case 11:
                printf("\n Prikaz serijske: \n");
                print_serial();
                break;
            case 12:
                printf("\n Prikaz sekvencijalne: \n");
                print_sequential();
                break;
            case 13:
                printf("\n Prikaz indeks-sekvencijalne: \n");
                print_primary_zone();
                print_index_zone();
                print_overflow_zone();
                break;
            case 14:
                printf("\n Prikaz serijske za reorganizaciju: \n");
                print_temp_serial();
            default:
                break;
        }

        printf("\n\n");
        // system("cls");

    } while(chosen_menu != 10);

    return 0;
}

void create_empty_file() {
    FILE *file;
    char file_name[FILE_NAME_LIMIT];

    printf("\n Unesite naziv datoteke: ");
    fflush(stdin);
    gets(file_name);

    file = fopen(file_name, "wb");
    if(file) {
        printf("\n Datoteka uspesno formirana! \n");
    }
    else {
        printf("\n Doslo je do greske pri formiranju datoteke! \n");
    }

    fclose(file);
}

void choose_active_file() {
    FILE *active_file;
    char file_name[FILE_NAME_LIMIT];

    printf("\n Unesite naziv zeljene aktivne datoteke: ");
    fflush(stdin);
    gets(file_name);

    active_file = fopen(file_name, "rb");
    if(active_file) {
        printf("\n Datoteka je proglasena aktivnom! \n");
        strcpy(active_file_name, file_name);
        fclose(active_file);
    }
    else {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
        return;
    }
    fclose(active_file);
}

void show_active_file_name() {
    if(strcmp(active_file_name, "")) {
        printf("\n %s \n", active_file_name);
    }
    else {
        printf("\n Nije odabrana aktivna datoteka! \n");
    }
}

void create_serial_file() {
    FILE *serial = fopen("serial.bin", "ab+");
    int i;

    if(!serial) {
        printf("\n Doslo je do greske pri otvaranju / kreiranju serijske datoteke! \n");
        return;
    }

    for(i = 0; i < BLOCK_FACTOR; i++) {
        Record *record = (Record *)malloc(sizeof(Record));
        printf("\n Slog %d:", i + 1);
        get_record_from_user(record);
        fwrite(record, sizeof(Record), 1, serial);
    }

    fclose(serial);
}

void create_sequential_file(int reorganization) {
    FILE *serial, *sequential;
    Block block;
    Record record;
    Record_list *head = NULL, *iterator;
    int i = 0;

    if(reorganization) {
        sequential = fopen("temp_sequential.bin", "wb");
        serial = fopen("temp_serial.bin", "rb");
    }
    else {
        sequential = fopen("sequential.bin", "wb");
        serial = fopen("serial.bin", "rb");
    }
    if(serial == NULL || sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteka!\n");
        return;
    }

    // procitaj i dodaj u listu
    while(fread(&record, sizeof(Record), 1, serial)){
        add_record(&head, &record);
    }

    // prodji kroz listu i zapisi blokove u serijsku
    iterator = head;
    do {
        block.records[i++] = iterator->record;
        if(i == BLOCK_FACTOR) {
            fwrite(&block, sizeof(Block), 1, sequential);
            i = 0;
        }
        iterator = iterator->next;
    } while(iterator != NULL);

    delete_list(&head);
    free(iterator);
    fclose(serial);
    fclose(sequential);
}

void create_index_sequential_file(int reorganization) {
    FILE *sequential, *index_sequential;
    int i;

    if(reorganization) {
        sequential = fopen("temp_sequential.bin", "rb");
    }
    else {
        sequential = fopen("sequential.bin", "rb");
    }

    index_sequential = fopen("index_sequential.bin", "wb");
    if(sequential == NULL || index_sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke!\n");
        return;
    }

    // pronadji broj blokova sekvencijane
    fseek(sequential, 0, SEEK_END);
    int number_of_records = ftell(sequential) / sizeof(Record);
    rewind(sequential);

    // broj dozvoljenih slogova u bloku primarne zone
    int num_allowed_rec_bpz = (int) (IBF * BLOCK_FACTOR); // ibf * BLOCK_FACTOR
    int number_of_blocks = number_of_records / num_allowed_rec_bpz;

    if(number_of_records % num_allowed_rec_bpz > 0) {
        number_of_blocks++;
    }

    Header header = init_header();
    fwrite(&header, sizeof(Header), 1, index_sequential);


    Record record;
    // niz blokova primarne zone
    Block_primary_zone bpz[number_of_blocks];

    // inicijalizovao sa nulama, jer mi izbacivalo smece u konzoli
    int j;
    for(i = 0; i < number_of_blocks; i++) {
        bpz[i].address = -1;
        for(j = 0; j < BLOCK_FACTOR; j++) {
            bpz[i].records[j].id = -1;
            bpz[i].records[j].furniture_type[0] = 0;
            bpz[i].records[j].model_name[0] = 0;
            bpz[i].records[j].production_date[0] = 0;
            bpz[i].records[j].production_time[0] = 0;
            bpz[i].records[j].weight = 0;
            bpz[i].records[j].status = 0;
        }
    }

    // sacuvaj broj blokova u primarnoj zoni, trebace ti kasnije
    header.block_count_primary_zone = number_of_blocks;

    int bpz_max[number_of_blocks];
    int bpz_address[number_of_blocks];
    int bpz_counter = 0, record_counter = 0;


    while(fread(&record, sizeof(Record), 1, sequential)) {

        if(record_counter < num_allowed_rec_bpz) {
            bpz[bpz_counter].records[record_counter++] = record;
        }
        else {
            // zapisi maksimalan kljuc
            bpz_max[bpz_counter] = bpz[bpz_counter].records[--record_counter].id;
            // ako je popunjen, popuni sledeci blok
            bpz_counter++;
            // zapisi u sledeci blog prim zone
            record_counter = 0;
            bpz[bpz_counter].records[record_counter++] = record;
        }
    }

    if(number_of_blocks % num_allowed_rec_bpz != 0) {
        // zapisi maksimalni kljuc od poslenjeg upisanog bloka primarne zone
        bpz_max[bpz_counter] = bpz[bpz_counter].records[number_of_blocks % num_allowed_rec_bpz].id;
        //printf("SADA: %d", bpz_max[bpz_counter]);
    }

    // zapisi adrese blokova i upisi primarnu zonu u datoteku
    header.primary_zone_adress = ftell(index_sequential);
    fseek(index_sequential, 0, SEEK_SET);

    // azuriraj adresu sada kada je znas
    fwrite(&header, sizeof(Header), 1, index_sequential);

    // prepisivanje iz sekvencijalne u primarnu zonu i cuvanje adresa u niz
    for(i = 0; i < number_of_blocks; i++) {
        bpz_address[i] = ftell(index_sequential);
        fwrite(&bpz[i], sizeof(Block_primary_zone), 1, index_sequential);
    }

    /*
    // sadrzaj maksimalnih kljuceva u blokovima i njihovih adresa
    for(i = 0; i < number_of_blocks; i++) {
        printf("%d ", bpz_max[i]);
        printf("%d ", bpz_address[i]);
    }
    */

    // INDEX ZONA

    // sacuvaj lokaciju indeks zone u header
    header.index_zone_address = ftell(index_sequential);
    fseek(index_sequential, 0, SEEK_SET);
    fwrite(&header, sizeof(Header), 1, index_sequential);

    // vrate se da nastavis
    fseek(index_sequential, header.index_zone_address, SEEK_SET);

    // postavljamo poslenji kljuc na 999999999
    bpz_max[number_of_blocks - 1] = MAX_KEY_VALUE;

    // stavljam u niz adrese
    Block_index_zone *biz = (Block_index_zone *)malloc(sizeof(Block_index_zone) * bpz_counter);
    int *addresses = (int *)malloc(sizeof(int) * bpz_counter);
    j = 0;

    for(i = 0; i <= bpz_counter; i++) {
        printf("\t %d", bpz_max[i]);
    }

    // generisem kljuceve viseg nivoa
    for(i = 0; i < bpz_counter / 2; i++){
        biz[i].keys[0] = bpz_max[j];
        biz[i].address[0] = bpz_address[j++];
        biz[i].keys[1] = bpz_max[j];
        biz[i].address[1] = bpz_address[j++];
        addresses[i] = ftell(index_sequential);
        //printf("\nKljuc 1: %d ", biz[i].keys[0]);
        //printf("\nKljuc 2: %d ", biz[i].keys[1]);
        fwrite(&biz[i], sizeof(Block_index_zone), 1, index_sequential);
        header.block_count_index_zone++;
    }
    if((bpz_counter + 1) % 2 == 1) {
        biz[i].keys[0] = bpz_max[j];
        biz[i].keys[1] = -1;
        biz[i].address[0] = bpz_address[j];
        biz[i].address[1] = -1;
        addresses[i] = ftell(index_sequential);
        // printf("\nKljuc neparan: %d %d", biz[i].keys[0], biz[i].keys[1]);
        fwrite(&biz[i], sizeof(Block_index_zone), 1, index_sequential);
        header.block_count_index_zone++;
    }

    // azuriraj podatke u headeru
    fseek(index_sequential, 0, SEEK_SET);
    fwrite(&header, sizeof(Header), 1, index_sequential);
    // vrate se da nastavis
    fseek(index_sequential, 0, SEEK_END);

    // racunanje potrebnih formula
    int h = (int)ceil(log2(number_of_blocks));
    int *nodes_count = (int *)malloc(sizeof(int) * (h + 1));
    for (i = 1; i <= h; i++) {
        nodes_count[i] = (int)ceil((double)number_of_blocks / pow(2.0, h - i + 1));
    }

    Block_index_zone * preth;
    Block_index_zone * novi;
    int *adr_preth;
    int *adr_novi;

    // gornji nivo nodes_count[h-2]
    // srednji nivo nodes_count[h-1]
    // donji nivo nodes_count[h]


    // prodji kroz stablo i generisi cvorove
    for (i = 1; i < h; i++) {

        if (i == 1){
            preth = biz;
            adr_preth = addresses;
        }
        else {
            preth = novi;
            adr_preth = adr_novi;
        }

        novi = (Block_index_zone *)malloc(sizeof(nodes_count[h - i] + 1));
        adr_novi = (int *)malloc(sizeof(nodes_count[h - i] + 1));

        /*
        printf("\nBroj cvorova: %d", nodes_count[h - i + 1]);

        for(j = 0; j < nodes_count[h - i + 1]; j++) {
            //printf("\nkljucevi %d %d \t", preth[j].keys[0], preth[j].keys[1]);
        }
        printf("\n Nivo stabla: %d", h - i);
        */

        int p = 0;
        for(j = 0; j < nodes_count[h - i] - 1; j++) {
            novi[j].keys[0] = preth[p].keys[1];
            novi[j].address[0] = adr_preth[p++];
            novi[j].keys[1] = preth[p].keys[1];
            novi[j].address[1] = adr_preth[p++];
            adr_novi[j] = ftell(index_sequential);
            fwrite(&novi[j], sizeof(Block_index_zone), 1, index_sequential);
            header.block_count_index_zone++;
            //printf("\nPROBAJ: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        //printf("dasda %d" ,preth[nodes_count[h - i + 1] - 1].keys[1]);
        if(preth[nodes_count[h - i + 1] - 1].keys[1] == -1 && h - i != 1) {
            novi[j].keys[0] = preth[p].keys[0];
            novi[j].address[0] = adr_preth[p++];
            novi[j].keys[1] = -1;
            novi[j].address[1] = -1;
            printf("\nPROBAJ1: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        else {
            if(h - i == 1 && preth[nodes_count[h - i + 1] - 1].keys[1] == -1) {
                novi[j].keys[0] = preth[p].keys[1];
                novi[j].address[0] = adr_preth[p++];
                novi[j].keys[1] = preth[p].keys[0];
                novi[j].address[1] = adr_preth[p++];
            }
            else {
                novi[j].keys[0] = preth[p].keys[1];
                novi[j].address[0] = adr_preth[p++];
                novi[j].keys[1] = preth[p].keys[1];
                novi[j].address[1] = adr_preth[p++];
            }
            //printf("\nPROBAJ2: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        adr_novi[j] = ftell(index_sequential);
        fwrite(&novi[j], sizeof(Block_index_zone), 1, index_sequential);
        header.block_count_index_zone++;

        fseek(index_sequential, 0, SEEK_SET);
        fwrite(&header, sizeof(Header), 1, index_sequential);
        fseek(index_sequential, 0, SEEK_END);
    }

    // sada pocinje zona prekoracenja
    header.overflow_zone_address = ftell(index_sequential);
    header.free_overflow_block = header.overflow_zone_address;

    // inicijalizuj zonu prekoracenja
    Block_overflow_zone boz;
    for(j = 0; j < header.block_count_primary_zone; j++) {
        boz.address = -1;
        boz.status = 0;
        boz.records[0].id = -1;
        fwrite(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
    }

    // azuriraj podatke u headeru
    fseek(index_sequential, 0, SEEK_SET);
    fwrite(&header, sizeof(Header), 1, index_sequential);

    fclose(sequential);
    fclose(index_sequential);

    // print_index_zone();
    // print_overflow_zone();
}

void write_to_active_file() {
    // ukoliko kljuc ne postoji
    Record record;
    get_record_from_user(&record);
    // print_record(&record);
    add_record_to_primary(record);
}

void search_active_file() {
    FILE *index_sequential = fopen("index_sequential.bin", "rb");
    int id;

    printf("\n Unesite ID: ");
    fflush(stdin);
    scanf("%d", &id);

    Search_result sr = search_primary_overflow(id);

    Block_primary_zone bpz;
    if(sr.index != -1) {
        fseek(index_sequential, sr.primary_zone_address, SEEK_SET);
        fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);

        printf("\n PRETRAGA USPESNA! PRONADJEN U PRIMARNOM BLOKU!");
        print_record(&bpz.records[sr.index]);
    }
    else if(sr.overflow_zone != -1) {
        fseek(index_sequential, sr.overflow_zone, SEEK_SET);

        Block_overflow_zone boz;
        fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential);

        printf("\n PRETRAGA USPESNA! PRONADJEN U PREKORACIOCIMA!");
        print_record(&boz.records[0]);
    }
    else {
        printf("\n NIJE PRONADJENO");
    }
    fclose(index_sequential);
}

void reorganization_active_file() {
    FILE *index_sequential = fopen("index_sequential.bin", "rb");
    FILE *temp_serial = fopen("temp_serial.bin", "wb");
    int i, j;

    if(index_sequential == NULL || temp_serial == NULL) {
        printf("Doslo je do greske pri otvaranju datoteke!\n");
    }

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);

    Block_primary_zone bpz;
    for(i = 0; i < header.block_count_index_zone; i++) {
        fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);
        for(j = 0; j < BLOCK_FACTOR; j++) {
            if(bpz.records[j].status == 1) {
                fwrite(&bpz.records[j], sizeof(Record), 1, temp_serial);
            }
        }
    }

    fseek(index_sequential, header.overflow_zone_address, SEEK_SET);
    Block_overflow_zone boz;
    for(i = 0; i < BLOCK_FACTOR; i++) {
        fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
        if(boz.status == 1) {
            fwrite(&boz.records[0], sizeof(Record), 1, temp_serial);
        }
    }
    fclose(index_sequential);
    fclose(temp_serial);

    // kreiraj sekvecnijalnu u reorg modu
    create_sequential_file(1);
    // pokreni ponovo generisanje stabla i ostalog
    //system("PAUSE");
    create_index_sequential_file(1);
}

// pomocne funkcije
void get_record_from_user(Record *record) {
    // TODO validacija

    printf("\n Unesite identifikacioni broj(9 cifara): ");
    fflush(stdin);
    scanf("%d", &record->id);

    printf(" Unesite vrstu namestaja(najvise 70 karaktera): ");
    fflush(stdin);
    gets(record->furniture_type);

    printf(" Unesite datum proizvodnje namestaja(DDMMGGG): ");
    fflush(stdin);
    gets(record->production_date);

    printf(" Unesite vreme proizvodnje namestaja(HHMM): ");
    fflush(stdin);
    gets(record->production_time);

    printf(" Unesite naziv modela namestaja(do 50 karaktera): ");
    fflush(stdin);
    gets(record->model_name);

    printf(" Unesite tezinu modela namestaja(do 10000 kg): ");
    fflush(stdin);
    scanf("%d", &record->weight);

    record->status = 1;
}

void add_record_to_primary(Record record) {
    //FILE *index_sequential = fopen("index_sequential.bin", "rb+");
    int address_primary = find_primary_address_block_to_write(record.id);
    FILE *index_sequential = fopen("index_sequential.bin", "rb+");
    int i;

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);

    // Procitaj blok pronadjen u primarnoj zoni
    Block_primary_zone bpz;
    fseek(index_sequential, address_primary, SEEK_SET);
    fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);

    // izbroj popunjenost bloka
    int count = 0;
    for(i = 0; i < BLOCK_FACTOR; i++) {
        count += bpz.records[i].status;
        printf("%d ", count);
    }

    // ako je blok pun, onda automatski prebaci poslednji u prekoracioce
    if(count == BLOCK_FACTOR) {

        printf("Postavljam vrednosti bloka!\n");
        Block_overflow_zone boz;
        boz.address = bpz.address;
        boz.status = 1;
        memcpy(&boz.records[0], &bpz.records[BLOCK_FACTOR - 1], sizeof(Record));

        printf("Azuriram vrednost adrese primarnog bloka!\n");
        bpz.address = header.free_overflow_block;

        printf("Zapisujem boz bloka!\n");
        fseek(index_sequential, header.free_overflow_block, SEEK_SET);
        fwrite(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
        printf("Azuriram header!\n");
        header.free_overflow_block = ftell(index_sequential);
    }

    int index;
    for(i = 0; i < BLOCK_FACTOR; i++) {
        //printf("%d ", bpz.records[i].id);
        if(bpz.records[i].id == -1 || bpz.records[i].status == 0) {
            index = i;
        }
        else if(bpz.records[i].id > record.id) {
            index = i;
            break;
        }
    }

    int j;
    for(j = BLOCK_FACTOR - 2; j >= index; j--) {
        memcpy(&bpz.records[j+1], &bpz.records[j], sizeof(Record));
    }

    printf("\t %d", index);
    bpz.records[index].id = record.id;
    strcpy(bpz.records[index].furniture_type, record.furniture_type);
    strcpy(bpz.records[index].model_name, record.model_name);
    strcpy(bpz.records[index].production_date, record.production_date);
    strcpy(bpz.records[index].production_time, record.production_time);
    bpz.records[index].status = record.status;
    bpz.records[index].weight = record.weight;

    /*
    for(i = 0; i < BLOCK_FACTOR; i++) {
        printf("Id: %d", bpz.records[i].id);
    }
    */
    fseek(index_sequential, 0, SEEK_SET);
    fwrite(&header, sizeof(Header), 1, index_sequential);

    // sacuvaj modifikovani blok
    fseek(index_sequential, address_primary, SEEK_SET);
    fwrite(&bpz, sizeof(Block_primary_zone), 1, index_sequential);


    fclose(index_sequential);
}

// pretraga prolaskom kroz primarnu zonu
Search_result search_primary_overflow(int id) {
    // pretraga
    FILE *index_sequential = fopen("index_sequential.bin", "rb");
    Block_index_zone biz;
    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);


    fseek(index_sequential, (header.index_zone_address + sizeof(Block_index_zone)* (header.block_count_index_zone - 1)), SEEK_SET);
    fread(&biz, sizeof(Block_index_zone), 1, index_sequential);

    int i = 2; // visina stabla, modifikovati kasnije
    int next_address;

    //printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
    for(; i > 0; i--) {
        if(id <= biz.keys[0]) {
            next_address = biz.address[0];
        }
        else {
            next_address = biz.address[1];
        }
        //printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
        fseek(index_sequential, next_address, SEEK_SET);
        fread(&biz, sizeof(Block_index_zone), 1, index_sequential);
        //printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
    }
    Block_primary_zone bpz;
    int address_primary;
    if(id <= biz.keys[0]) {
        address_primary = biz.address[0];
    }
    else {
        address_primary = biz.address[1];
    }
    fseek(index_sequential, address_primary, SEEK_SET);
    fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);

    for(i = 0; i < BLOCK_FACTOR; i++) {
        //print_record(&bpz.records[i]);
        if(id == bpz.records[i].id) {
            fclose(index_sequential);
            Search_result sr;
            sr.index = i;
            sr.overflow_zone = 0;
            sr.primary_zone_address = address_primary;
            return sr;
        }
    }

    if(bpz.address != -1) {
        Block_overflow_zone boz;
        boz.address = bpz.address;

        printf("Idemo u pretragu prekoracioca! \n");
        do {
            int last = boz.address;
            fseek(index_sequential, boz.address, SEEK_SET);
            fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
            printf(" %d %d", boz.records[0].id, id);

            if(boz.records[0].id == id) {
                printf("Gledam da li su id jednaki! \n");
                Search_result sr;
                sr.index = -1;
                sr.overflow_zone = last;
                sr.primary_zone_address = address_primary;
                return sr;
            }
        } while(boz.address != -1);
        printf("Dosao sam do poslednjeg, nema nista! \n");
    }

    fclose(index_sequential);
    Search_result sr;
    sr.index = -1;
    sr.overflow_zone = -1;
    sr.primary_zone_address = -1;

    return sr;
}

int find_primary_address_block_to_write(int id) {
    // pretraga
    FILE *index_sequential = fopen("index_sequential.bin", "rb");
    Block_index_zone biz;
    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);


    fseek(index_sequential, (header.index_zone_address + sizeof(Block_index_zone)* (header.block_count_index_zone - 1)), SEEK_SET);
    fread(&biz, sizeof(Block_index_zone), 1, index_sequential);

    int i = 2; // visina stabla, modifikovati kasnije
    int next_address;

    printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
    for(; i > 0; i--) {
        if(id <= biz.keys[0]) {
            next_address = biz.address[0];
        }
        else {
            next_address = biz.address[1];
        }
        //printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
        fseek(index_sequential, next_address, SEEK_SET);
        fread(&biz, sizeof(Block_index_zone), 1, index_sequential);
        printf("Kljucevi: %d %d\n", biz.keys[0], biz.keys[1]);
    }

    int address_primary;
    if(id <= biz.keys[0]) {
        address_primary = biz.address[0];
    }
    else {
        address_primary = biz.address[1];
    }
    //fseek(index_sequential, next_address, SEEK_SET);
    //fread(&biz, sizeof(Block_index_zone), 1, index_sequential);
    fclose(index_sequential);

    return address_primary;
}

Header init_header() {
    Header header;
    header.block_count_primary_zone = 0;
    header.block_count_index_zone = 0;
    header.primary_zone_adress = 0;
    header.index_zone_address = 0;
    header.overflow_zone_address = 0;
    header.free_overflow_block = 0;
    return header;
}
