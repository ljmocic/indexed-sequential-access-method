#include "validation.h"
#include "defs.h"

int validate_id_serial(int id) {
    if(id > 0 || id <= MAX_KEY_VALUE) {
        Search_result sr = search_serial(id);
        if(sr.found == 0) {
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
}

int validate_id_index_sequential(int id) {
    if(id > 0 || id <= MAX_KEY_VALUE) {
        Search_result sr = search_primary_overflow(id);
        if(sr.found == 0) {
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
}

int validate_furniture_type(char *type) {
    if(strlen(type) < FURNITURE_TYPE_LIMIT) {
        return 1;
    }
    else {
        return 0;
    }
}

int validate_date(char *date) {
    if(strlen(date) == PRODUCTION_DATE_LIMIT) {
        return 1;
    }
    else {
        return 0;
    }
}

int validate_time(char *time) {
    if(strlen(time) == PRODUCTION_TIME_LIMIT) {
        return 1;
    }
    else {
        return 0;
    }
}

int validate_model_name(char *name) {
    if(strlen(name) != 0 || strlen(name) < MODEL_NAME_LIMIT) {
        return 1;
    }
    else {
        return 0;
    }
}

int validate_weight(int weight) {
    if(weight > 0 && weight < WEIGHT_LIMIT) {
        return 1;
    }
    return 0;
}
