#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define FILE_NAME_LIMIT 100
#define BLOCK_FACTOR 5

typedef struct {
    char id[9];
    char furniture_type[70];
    char manufacture_date[8];
    char manufacture_time[6];
    char model_name[50];
    char weight[10000];
} Furniture;

typedef struct {
    Furniture furniture[BLOCK_FACTOR];
} Block;

#endif // DEFS_H_INCLUDED
