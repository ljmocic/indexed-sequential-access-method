#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define FILE_NAME_LIMIT 100
#define BLOCK_FACTOR 5

typedef struct {
    unsigned long id;
    char furniture_type[70];
    char manufacture_date[8];
    char manufacture_time[6];
    char model_name[50];
    char weight[10000];
} Record;

typedef struct {
    Record records[BLOCK_FACTOR];
} Block;

typedef struct record_list {
    Record record;
    struct record_list *next;
} record_list;


#endif // DEFS_H_INCLUDED
