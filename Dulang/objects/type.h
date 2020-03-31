//
//  type.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 25.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef type_h
#define type_h

#include <stdio.h>
#include <stdlib.h>
typedef struct _basic_type{
#define _BASIC_T const char * basic_name; size_t size; int typeid;
    _BASIC_T
} dul_type;

typedef struct _parametrized_type{
    _BASIC_T
    dul_type * param_t;
} dul_param_type;

typedef struct _layout_type{
    _BASIC_T
    int layout_size;
    int layout_cap;
    struct field{
        const char * name;
        dul_type * field_t;
        //is null if non-static
        void * staticValue;
    } * layout;
} dul_layout_type;

extern dul_type BOOL_T;
extern dul_type INT_T;
extern dul_type FLOAT_T;

dul_layout_type* create_layout_type(void);
dul_param_type* create_param_type(dul_type * param_type);
void add_entry_to_layout_type(dul_layout_type * t, struct field entry);
void* get_static_value(dul_layout_type *, const char *);
dul_type * get_field_type(dul_layout_type*, const char *);
size_t layout_count_offset(dul_layout_type *, const char*);
#define LAYOUT_NOT_CONTAINS_ENTRY 0
#define LAYOUT_WRONG_ENTRY_TYPE 1
#define LAYOUT_ENTRY_CONTAINS 2
#define LAYOUT_ENTRY_UNDETERMINED 3
int contains_entry(dul_layout_type * layout, struct field entry);
const char * get_var_at_layout(dul_layout_type*layout, size_t offset);
#define BOOL_ID 0
#define INT_ID 1
#define FLOAT_ID 2
#define isNumericType(t)  ((t)==INT_ID || (t)==FLOAT_ID)


#endif /* type_h */
