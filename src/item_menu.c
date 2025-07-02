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

    sqlite3 *db;

    if(open_db(&db) != 0) {
        exit(1);
    }

    sqlite3_stmt *stmt;
    char *sql = "SELECT item_code, item_name "
                "FROM items "
                "WHERE item_code LIKE ?;";

    if(prepare_stmt(db, sql, &stmt) == 1) {
        sqlite3_close(db);
        return;
    }

    char input[itm_cd_ln];
    

    printf("Enter Item Code: ");
    read_input(input, sizeof(input));

    char bind_param[itm_cd_ln + 10];

    snprintf(bind_param, sizeof(bind_param), "%%%s%%", input); // This adds % to the start and end of input so fuzzy search is possible with LIKE clause

    sqlite3_bind_text(stmt, 1, bind_param, -1, SQLITE_TRANSIENT);

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

void edit_item(void) {
    sqlite3 *db;

    if(open_db(&db) != 0) {
        exit(1);
    }

    sqlite3_stmt *stmt;
    char *select_sql = "SELECT item_code, item_name "
                "FROM items "
                "WHERE item_code = ?;";
    char *edit_sql = "UPDATE items SET item_code = ?, item_name = ? "
                "WHERE item_code = ?;";

    if(prepare_stmt(db, select_sql, &stmt) == 1) {
        sqlite3_close(db);
        return;
    }

    char input[itm_cd_ln];
    char new_itm_code[itm_cd_ln];
    char new_itm_name[itm_nm_ln];
    

    printf("Enter Item Code of Item to edit: ");
    read_input(input, sizeof(input));

    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);



    int rc = sqlite3_step(stmt);
    if(rc == SQLITE_DONE) {
        printf("No Records of the specified ID Found\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        wait_for_enter();
        return;
    } else if (rc != SQLITE_ROW) {
        fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        wait_for_enter();
        return;
    }

    printf("\nOld Item Details\n");
    printf("\n%-12.10s%-22.20s\n", "Item Code", "Item Name");
    printf("===================================\n");
    const unsigned char *item_name = sqlite3_column_text(stmt, 1);
    const unsigned char *item_code = sqlite3_column_text(stmt, 0);

    printf("%-12.10s%-22.20s\n", item_code, item_name);
 
    sqlite3_finalize(stmt);

    printf("\nUpdated Item Details\n\n");
    printf("Enter Updated Item Code: ");
    read_input(new_itm_code, sizeof(new_itm_code));

    printf("Enter Updated Item Name: ");
    read_input(new_itm_name, sizeof(new_itm_name));

    if(prepare_stmt(db, edit_sql, &stmt) == 1) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, new_itm_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, new_itm_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, input, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        wait_for_enter();
        return;
    }

    printf("Record Updated Successfully");

    sqlite3_finalize(stmt);

    sqlite3_close(db);
    wait_for_enter();
}

void delete_item(void) {
    printf("This is the Delete Item Function");
    wait_for_enter();
}