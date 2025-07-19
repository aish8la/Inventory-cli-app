#include "sqlite3.h"
#include <stdio.h>

//Format to display Item List tables
void print_item_header(void) {
    printf("\n%-4s%-12.10s%-22.20s\n", "Sn", "Item Code", "Item Name");
    printf("===================================\n");
}

void print_item_row(sqlite3_stmt *stmt, int sn) {
    const char *item_code = (const char *)sqlite3_column_text(stmt, 0);
    const char *item_name = (const char *)sqlite3_column_text(stmt, 1);

    printf("%-4d%-12.10s%-22.20s\n", sn, item_code, item_name);
}

//Format to display Item List tables
void print_stock_header(void) {
    printf("\n%-4s%-12.10s%-22.20s%-12s%-16s\n", "Sn", "Item Code", "Item Name", "Quantity", "Total Value");
    printf("===================================\n");
}

void print_stoc_row(sqlite3_stmt *stmt, int sn) {
    const char *item_code = (const char *)sqlite3_column_text(stmt, 0);
    const char *item_name = (const char *)sqlite3_column_text(stmt, 1);
    int qty = sqlite3_column_int(stmt, 2);
    double value = sqlite3_column_double(stmt, 3);


    printf("%-4d%-12.10s%-22.20s%-12d%-16.2f\n", sn, item_code, item_name, qty, value);
}

//Function to display table
int display_table(sqlite3 *db, sqlite3_stmt *stmt, void (*display_header)(void), void (*display_row)(sqlite3_stmt *stmt, int sn)) {
    int rc, found = 0, count = 0;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        count++;
        if(!found) {
            found = 1;
            display_header();
        }
        display_row(stmt, count);
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