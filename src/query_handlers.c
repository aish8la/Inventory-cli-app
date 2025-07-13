#include "sqlite3.h"
#include <stdio.h>

//Format to display Item List tables
void print_item_header(void) {
    printf("\n%-12.10s%-22.20s\n", "Item Code", "Item Name");
    printf("===================================\n");
}

void print_item_row(sqlite3_stmt *stmt) {
    const char *item_code = (const char *)sqlite3_column_text(stmt, 0);
    const char *item_name = (const char *)sqlite3_column_text(stmt, 1);

    printf("%-12.10s%-22.20s\n", item_code, item_name);
}

//Function to display table
int display_table(sqlite3 *db, sqlite3_stmt *stmt, void (*display_header)(void), void (*display_row)(sqlite3_stmt *stmt)) {
    int rc, found = 0;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        if(!found) {
            found = 1;
            display_header();
        }
        display_row(stmt);
    }

    if (!found) {
        printf("No matching records found.\n");
        return 1;
    } else if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}