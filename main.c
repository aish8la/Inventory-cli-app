#include <stdio.h>
#include "login.h"
#include <stdlib.h>

int main(void) {

    int login_attempts = 0;

    do
    {
        if(login() == 1) {
            break;
        } else {
            login_attempts++;
            system("cls");
        }

    } while (login_attempts < 3);

    system("cls");

    if(login_attempts >= 3) {
        printf("Max Login Attempts reached, stopping program");
        return 0;
    }
    
    getchar();

    return 0;
}