#include <stdlib.h>
#include "defs.h"
#include "list.h"

void add_record(Record_list** head, Record* record) {

    Record_list *prev, *temp;
    Record_list *data = (Record_list*) malloc(sizeof(Record_list));
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

void delete_list(Record_list** glava) {
    Record_list *temp = *glava;
    Record_list *last;

    while (temp) {
        last = temp;
        temp = temp->next;
        free(last);
    }

    *glava = NULL;
}
