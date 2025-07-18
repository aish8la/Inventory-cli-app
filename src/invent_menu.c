#include "invent_menu.h"
#include "utilities.h"
#include "sqlite3.h"
#include "globals.h"
#include "display_table.h"
#include "sqlite_helpers.h"
#include <stdio.h>
#include "stdlib.h"
#include "init_db.h"

int add_stock(void) {
    sqlite3 *db = get_db();
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

    //TODO: May implement a function to display and also fetch the item id
    //Here i reset the above stmt since display steps through it till it is done
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
       printf("\n\nQuantity or Value should be a non zero positive number\n\n"); 
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
    sqlite3_bind_int(add_stmt, 3, qty);
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
        wait_for_enter();
        return 0;
}

int issue_stock(void) {
    sqlite3 *db = get_db();
    sqlite3_stmt *select_itm_stmt = NULL;
    sqlite3_stmt *select_add_stmt = NULL;
    sqlite3_stmt *issue_stmt = NULL;
    sqlite3_stmt *update_itm_stmt = NULL;
    sqlite3_stmt *update_add_stmt = NULL;

    char input_itm_code[ITEM_CODE_LENGTH + 1];
    int issue_qty;
    double unit_cost;
    int item_id;
    int current_qty;

    char *select_itm_sql = "SELECT item_code, item_name, current_qty, total_value, id "
                        "FROM items "
                        "WHERE item_code = ?;";
    char *select_add_txn_sql = "SELECT id, unused_qty, unit_cost "
                        "FROM stock_additions "
                        "WHERE item_id = ? AND unused_qty > 0 "
                        "ORDER BY id ASC;";                       
    char *insert_sql = "INSERT INTO stock_issues "
                        "(issued_qty, item_id, stock_addition_id) "
                        "VALUES (?, ?, ?);";
    char *update_itm_sql = "UPDATE items "
                        "SET current_qty = current_qty - ?, "
                        "total_value = total_value - ? "
                        "WHERE id = ?;";
    char *update_add_sql = "UPDATE stock_additions "
                        "SET unused_qty = unused_qty - ?, "
                        "WHERE id = ?;";

    printf("Enter Item Code of Item: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    if(prepare_stmt(db, select_itm_sql, &select_itm_stmt) == 1) {
        goto cleanup;
    }
    
    sqlite3_bind_text(select_itm_stmt, 1, input_itm_code, -1, SQLITE_TRANSIENT);

    printf("\nItem to Issue Stock\n");

    int found = display_table(db, select_itm_stmt, print_stock_header, print_stoc_row);
    if(found != 0) {
        goto cleanup;
    }

    //TODO: May implement a function to display and also fetch the item id
    //Here i reset the above stmt since display steps through it till it is done
    sqlite3_reset(select_itm_stmt);
    sqlite3_step(select_itm_stmt);
    current_qty = sqlite3_column_int(select_itm_stmt, 2);
    item_id = sqlite3_column_int(select_itm_stmt, 4);

    if(current_qty <= 0) {
        printf("\nSelected Item has no stock. Add Stock before Issue.\n");
        goto cleanup;
    }

    printf("\nEnter the Stock Issue Details;\n\n");

    printf("Enter Quantity: ");
    scanf("%d", &issue_qty);
    clear_input_buffer();

    if(!(issue_qty > 0)) {
       printf("\n\nQuantity should be a non zero positive number\n\n"); 
       goto cleanup;
    }

    if(!(issue_qty > current_qty)) {
       printf("\n\nNot enough stock to issue.\n\n"); 
       goto cleanup;
    }

    if(begin_txn(db) != 0) {
        goto cleanup;
    }

    if(prepare_stmt(db, select_add_txn_sql, &select_add_stmt) == 1) {
        goto cleanup;
    }

    sqlite3_bind_int(select_add_stmt, 1, item_id);

    while(1) {
        //TODO: Implement a loop that steps the add stmnt and allocates qty in a loop
    }


    // if (step_and_check(db, select_add_stmt, 0) != 0) {
    //     goto txn_error;
    // }


    // if(prepare_stmt(db, update_sql, &update_stmt) == 1) {
    //     goto txn_error;
    // }

    // sqlite3_bind_int(update_stmt, 1, qty);
    // sqlite3_bind_double(update_stmt, 2, unit_cost);
    // sqlite3_bind_int(update_stmt, 3, item_id);

    // if (step_and_check(db, update_stmt, 0) != 0) {
    //     goto txn_error;
    // }

    // if (commit_txn(db) != 0) {
    //     goto txn_error;
    // }
 
    goto success;

    success:
        printf("\nStock Issued Successfully");
        goto cleanup;

    txn_error:
        rollback_txn(db);
        printf("\nTransaction Error Occurred\n");
        goto cleanup;

    cleanup:
        // if (stmt) sqlite3_finalize(stmt);
        // if (add_stmt) sqlite3_finalize(add_stmt);
        // if (update_stmt) sqlite3_finalize(update_stmt);
        wait_for_enter();
        return 0;
}

int view_additions(void) {
    wait_for_enter();
    return 0;
}

int view_issues(void) {
    wait_for_enter();
    return 0;
}

int view_inventory(void) {
    wait_for_enter();
    return 0;
}