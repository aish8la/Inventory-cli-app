#include "menu.h"
#include "globals.h"
#include "invent_menu.h"
#include "item_menu.h"
#include "login.h"
#include "string.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>

// This function will return a filtered list of menu items and is a helper for
// the run_menu function It accepts the original menu list, the count for the
// list and a pointer to the returned list count variable this will use a
// dynamically allocated memory so free the memory with free() after the
// finishing
int filter_menu(Menu_Item *menu_list, int count, int *ret_arr_count,
                Menu_Item **new_list)
{
  const User *current_user = get_current_user();
  int user_access_lvl = current_user->access_level;
  *ret_arr_count = 0;

  for (int i = 0; i < count; i++)
  {
    if (menu_list[i].req_access_lvl <= user_access_lvl)
    {
      (*ret_arr_count)++;
    }
  }

  *new_list = (Menu_Item *)malloc(*ret_arr_count * sizeof(Menu_Item));

  if (*new_list == NULL)
  {
    printf("Failed to allocate memory for new list at menu.c");
    return 1;
  }

  int j = 0; // index for the new list array

  for (int i = 0; i < count; i++)
  {
    if (menu_list[i].req_access_lvl <= user_access_lvl)
    {
      (*new_list)[j] = menu_list[i];
      j++;
    }
  }

  return 0;
}

/*This is the menu runner function that will take the Menu_Item type array that
contains a list of defined menu items and display them on the CLI*/
int run_menu(const char *title, Menu_Item *items, int count)
{
  int choice;
  int filtered_count;
  Menu_Item *filtered_list = NULL;

  int err = filter_menu(items, count, &filtered_count, &filtered_list);

  if (err != 0)
  {
    return 1;
  }

  while (1)
  {
    clear_console();
    printf("\n");
    printf("================================\n");
    printf("  %s\n", title);
    printf("================================\n");
    printf("\n");

    for (int i = 0; i < filtered_count; i++)
    {
      printf("Press [%d] => %s\n", i + 1, filtered_list[i].label);
    }
    printf("Press [0] => Back\n");

    printf("Choose => ");
    scanf("%d", &choice);
    clear_input_buffer();
    clear_console();

    if (choice == 0)
    {
      break;
    }

    if (choice < 1 || choice > filtered_count)
    {

      printf("\nInvalid Choice. Try again.");
      wait_for_enter();
      continue;
    }

    printf("\n");
    printf("================================\n");
    printf("  %s\n", filtered_list[choice - 1].label);
    printf("================================\n");
    printf("\n");
    int err = filtered_list[choice - 1].action();

    if (err != 0)
    {
      if (filtered_list)
        free(filtered_list);
      return 1;
    }
  }

  if (filtered_list)
    free(filtered_list); // this frees the filtered list memory
  return 0;
}

/*This is the run main menu function that will call the run_menu function with
 * the main menu item list*/
int run_main_menu(void)
{

  Menu_Item main_menu[] = {
      {"Inventory Transactions", invent_menu, 1},
      {"Items Menu", item_menu, 1},
  };

  int menu_count = sizeof(main_menu) / sizeof(main_menu[0]);

  const char *title = "MAIN MENU";

  int err = run_menu(title, main_menu, menu_count);

  if (err != 0)
    return 1;

  return 0;
}

int item_menu(void)
{

  Menu_Item item_menu[] = {
      {"Add Item", add_item, 2},
      {"View Items", view_items, 1},
      {"Search Item", search_item, 1},
      {"Edit Item", edit_item, 2},
      {"Delete Item", delete_item, 3},
  };

  int menu_count = sizeof(item_menu) / sizeof(item_menu[0]);

  const char *title = "ITEM MENU";

  int err = run_menu(title, item_menu, menu_count);

  if (err != 0)
    return 1;

  return 0;
}

int invent_menu(void)
{

  Menu_Item invent_menu[] = {
      {"Stock Addition Menu", stock_add_menu, 1},
      {"Stock Issue Menu", stock_issue_menu, 1},
      {"View Inventory", view_inventory, 1},
  };

  int menu_count = sizeof(invent_menu) / sizeof(invent_menu[0]);

  const char *title = "INVENTORY MENU";

  int err = run_menu(title, invent_menu, menu_count);

  if (err != 0)
    return 1;

  return 0;
}

int stock_add_menu(void)
{

  Menu_Item add_menu[] = {
      {"Add Stock", add_stock, 2},
      {"View Stock Additions", view_additions, 1},
      {"Delete Stock Addition Entry", delete_stock_addition, 3},
  };

  int menu_count = sizeof(add_menu) / sizeof(add_menu[0]);

  const char *title = "STOCK ADDITION MENU";

  int err = run_menu(title, add_menu, menu_count);

  if (err != 0)
    return 1;

  return 0;
}

int stock_issue_menu(void)
{

  Menu_Item issue_menu[] = {
      {"Issue Stock", issue_stock, 2},
      {"View Issues", view_issues, 1},
      {"Delete Stock Issue Entry", delete_stock_issue, 3}};

  int menu_count = sizeof(issue_menu) / sizeof(issue_menu[0]);

  const char *title = "STOCK ISSUE MENU";

  int err = run_menu(title, issue_menu, menu_count);

  if (err != 0)
    return 1;

  return 0;
}