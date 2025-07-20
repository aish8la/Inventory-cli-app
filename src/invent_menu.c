#include "invent_menu.h"
#include "utilities.h"
#include "sqlite3.h"
#include "globals.h"
#include "display_table.h"
#include "sqlite_helpers.h"
#include <stdio.h>
#include "stdlib.h"
#include "init_db.h"
#include "db_functions.h"

int add_stock(void) {
    char input_itm_code[ITEM_CODE_LENGTH];
    int qty;
    double unit_cost;
    Item item;
    
    printf("Enter Item Code of Item: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    int result = db_get_item_by_code(input_itm_code, &item);
    if(result != D_SUCCESS) {
        printf("\n\nItem could not be found\n");
        goto cleanup;
    }

    printf("\nItem to Add Stock\n");

    display_selected_item(&item);

    printf("\nEnter the Stock Addition Details;\n\n");

    printf("Enter Quantity: ");
    scanf("%d", &qty);
    clear_input_buffer();

    printf("Enter Unit Cost: ");
    scanf("%lf", &unit_cost);
    clear_input_buffer();

    if(qty <= 0 || unit_cost <= 0) {
       printf("\n\nQuantity or Value should be a non zero positive number\n\n"); 
       goto cleanup;
    }

    printf("Total Cost is : %.2lf \n", qty * unit_cost);

    result = db_add_stock(item.item_id, qty, unit_cost);

    if(result == D_SUCCESS) {
        printf("\n\nStock Added Successfully\n");
    } else {
        printf("\n\nError Adding Stock\n");
    }
 
    goto cleanup;

    cleanup:
        wait_for_enter();
        return 0;
}

int issue_stock(void) {
    char input_itm_code[ITEM_CODE_LENGTH + 1];
    int issue_qty;
    Item item;

    printf("Enter Item Code of Item: ");
    read_input(input_itm_code, sizeof(input_itm_code));

    int result = db_get_item_by_code(input_itm_code, &item);
    if(result != D_SUCCESS) {
        printf("\n\nItem could not be found\n");
        goto cleanup;
    }

    printf("\nItem to Issue Stock\n");

    display_selected_item_qty(&item);

    if(item.current_qty <= 0) {
        printf("\nSelected Item has no stock. Add Stock before Issue.\n");
        goto cleanup;
    }


    printf("\nEnter the Stock Issue Details;\n\n");

    printf("Enter Quantity: ");
    scanf("%d", &issue_qty);
    clear_input_buffer();

    if(issue_qty <= 0) {
       printf("\n\nQuantity should be a non zero positive number\n\n"); 
       goto cleanup;
    }

    if(issue_qty > item.current_qty) {
       printf("\n\nNot enough stock to issue.\n\n"); 
       goto cleanup;
    }

    result = db_issue_stock(item, issue_qty);

    switch (result)
    {
    case D_SUCCESS:
        printf("\nStock Issued Successfully");
        break;
    case D_NOT_ENOUGH_FIFO_STOCK:
        printf("\nNot enough FIFO stock available. Data Inconsistent.");
        break;          
    default:
        printf("\nError issuing stock");
        break;
    }
 
    goto cleanup;

    cleanup:
        wait_for_enter();
        return 0;
}

int view_additions(void) {
    Stock_Addition *additions = NULL;
    int count = 0;

    int result = db_get_all_stock_additions(&additions, &count);

    switch (result) {
        case D_SUCCESS:
            display_additions_table(additions, count, format_additions_header, format_additions_row);
            break;
        case D_NOT_FOUND:
            printf("\nNo stock additions found.\n");
            break;
        default:
            printf("\nError retrieving stock additions.\n");
            break;
    }

    if (additions) {
        free(additions);
    }

    wait_for_enter();
    return 0;
}

int view_issues(void) {
    Stock_Issue *issues = NULL;
    int count = 0;

    int result = db_get_all_stock_issues(&issues, &count);

    switch (result) {
    case D_SUCCESS:
        display_issues_table(issues, count, format_issues_header, format_issues_row);
        break;
    case D_NOT_FOUND:
        printf("\nNo stock issues found.\n");
        break;
    default:
        printf("\nError retrieving stock issues.\n");
        break;
    }

    if (issues) {
        free(issues);
    }

    wait_for_enter();
    return 0;
}

int view_inventory(void) {
    wait_for_enter();
    return 0;
}