#include "utilities.h"
#include <stdio.h>
#include <string.h>

/*This function will read the input into a buffer, replace \n with \0 if present and 
clear the input buffer of chars if buffer overflows and input buffer has any leftover due to it*/
void read_input(char *str, int size) {
    fgets(str, size, stdin);

    char *str_ptr = strchr(str, '\n');

    if (str_ptr != NULL) {
        *str_ptr = '\0';
    } else {
        int ch;

        while((ch = getchar()) != '\n' && ch != EOF);
    }
}


#include <stdlib.h>

void clear_console(void) {
#ifdef _WIN32
    system("cls");   // Windows
#else
    system("clear"); // Unix/Linux/Mac
#endif
}