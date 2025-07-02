#include "menu.h"
#include <stdio.h>
#include "utilities.h"
#include "string.h"
#include <stdlib.h>
#include <login.h>
#include "item_menu.h"
#include "invent_menu.h"

/*This is a type def of a Menu item structure which will hold the pointer to the menu item function
and the label of the menu item
this allows for dynamic menu generation*/

typedef struct {
    const char* label;
    void (*action)(void);
    int req_access_lvl; // check login.h for more
} Menu_Item;

//TODO:move this to a session source and header file. currently need to include login.h to access this
extern User current_user;

/*These are prototypes of the main menu items. function definitions are below*/
void invent_menu(void);
void item_menu(void);
void setting_menu(void);


//This function will return a filtered list of menu items and is a helper for the run_menu function
//It accepts the original menu list, the count for the list and a pointer to the returned list count variable
//this will use a dynamically allocated memory so free the memory with free() after the finishing
Menu_Item *filter_menu(Menu_Item *menu_list, int count, int *ret_arr_count) {
    int user_access_lvl = current_user.access_level;
    Menu_Item *new_list;
    *ret_arr_count = 0;

    for(int i = 0; i < count; i++) {
        if(menu_list[i].req_access_lvl <= user_access_lvl) {
            (*ret_arr_count)++;
        }
    }

    new_list = (Menu_Item *)malloc(*ret_arr_count * sizeof(Menu_Item));

    if(new_list == NULL) {
        printf("Failed to allocate memory for new list at menu.c");
        exit(0);
    }

    int j = 0; // index for the new list array

    for(int i = 0; i < count; i++) {
        if(menu_list[i].req_access_lvl <= user_access_lvl) {
            new_list[j] = menu_list[i];
            j++;
        }
    }

    return new_list;

}

/*This is the menu runner function that will take the Menu_Item type array that
contains a list of defined menu items and display them on the CLI*/
void run_menu(const char* title, Menu_Item* items, int count) {
    int choice;
    int filtered_count;
    Menu_Item *filtered_list;

    filtered_list = filter_menu(items, count, &filtered_count);

    while (1) {
        clear_console();
        printf("\n ======== %s ======= \n", title);

        for(int i = 0; i < filtered_count; i++) {
            printf("%d. %s\n", i + 1, filtered_list[i].label);
        }
        printf("0. Back\n");

        printf("Choose => ");
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

        filtered_list[choice -1].action();

    }

    free(filtered_list);// this frees the filtered list memory
    filtered_list = NULL;//null assigned to pointer to prevent issues with dangling pointer
}

/*This is the run main menu function that will call the run_menu function with the main menu item list*/
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

void item_menu(void) {

    Menu_Item item_menu[] = {
        {"Add Item", add_item, 2},
        {"View Items", view_items, 1},
        {"Search Item", search_item, 1},
        {"Edit Item", edit_item, 2},
        {"Delete Item", delete_item, 1},
    };

    int menu_count = sizeof(item_menu) / sizeof(item_menu[0]);

    const char* title = "Item Menu";

    run_menu(title, item_menu, menu_count);
}

void invent_menu(void) {

    Menu_Item invent_menu[] = {
        {"Add Stock", add_stock, 2},
        {"Issue Stock", issue_stock, 2},
        {"View Stock Additions", view_additions, 1},
        {"View Issues", view_issues, 1},
        {"View Inventory", view_inventory, 1},
    };

    int menu_count = sizeof(invent_menu) / sizeof(invent_menu[0]);

    const char* title = "Inventory Menu";

    run_menu(title, invent_menu, menu_count);
}

void setting_menu(void) {
    printf("This is the setting menu");
    wait_for_enter();
}