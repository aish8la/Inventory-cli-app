#ifndef DISPLAY_TABLE_H
#define DISPLAY_TABLE_H

#include "sqlite3.h"

void print_item_header(void);
void print_item_row(sqlite3_stmt *stmt, int sn);

void print_stock_header(void);
void print_stoc_row(sqlite3_stmt *stmt, int sn);


int display_table(sqlite3 *db, sqlite3_stmt *stmt, void (*display_header)(void), void (*display_row)(sqlite3_stmt *stmt, int sn));

#endif