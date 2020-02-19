//
//  process_filename.h
//  Dulang
//
//  Created by Дмитрий Маслюков on 16.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef process_filename_h
#define process_filename_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * fname_to_modname(const char * filename);

char * modname_to_fname(const char * modname);

#endif /* process_filename_h */
