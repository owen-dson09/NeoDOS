#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "util.h"

typedef void (*keyboard_callback_t)(char c);

void keyboard_init(keyboard_callback_t callback);
void keyboard_disable(void);
void keyboard_handler(void);

#endif