#include "defs.h"
#include "output.h"
#include "list.h"
#include "validation.h"

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
        printf(" 15. Prikazi temp_sequential za reorganizaciju \n");
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
                create_sequential_file(STD_MODE);
                break;
            case 6:
                create_index_sequential_file(STD_MODE);
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
                break;
            case 15:
                printf("\n Prikaz sekvencijalne za reorganizaciju: \n");
                print_temp_sequential();
                break;
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
        return;
    }

    fclose(file);
}

void choose_active_file() {
    FILE *active_file;
    char file_name[FILE_NAME_LIMIT];

    printf("\n Unesite naziv zeljene aktivne datoteke: ");
    fflush(stdin);
    gets(file_name);

    if(!validate_file_name(file_name)) {
        printf("\n Ime datoteke nije validno! \n");
        return;
    }

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
    fclose(serial);

    for(i = 0; i < BLOCK_FACTOR; i++) {
        Record *record = (Record *)malloc(sizeof(Record));
        printf("\n Slog %d:", i + 1);
        get_record_from_user_serial(record);
        serial = fopen("serial.bin", "ab+");
        fwrite(record, sizeof(Record), 1, serial);
        fclose(serial);
    }

    fclose(serial);
    printf("\n Serijska datoteka uspesno kreirana/azurirana! \n");
}

void create_sequential_file(int reorganization) {
    FILE *serial, *sequential;
    Block block;
    Record record;
    Record_list *head = NULL, *iterator;
    int i = 0, j;

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
            // printf("blok: %d %d %d %d %d", block.records[0].id, block.records[1].id, block.records[2].id, block.records[3].id, block.records[4].id);
            i = 0;
        }
        iterator = iterator->next;
    } while(iterator != NULL);

    // zapisi poslednji blok u slucaju reorganizacije
    for(j = 0; j < i; j++) {
        fwrite(&block.records[j], sizeof(Record), 1, sequential);
    }

    delete_list(&head);
    free(iterator);
    fclose(serial);
    fclose(sequential);

    printf("\n Sekvencijalna datoteka uspesno kreirana! \n");
}

