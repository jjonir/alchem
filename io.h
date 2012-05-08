#ifndef _IO_H
#define _IO_H

#include "workspace.h"

void init_io(void);
void end_io(void);
void display(workspace_t *w);
int8_t process_input_blocking(workspace_t *w);

#endif
