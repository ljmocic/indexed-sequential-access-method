#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

// orgranicenja sloga
#define FILE_NAME_LIMIT 200
#define FURNITURE_TYPE_LIMIT 70
#define PRODUCTION_DATE_LIMIT 8
#define PRODUCTION_TIME_LIMIT 4
#define MODEL_NAME_LIMIT 50
#define WEIGHT_LIMIT 10000

// ostala ogranicenja
#define BLOCK_FACTOR 5
#define BLOCK_FACTOR_INDEX_ZONE 2
#define BLOCK_FACTOR_OVERFLOW_ZONE 1
#define IBF 0.6
#define MAX_KEY_VALUE 999999999

//
#define STD_MODE 0
#define ORG_MODE 1

// naziv aktivne datoteke
char active_file_name[FILE_NAME_LIMIT];

// modeli
typedef struct Record {
    int id;
    char furniture_type[FURNITURE_TYPE_LIMIT + 1];
    char production_date[PRODUCTION_DATE_LIMIT + 1];
    char production_time[PRODUCTION_TIME_LIMIT + 1];
    char model_name[MODEL_NAME_LIMIT + 1];
    int weight;
    int status;
} Record;

typedef struct Record_node {
    Record record;
    struct Record_node *next;
} Record_list;

typedef struct Block {
    Record records[BLOCK_FACTOR];
} Block;

typedef struct Block_primary_zone {
    Record records[BLOCK_FACTOR];
    int address;
} Block_primary_zone;

typedef struct Block_index_zone {
    int keys[BLOCK_FACTOR_INDEX_ZONE];
    int address[BLOCK_FACTOR_INDEX_ZONE];
} Block_index_zone;

typedef struct Block_overflow_zone {
    Record records[BLOCK_FACTOR_OVERFLOW_ZONE];
    int address;
    int status;
} Block_overflow_zone;

typedef struct Header {
    int primary_zone_adress;
    int index_zone_address;
    int overflow_zone_address;
    int block_count_primary_zone;
    int block_count_index_zone;
    int free_overflow_block;
} Header;

typedef struct Search_result {
    int found;
    int primary_zone_address;
    int overflow_zone;
    int index;
    Record record;
} Search_result;

// deklaracije koriscenih funkcija
void create_empty_file();
void choose_active_file();
void show_active_file_name();
void create_serial_file();
void create_sequential_file();
void create_index_sequential_file();
void write_to_active_file();
void search_active_file();
void reorganization_active_file();

// pomocne fije
void get_record_from_user_serial(Record*);
void get_record_from_user_index_sequential(Record*);
void add_record_to_active(Record);
Search_result search_primary_overflow(int);
Search_result search_serial(int);
int find_primary_address_block_to_write(int id);
Header init_header();
void init_primary_zone(Block_primary_zone *, int);

#endif // DEFS_H_INCLUDED
