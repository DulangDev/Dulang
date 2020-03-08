//
//  coro_sheduler.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 03.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef coro_sheduler_h
#define coro_sheduler_h

#include "frame.h"

typedef struct _coronode{
    context * ctx;
    struct _coronode * next;
} coro_node;

#endif /* coro_sheduler_h */
