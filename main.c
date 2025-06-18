#include <stdio.h>
#include "login.h"
#include <stdlib.h>
#include "utilities.h"

int main(void) {

    int login_attempts = 0;
    int rem_logins = MAX_LOGIN_ATTEMPTS;

    do
    {
        clear_console();

        printf("Remaining Login Attempts: %d \n\n", rem_logins);

        if(login() == 1) {
            break;
        } else {
            login_attempts++;
            rem_logins--;
        }

    } while (login_attempts < MAX_LOGIN_ATTEMPTS);


    if(login_attempts >= MAX_LOGIN_ATTEMPTS) {
        printf("Max Login Attempts reached, stopping program");
        return 0;
    }
    
    getchar();

    return 0;
}