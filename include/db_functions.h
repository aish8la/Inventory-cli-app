#ifndef DB_FUNCTIONS_H
#define DB_FUNCTIONS_H

#include "globals.h"
#include "sqlite3.h"

enum db_error {
    D_SUCCESS,
    D_ERROR,
    D_NOT_FOUND,

};

typedef struct {
    int item_id;
    char item_code[ITEM_CODE_LENGTH + 1];
    char item_name[ITEM_NAME_LENGTH + 1];
    int current_qty;
    double total_value;
} Item;



int db_get_item_by_code(const char *input_item_code, Item *item);
int db_add_stock(int item_id, int qty, double unit_cost);

#endif