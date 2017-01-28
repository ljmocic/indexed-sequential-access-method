#include "defs.h"

#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

// lista
void add_record(Record_list** head, Record* record);
void delete_list(Record_list** head);

#endif // LIST_H_INCLUDED
