//
//  object.hpp
//  Dulang
//
//  Created by Дмитрий Маслюков on 06.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include <stdlib.h>

// object internally is not an object at dulang user-space
// it is some complex data, that is heap allocated
// and provides some interface to work with
// besides standard functions such as copy, equals and iterable interface it can have methods
// methods are either builtin or user-space functions
// each object has a type that stores information how to use this region of data

struct type;





typedef struct {
    char gc_info;
    struct type * type;
} object;

// numbers and booleans are atomic, and not heap-allocated, so this interface provides convinient access to the values
typedef union {
    double value;
    long bits;
    object* obj_value;
} value_t;

#endif /* object_hpp */
