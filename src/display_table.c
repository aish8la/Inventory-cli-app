#include "globals.h"
#include "sqlite3.h"
#include <stdio.h>

// Item Headers
void format_item_header(void) {
  printf("\n%-5s %-15s %-30s\n", "Sn.", "Item Code", "Item Name");
  printf("%-5s %-15s %-30s\n", "----", "----------", "---------");
}

// Item Rows
void format_item_row(const Item *item, int index) {
  printf("%-5d %-15s %-30s\n", index + 1, item->item_code, item->item_name);
}

// Inventory Header
void format_inventory_header(void) {
  printf("\n%-5s %-15s %-30s %-12s %-12s\n", "Sn.", "Item Code", "Item Name",
         "Current Qty", "Total Value");
  printf("%-5s %-15s %-30s %-12s %-12s\n", "----", "----------", "---------",
         "-----------", "-----------");
}

// Inventory Row
void format_inventory_row(const Item *item, int index) {
  printf("%-5d %-15s %-30s %-12d %-12.2f\n", index + 1, item->item_code,
         item->item_name, item->current_qty, item->total_value);
}

// Display Item Rows
void display_item_table(Item *items, int count, void (*print_header)(void),
                        void (*print_row)(const Item *, int)) {
  if (!items || count <= 0) {
    printf("No items to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++) {
    print_row(&items[i], i);
  }

  printf("\nTotal Items: %d\n", count);
}

void display_selected_item(const Item *item) {
  printf("Code: [%s]\nName: [%s]\n\n", item->item_code, item->item_name);
}

void display_selected_item_qty(const Item *item) {
  printf("Code: [%s]\nName: [%s]\nAvailable Qty: [%d]\n\n", item->item_code,
         item->item_name, item->current_qty);
}

void display_selected_addition(const Stock_Addition *addition) {
  printf("Addition ID   : [%d]\n", addition->addition_id);
  printf("Item Code     : [%s]\n", addition->item_code);
  printf("Item Name     : [%s]\n", addition->item_name);
  printf("Added Qty     : [%d]\n", addition->added_qty);
  printf("Unit Cost     : [%.2f]\n", addition->unit_cost);
  printf("Unused Qty    : [%d]\n", addition->unused_qty);
  printf("Total Cost    : [%.2f]\n\n", addition->total_cost);
}

void display_selected_stock_issue(const Stock_Issue *issue) {
  printf("Issue ID      : [%d]\n", issue->issue_id);
  printf("Item Code     : [%s]\n", issue->item_code);
  printf("Item Name     : [%s]\n", issue->item_name);
  printf("Issued Qty    : [%d]\n", issue->issued_qty);
  printf("Total Cost    : [%.2f]\n\n", issue->total_cost);
}

// Print Header Formats
// Additions Headers
//  Print header for Stock_Addition list
void format_additions_header(void) {
  printf("\n%-5s  %-8s  %-15s  %-30s  %-10s  %-10s  %-10s  %-12s\n", "Sn.",
         "Add.ID", "Item Code", "Item Name", "AddedQty", "UnitCost",
         "UnusedQty", "TotalCost");
  printf("%-5s  %-8s  %-15s  %-30s  %-10s  %-10s  %-10s  %-12s\n", "----",
         "-------", "---------", "---------", "--------", "--------",
         "--------", "---------");
}

// Print one row for a Stock_Addition
void format_additions_row(const Stock_Addition *addition, int index) {
  printf("%-5d  %-8d  %-15s  %-30s  %-10d  %-10.2f  %-10d  %-12.2f\n",
         index + 1, addition->addition_id, addition->item_code,
         addition->item_name, addition->added_qty, addition->unit_cost,
         addition->unused_qty, addition->total_cost);
}

// Display Addition Table
void display_additions_table(Stock_Addition *additions, int count,
                             void (*print_header)(void),
                             void (*print_row)(const Stock_Addition *, int)) {
  if (!additions || count <= 0) {
    printf("No Additions to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++) {
    print_row(&additions[i], i);
  }

  printf("\nTotal Additions: %d\n", count);
}

// Stock Issues Display

// Print header for StockIssue list
void format_issues_header(void) {
  printf("\n"
         "%-5s  %-8s  %-15s  %-30s  %-10s  %-12s\n",
         "Sn.", "IssueID", "Item Code", "Item Name", "IssuedQty", "TotalCost");
  printf("%-5s  %-8s  %-15s  %-30s  %-10s  %-12s\n", "----", "--------",
         "---------", "---------", "---------", "---------");
}

// Print one row for a StockIssue
void format_issues_row(const Stock_Issue *issue, int index) {
  printf("%-5d  %-8d  %-15s  %-30s  %-10d  %-12.2f\n", index + 1,
         issue->issue_id, issue->item_code, issue->item_name, issue->issued_qty,
         issue->total_cost);
}

// Display Issues Table
void display_issues_table(Stock_Issue *issues, int count,
                          void (*print_header)(void),
                          void (*print_row)(const Stock_Issue *, int)) {
  if (!issues || count <= 0) {
    printf("No Issues to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++) {
    print_row(&issues[i], i);
  }

  printf("\nTotal Issues: %d\n", count);
}