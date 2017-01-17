#include "defs.h"
#include "list.h"
#include <stdlib.h>

void add_record(record_list** head, Record* record) {

    record_list *prev, *temp;
    record_list *data = (record_list*) malloc(sizeof(record_list));
    memcpy(&data->record, record, sizeof(Record));

    if ((*head) == NULL || (record->id < (*head)->record.id)) {
        data->next = *head;
        *head = data;
    }
    else {
        prev = *head;
        temp = (*head)->next;
        while(1) {
            if((temp == NULL) || (temp->record.id > record->id)) {
                prev->next = data;
                data->next = temp;
                break;
            }
            else {
                prev = temp;
                temp = temp->next;
            }
        }
    }
}

void delete_list(record_list** glava) {
    record_list *temp = *glava;
    record_list *last;

    while (temp) {
        last = temp;
        temp = temp->next;
        free(last);
    }

    *glava = NULL;
}
