#include "defs.h"

void add_record(Record_list** head, Record* record) {
    Record_list *prev, *temp, *data = (Record_list*) malloc(sizeof(Record_list));
    memcpy(&data->record, record, sizeof(Record));

    // ako je lista prazna ili je id na pocetku liste veci od id record-a, onda dodaj na pocetak i azuriraj head
    if ((*head) == NULL || (record->id < (*head)->record.id)) {
        data->next = *head;
        *head = data;
    }
    else {
        prev = *head;
        temp = (*head)->next;
        while(1) {
            // ako je pokazivac na sledeci ne pokazuje ni na koga ili je id temp-a veci od record.id, ubaci
            if((temp == NULL) || (temp->record.id > record->id)) {
                // prevezivanje pokazivaca
                prev->next = data;
                data->next = temp;
                break;
            }
            else {
                // iteriraj dalje
                prev = temp;
                temp = temp->next;
            }
        }
    }
}

void delete_list(Record_list** head) {
    Record_list *temp = *head;
    Record_list *last;

    // prodji kroz listu i oslobodi memoriju
    while (temp) {
        last = temp;
        temp = temp->next;
        free(last);
    }

    *head = NULL;
}
