#ifndef DB_FUNCTIONS_H
#define DB_FUNCTIONS_H

#include "globals.h"
#include "sqlite3.h"

enum db_error {
    D_SUCCESS,
    D_ERROR,
    D_NOT_FOUND,
    D_NOT_ENOUGH_STOCK,
    D_NOT_ENOUGH_FIFO_STOCK,
    D_UNIQUE_CONSTRAINT_VIOLATION,
    D_MEMORY_ALLOC_FAILED,
    D_FOREIGNKEY_VIOLATION,
};

//Item Functions
int db_add_item(const char *item_code, const char *item_name);
int db_get_all_items(Item **items, int *count);
int db_get_item_by_code(const char *input_item_code, Item *item);
int db_update_item(const char *old_item_code, const char *new_item_code, const char *new_item_name);
int db_delete_item(const char *item_code);

//Inventory Functions
int db_add_stock(int item_id, int qty, double unit_cost);
int db_issue_stock(Item item, int issue_qty);

#endif