#include "sqlite3.h"
#include <stdio.h>
#include "globals.h"

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
    printf("=====================================================================\n");
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

//Print Header Formats
//Item Headers
void format_item_header(void) {
    printf("\n%-5s %-15s %-30s %-12s %-12s\n", "Sn.", "Item Code", "Item Name", "Current Qty", "Total Value");
    printf("%-5s %-15s %-30s %-12s %-12s\n", "----", "----------", "---------", "-----------", "-----------");
}


//Print Row Formats
//Item Rows
void format_item_row(const Item *item, int index) {
    printf("%-5d %-15s %-30s %-12d %-12.2f\n",
           index + 1,
           item->item_code,
           item->item_name,
           item->current_qty,
           item->total_value);
}


//Display Item Rows
void display_item_table(Item *items, int count, void (*print_header)(void), void (*print_row)(const Item *, int)) {
    if (!items || count <= 0) {
        printf("No items to display.\n");
        return;
    }

    if (print_header) print_header();

    for (int i = 0; i < count; i++) {
        print_row(&items[i], i);
    }

    printf("\nTotal Items: %d\n", count);
}


void display_selected_item(const Item *item) {
    printf("Code: [%s]\nName: [%s]\n\n", item->item_code, item->item_name);
}

void display_selected_item_qty(const Item *item) {
    printf("Code: [%s]\nName: [%s]\nAvailable Qty: [%d]\n\n", item->item_code, item->item_name, item->current_qty);
}