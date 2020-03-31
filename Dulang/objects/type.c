//
//  type.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 25.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "type.h"
#include <string.h>

#define INIT_CLASS_SIZE 15

int max_typeid = 3;


dul_layout_type* create_layout_type(void){
    dul_layout_type *new_type = (dul_layout_type*)malloc(sizeof(dul_layout_type));
    new_type->basic_name = "layout";
    new_type->layout_size = 0;
    new_type->layout_cap = INIT_CLASS_SIZE;
    new_type->size = 0;
    new_type->typeid = max_typeid++;
    new_type->layout = (struct field*)malloc(INIT_CLASS_SIZE * sizeof(struct field));
    return new_type;
}
dul_param_type* create_param_type(dul_type * param_type);
void add_entry_to_layout_type(dul_layout_type * t, struct field entry){
    if(t->layout_cap == ++t->layout_size){
        t->layout_cap*=2;
        t->layout = (struct field*)realloc(t->layout, t->layout_cap* sizeof(struct field));
    }
    t->layout[t->layout_size - 1] = entry;
    t->size += entry.field_t->size;
}

int compare_types(dul_type*, dul_type*);
const char * get_var_at_layout(dul_layout_type*layout, size_t offset){
    size_t offset_accumulated = 0;
    int idx = 0;
    while (offset_accumulated != offset && offset_accumulated < layout->size){
        offset_accumulated += layout->layout[idx].field_t->size;
        idx++;
    }
    if(offset_accumulated < layout->size)
        return layout->layout[idx].name;
    return 0;
}

int contains_entry(dul_layout_type * layout, struct field entry){
    for(int i = 0; i < layout->layout_size; i++){
        if(strcmp(layout->layout[i].name, entry.name) == 0){
            //TODO: actual type comparison
            if(layout->layout[i].field_t == entry.field_t || layout->layout[i].field_t == NULL || entry.field_t == NULL){
                return LAYOUT_ENTRY_CONTAINS;
            }else{
                return LAYOUT_WRONG_ENTRY_TYPE;
            }
        }
    }
    return LAYOUT_NOT_CONTAINS_ENTRY;
}

dul_type BOOL_T = {
    "Bool",
    1,
    BOOL_ID
};
dul_type INT_T = {
    "Int",
    8,
    INT_ID
};
dul_type FLOAT_T = {
    "Float",
    sizeof(double),
    FLOAT_ID
};

dul_type * get_field_type(dul_layout_type* layout, const char *name){
    for(int i = 0; i < layout->layout_size; i++){
        if(strcmp(layout->layout[i].name,name) == 0){
            return layout->layout[i].field_t;
        }
    }
    return NULL;
}

void* get_static_value(dul_layout_type *t, const char *name){
    for(int i = 0; i < t->layout_size; i++){
        if(strcmp(t->layout[i].name,name) == 0){
            return t->layout[i].staticValue;
        }
    }
    return NULL;
}

size_t layout_count_offset(dul_layout_type *layout, const char*name){
    size_t offset = 0;
    for(int i = 0; i < layout->layout_size; i++){
        if(strcmp(layout->layout[i].name,name) == 0){
            return offset;
        }
        offset += layout->layout[i].field_t->size;
    }
    return -1;
    
}