void create_index_sequential_file(int reorganization) {
    FILE *sequential, *index_sequential;
    int i, j;

    if(reorganization) {
        sequential = fopen("temp_sequential.bin", "rb");
    }
    else {
        sequential = fopen("sequential.bin", "rb");
    }

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka! \n");
        return;
    }

    index_sequential = fopen(active_file_name, "wb");

    if(sequential == NULL || index_sequential == NULL) {
        printf("\n Doslo je do greske pri otvaranju datoteke!\n");
        return;
    }

    // pronadji broj blokova sekvencijane
    fseek(sequential, 0, SEEK_END);
    int number_of_records = ftell(sequential) / sizeof(Record);
    rewind(sequential);

    // broj inicijalno dozvoljenih slogova u bloku primarne zone
    int num_allowed_rec_bpz = (int) (IBF * BLOCK_FACTOR);
    int number_of_blocks = number_of_records / num_allowed_rec_bpz;

    // odvoji blok za preostale slogove
    if(number_of_records % num_allowed_rec_bpz > 0) {
        number_of_blocks++;
    }

    // zapisi header na pocetak datoteke
    Header header = init_header();
    header.block_count_primary_zone = number_of_blocks;
    fwrite(&header, sizeof(Header), 1, index_sequential);

    // sacuvaj broj blokova u primarnoj zoni, trebace ti kasnije

    // niz blokova primarne zone
    Block_primary_zone bpz[number_of_blocks + 1];

    // inicijalizujemo blokove primarne zone
    init_primary_zone(bpz, number_of_blocks);

    // maksimalne vrednosti kljuca i njihove adrese i dodatni brojaci
    int bpz_max[number_of_blocks];
    int bpz_address[number_of_blocks];
    int bpz_counter = 0, record_counter = 0;

    Record record;
    while(fread(&record, sizeof(Record), 1, sequential)) {

        if(record_counter < num_allowed_rec_bpz) {
            // ako blok nije popunjen do dozvoljenog, samo upisi slog
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
        // print_record(&record);
    }

    if(number_of_blocks % num_allowed_rec_bpz != 0) {
        // zapisi maksimalni kljuc od poslenjeg upisanog bloka primarne zone
        bpz_max[bpz_counter] = bpz[bpz_counter].records[number_of_blocks % num_allowed_rec_bpz].id;
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

    // generisem kljuceve viseg nivoa
    for(i = 0; i < number_of_blocks / 2; i++){
        biz[i].keys[0] = bpz_max[j];
        biz[i].address[0] = bpz_address[j++];
        biz[i].keys[1] = bpz_max[j];
        biz[i].address[1] = bpz_address[j++];
        addresses[i] = ftell(index_sequential);
        // printf("\nKljuc 1: %d ", biz[i].keys[0]);
        // printf("\nKljuc 2: %d ", biz[i].keys[1]);
        fwrite(&biz[i], sizeof(Block_index_zone), 1, index_sequential);
        header.block_count_index_zone++;
    }
    if(number_of_blocks % 2 == 1) {
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

    Block_index_zone *preth;
    Block_index_zone *novi;
    int *adr_preth;
    int *adr_novi;

    // gornji nivo nodes_count[h-2]
    // srednji nivo nodes_count[h-1]
    // donji nivo nodes_count[h]

    // prodji kroz stablo i generisi cvorove
    for (i = 1; i < h; i++) {

        // ako je prvi prolaz, mora da generise iz prethodnih nizova u kojima su listovi
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
            printf("\nkljucevi %d %d \t", preth[j].keys[0], preth[j].keys[1]);
        }
        printf("\n Nivo stabla: %d", h - i);
        */

        int p = 0;
        // postavi kljuceve od donjih levih, i uzmi adrese
        for(j = 0; j < nodes_count[h - i] - 1; j++) {
            novi[j].keys[0] = preth[p].keys[1];
            novi[j].address[0] = adr_preth[p++];
            novi[j].keys[1] = preth[p].keys[1];
            novi[j].address[1] = adr_preth[p++];
            adr_novi[j] = ftell(index_sequential);
            fwrite(&novi[j], sizeof(Block_index_zone), 1, index_sequential);
            header.block_count_index_zone++;
            // printf("\nPROBAJ PARNI: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        // ako je u prethodnom levi bio -1, i nije gornji nivo stabla, onda upisi maksimalan kljuc
        if(preth[nodes_count[h - i + 1] - 1].keys[1] == -1 && h - i != 1) {
            novi[j].keys[0] = 999999999;
            novi[j].address[0] = adr_preth[p++];
            novi[j].keys[1] = -1;
            novi[j].address[1] = -1;
            // printf("\nPROBAJ GRANICNI: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        // ako je gornji nivo stabla i desni je -1, automatski upisi maks u gornji levi // proveriti jos da li je uopste potrebno
        if(h - i == 1 && preth[nodes_count[h - i + 1] - 1].keys[1] == -1) {
            novi[j].keys[0] = preth[p].keys[1];
            novi[j].address[0] = adr_preth[p++];
            novi[j].keys[1] = preth[p].keys[0];
            novi[j].address[1] = adr_preth[p++];
            // printf("\nPROBAJ2: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }
        else {
            // slucajevi za poslednji nivo stabla
            if(h - i == 1 && preth[nodes_count[h - i + 1] - 1].keys[1] == -1) {
                novi[j].keys[0] = preth[p].keys[1];
                novi[j].address[0] = adr_preth[p++];
                novi[j].keys[1] = 999999999;
                novi[j].address[1] = adr_preth[p++];
            }
            else if(h - i == 1){
                novi[j].keys[0] = preth[p].keys[1];
                novi[j].address[0] = adr_preth[p++];
                novi[j].keys[1] = preth[p].keys[1];
                novi[j].address[1] = adr_preth[p++];
            }
            // printf("\nPROBAJ3 GLAVNI: %d %d\n", novi[j].keys[0], novi[j].keys[1]);
        }

        adr_novi[j] = ftell(index_sequential);
        fwrite(&novi[j], sizeof(Block_index_zone), 1, index_sequential);
        header.block_count_index_zone++;

        // vrati se na kraj datoteke
        fseek(index_sequential, 0, SEEK_SET);
        fwrite(&header, sizeof(Header), 1, index_sequential);
        fseek(index_sequential, 0, SEEK_END);
    }

    // ZONA PREKORACENJA

    // postavi adrese u header
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

    free(preth);
    free(novi);
    free(biz);
    free(addresses);
    fclose(sequential);
    fclose(index_sequential);

    // print_index_zone();
    // print_overflow_zone();

    printf("\n Indeks sekvencijalna datoteka uspesno kreirana! \n");
}

void write_to_active_file() {
    Record record;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        return;
    }

    get_record_from_user_index_sequential(&record);
    // print_record(&record);
    add_record_to_active(record);
}

void search_active_file() {
    int id;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        return;
    }

    FILE *index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("\n Aktivni fajl nije odabran!");
        return;
    }

    printf("\n Unesite ID: ");
    fflush(stdin);
    scanf("%d", &id);

    Search_result sr = search_primary_overflow(id);

    if(sr.index != -1) {
        printf("\n PRETRAGA USPESNA! PRONADJEN U PRIMARNOM BLOKU!");
        printf("\n Adresa bloka p.zone: %d", sr.primary_zone_address);
        printf("\n Redni broj sloga u bloku: %d", sr.index + 1);
        print_record(&sr.record);
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
    FILE *index_sequential, *temp_serial;
    int i, j;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        return;
    }

    index_sequential = fopen(active_file_name, "rb");
    temp_serial = fopen("temp_serial.bin", "wb");

    if(index_sequential == NULL || temp_serial == NULL) {
        printf("Doslo je do greske pri otvaranju datoteke!\n");
    }

    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);

    Block_primary_zone bpz;
    for(i = 0; i < header.block_count_index_zone + 1; i++) {
        fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);
        for(j = 0; j < BLOCK_FACTOR; j++) {
            if(bpz.records[j].status == 1) {
                fwrite(&bpz.records[j], sizeof(Record), 1, temp_serial);
                print_record(&bpz.records[j]);
            }
        }
    }

    fseek(index_sequential, header.overflow_zone_address, SEEK_SET);
    Block_overflow_zone boz;

    int count_index_zone = (header.free_overflow_block - header.overflow_zone_address) / sizeof(Block_overflow_zone);

    for(i = 0; i < count_index_zone; i++) {
        fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
        if(boz.status == 1 && boz.records[0].id != -1) {
            fwrite(&boz.records[0], sizeof(Record), 1, temp_serial);
            print_record(&boz.records[0]);
        }
    }
    fclose(index_sequential);
    fclose(temp_serial);

    // kreiraj sekvecnijalnu u reorg modu
    create_sequential_file(ORG_MODE);
    // pokreni ponovo generisanje stabla i ostalog
    // system("PAUSE");
    create_index_sequential_file(ORG_MODE);
}

