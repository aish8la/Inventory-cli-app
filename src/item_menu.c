#include "item_menu.h"
#include <stdio.h>
#include "utilities.h"
#include "sqlite3.h"
#include "sqlite_helpers.h"
#include <stdlib.h>
#include <ctype.h>

const int itm_cd_ln = 8; 
const int itm_nm_ln = 30;

void add_item(void) {

    sqlite3 *db;

    if(open_db(&db) != 0) {
        exit(1);
    }

    sqlite3_stmt *stmt;//an object that will store the sql statement in byte code and be used to run the other functions on

    //The ? are binding parameters to whom, values will be bound to using sqlite3_bind* functions
    char *sql = "INSERT INTO items (item_code, item_name) "
                "VALUES (?, ?);";

    /*Prepare statement is where the sql statement is translated into byte code for the statement to be run 
    (this is wrapper function that will run the sqlite3_prepare_v2 and also handle errors)*/
    if(prepare_stmt(db, sql, &stmt) == 1) {
        sqlite3_close(db);
        return;
    }

    char add_item = 'y';

    while(tolower(add_item) == 'y') {

    clear_console();

    char item_code[itm_cd_ln];
    char item_name[itm_nm_ln];

    printf("Enter Item Code (Must be Unique): ");
    read_input(item_code, sizeof(item_code));

    printf("Enter Item Name: ");
    read_input(item_name, sizeof(item_name));

    //This is where the values from the variables are bound to the sql statement bind parameters as mentioned above
    sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_TRANSIENT);

    //sqlite3_step is used to execute the insert using the prepared statement
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Sqlite Insert Error: %s\n", sqlite3_errmsg(db));
            if (sqlite3_errcode(db) == SQLITE_CONSTRAINT) {
                printf("Item code already exists. Please use a unique code.\n");
            }
        sqlite3_close(db);
        wait_for_enter();
        return;
    }

    sqlite3_reset(stmt);

    printf("Added Item, would you like to add another item ? (Y)es / (N)o => ");
    scanf("%c", &add_item);
    clear_input_buffer();

    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    wait_for_enter();
}

void view_items(void) {

    sqlite3 *db;

    if(open_db(&db) != 0) {
        exit(1);
    }

    sqlite3_stmt *stmt;
    char *sql = "SELECT item_code, item_name "
                "FROM items;";

    if(prepare_stmt(db, sql, &stmt) == 1) {
        sqlite3_close(db);
        return;
    }

    printf("\n%-12.10s%-22.20s\n", "Item Code", "Item Name");
    printf("===================================\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *item_name = sqlite3_column_text(stmt, 1);
        const unsigned char *item_code = sqlite3_column_text(stmt, 0);

        printf("%-12.10s%-22.20s\n", item_code, item_name);

    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    wait_for_enter();
}

void search_item(void) {
    printf("This is the Search Item Function");
    wait_for_enter();
}

void edit_item(void) {
    printf("This is the Edit Item Function");
    wait_for_enter();
}

void delete_item(void) {
    printf("This is the Delete Item Function");
    wait_for_enter();
}