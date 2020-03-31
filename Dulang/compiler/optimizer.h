//
//  optimizer.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 24.03.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef optimizer_h
#define optimizer_h

#include "astgen.h"

//some optimizations directly with AST

// optimizes expressions such as stat * stat, etc
// some extra math optimizations that are easier to execute
void optimize_statics_math(astnode * root);

#endif /* optimizer_h */
