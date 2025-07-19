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
    sqlite3_stmt *relation_stmt = NULL;
    sqlite3_stmt *update_itm_stmt = NULL;
    sqlite3_stmt *update_add_stmt = NULL;

    char input_itm_code[ITEM_CODE_LENGTH + 1];
    int issue_qty;
    int item_id;
    int current_qty;
    int issue_id;

    char *select_itm_sql = "SELECT item_code, item_name, current_qty, total_value, id "
                        "FROM items "
                        "WHERE item_code = ?;";
    char *select_add_txn_sql = "SELECT id, unused_qty, unit_cost "
                        "FROM stock_additions "
                        "WHERE item_id = ? AND unused_qty > 0 "
                        "ORDER BY id ASC;";
    char *insert_issue_sql = "INSERT INTO stock_issues "
                        "(issued_qty, item_id) "
                        "VALUES (?, ?);";
    char *insert_relation_sql = "INSERT INTO stock_issues_add_relation "
                        "(issued_qty, stock_issues_id, stock_addition_id) "
                        "VALUES (?, ?, ?);";
    char *update_itm_sql = "UPDATE items "
                        "SET current_qty = current_qty - ?, "
                        "total_value = total_value - ? "
                        "WHERE id = ?;";
    char *update_add_sql = "UPDATE stock_additions "
                        "SET unused_qty = unused_qty - ? "
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

    if(issue_qty > current_qty) {
       printf("\n\nNot enough stock to issue.\n\n"); 
       goto cleanup;
    }

    if(begin_txn(db) != 0) {
        goto cleanup;
    }

    if(prepare_stmt(db, insert_issue_sql, &issue_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(issue_stmt, 1, issue_qty);
    sqlite3_bind_int(issue_stmt, 2, item_id);

    if (step_and_check(db, issue_stmt, 0) != 0) {
        goto txn_error;
    }

    issue_id = (int)sqlite3_last_insert_rowid(db);

    if(prepare_stmt(db, select_add_txn_sql, &select_add_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(select_add_stmt, 1, item_id);

    if(prepare_stmt(db, insert_relation_sql, &relation_stmt) == 1) {
        goto txn_error;
    }

    if(prepare_stmt(db, update_add_sql, &update_add_stmt) == 1) {
        goto txn_error;
    }

    int remaining_qty = issue_qty;
    double total_cost = 0;

    while(1) {
        int add_txn_id;
        int txn_unused_qty;
        double txn_unit_cost;
        int issue_qty_current_txn = remaining_qty;

        int rc = sqlite3_step(select_add_stmt);

        if(rc != SQLITE_ROW) {
            if(rc != SQLITE_DONE) {
                fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
            } else {
                break;
            }
        }

        add_txn_id = sqlite3_column_int(select_add_stmt, 0);
        txn_unused_qty = sqlite3_column_int(select_add_stmt, 1);
        txn_unit_cost = sqlite3_column_double(select_add_stmt, 2);

        if(txn_unused_qty < issue_qty_current_txn) {
            issue_qty_current_txn = txn_unused_qty;
            remaining_qty -= txn_unused_qty;
        }

        total_cost += (txn_unit_cost * issue_qty_current_txn);



        sqlite3_bind_int(relation_stmt, 1, issue_qty_current_txn);
        sqlite3_bind_int(relation_stmt, 2, issue_id);
        sqlite3_bind_int(relation_stmt, 3, add_txn_id);

        if (step_and_check(db, relation_stmt, 0) != 0) {
            goto txn_error;
        }



        sqlite3_bind_int(update_add_stmt, 1, issue_qty_current_txn);
        sqlite3_bind_int(update_add_stmt, 2, add_txn_id);

        if(remaining_qty <= 0) {
            break;
        }

        sqlite3_reset(relation_stmt);
        sqlite3_reset(update_add_stmt);

    }

    if (remaining_qty > 0) {
        printf("\nNot enough FIFO stock available to issue. Transaction aborted.\n");
        goto txn_error;
    }


    if(prepare_stmt(db, update_itm_sql, &update_itm_stmt) == 1) {
        goto txn_error;
    }

    sqlite3_bind_int(update_itm_stmt, 1, issue_qty);
    sqlite3_bind_double(update_itm_stmt, 2, total_cost);
    sqlite3_bind_int(update_itm_stmt, 3, item_id);

    if (step_and_check(db, relation_stmt, 0) != 0) {
        goto txn_error;
    }

    if (commit_txn(db) != 0) {
        goto txn_error;
    }
 
 
    goto success;

    success:
        printf("\nStock Issued Successfully");
        goto cleanup;

    txn_error:
        rollback_txn(db);
        printf("\nTransaction Error Occurred\n");
        goto cleanup;

    cleanup:
        if (select_itm_stmt) sqlite3_finalize(select_itm_stmt);
        if (select_add_stmt) sqlite3_finalize(select_add_stmt);
        if (issue_stmt) sqlite3_finalize(issue_stmt);
        if (relation_stmt) sqlite3_finalize(relation_stmt);
        if (update_itm_stmt) sqlite3_finalize(update_itm_stmt);
        if (update_add_stmt) sqlite3_finalize(update_add_stmt);
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