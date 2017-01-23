#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define FILE_NAME_LIMIT 100
#define ADDRESS_SIZE 10
#define BLOCK_FACTOR 3
#define BLOCK_FACTOR_INDEX_ZONE 2

#define SERIAL 1
#define SEQUENTIAL 2
#define PRIMARY_ZONE 3


// record types
typedef struct {
    unsigned long id;
    char furniture_type[70];
    char manufacture_date[8];
    char manufacture_time[6];
    char model_name[50];
    char weight[10000];
    char status;
} Record;

typedef struct {
    unsigned long id;
    unsigned long address;
} Record_index_zone;

typedef struct {
    unsigned long address;
} Record_overflow_zone;

typedef struct {
    Record record;
    struct Record_list *next;
} Record_list;

// block types
typedef struct {
    Record records[BLOCK_FACTOR];
} Block;

typedef struct {
    Record records[BLOCK_FACTOR];
} Block_primary_zone;

typedef struct {
    Record_index_zone records[BLOCK_FACTOR_INDEX_ZONE];
} Block_index_zone;

typedef struct {
    char adress[ADDRESS_SIZE];
    Record record;
    char next[ADDRESS_SIZE];
} Block_overflow_zone;

typedef struct {
    unsigned long left_id;
    unsigned long right_id;
    struct Linked_tree_node *left;
    struct Linked_tree_node *right;
} Linked_tree_node;

#endif // DEFS_H_INCLUDED
