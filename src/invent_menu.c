#include "invent_menu.h"
#include "db_functions.h"
#include "display_table.h"
#include "globals.h"
#include "init_db.h"
#include "sqlite3.h"
#include "sqlite_helpers.h"
#include "stdlib.h"
#include "utilities.h"
#include <stdio.h>

int add_stock(void) {
  char input_itm_code[ITEM_CODE_LENGTH];
  int qty;
  double unit_cost;
  Item item;

  printf("Enter Item Code of Item: ");
  read_input(input_itm_code, sizeof(input_itm_code));

  int result = db_get_item_by_code(input_itm_code, &item);
  if (result != D_SUCCESS) {
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

  if (qty <= 0 || unit_cost <= 0) {
    printf("\n\nQuantity or Value should be a non zero positive number\n\n");
    goto cleanup;
  }

  printf("Total Cost is : %.2lf \n", qty * unit_cost);

  result = db_add_stock(item.item_id, qty, unit_cost);

  if (result == D_SUCCESS) {
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
  if (result != D_SUCCESS) {
    printf("\n\nItem could not be found\n");
    goto cleanup;
  }

  printf("\nItem to Issue Stock\n");

  display_selected_item_qty(&item);

  if (item.current_qty <= 0) {
    printf("\nSelected Item has no stock. Add Stock before Issue.\n");
    goto cleanup;
  }

  printf("\nEnter the Stock Issue Details;\n\n");

  printf("Enter Quantity: ");
  scanf("%d", &issue_qty);
  clear_input_buffer();

  if (issue_qty <= 0) {
    printf("\n\nQuantity should be a non zero positive number\n\n");
    goto cleanup;
  }

  if (issue_qty > item.current_qty) {
    printf("\n\nNot enough stock to issue.\n\n");
    goto cleanup;
  }

  result = db_issue_stock(item, issue_qty);

  switch (result) {
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
    display_additions_table(additions, count, format_additions_header,
                            format_additions_row);
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
    display_issues_table(issues, count, format_issues_header,
                         format_issues_row);
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
  Item *items = NULL;
  int count = 0;

  int result = db_get_all_items(&items, &count);
  if (result == D_NOT_FOUND) {
    printf("No items found in Inventory.\n");
  } else if (result != D_SUCCESS) {
    printf("\nDatabase Error\n\n");
  } else {
    display_item_table(items, count, format_inventory_header,
                       format_inventory_row);
  }

  if (items)
    free(items);
  wait_for_enter();
  return 0;
}

int delete_stock_addition(void) {
  int addition_id;
  Stock_Addition addition;

  printf("\n====WARNING!! DELETING STOCK ADDITIONS COULD RESULT IN INCORRECT "
         "FIFO ALLOCATION!!====\n");

  printf("Enter Stock Addition ID to Delete: ");
  scanf("%d", &addition_id);
  clear_input_buffer();

  // Check if addition exists and display
  int result = db_get_stock_addition_by_id(addition_id, &addition);
  if (result != D_SUCCESS) {
    printf("Stock Addition with ID '%d' not found.\n", addition_id);
    goto cleanup;
  }

  printf("\nAddition Selected for Deletion\n");
  display_selected_addition(&addition);

  // Check if any quantity has been used
  if (addition.unused_qty != addition.added_qty) {
    printf("\nWARNING: This addition has been used. Cannot Delete.\n");
    goto cleanup;
  }

  // Prompt user for confirmation
  char *prompt = "\nConfirm Delete Operation ?\n";
  char *cancel_msg = "\nCancelled Delete Operation.\n";

  if (get_user_confirmation(prompt, cancel_msg) != 0) {
    goto cleanup;
  }

  // Perform the delete operation
  result = db_delete_stock_addition(addition);
  switch (result) {
  case D_SUCCESS:
    printf("Stock Addition [%d] Deleted Successfully", addition_id);
    break;
  case D_FOREIGNKEY_VIOLATION:
    printf("This stock addition has been used in stock issues and cannot be "
           "deleted.");
    break;
  default:
    printf("Error deleting stock addition");
    break;
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}

int delete_stock_issue(void) {
  int issue_id;
  Stock_Issue issue;

  printf("\n====WARNING!! DELETING STOCK ISSUES COULD RESULT IN INCORRECT FIFO "
         "ALLOCATION!!====\n");

  printf("Enter Stock Issue ID to Delete: ");
  scanf("%d", &issue_id);
  clear_input_buffer();

  // Check if issue exists and display
  int result = db_get_stock_issue_by_id(issue_id, &issue);
  if (result != D_SUCCESS) {
    printf("Stock Issue with ID '%d' not found.\n", issue_id);
    goto cleanup;
  }

  printf("\nStock Issue Selected for Delete Operation\n");
  display_selected_stock_issue(&issue);

  // Prompt user for confirmation
  char *prompt = "\nConfirm Delete Operation ?\n";
  char *cancel_msg = "\nCancelled Delete Operation.\n";

  if (get_user_confirmation(prompt, cancel_msg) != 0) {
    goto cleanup;
  }

  // Perform the delete operation
  result = db_delete_stock_issue(issue);
  switch (result) {
  case D_SUCCESS:
    printf("Stock Issue [%d] Deleted Successfully", issue_id);
    break;
  default:
    printf("Error deleting stock issue");
    break;
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}