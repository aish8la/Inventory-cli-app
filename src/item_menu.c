#include "item_menu.h"
#include <stdio.h>
#include "utilities.h"
#include "sqlite3.h"
#include "sqlite_helpers.h"
#include <stdlib.h>


void add_item(void) {
    printf("This is the Add Item Function");
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
        return;
    }

    printf("\n ======== %s ======= \n", "View Item List");
    printf("\n%-12.10s%-22.20s\n", "Item Code", "Item Name");
    printf("===================================\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *item_name = sqlite3_column_text(stmt, 1);
        const unsigned char *item_code = sqlite3_column_text(stmt, 0);

        printf("%-12.10s%-22.20s\n", item_code, item_name);

    }

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