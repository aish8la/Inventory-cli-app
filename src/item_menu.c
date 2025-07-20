#include "item_menu.h"
#include <stdio.h>
#include "utilities.h"
#include "sqlite3.h"
#include "sqlite_helpers.h"
#include <stdlib.h>
#include <ctype.h>
#include "display_table.h"
#include "globals.h"
#include "init_db.h"
#include "db_functions.h"


int add_item(void) {
    char item_code[ITEM_CODE_LENGTH + 1];
    char item_name[ITEM_NAME_LENGTH + 1];

    while(1) {

        clear_console();

        printf("Enter Item Code (Must be Unique): ");
        read_input(item_code, sizeof(item_code));

        printf("Enter Item Name: ");
        read_input(item_name, sizeof(item_name));

        if(item_code[0] == '\0' || item_name[0] == '\0') {
            printf("Value Cannot be Blank");
            goto cleanup;
        }

        //Perform Database operation
        int result = db_add_item(item_code, item_name);
        if(result == D_SUCCESS) {
            printf("\nAdded New Item [%s] Successfully.\n", item_code);
        } else if(result == D_UNIQUE_CONSTRAINT_VIOLATION) {
            printf("Item code already exists. Please use a unique code.\n");
            goto cleanup;
        } else {
            printf("Error adding item.\n");
            goto cleanup;
        }

        const char *add_another_msg = "\nWould you like to add another item ?";
        const char *cancel_msg = "\nDone Adding Items.";

        if(get_user_confirmation(add_another_msg, cancel_msg) != 0) {
            break;
        }
    }

    goto cleanup;

    cleanup:
        wait_for_enter();
        return 0;
}

int view_items(void) {

    Item *items = NULL;
    int count = 0;

    int result = db_get_all_items(&items, &count);
    if(result == D_NOT_FOUND) {
        printf("No items found in the database.\n");
    } else if (result != D_SUCCESS) {
        printf("\nDatabase Error\n\n");
    } else {
        display_item_table(items, count, format_item_header, format_item_row);
    }




    if (items) free(items);
    wait_for_enter();
    return 0;
}

int search_item(void) {
    char input_itm_code[ITEM_CODE_LENGTH];
    Item item;
    
    printf("Enter Item Code of Item to Search: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    int result = db_get_item_by_code(input_itm_code, &item);
    if(result == D_NOT_FOUND) {
        printf("\n\nItem could not be found\n");
    } else if(result != D_SUCCESS) {
        printf("\nDatabase Error\n\n");
    } else {
        //Uses the display format item header and row function to display the searched item
        format_item_header();
        format_item_row(&item, 0);
    }


    wait_for_enter();
    return 0;
}

int edit_item(void) {
    char input_itm_code[ITEM_CODE_LENGTH + 1];
    char new_itm_code[ITEM_CODE_LENGTH + 1];
    char new_itm_name[ITEM_NAME_LENGTH + 1];
    Item item;

    //Check if Item Exists and displays it
    printf("Enter Item Code of Item to edit: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    int result = db_get_item_by_code(input_itm_code, &item);
    if(result != D_SUCCESS) {
        printf("Item with code '%s' not found.\n", input_itm_code);
        goto cleanup;
    }

    printf("\nOld Item Details\n");
    display_selected_item(&item);


    printf("\nEnter Updated Item Details;\n\n");

    printf("Enter Updated Item Code: ");
    read_input(new_itm_code, sizeof(new_itm_code));

    printf("Enter Updated Item Name: ");
    read_input(new_itm_name, sizeof(new_itm_name));

    if(new_itm_code[0] == '\0' || new_itm_name[0] == '\0') {
        printf("Value Cannot be Blank");
        goto cleanup;
    }

    //Perform Record Update
    result = db_update_item(input_itm_code, new_itm_code, new_itm_name);
    switch (result) {
        case D_SUCCESS:
            printf("Record Updated Successfully");
            break;
        case D_NOT_FOUND:
            printf("Item not found for update");
            break;
        case D_UNIQUE_CONSTRAINT_VIOLATION:
            printf("New item code already exists. Please use a unique code.");
            break;
        default:
            printf("Error updating item");
            break;
    }

    goto cleanup;

    cleanup:
        wait_for_enter();
        return 0;
}

int delete_item(void) {
    char input_itm_code[ITEM_CODE_LENGTH + 1];
    Item item;

    printf("Enter Item Code of Item to Delete: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    //Check if Item exist and display
    int result = db_get_item_by_code(input_itm_code, &item);
    if(result != D_SUCCESS) {
        printf("Item with code '%s' not found.\n", input_itm_code);
        goto cleanup;
    }

    printf("\nItem Selected for Delete Operation\n");
    display_selected_item(&item);

    //Prompt User if they want to delete the selected item
    char *prompt = "\nConfirm Delete Operation ?\n";
    char *cancel_msg = "\nCancelled Delete Operation.\n";

    if(get_user_confirmation(prompt, cancel_msg) != 0) {
        goto cleanup;
    }

    //Perform the Delete Operation
    result = db_delete_item(input_itm_code);
    switch (result) {
        case D_SUCCESS:
            printf("Item [%s] Deleted Successfully", input_itm_code);
            break;
        case D_FOREIGNKEY_VIOLATION:
            printf("Item has one or more related transactions. Delete those first.");
            break;
        default:
            printf("Error deleting item");
            break;
    }

    goto cleanup;

    cleanup:
        wait_for_enter();
        return 0;
}