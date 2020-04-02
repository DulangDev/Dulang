//
//  func_wrappers.c
//  Dulang
//
//  Created by Дмитрий Маслюков on 01.04.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "func_wrappers.h"
void print_integer(void* val){
    printf("%lld\n", *(int64_t*)val);
}
