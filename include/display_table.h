#ifndef DISPLAY_TABLE_H
#define DISPLAY_TABLE_H

#include "sqlite3.h"
#include "globals.h"

void print_item_header(void);
void print_item_row(sqlite3_stmt *stmt, int sn);

void print_stock_header(void);
void print_stoc_row(sqlite3_stmt *stmt, int sn);


int display_table(sqlite3 *db, sqlite3_stmt *stmt, void (*display_header)(void), void (*display_row)(sqlite3_stmt *stmt, int sn));

void format_item_header(void);
void format_item_row(const Item *item, int index);
void display_item_table(Item *items, int count, void (*print_header)(void), void (*print_row)(const Item *, int));

#endif