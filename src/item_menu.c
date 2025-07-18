#include "item_menu.h"
#include <stdio.h>
#include "utilities.h"
#include "sqlite3.h"
#include "sqlite_helpers.h"
#include <stdlib.h>
#include <ctype.h>
#include "display_table.h"
#include "globals.h"


int add_item(void) {

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char item_code[ITEM_CODE_LENGTH];
    char item_name[ITEM_NAME_LENGTH];

    //The ? are binding parameters to whom, values will be bound to using sqlite3_bind* functions
    char *sql = "INSERT INTO items (item_code, item_name) "
                "VALUES (?, ?);";


    if(open_db(&db) != 0) {
        return 1;
    }


    /*Prepare statement is where the sql statement is translated into byte code for the statement to be run 
    (this is wrapper function that will run the sqlite3_prepare_v2 and also handle errors)*/
    if(prepare_stmt(db, sql, &stmt) == 1) {
        goto cleanup;
    }

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

    //This is where the values from the variables are bound to the sql statement bind parameters as mentioned above
    sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_TRANSIENT);

    //sqlite3_step is used to execute the insert using the prepared statement
    if(step_and_check(db, stmt, 0) != 0) {
        if (sqlite3_errcode(db) == SQLITE_CONSTRAINT) {
            printf("Item code already exists. Please use a unique code.\n");
        }
        goto cleanup;
    }

    sqlite3_reset(stmt);

    printf("\nAdded New Item [%s] Successfully.\n", item_code);

    const char *add_another_msg = "\nWould you like to add another item ?";
    const char *cancel_msg = "\nDone Adding Items.";

    if(get_user_confirmation(add_another_msg, cancel_msg) != 0) {
        break;
    }

    }

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
        return 0;
}

int view_items(void) {

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char *sql = "SELECT item_code, item_name "
                "FROM items;";

    if(open_db(&db) != 0) {
        return 1;
    }


    if(prepare_stmt(db, sql, &stmt) == 1) {
        goto cleanup;
    }

    display_table(db, stmt, print_item_header, print_item_row);

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
        return 0;
}

int search_item(void) {

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char *sql = "SELECT item_code, item_name "
                "FROM items "
                "WHERE item_code LIKE ?;";


    char input[ITEM_CODE_LENGTH];
    char bind_param[ITEM_CODE_LENGTH + 10];

    if(open_db(&db) != 0) {
        return 1;
    }


    if(prepare_stmt(db, sql, &stmt) == 1) {
        goto cleanup;
    }

    printf("Enter Item Code: ");
    read_input(input, sizeof(input));

    snprintf(bind_param, sizeof(bind_param), "%%%s%%", input); // This adds % to the start and end of input so fuzzy search is possible with LIKE clause

    sqlite3_bind_text(stmt, 1, bind_param, -1, SQLITE_TRANSIENT);

    display_table(db, stmt, print_item_header, print_item_row);

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
        return 0;
}

int edit_item(void) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char *select_sql = "SELECT item_code, item_name "
                "FROM items "
                "WHERE item_code = ?;";
    char *edit_sql = "UPDATE items SET item_code = ?, item_name = ? "
                "WHERE item_code = ?;";

    char input[ITEM_CODE_LENGTH];
    char new_itm_code[ITEM_CODE_LENGTH];
    char new_itm_name[ITEM_NAME_LENGTH];

    if(open_db(&db) != 0) {
        return 1;
    }

    if(prepare_stmt(db, select_sql, &stmt) == 1) {
        goto cleanup;
    }

    printf("Enter Item Code of Item to edit: ");
    read_input(input, sizeof(input));

    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);

    //TODO: Create a helper to select item and show the selected items
    printf("\nOld Item Details\n");

    int found = display_table(db, stmt, print_item_header, print_item_row);
    if(found != 0) {
        goto cleanup;
    }
 
    sqlite3_finalize(stmt);
    stmt = NULL;

    printf("\nEnter Updated Item Details;\n\n");

    printf("Enter Updated Item Code: ");
    read_input(new_itm_code, sizeof(new_itm_code));

    printf("Enter Updated Item Name: ");
    read_input(new_itm_name, sizeof(new_itm_name));

    if(new_itm_code[0] == '\0' || new_itm_name[0] == '\0') {
        printf("Value Cannot be Blank");
        goto cleanup;
    }

    if(prepare_stmt(db, edit_sql, &stmt) == 1) {
        goto cleanup;
    }

    sqlite3_bind_text(stmt, 1, new_itm_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, new_itm_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, input, -1, SQLITE_TRANSIENT);

    if (step_and_check(db, stmt, 0) != 0) {
        goto cleanup;
    }

    printf("Record Updated Successfully");

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
        return 0;
}

int delete_item(void) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char *select_sql = "SELECT item_code, item_name "
                "FROM items "
                "WHERE item_code = ?;";
    char *delete_sql = "DELETE FROM items "
                "WHERE item_code = ?;";

    if(open_db(&db) != 0) {
        return 1;
    }

    if(prepare_stmt(db, select_sql, &stmt) == 1) {
        goto cleanup;
    }

    char input[ITEM_CODE_LENGTH];

    printf("Enter Item Code of Item to Delete: ");
    read_input(input, sizeof(input));

    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);

    printf("\nItem Selected for Delete Operation\n\n");
    
    int found = display_table(db, stmt, print_item_header, print_item_row);
    if(found != 0) {
        goto cleanup;
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    char *prompt = "\nConfirm Delete Operation ?\n";
    char *cancel_msg = "\nCancelled Delete Operation.\n";

    if(get_user_confirmation(prompt, cancel_msg) != 0) {
        goto cleanup;
    }

    if(prepare_stmt(db, delete_sql, &stmt) == 1) {
        goto cleanup;
    }

    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);

    if (step_and_check(db, stmt, 0) != 0) {
        goto cleanup;
    }

    printf("Item [%s] Deleted Successfully", input);

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
        return 0;
}