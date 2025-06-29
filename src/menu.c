#include "menu.h"
#include <stdio.h>
#include "utilities.h"
#include "string.h"

/*This is a type def of a Menu item structure which will hold the pointer to the menu item function
and the label of the menu item
this allows for dynamic menu generation*/

typedef struct {
    const char* label;
    void (*action)(void);
    int req_access_lvl; // check login.h for more
} Menu_Item;


/*These are main menu items for testing*/
void invent_menu(void) {
    printf("This is the Inventory Transaction Menu");
    wait_for_enter();
}

void item_menu(void) {
    printf("This is the Item Menu");
    wait_for_enter();
}

void setting_menu(void) {
    printf("This is the setting menu");
    wait_for_enter();
}

/*This is the menu runner function that will take the Menu_Item type array that
contains a list of defined menu items and display them on the CLI*/
void run_menu(const char* title, Menu_Item* items, int count) {
    int choice;
    

    while (1) {
        clear_console();
        printf("\n ======== %s ======= \n", title);

        for(int i = 0; i < count; i++) {
            printf("%d. %s\n", i + 1, items[i].label);
        }
        printf("0. Back\n");

        printf("Chose => ");
        scanf("%d", &choice);
        clear_input_buffer();
        clear_console();

        if(choice == 0) {
            break;
        }

        if(choice < 1 || choice > count) {

            printf("\nInvalid Choice. Try again.");
            wait_for_enter();
            continue;
        }

        items[choice -1].action();

    }
}

/*This is the run main menu function that will call the run_menu function with the main menu item list
This is for testing
Replace this with conditional list for when role based permission functionality is added*/
void run_main_menu(void) {

    Menu_Item main_menu[] = {
        {"Inventory Transactions", invent_menu, 1},
        {"Items Menu", item_menu, 1},
        {"Settings", setting_menu, 3},
    };

    int menu_count = sizeof(main_menu) / sizeof(main_menu[0]);

    const char* title = "Main Menu";

    run_menu(title, main_menu, menu_count);
}

