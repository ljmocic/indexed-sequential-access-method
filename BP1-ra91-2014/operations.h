#ifndef OPERATIONS_H_INCLUDED
#define OPERATIONS_H_INCLUDED
#include "defs.h"

void create_empty_file();
void choose_active_file();
void show_active_file_name();
void create_serial_file();
void create_sequential_file();
void create_active_file();
void write_to_active_file();
void search_active_file();
void reorganization_active_file();

void printRecord(Record*);
void print_file(char[]);

#endif // OPERATIONS_H_INCLUDED
