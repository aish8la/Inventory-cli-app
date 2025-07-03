#ifndef UTILITIES_H
#define UTILITIES_H

void read_input(char *str, int size);

void clear_console(void);

void wait_for_enter(void);

void clear_input_buffer(void);

int get_user_confirmation(const char *prompt_msg, const char *cancel_msg);

#endif