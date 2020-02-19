//
//  object.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 13.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef object_h
#define object_h
//  object is a structure allocated on heap that provides some methods,
//  props, whatever
//  it is registered in gc and has address that ends in 7
typedef struct dulobject{
    int refcount;
    struct dulobject * prototype;
} object;


#endif /* object_h */
