//
//  type.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 06.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef type_h
#define type_h

#include "object.h"

#define DUL_FAST_ARGS object ** args, int argcount
#define DUL_EMPTY_ARG 0, 0
//  allocator is called only externally for type coersion, e.g. String(0)
//  or Array(0, "str", a)
typedef object *    (*allocator_func)   (DUL_FAST_ARGS);
// destructor is called when object is no longer reachable
typedef void        (*destructor_func)  (object*);
typedef object *    (*copy_func)        (object*);

//returns boolean
typedef value_t (*comparator) (const object *, const object *);

typedef struct {
    object * (*init_iter)(object*);
    value_t (*num_subscr)(object*);
    value_t (*str_subscr)(object*);
} i_iterable;

typedef struct type {
    const char * name;
    //typeid is required for fast type checks
    char _typeid;
    allocator_func allocator;
    destructor_func destructor;
    copy_func copy;
    comparator comp;
    i_iterable iteration_traits;
} typeobject;

#define STRING_TID 0
#define ARRAY_TID 1
#define OBJECT_TID 2
#define TUPLE_TID 3
#define CHANNEL_TID 4

#endif /* type_h */
