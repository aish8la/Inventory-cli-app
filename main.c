#include <stdio.h>
#include "login.h"
#include <stdlib.h>
#include "utilities.h"
#include "menu.h"
#include "init_db.h"

int main(void) {

    int db_rc = initialize_db();

    if(db_rc == 1) {
        printf("Failed to initialize DB");
        return 1;
    }

    int login_attempts = 0;
    int rem_logins = MAX_LOGIN_ATTEMPTS;

    do
    {
        clear_console();

        if(login() == 1) {
            break;
        } else {
            login_attempts++;
            rem_logins--;
        }

        printf("\n\nLogin failed. Invalid username or password.\n\n");

        if(rem_logins != 0) {
            printf("You have %d more attempts left\n", rem_logins);
            wait_for_enter();
        }

    } while (login_attempts < MAX_LOGIN_ATTEMPTS);


    if(login_attempts >= MAX_LOGIN_ATTEMPTS) {
        printf("Max Login Attempts reached, stopping program");
        return 0;
    }

    run_main_menu();
    
    wait_for_enter();

    return 0;
}