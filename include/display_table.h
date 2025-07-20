#ifndef DISPLAY_TABLE_H
#define DISPLAY_TABLE_H

#include "sqlite3.h"
#include "globals.h"
void format_inventory_header(void);
void format_inventory_row(const Item *item, int index);
void format_item_header(void);
void format_item_row(const Item *item, int index);
void display_item_table(Item *items, int count, void (*print_header)(void), void (*print_row)(const Item *, int));

void display_selected_item(const Item *item);
void display_selected_item_qty(const Item *item);
void display_selected_addition(const Stock_Addition *addition);
void display_selected_stock_issue(const Stock_Issue *issue);

void format_additions_header(void);
void format_additions_row(const Stock_Addition *addition, int index);
void display_additions_table(Stock_Addition *additions, int count, void (*print_header)(void), void (*print_row)(const Stock_Addition *, int));

void format_issues_header(void);
void format_issues_row(const Stock_Issue *issue, int index);
void display_issues_table(Stock_Issue *issues, int count, void (*print_header)(void), void (*print_row)(const Stock_Issue *, int));

#endif