// pomocne funkcije
void get_record_from_user_serial(Record *record) {
    // TODO validacija
    char temp_string[FILENAME_MAX + 1];
    int validation = 0;

    do {
        printf("\n Unesite identifikacioni broj(9 cifara): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_id_serial(atoi(temp_string));
        if(validation) {
            record->id = atoi(temp_string);
        }
    } while(validation == 0);

    do {
        printf(" Unesite vrstu namestaja(najvise 70 karaktera): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_furniture_type(temp_string);
        if(validation) {
            strcpy(record->furniture_type, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite datum proizvodnje namestaja(DDMMGGGG): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_date(temp_string);
        if(validation) {
            strcpy(record->production_date, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite vreme proizvodnje namestaja(HHMM): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_time(temp_string);
        if(validation) {
            strcpy(record->production_time, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite naziv modela namestaja(do 50 karaktera): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_model_name(temp_string);
        if(validation) {
            strcpy(record->model_name, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite tezinu modela namestaja(do 10000 kg): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_weight(atoi(temp_string));
        if(validation) {
            record->weight = atoi(temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    record->status = 1;
}

void get_record_from_user_index_sequential(Record *record) {
    char temp_string[FILENAME_MAX + 1];
    int validation = 0;

    do {
        printf("\n Unesite identifikacioni broj(9 cifara): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_id_index_sequential(atoi(temp_string));
        if(validation) {
            record->id = atoi(temp_string);
        }
    } while(validation == 0);

    do {
        printf(" Unesite vrstu namestaja(najvise 70 karaktera): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_furniture_type(temp_string);
        if(validation) {
            strcpy(record->furniture_type, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite datum proizvodnje namestaja(DDMMGGGG): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_date(temp_string);
        if(validation) {
            strcpy(record->production_date, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite vreme proizvodnje namestaja(HHMM): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_time(temp_string);
        if(validation) {
            strcpy(record->production_time, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite naziv modela namestaja(do 50 karaktera): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_model_name(temp_string);
        if(validation) {
            strcpy(record->model_name, temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    do {
        printf(" Unesite tezinu modela namestaja(do 10000 kg): ");
        fflush(stdin);
        gets(temp_string);
        validation = validate_weight(atoi(temp_string));
        if(validation) {
            record->weight = atoi(temp_string);
        }
        else {
            printf(" Podatak nije validan! \n");
        }
    } while(validation == 0);

    record->status = 1;
}

void add_record_to_active(Record record) {
    int i;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        return;
    }

    int address_primary = find_primary_address_block_to_write(record.id);
    FILE *index_sequential = fopen(active_file_name, "rb+");

    if(index_sequential == NULL) {
        printf("Doslo je do greske pri ucitavanju datoteke!\n");
        return;
    }

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
        //printf("%d ", count);
    }


    if(count == BLOCK_FACTOR && bpz.address == -1 && bpz.records[BLOCK_FACTOR - 1].id < record.id && header.primary_zone_adress + (header.block_count_primary_zone - 1) * sizeof(Block_primary_zone) == address_primary){
        // samo dodaj u prvi prekoracilac
        // samo za poslednji blok
        Block_overflow_zone boz;
        boz.address = bpz.address;
        boz.status = 1;
        memcpy(&boz.records[0], &record, sizeof(Record));

        bpz.address = header.free_overflow_block;

        fseek(index_sequential, header.free_overflow_block, SEEK_SET);
        fwrite(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
        // printf("Azuriram header!\n");
        header.free_overflow_block = ftell(index_sequential);

        fseek(index_sequential, 0, SEEK_SET);
        fwrite(&header, sizeof(Header), 1, index_sequential);

        // sacuvaj modifikovani blok
        fseek(index_sequential, address_primary, SEEK_SET);
        fwrite(&bpz, sizeof(Block_primary_zone), 1, index_sequential);

        fclose(index_sequential);
        return;
    }
    if(count == BLOCK_FACTOR) {

        // TODO ukloniti bug vezan za dodavanje prvog u prekoracioce ukoliko je 12 23 34 41 51 | 61 -> 12 23 34 41 51 | 59 61
        Block_overflow_zone boz_temp;
        fseek(index_sequential, bpz.address, SEEK_SET);
        fread(&boz_temp, sizeof(Block_overflow_zone), 1, index_sequential);
        if(bpz.records[BLOCK_FACTOR - 1].id < record.id && boz_temp.records[0].id > record.id ) {
            Block_overflow_zone boz;
            boz.address = bpz.address;
            boz.status = 1;
            memcpy(&boz.records[0], &record, sizeof(Record));



            // printf("Azuriram vrednost adrese primarnog bloka!\n");
            bpz.address = header.free_overflow_block;

            // printf("Zapisujem boz bloka!\n");
            if(header.free_overflow_block >= header.index_zone_address + 5 * sizeof(Block_overflow_zone)){
                printf("\n Blok zone prekoracenja popunjen do kraja!\n");
                return;
            }
            else {
                fseek(index_sequential, header.free_overflow_block, SEEK_SET);
                fwrite(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
                // printf("Azuriram header!\n");
                header.free_overflow_block = ftell(index_sequential);
            }
            fseek(index_sequential, 0, SEEK_SET);
            fwrite(&header, sizeof(Header), 1, index_sequential);

            // sacuvaj modifikovani blok
            fseek(index_sequential, address_primary, SEEK_SET);
            fwrite(&bpz, sizeof(Block_primary_zone), 1, index_sequential);
            fclose(index_sequential);
            return;

        }
        else {
            // printf("Postavljam vrednosti bloka!\n");
            Block_overflow_zone boz;
            boz.address = bpz.address;
            boz.status = 1;
            memcpy(&boz.records[0], &bpz.records[BLOCK_FACTOR - 1], sizeof(Record));

            // printf("Azuriram vrednost adrese primarnog bloka!\n");
            bpz.address = header.free_overflow_block;

            // printf("Zapisujem boz bloka!\n");
            if(header.free_overflow_block >= header.index_zone_address + 5 * sizeof(Block_overflow_zone)){
                printf("\n Blok zone prekoracenja popunjen do kraja!\n");
                return;
            }
            else {
                fseek(index_sequential, header.free_overflow_block, SEEK_SET);
                fwrite(&boz, sizeof(Block_overflow_zone), 1, index_sequential);
                // printf("Azuriram header!\n");
                header.free_overflow_block = ftell(index_sequential);
            }
        }
    }

    int index;
    for(i = 0; i < BLOCK_FACTOR; i++) {
        //printf("%d ", bpz.records[i].id);
        if(bpz.records[i].id == -1 || bpz.records[i].status == 0) {
            index = i;
            // printf("Nasao index: %d \t", index);
            break;
        }
        else if(bpz.records[i].id > record.id) {
            index = i;
            // printf("Nasao index: %d \t", index);
            break;
        }
    }

    int j;
    for(j = BLOCK_FACTOR - 2; j >= index; j--) {
        memcpy(&bpz.records[j+1], &bpz.records[j], sizeof(Record));
    }

    bpz.records[index].id = record.id;
    strcpy(bpz.records[index].furniture_type, record.furniture_type);
    strcpy(bpz.records[index].model_name, record.model_name);
    strcpy(bpz.records[index].production_date, record.production_date);
    strcpy(bpz.records[index].production_time, record.production_time);
    bpz.records[index].status = record.status;
    bpz.records[index].weight = record.weight;

    fseek(index_sequential, 0, SEEK_SET);
    fwrite(&header, sizeof(Header), 1, index_sequential);

    // sacuvaj modifikovani blok
    fseek(index_sequential, address_primary, SEEK_SET);
    fwrite(&bpz, sizeof(Block_primary_zone), 1, index_sequential);


    fclose(index_sequential);
}

Search_result search_primary_overflow(int id) {
    // pretraga

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        Search_result sr;
        sr.found = 0;
        return sr;
    }

    FILE *index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("Doslo je do greske pri ucitavanju datoteke!\n");
    }

    Block_index_zone biz;
    Header header;
    fread(&header, sizeof(Header), 1, index_sequential);

    // procitaj pretposlednji blok indeks zone koji je zapravo root stabla
    fseek(index_sequential, (header.index_zone_address + sizeof(Block_index_zone)* (header.block_count_index_zone - 1)), SEEK_SET);
    fread(&biz, sizeof(Block_index_zone), 1, index_sequential);

    int i = (int)ceil(log2(header.block_count_primary_zone)) - 1;
    int next_address;

    // idi kroz stablo i nadji list u kom se nalazi adresa primarnog bloka
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
    // odaberi adresu u listu
    if(id <= biz.keys[0]) {
        address_primary = biz.address[0];
    }
    else {
        address_primary = biz.address[1];
    }
    // ucitaj blok primarne zone
    fseek(index_sequential, address_primary, SEEK_SET);
    fread(&bpz, sizeof(Block_primary_zone), 1, index_sequential);

    // iteriraj kroz blok da nadjes slog sa kljucem
    for(i = 0; i < BLOCK_FACTOR; i++) {
        //print_record(&bpz.records[i]);
        if(id == bpz.records[i].id) {
            fclose(index_sequential);
            Search_result sr;
            sr.index = i;
            sr.overflow_zone = 0;
            sr.found = 1;
            memcpy(&sr.record, &bpz.records[i], sizeof(Record));
            sr.primary_zone_address = address_primary;
            return sr;
        }
    }

    // ako ne nadjes, prodji kroz "listu" prekoracenja
    if(bpz.address != -1) {
        Block_overflow_zone boz;
        boz.address = bpz.address;

        //printf("Idemo u pretragu prekoracioca! \n");
        do {
            int last = boz.address;
            fseek(index_sequential, boz.address, SEEK_SET);

            // ako si dosao do kraja, izadji
            if(!fread(&boz, sizeof(Block_overflow_zone), 1, index_sequential)) {
                break;
            }

            // ako su id-evi jednaki onda vrati kao rezultat
            if(boz.records[0].id == id) {
                Search_result sr;
                sr.index = -1;
                sr.overflow_zone = last;
                sr.primary_zone_address = address_primary;
                sr.found = 1;
                return sr;
            }
        } while(boz.address != -1);
    }

    // zatvori datoteke i reci da nisi nasao taj id
    fclose(index_sequential);
    Search_result sr;
    sr.index = -1;
    sr.overflow_zone = -1;
    sr.primary_zone_address = -1;
    sr.found = 0;
    return sr;
}

Search_result search_serial(int id) {
    FILE *serial;
    Record record;
    Search_result sr;

    serial = fopen("serial.bin", "rb");
    if(!serial) {
        printf("\n Doslo je do greske pri otvaranju datoteke! \n");
    }

    // prodji kroz serijsku i pokusaj naci taj id, ako ga nema, onda vrati 0 u Search_result
    while(fread(&record, sizeof(Record), 1, serial)){
        if(record.id == id) {
            sr.found = 1;
            fclose(serial);
            return sr;
        }
    }
    sr.found = 0;

    fclose(serial);
    return sr;
}

int find_primary_address_block_to_write(int id) {
    FILE *index_sequential;
    Block_index_zone biz;
    Header header;

    if(!validate_file_name(active_file_name)) {
        printf("\n Nije odabrana aktivna datoteka!");
        return 0;
    }

    index_sequential = fopen(active_file_name, "rb");
    if(index_sequential == NULL) {
        printf("Doslo je do greske pri ucitavanju datoteke!\n");
        return 0;
    }

    // ucitaj root blok indeks zone
    fread(&header, sizeof(Header), 1, index_sequential);
    fseek(index_sequential, (header.index_zone_address + sizeof(Block_index_zone)* (header.block_count_index_zone - 1)), SEEK_SET);
    fread(&biz, sizeof(Block_index_zone), 1, index_sequential);

    int i = (int)ceil(log2(header.block_count_primary_zone)) - 1;
    int next_address;

    // prolaskom kroz "stablo" nadji adresu primarnog bloka kojoj id pripada
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
    }

    int address_primary;
    if(id <= biz.keys[0]) {
        address_primary = biz.address[0];
    }
    else {
        address_primary = biz.address[1];
    }
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

void init_primary_zone(Block_primary_zone *bpz, int number_of_blocks) {
    int i, j;
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
}
