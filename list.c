#include "defs.h"

void add_record(Record_list** head, Record* record) {
    Record_list *prev, *temp, *data = (Record_list*) malloc(sizeof(Record_list));
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

void delete_list(Record_list** head) {
    Record_list *temp = *head;
    Record_list *last;

    while (temp) {
        last = temp;
        temp = temp->next;
        free(last);
    }

    *head = NULL;
}