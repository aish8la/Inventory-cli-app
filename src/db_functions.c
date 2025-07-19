#include "db_functions.h"
#include "sqlite3.h"
#include <stdio.h>
#include "sqlite_helpers.h"
#include "init_db.h"

int db_get_item_by_code(const char *input_item_code, Item *item) {
    sqlite3 *db = get_db();
    sqlite3_stmt *search_stmt = NULL;
    int result = D_ERROR;

    char *select_sql = "SELECT id, item_code, item_name, current_qty, total_value "
                        "FROM items "
                        "WHERE item_code = ?;";


    if(prepare_stmt(db, select_sql, &search_stmt) == 1) {
        goto cleanup;
    }
    
    sqlite3_bind_text(search_stmt, 1, input_item_code, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(search_stmt);

    if(result == SQLITE_ROW) {
        item->item_id = sqlite3_column_int(search_stmt, 0);
        snprintf(item->item_code, sizeof(item->item_code), "%s", (char*)sqlite3_column_text(search_stmt, 1));
        snprintf(item->item_name, sizeof(item->item_name), "%s", (char*)sqlite3_column_text(search_stmt, 2));
        item->current_qty = sqlite3_column_int(search_stmt, 3);
        item->total_value = sqlite3_column_double(search_stmt, 4);
        result = D_SUCCESS;
    } else if (result == SQLITE_DONE) {
        result = D_NOT_FOUND;
    }

    goto cleanup;

    cleanup:
        if (search_stmt) sqlite3_finalize(search_stmt);
        return result;
}

int db_add_stock(int item_id, int qty, double unit_cost) {
    sqlite3 *db = get_db();
    sqlite3_stmt *add_stmt = NULL;
    sqlite3_stmt *update_stmt = NULL;
    int result = D_ERROR;
    double total_value = qty * unit_cost;

    //Insert Stock Addition Entry
    char *insert_sql = "INSERT INTO stock_additions "
                        "(added_qty, unit_cost, unused_qty, item_id) "
                        "VALUES (?, ?, ?, ?);";

    if(begin_txn(db) != 0) {
        goto cleanup;
    }

    if(prepare_stmt(db, insert_sql, &add_stmt) == 1) {
        goto cleanup;
    }

    sqlite3_bind_int(add_stmt, 1, qty);
    sqlite3_bind_double(add_stmt, 2, unit_cost);
    sqlite3_bind_int(add_stmt, 3, qty);
    sqlite3_bind_int(add_stmt, 4, item_id);

    if (step_and_check(db, add_stmt, 0) != 0) {
        goto txn_error;
    }


    //Update Items Table
    char *update_sql = "UPDATE items "
                        "SET current_qty = current_qty + ?, "
                        "total_value = total_value + ? "
                        "WHERE id = ?;";

    if(prepare_stmt(db, update_sql, &update_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(update_stmt, 1, qty);
    sqlite3_bind_double(update_stmt, 2, total_value);
    sqlite3_bind_int(update_stmt, 3, item_id);

    if (step_and_check(db, update_stmt, 0) != 0) {
        goto txn_error;
    }

    if (commit_txn(db) != 0) {
        goto txn_error;
    }

 
    result = D_SUCCESS;
    goto cleanup;

    txn_error:
        rollback_txn(db);
        goto cleanup;

    cleanup:
        if (add_stmt) sqlite3_finalize(add_stmt);
        if (update_stmt) sqlite3_finalize(update_stmt);
        return result;

}