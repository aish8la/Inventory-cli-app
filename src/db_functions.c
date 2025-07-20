#include "db_functions.h"
#include "sqlite3.h"
#include <stdio.h>
#include "sqlite_helpers.h"
#include "init_db.h"
#include "globals.h"
#include "stdlib.h"

//Item Menu DB Functions
int db_add_item(const char *item_code, const char *item_name) {
    sqlite3 *db = get_db();
    sqlite3_stmt *stmt = NULL;
    int result = D_ERROR;

    //The ? are binding parameters to whom, values will be bound to using sqlite3_bind* functions
    char *sql = "INSERT INTO items (item_code, item_name) "
                "VALUES (?, ?);";


    /*Prepare statement is where the sql statement is translated into byte code for the statement to be run 
    (this is wrapper function that will run the sqlite3_prepare_v2 and also handle errors)*/
    if(prepare_stmt(db, sql, &stmt) == 1) {
        goto cleanup;
    }

    //This is where the values from the variables are bound to the sql statement bind parameters as mentioned above
    sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_TRANSIENT);

    //sqlite3_step is used to execute the insert using the prepared statement
    if(step_and_check(db, stmt, 0) != 0) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_UNIQUE) {
            result = D_UNIQUE_CONSTRAINT_VIOLATION;
        }
        goto cleanup;
    }

    result = D_SUCCESS;


    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        return result;
}

int db_get_all_items(Item **items, int *count) {
    sqlite3 *db = get_db();
    sqlite3_stmt *stmt = NULL;
    *items = NULL;
    *count = 0;
    int result = D_ERROR;

    char *sql = "SELECT id, item_code, item_name, current_qty, total_value "
                "FROM items;";

    if(prepare_stmt(db, sql, &stmt) == 1) {
        goto cleanup;
    }

    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_count++;
    }

    if (row_count == 0) {
        result = D_NOT_FOUND;
        goto cleanup;
    }

    // Allocate memory
    *items = malloc(sizeof(Item) * row_count);
    if (!*items) {
        result = D_MEMORY_ALLOC_FAILED;
        goto cleanup;
    }

    sqlite3_reset(stmt);
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < row_count) {
        Item *item = &(*items)[i];//Dereferences the pointer to the array, gets the i-th Item and passes the memory address of that to the *item

        item->item_id = sqlite3_column_int(stmt, 0);

        const unsigned char *code = sqlite3_column_text(stmt, 1);
        const unsigned char *name = sqlite3_column_text(stmt, 2);

        snprintf(item->item_code, sizeof(item->item_code), "%s", code ? (const char *)code : "");//if null put empty string
        snprintf(item->item_name, sizeof(item->item_name), "%s", name ? (const char *)name : "");

        item->current_qty = sqlite3_column_int(stmt, 3);
        item->total_value = sqlite3_column_double(stmt, 4);

        i++;
    }

    *count = row_count;
    result = D_SUCCESS;    

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (result != D_SUCCESS && *items) {
            free(*items);
            *items = NULL;
            *count = 0;
        }
        return result;
}

int db_update_item(const char *old_item_code, const char *new_item_code, const char *new_item_name) {
    sqlite3 *db = get_db();
    sqlite3_stmt *stmt = NULL;
    int result = D_ERROR;

    char *edit_sql = "UPDATE items SET item_code = ?, item_name = ? "
                "WHERE item_code = ?;";

    if(prepare_stmt(db, edit_sql, &stmt) != 0) {
        goto cleanup;
    }

    sqlite3_bind_text(stmt, 1, new_item_code, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, new_item_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, old_item_code, -1, SQLITE_TRANSIENT);

    if (step_and_check(db, stmt, 0) != 0) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_UNIQUE) {
            result = D_UNIQUE_CONSTRAINT_VIOLATION; 
        }
        goto cleanup;
    }

    result = D_SUCCESS;

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        return result;
}

int db_delete_item(const char *item_code) {
    sqlite3 *db = get_db();
    sqlite3_stmt *stmt = NULL;
    int result = D_ERROR;

    char *delete_sql = "DELETE FROM items "
                "WHERE id = ?;";


    if(prepare_stmt(db, delete_sql, &stmt) == 1) {
        goto cleanup;
    }

    sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);

    if (step_and_check(db, stmt, 0) != 0) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_FOREIGNKEY) {
            result = D_FOREIGNKEY_VIOLATION;
        }
        goto cleanup;
    }

    result = D_SUCCESS;

    goto cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        return result;
}

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

//Inventory Menu DB Functions
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

