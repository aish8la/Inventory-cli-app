#include "invent_menu.h"
#include "utilities.h"
#include "sqlite3.h"
#include "globals.h"
#include "query_handlers.h"
#include "sqlite_helpers.h"
#include <stdio.h>
#include "stdlib.h"

void add_stock(void) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    sqlite3_stmt *add_stmt = NULL;
    sqlite3_stmt *update_stmt = NULL;

    char input[ITEM_CODE_LENGTH + 1];
    int qty;
    double unit_cost;
    int item_id;

    char *select_sql = "SELECT item_code, item_name, id "
                        "FROM items "
                        "WHERE item_code = ?;";
    char *insert_sql = "INSERT INTO stock_additions "
                        "(added_qty, unit_cost, unused_qty, item_id) "
                        "VALUES (?, ?, ?, ?);";
    char *update_sql = "UPDATE items "
                        "SET current_qty = current_qty + ?, "
                        "total_value = total_value + ? "
                        "WHERE id = ?;";

    if(open_db(&db) != 0) {
        goto error_cleanup;
        // exit(1);
    }

    printf("Enter Item Code of Item: ");
    read_input(input, sizeof(input));

    if(prepare_stmt(db, select_sql, &stmt) == 1) {
        goto error_cleanup;
    }
    
    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);

    printf("\nItem to Add Stock\n");

    int found = display_table(db, stmt, print_item_header, print_item_row);
    if(found != 0) {
        goto error_cleanup;
    }

    //TODO: fix this issue where display table above steps and completes the stmt 
    //for now i am resetting the statement
    sqlite3_reset(stmt);
    sqlite3_step(stmt);
    item_id = sqlite3_column_int(stmt, 2);

    printf("\nEnter the Stock Addition Details;\n\n");

    printf("Enter Quantity: ");
    scanf("%d", &qty);
    clear_input_buffer();

    printf("Enter Unit Cost: ");
    scanf("%lf", &unit_cost);
    clear_input_buffer();

    if(!(qty > 0) || !(unit_cost > 0)) {
       printf("Quantity or Value should be a non zero positive number"); 
       goto error_cleanup;
    }

    printf("Total Cost is : %.2lf \n", qty * unit_cost);

    if(begin_txn(db) != 0) {
        goto error_cleanup;
    }

    if(prepare_stmt(db, insert_sql, &add_stmt) == 1) {
        goto error_cleanup;
    }

    sqlite3_bind_int(add_stmt, 1, qty);
    sqlite3_bind_double(add_stmt, 2, unit_cost);
    sqlite3_bind_double(add_stmt, 3, qty);
    sqlite3_bind_int(add_stmt, 4, item_id);

    if (step_and_check(db, add_stmt, 0) != 0) {
        goto txn_error;
    }


    if(prepare_stmt(db, update_sql, &update_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(update_stmt, 1, qty);
    sqlite3_bind_double(update_stmt, 2, unit_cost);
    sqlite3_bind_int(update_stmt, 3, item_id);

    if (step_and_check(db, update_stmt, 0) != 0) {
        goto txn_error;
    }

    if (commit_txn(db) != 0) {
        goto txn_error;
    }
 
    goto success;
    //TODO: Refactor with goto statement for cleanup


    success:
        printf("\nStock Added Successfully");
        goto cleanup;

    error_cleanup:
        printf("\nError Occurred");
        goto cleanup;

    txn_error:
        rollback_txn(db);
        goto error_cleanup;

    cleanup:
        if (stmt) sqlite3_finalize(stmt);
        if (add_stmt) sqlite3_finalize(add_stmt);
        if (update_stmt) sqlite3_finalize(update_stmt);
        if (db) sqlite3_close(db);
        wait_for_enter();
}

void issue_stock(void) {
    wait_for_enter();
}

void view_additions(void) {
    wait_for_enter();
}

void view_issues(void) {
    wait_for_enter();
}

void view_inventory(void) {
    wait_for_enter();
}