//
//  execeasy.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 08.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef execeasy_h
#define execeasy_h

#include "frame.h"
#include "coro_sheduler.h"
#define INSTR_PER_ROT 1<<30
void exec_ctx(context * ctx);
static volatile int finished = 0;
void* thread_executable(void * arg);
void spawn_workers(int num);

#endif /* execeasy_h */