int db_issue_stock(Item item, int issue_qty) {
    sqlite3 *db = get_db();
    sqlite3_stmt *select_add_stmt = NULL;
    sqlite3_stmt *issue_stmt = NULL;
    sqlite3_stmt *relation_stmt = NULL;
    sqlite3_stmt *update_itm_stmt = NULL;
    sqlite3_stmt *update_add_stmt = NULL;
    int result = D_ERROR;
    int issue_id;
    double total_cost = 0;

    if(begin_txn(db) != 0) {
        goto cleanup;
    }

    if(item.current_qty < issue_qty) {
        result = D_NOT_ENOUGH_STOCK;
        goto cleanup;
    }

    //Insert Stock Issues
    char *insert_issue_sql = "INSERT INTO stock_issues "
                        "(issued_qty, item_id) "
                        "VALUES (?, ?);";

    if(prepare_stmt(db, insert_issue_sql, &issue_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(issue_stmt, 1, issue_qty);
    sqlite3_bind_int(issue_stmt, 2, item.item_id);

    if (step_and_check(db, issue_stmt, 0) != 0) {
        goto txn_error;
    }

    issue_id = (int)sqlite3_last_insert_rowid(db);


    //Get Stock Additions in FIFO Order
    char *select_add_txn_sql = "SELECT id, unused_qty, unit_cost "
                        "FROM stock_additions "
                        "WHERE item_id = ? AND unused_qty > 0 "
                        "ORDER BY id ASC;";

    if(prepare_stmt(db, select_add_txn_sql, &select_add_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(select_add_stmt, 1, item.item_id);

    //Prepare relation statement
    char *insert_relation_sql = "INSERT INTO stock_issues_add_relation "
                        "(issued_qty, stock_issues_id, stock_addition_id) "
                        "VALUES (?, ?, ?);";

    if(prepare_stmt(db, insert_relation_sql, &relation_stmt) == 1) {
        goto txn_error;
    }

    //Prepare update stock addition statement
    char *update_add_sql = "UPDATE stock_additions "
                        "SET unused_qty = unused_qty - ? "
                        "WHERE id = ?;";

    if(prepare_stmt(db, update_add_sql, &update_add_stmt) == 1) {
        goto txn_error;
    }

//Process FIFO stock issue
    int remaining_qty = issue_qty;
    while(remaining_qty > 0) {

        int rc = sqlite3_step(select_add_stmt);

        if(rc != SQLITE_ROW) {
            if(rc != SQLITE_DONE) {
                fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
            } else {
                result = D_NOT_ENOUGH_FIFO_STOCK;
            }
            goto txn_error;
        }

        int add_txn_id = sqlite3_column_int(select_add_stmt, 0);
        int txn_unused_qty = sqlite3_column_int(select_add_stmt, 1);
        double txn_unit_cost = sqlite3_column_double(select_add_stmt, 2);

        int issue_qty_current_txn;

        //This checks if the unused qty of the selected add txn is more than the remain qty to be issued
        if(txn_unused_qty < remaining_qty) {
            issue_qty_current_txn = txn_unused_qty;
        } else {
            issue_qty_current_txn = remaining_qty;
        }

        remaining_qty -= issue_qty_current_txn;

        total_cost += (txn_unit_cost * issue_qty_current_txn);

        //Insert relation row 
        sqlite3_bind_int(relation_stmt, 1, issue_qty_current_txn);
        sqlite3_bind_int(relation_stmt, 2, issue_id);
        sqlite3_bind_int(relation_stmt, 3, add_txn_id);

        if (step_and_check(db, relation_stmt, 0) != 0) {
            goto txn_error;
        }


        //Update selected Stock Addition entry
        sqlite3_bind_int(update_add_stmt, 1, issue_qty_current_txn);
        sqlite3_bind_int(update_add_stmt, 2, add_txn_id);

        if (step_and_check(db, update_add_stmt, 0) != 0) {
            goto txn_error;
        }

        sqlite3_reset(relation_stmt);
        sqlite3_reset(update_add_stmt);
    }
    
    //Update Item quantities and value
    char *update_itm_sql = "UPDATE items "
                        "SET current_qty = current_qty - ?, "
                        "total_value = total_value - ? "
                        "WHERE id = ?;";

    if(prepare_stmt(db, update_itm_sql, &update_itm_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(update_itm_stmt, 1, issue_qty);
    sqlite3_bind_double(update_itm_stmt, 2, total_cost);
    sqlite3_bind_int(update_itm_stmt, 3, item.item_id);

    if (step_and_check(db, update_itm_stmt, 0) != 0) {
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
        if (select_add_stmt) sqlite3_finalize(select_add_stmt);
        if (issue_stmt) sqlite3_finalize(issue_stmt);
        if (relation_stmt) sqlite3_finalize(relation_stmt);
        if (update_itm_stmt) sqlite3_finalize(update_itm_stmt);
        if (update_add_stmt) sqlite3_finalize(update_add_stmt);
        return result;
}