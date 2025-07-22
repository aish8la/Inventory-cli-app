/*
 * Combined C Source File
 * Project: Inventory Management System
 * 
 * 
 * Headers combined: globals.h, utilities.h, sqlite_helpers.h, login.h, display_table.h, menu.h, item_menu.h, invent_menu.h, db_functions.h, init_db.h
 * Sources combined: utilities.c, sqlite_helpers.c, init_db.c, db_functions.c, login.c, display_table.c, item_menu.c, invent_menu.c, menu.c, main.c
 */

// ==================== STANDARD LIBRARY INCLUDES ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==================== SQLITE AMALGAMATION NOTICE ====================
// NOTE: This project depends on SQLite.
// To compile, ensure that `sqlite3.h` and `sqlite3.c` (amalgamation) are present in the same folder.
// The following include assumes that `sqlite3.h` is available locally.
#include "sqlite3.h"

// ==================== HEADERS ====================

// ----- globals.h -----
// CONSTRAINTS

// Items Constraints
#define ITEM_CODE_LENGTH 8
#define ITEM_NAME_LENGTH 30

// User Constraints
#define MAX_USER_PASSWORD_LENGTH 15
#define MAX_NUMBER_OF_USERS 5
#define MAX_LOGIN_ATTEMPTS 3
#define MAX_USER_NAME_LENGTH 15

// STRUCTS

// Login Related Structs
typedef struct
{
  char username[MAX_USER_NAME_LENGTH];
  char password[MAX_USER_PASSWORD_LENGTH];
  int access_level; // user access level 1 to 3 with 3 being the highest access
                    // level and 1 being read only with limited access
} User;

typedef struct
{
  User user_arr[MAX_NUMBER_OF_USERS];
  int user_count;
} User_Array;

// Item Structu
typedef struct
{
  int item_id;
  char item_code[ITEM_CODE_LENGTH + 1];
  char item_name[ITEM_NAME_LENGTH + 1];
  int current_qty;
  double total_value;
} Item;

typedef struct
{
  int addition_id;
  int item_id;
  char item_code[ITEM_CODE_LENGTH + 1];
  char item_name[ITEM_NAME_LENGTH + 1];
  int added_qty;
  double unit_cost;
  int unused_qty;
  double total_cost;
} Stock_Addition;

typedef struct
{
  int issue_id;
  int item_id;
  char item_code[ITEM_CODE_LENGTH + 1];
  char item_name[ITEM_NAME_LENGTH + 1];
  int issued_qty;
  double total_cost;
} Stock_Issue;

/*This is a type def of a Menu item structure which will hold the pointer to the
menu item function and the label of the menu item this allows for dynamic menu
generation*/
typedef struct
{
  const char *label;
  int (*action)(void);
  int req_access_lvl; // check login.h for more
} Menu_Item;

// ----- utilities.h -----
void read_input(char *str, int size);

void clear_console(void);

void wait_for_enter(void);

void clear_input_buffer(void);

int get_user_confirmation(const char *prompt_msg, const char *cancel_msg);

// ----- sqlite_helpers.h -----
int run_sql(sqlite3 *db, const char *sql);

int run_sql_with_cb(sqlite3 *db, const char *sql,
                    int (*callback)(void *, int, char **, char **),
                    void *cbdata);

int prepare_stmt(sqlite3 *db, char *sql, sqlite3_stmt **stmt);

int step_and_check(sqlite3 *db, sqlite3_stmt *stmt, int expect_row);

int begin_txn(sqlite3 *db);
int rollback_txn(sqlite3 *db);
int commit_txn(sqlite3 *db);

// ----- login.h -----
int login(void);
const User *get_current_user(void);

// ----- display_table.h -----
void format_inventory_header(void);
void format_inventory_row(const Item *item, int index);
void format_item_header(void);
void format_item_row(const Item *item, int index);
void display_item_table(Item *items, int count, void (*print_header)(void),
                        void (*print_row)(const Item *, int));

void display_selected_item(const Item *item);
void display_selected_item_qty(const Item *item);
void display_selected_addition(const Stock_Addition *addition);
void display_selected_stock_issue(const Stock_Issue *issue);

void format_additions_header(void);
void format_additions_row(const Stock_Addition *addition, int index);
void display_additions_table(Stock_Addition *additions, int count,
                             void (*print_header)(void),
                             void (*print_row)(const Stock_Addition *, int));

void format_issues_header(void);
void format_issues_row(const Stock_Issue *issue, int index);
void display_issues_table(Stock_Issue *issues, int count,
                          void (*print_header)(void),
                          void (*print_row)(const Stock_Issue *, int));

// ----- menu.h -----
int run_main_menu(void);

int invent_menu(void);
int item_menu(void);
int stock_add_menu(void);
int stock_issue_menu(void);

// ----- item_menu.h -----
int add_item(void);
int view_items(void);
int search_item(void);
int edit_item(void);
int delete_item(void);

// ----- invent_menu.h -----
int add_stock(void);
int issue_stock(void);
int view_additions(void);
int view_issues(void);
int view_inventory(void);
int delete_stock_addition(void);
int delete_stock_issue(void);

// ----- db_functions.h -----
enum db_error
{
  D_SUCCESS,
  D_ERROR,
  D_NOT_FOUND,
  D_NOT_ENOUGH_STOCK,
  D_NOT_ENOUGH_FIFO_STOCK,
  D_UNIQUE_CONSTRAINT_VIOLATION,
  D_MEMORY_ALLOC_FAILED,
  D_FOREIGNKEY_VIOLATION,
};

// Item Functions
int db_add_item(const char *item_code, const char *item_name);
int db_get_all_items(Item **items, int *count);
int db_get_item_by_code(const char *input_item_code, Item *item);
int db_update_item(const char *old_item_code, const char *new_item_code,
                   const char *new_item_name);
int db_delete_item(const char *item_code);

// Inventory Functions
int db_add_stock(int item_id, int qty, double unit_cost);
int db_issue_stock(Item item, int issue_qty);
int db_get_all_stock_additions(Stock_Addition **additions, int *count);
int db_get_all_stock_issues(Stock_Issue **issues, int *count);
int db_get_stock_addition_by_id(int addition_id, Stock_Addition *addition);
int db_delete_stock_addition(Stock_Addition addition);
int db_get_stock_issue_by_id(int issue_id, Stock_Issue *issue);
int db_delete_stock_issue(Stock_Issue issue);

// ----- init_db.h -----
int initialize_db(void);

int connect_db(const char *db_name);
sqlite3 *get_db(void);
void disconnect_db(void);

// ==================== SOURCES ====================

// ----- utilities.c -----
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*This function will read the input into a buffer, replace \n with \0 if present
and clear the input buffer of chars if buffer overflows and if input buffer has
any leftover due to it*/
void read_input(char *str, int size)
{

  fgets(str, size, stdin);

  char *str_ptr = strchr(str, '\n');

  if (str_ptr != NULL)
  {
    *str_ptr = '\0';
  }
  else
  {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
      ;
  }
}

// A macro to set the clear_console function based on operating system
#include <stdlib.h>

void clear_console(void)
{
#ifdef _WIN32
  system("cls"); // Windows
#else
  system("clear"); // Unix/Linux/Mac
#endif
}

/*A customer function to wait for input even if input buffer has unconsumed
input this will consume input with getchar.*/
void wait_for_enter(void)
{
  int ch;

  printf("\nPress Enter to continue...");
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

void clear_input_buffer(void)
{
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

int get_user_confirmation(const char *prompt_msg, const char *cancel_msg)
{

  char answer;

  printf("%s [Y]es / [N]o => ", prompt_msg);
  scanf("%c", &answer);
  clear_input_buffer();

  if (tolower(answer) != 'y')
  {
    printf("%s", cancel_msg);
    return 1;
  }

  return 0;
}

// ----- sqlite_helpers.c -----
#include <stdio.h>

int run_sql(sqlite3 *db, const char *sql)
{
  int rc;
  char *err_msg = NULL;

  rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL Error: %s\n", err_msg);
    sqlite3_free(err_msg);
    err_msg = NULL;
    wait_for_enter();
    return 1;
  }

  return 0;
}

int run_sql_with_cb(sqlite3 *db, const char *sql,
                    int (*callback)(void *, int, char **, char **),
                    void *cbdata)
{

  int rc;
  char *err_msg = NULL;

  rc = sqlite3_exec(db, sql, callback, cbdata, &err_msg);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL Error: %s\n", err_msg);
    sqlite3_free(err_msg);
    err_msg = NULL;
    wait_for_enter();
    return 1;
  }

  return 0;
}

int prepare_stmt(sqlite3 *db, char *sql, sqlite3_stmt **stmt)
{

  int rc = sqlite3_prepare_v2(db, sql, -1, stmt, NULL);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
    wait_for_enter();
    return 1;
  }

  return 0;
}

int step_and_check(sqlite3 *db, sqlite3_stmt *stmt, int expect_row)
{
  int rc = sqlite3_step(stmt);

  if (expect_row)
  {
    if (rc == SQLITE_DONE)
    {
      printf("No matching records found.\n");
      return 1;
    }
    else if (rc != SQLITE_ROW)
    {
      fprintf(stderr, "SQLite Error: %s\n", sqlite3_errmsg(db));
      return -1;
    }
  }
  else
  {
    if (rc != SQLITE_DONE)
    {
      fprintf(stderr, "SQLite Error: %s\n", sqlite3_errmsg(db));
      return -1;
    }
  }

  return 0;
}

int begin_txn(sqlite3 *db)
{
  if (run_sql(db, "BEGIN TRANSACTION;") != 0)
  {
    return 1;
  }
  return 0;
}

int rollback_txn(sqlite3 *db)
{
  if (run_sql(db, "ROLLBACK;") != 0)
  {
    return 1;
  }
  return 0;
}

int commit_txn(sqlite3 *db)
{
  if (run_sql(db, "COMMIT;") != 0)
  {
    rollback_txn(db);
    return 1;
  }
  return 0;
}

// ----- init_db.c -----
#include <stdio.h>
#include <stdlib.h>

static sqlite3 *db_instance = NULL;

int connect_db(const char *db_name)
{

  if (db_instance != NULL)
  {
    return 0;
  }

  int rc = sqlite3_open(db_name, &db_instance);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Database could not be opened: %s\n",
            sqlite3_errmsg(db_instance));
    db_instance = NULL;
    wait_for_enter();
    return 1;
  }

  rc = sqlite3_exec(db_instance, "PRAGMA foreign_keys = ON", 0, 0, NULL);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Foreign Key could not be enabled: %s\n",
            sqlite3_errmsg(db_instance));
    db_instance = NULL;
    wait_for_enter();
    return 1;
  }

  return 0;
}

sqlite3 *get_db(void) { return db_instance; }

void disconnect_db(void)
{
  if (db_instance != NULL)
  {
    sqlite3_close(db_instance);
    db_instance = NULL;
  }
}

// This is a callback for sqlite_exec to check 1 flag value from the database
// and write it to the var address give to the sqlite_exec function
int flag_value_callback(void *data, int argc, char **argv,
                        char **arg_col_name)
{
  int *flag_var =
      (int *)data; // typecasting the generic pointer into a integer pointer
  if (argc > 0 && argv[0])
  {
    *flag_var = atoi(argv[0]);
  }
  else
  {
    *flag_var = 0;
  }

  return 0;
}

// returns 0 if the current run is not the first time running (by checking the
// flags table). returns 1 in case of sqlite errors or first time running;
int check_init_flag(void)
{
  sqlite3 *db = get_db();
  int flag_value = 0;
  char *set_init_flag = "CREATE TABLE IF NOT EXISTS flags ("
                        "flag_name  TEXT UNIQUE,"
                        "flag_value INT"
                        ");"
                        "INSERT OR IGNORE INTO flags (flag_name, flag_value) "
                        "VALUES ('not_initial_run', 0);";

  char *get_init_flag =
      "SELECT flag_value FROM flags WHERE flag_name = 'not_initial_run';";

  if (run_sql(db, set_init_flag) != 0)
  {
    return 1;
  }

  if (run_sql_with_cb(db, get_init_flag, flag_value_callback, &flag_value) !=
      0)
  {
    return 1;
  }

  if (flag_value)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// These are the initial queries to run
const char *initial_queries[] = {
    // 1) The first query creates an item table
    "CREATE TABLE IF NOT EXISTS items ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "item_code TEXT UNIQUE NOT NULL,"
    "item_name TEXT,"
    "current_qty INTEGER DEFAULT 0,"
    "total_value REAL DEFAULT 0.0);",

    // 2)This creates the item addition table
    "CREATE TABLE IF NOT EXISTS stock_additions ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "added_qty INTEGER CHECK(added_qty != 0),"
    "unit_cost REAL DEFAULT 0,"
    "unused_qty INTEGER,"
    "item_id INTEGER NOT NULL, "
    "FOREIGN KEY(item_id) REFERENCES items (id));",

    // 3)This creates the item issue table
    "CREATE TABLE IF NOT EXISTS stock_issues ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "issued_qty INTEGER CHECK(issued_qty != 0),"
    "item_id INTEGER NOT NULL, "
    "FOREIGN KEY(item_id) REFERENCES items (id));",

    // 4)This creates the relationship table for stock issues and stock
    // additions
    "CREATE TABLE IF NOT EXISTS stock_issues_add_relation ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "issued_qty INTEGER CHECK(issued_qty != 0),"
    "stock_issues_id INTEGER NOT NULL, "
    "stock_addition_id INTEGER NOT NULL, "
    "FOREIGN KEY(stock_issues_id) REFERENCES stock_issues (id), "
    "FOREIGN KEY(stock_addition_id) REFERENCES stock_additions (id));",

    // 5)This query updates flag value
    "UPDATE flags SET flag_value = 1 WHERE flag_name = 'not_initial_run';",

    // 6)These queries insert items
    "INSERT INTO items (item_code, item_name) VALUES "
    "('ITM-001', 'PENCIL'),"
    "('ITM-002', 'ERASER'),"
    "('ITM-003', 'MARKER'),"
    "('ITM-004', 'RULER'),"
    "('ITM-005', 'SHARPENER');"};

int initialize_db(void)
{

  if (connect_db("data.db") != 0)
  {
    return 1;
  }

  sqlite3 *db = get_db();

  /*check init returns 0 if the current run is not the first time so this check
  will return without running database initialization*/
  if (!check_init_flag())
  {
    return 0;
  }

  int query_count = sizeof(initial_queries) / sizeof(initial_queries[0]);

  for (int i = 0; i < query_count; i++)
  {
    if (run_sql(db, initial_queries[i]) != 0)
    {
      return 1;
    }
  }

  return 0;
}

// ----- db_functions.c -----
#include <stdio.h>

// Item Menu DB Functions
int db_add_item(const char *item_code, const char *item_name)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  int result = D_ERROR;

  // The ? are binding parameters to whom, values will be bound to using
  // sqlite3_bind* functions
  char *sql = "INSERT INTO items (item_code, item_name) "
              "VALUES (?, ?);";

  /*Prepare statement is where the sql statement is translated into byte code
  for the statement to be run (this is wrapper function that will run the
  sqlite3_prepare_v2 and also handle errors)*/
  if (prepare_stmt(db, sql, &stmt) == 1)
  {
    goto cleanup;
  }

  // This is where the values from the variables are bound to the sql statement
  // bind parameters as mentioned above
  sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_TRANSIENT);

  // sqlite3_step is used to execute the insert using the prepared statement
  if (step_and_check(db, stmt, 0) != 0)
  {
    if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_UNIQUE)
    {
      result = D_UNIQUE_CONSTRAINT_VIOLATION;
    }
    goto cleanup;
  }

  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}

int db_get_all_items(Item **items, int *count)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  *items = NULL;
  *count = 0;
  int result = D_ERROR;

  char *sql = "SELECT id, item_code, item_name, current_qty, total_value "
              "FROM items;";

  if (prepare_stmt(db, sql, &stmt) == 1)
  {
    goto cleanup;
  }

  int row_count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    row_count++;
  }

  if (row_count == 0)
  {
    result = D_NOT_FOUND;
    goto cleanup;
  }

  // Allocate memory
  *items = malloc(sizeof(Item) * row_count);
  if (!*items)
  {
    result = D_MEMORY_ALLOC_FAILED;
    goto cleanup;
  }

  sqlite3_reset(stmt);
  int i = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW && i < row_count)
  {
    Item *item =
        &(*items)[i]; // Dereferences the pointer to the array, gets the i-th
                      // Item and passes the memory address of that to the *item

    item->item_id = sqlite3_column_int(stmt, 0);

    const unsigned char *code = sqlite3_column_text(stmt, 1);
    const unsigned char *name = sqlite3_column_text(stmt, 2);

    snprintf(item->item_code, sizeof(item->item_code), "%s",
             code ? (const char *)code : ""); // if null put empty string
    snprintf(item->item_name, sizeof(item->item_name), "%s",
             name ? (const char *)name : "");

    item->current_qty = sqlite3_column_int(stmt, 3);
    item->total_value = sqlite3_column_double(stmt, 4);

    i++;
  }

  *count = row_count;
  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  if (result != D_SUCCESS && *items)
  {
    free(*items);
    *items = NULL;
    *count = 0;
  }
  return result;
}

int db_update_item(const char *old_item_code, const char *new_item_code,
                   const char *new_item_name)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  int result = D_ERROR;

  char *edit_sql = "UPDATE items SET item_code = ?, item_name = ? "
                   "WHERE item_code = ?;";

  if (prepare_stmt(db, edit_sql, &stmt) != 0)
  {
    goto cleanup;
  }

  sqlite3_bind_text(stmt, 1, new_item_code, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, new_item_name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, old_item_code, -1, SQLITE_TRANSIENT);

  if (step_and_check(db, stmt, 0) != 0)
  {
    if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_UNIQUE)
    {
      result = D_UNIQUE_CONSTRAINT_VIOLATION;
    }
    goto cleanup;
  }

  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}

int db_delete_item(const char *item_code)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  int result = D_ERROR;

  char *delete_sql = "DELETE FROM items "
                     "WHERE item_code = ?;";

  if (prepare_stmt(db, delete_sql, &stmt) == 1)
  {
    goto cleanup;
  }

  sqlite3_bind_text(stmt, 1, item_code, -1, SQLITE_TRANSIENT);

  if (step_and_check(db, stmt, 0) != 0)
  {
    if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_FOREIGNKEY)
    {
      result = D_FOREIGNKEY_VIOLATION;
    }
    goto cleanup;
  }

  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}

int db_get_item_by_code(const char *input_item_code, Item *item)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *search_stmt = NULL;
  int result = D_ERROR;

  char *select_sql =
      "SELECT id, item_code, item_name, current_qty, total_value "
      "FROM items "
      "WHERE item_code = ?;";

  if (prepare_stmt(db, select_sql, &search_stmt) == 1)
  {
    goto cleanup;
  }

  sqlite3_bind_text(search_stmt, 1, input_item_code, -1, SQLITE_TRANSIENT);

  result = sqlite3_step(search_stmt);

  if (result == SQLITE_ROW)
  {
    item->item_id = sqlite3_column_int(search_stmt, 0);
    snprintf(item->item_code, sizeof(item->item_code), "%s",
             (char *)sqlite3_column_text(search_stmt, 1));
    snprintf(item->item_name, sizeof(item->item_name), "%s",
             (char *)sqlite3_column_text(search_stmt, 2));
    item->current_qty = sqlite3_column_int(search_stmt, 3);
    item->total_value = sqlite3_column_double(search_stmt, 4);
    result = D_SUCCESS;
  }
  else if (result == SQLITE_DONE)
  {
    result = D_NOT_FOUND;
  }

  goto cleanup;

cleanup:
  if (search_stmt)
    sqlite3_finalize(search_stmt);
  return result;
}

// Inventory Menu DB Functions
int db_add_stock(int item_id, int qty, double unit_cost)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *add_stmt = NULL;
  sqlite3_stmt *update_stmt = NULL;
  int result = D_ERROR;
  double total_value = qty * unit_cost;

  // Insert Stock Addition Entry
  char *insert_sql = "INSERT INTO stock_additions "
                     "(added_qty, unit_cost, unused_qty, item_id) "
                     "VALUES (?, ?, ?, ?);";

  if (begin_txn(db) != 0)
  {
    goto cleanup;
  }

  if (prepare_stmt(db, insert_sql, &add_stmt) == 1)
  {
    goto cleanup;
  }

  sqlite3_bind_int(add_stmt, 1, qty);
  sqlite3_bind_double(add_stmt, 2, unit_cost);
  sqlite3_bind_int(add_stmt, 3, qty);
  sqlite3_bind_int(add_stmt, 4, item_id);

  if (step_and_check(db, add_stmt, 0) != 0)
  {
    goto txn_error;
  }

  // Update Items Table
  char *update_sql = "UPDATE items "
                     "SET current_qty = current_qty + ?, "
                     "total_value = total_value + ? "
                     "WHERE id = ?;";

  if (prepare_stmt(db, update_sql, &update_stmt) == 1)
  {
    goto txn_error;
  }

  sqlite3_bind_int(update_stmt, 1, qty);
  sqlite3_bind_double(update_stmt, 2, total_value);
  sqlite3_bind_int(update_stmt, 3, item_id);

  if (step_and_check(db, update_stmt, 0) != 0)
  {
    goto txn_error;
  }

  if (commit_txn(db) != 0)
  {
    goto txn_error;
  }

  result = D_SUCCESS;
  goto cleanup;

txn_error:
  rollback_txn(db);
  goto cleanup;

cleanup:
  if (add_stmt)
    sqlite3_finalize(add_stmt);
  if (update_stmt)
    sqlite3_finalize(update_stmt);
  return result;
}

int db_issue_stock(Item item, int issue_qty)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *select_add_stmt = NULL;
  sqlite3_stmt *issue_stmt = NULL;
  sqlite3_stmt *relation_stmt = NULL;
  sqlite3_stmt *update_itm_stmt = NULL;
  sqlite3_stmt *update_add_stmt = NULL;
  int result = D_ERROR;
  int issue_id;
  double total_cost = 0;

  if (begin_txn(db) != 0)
  {
    goto cleanup;
  }

  if (item.current_qty < issue_qty)
  {
    result = D_NOT_ENOUGH_STOCK;
    goto cleanup;
  }

  // Insert Stock Issues
  char *insert_issue_sql = "INSERT INTO stock_issues "
                           "(issued_qty, item_id) "
                           "VALUES (?, ?);";

  if (prepare_stmt(db, insert_issue_sql, &issue_stmt) == 1)
  {
    goto txn_error;
  }

  sqlite3_bind_int(issue_stmt, 1, issue_qty);
  sqlite3_bind_int(issue_stmt, 2, item.item_id);

  if (step_and_check(db, issue_stmt, 0) != 0)
  {
    goto txn_error;
  }

  issue_id = (int)sqlite3_last_insert_rowid(db);

  // Get Stock Additions in FIFO Order
  char *select_add_txn_sql = "SELECT id, unused_qty, unit_cost "
                             "FROM stock_additions "
                             "WHERE item_id = ? AND unused_qty > 0 "
                             "ORDER BY id ASC;";

  if (prepare_stmt(db, select_add_txn_sql, &select_add_stmt) == 1)
  {
    goto txn_error;
  }

  sqlite3_bind_int(select_add_stmt, 1, item.item_id);

  // Prepare relation statement
  char *insert_relation_sql =
      "INSERT INTO stock_issues_add_relation "
      "(issued_qty, stock_issues_id, stock_addition_id) "
      "VALUES (?, ?, ?);";

  if (prepare_stmt(db, insert_relation_sql, &relation_stmt) == 1)
  {
    goto txn_error;
  }

  // Prepare update stock addition statement
  char *update_add_sql = "UPDATE stock_additions "
                         "SET unused_qty = unused_qty - ? "
                         "WHERE id = ?;";

  if (prepare_stmt(db, update_add_sql, &update_add_stmt) == 1)
  {
    goto txn_error;
  }

  // Process FIFO stock issue
  int remaining_qty = issue_qty;
  while (remaining_qty > 0)
  {

    int rc = sqlite3_step(select_add_stmt);

    if (rc != SQLITE_ROW)
    {
      if (rc != SQLITE_DONE)
      {
        fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
      }
      else
      {
        result = D_NOT_ENOUGH_FIFO_STOCK;
      }
      goto txn_error;
    }

    int add_txn_id = sqlite3_column_int(select_add_stmt, 0);
    int txn_unused_qty = sqlite3_column_int(select_add_stmt, 1);
    double txn_unit_cost = sqlite3_column_double(select_add_stmt, 2);

    int issue_qty_current_txn;

    // This checks if the unused qty of the selected add txn is more than the
    // remain qty to be issued
    if (txn_unused_qty < remaining_qty)
    {
      issue_qty_current_txn = txn_unused_qty;
    }
    else
    {
      issue_qty_current_txn = remaining_qty;
    }

    remaining_qty -= issue_qty_current_txn;

    total_cost += (txn_unit_cost * issue_qty_current_txn);

    // Insert relation row
    sqlite3_bind_int(relation_stmt, 1, issue_qty_current_txn);
    sqlite3_bind_int(relation_stmt, 2, issue_id);
    sqlite3_bind_int(relation_stmt, 3, add_txn_id);

    if (step_and_check(db, relation_stmt, 0) != 0)
    {
      goto txn_error;
    }

    // Update selected Stock Addition entry
    sqlite3_bind_int(update_add_stmt, 1, issue_qty_current_txn);
    sqlite3_bind_int(update_add_stmt, 2, add_txn_id);

    if (step_and_check(db, update_add_stmt, 0) != 0)
    {
      goto txn_error;
    }

    sqlite3_reset(relation_stmt);
    sqlite3_reset(update_add_stmt);
  }

  // Update Item quantities and value
  char *update_itm_sql = "UPDATE items "
                         "SET current_qty = current_qty - ?, "
                         "total_value = total_value - ? "
                         "WHERE id = ?;";

  if (prepare_stmt(db, update_itm_sql, &update_itm_stmt) == 1)
  {
    goto txn_error;
  }

  sqlite3_bind_int(update_itm_stmt, 1, issue_qty);
  sqlite3_bind_double(update_itm_stmt, 2, total_cost);
  sqlite3_bind_int(update_itm_stmt, 3, item.item_id);

  if (step_and_check(db, update_itm_stmt, 0) != 0)
  {
    goto txn_error;
  }

  if (commit_txn(db) != 0)
  {
    goto txn_error;
  }

  result = D_SUCCESS;
  goto cleanup;

txn_error:
  rollback_txn(db);
  goto cleanup;

cleanup:
  if (select_add_stmt)
    sqlite3_finalize(select_add_stmt);
  if (issue_stmt)
    sqlite3_finalize(issue_stmt);
  if (relation_stmt)
    sqlite3_finalize(relation_stmt);
  if (update_itm_stmt)
    sqlite3_finalize(update_itm_stmt);
  if (update_add_stmt)
    sqlite3_finalize(update_add_stmt);
  return result;
}

int db_get_all_stock_additions(Stock_Addition **additions, int *count)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  *additions = NULL;
  *count = 0;
  int result = D_ERROR;

  char *sql = "SELECT sa.id, sa.item_id, i.item_code, i.item_name, "
              "sa.added_qty, sa.unit_cost, sa.unused_qty, "
              "(sa.added_qty * sa.unit_cost) AS total_cost "
              "FROM stock_additions AS sa "
              "JOIN items AS i ON sa.item_id = i.id "
              "ORDER BY sa.id ASC;";

  if (prepare_stmt(db, sql, &stmt) != 0)
  {
    goto cleanup;
  }

  int row_count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    row_count++;
  }

  if (row_count == 0)
  {
    result = D_NOT_FOUND;
    goto cleanup;
  }

  *additions = malloc(sizeof(Stock_Addition) * row_count);
  if (!*additions)
  {
    result = D_MEMORY_ALLOC_FAILED;
    goto cleanup;
  }

  sqlite3_reset(stmt);
  int i = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW && i < row_count)
  {
    Stock_Addition *addition = &(*additions)[i];

    addition->addition_id = sqlite3_column_int(stmt, 0);
    addition->item_id = sqlite3_column_int(stmt, 1);

    const unsigned char *code = sqlite3_column_text(stmt, 2);
    const unsigned char *name = sqlite3_column_text(stmt, 3);

    snprintf(addition->item_code, sizeof(addition->item_code), "%s",
             code ? (const char *)code : "");
    snprintf(addition->item_name, sizeof(addition->item_name), "%s",
             name ? (const char *)name : "");

    addition->added_qty = sqlite3_column_int(stmt, 4);
    addition->unit_cost = sqlite3_column_double(stmt, 5);
    addition->unused_qty = sqlite3_column_int(stmt, 6);
    addition->total_cost = sqlite3_column_double(stmt, 7);

    i++;
  }

  *count = row_count;
  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  if (result != D_SUCCESS && *additions)
  {
    free(*additions);
    *additions = NULL;
    *count = 0;
  }
  return result;
}

int db_get_all_stock_issues(Stock_Issue **issues, int *count)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  *issues = NULL;
  *count = 0;
  int result = D_ERROR;

  char *sql =
      "SELECT si.id, si.item_id, i.item_code, i.item_name, "
      "si.issued_qty, "
      "SUM(siar.issued_qty * sa.unit_cost) AS total_cost "
      "FROM stock_issues AS si "
      "JOIN items AS i ON si.item_id = i.id "
      "JOIN stock_issues_add_relation AS siar ON si.id = siar.stock_issues_id "
      "JOIN stock_additions AS sa ON siar.stock_addition_id = sa.id "
      "GROUP BY si.id, si.item_id, i.item_code, i.item_name, si.issued_qty "
      "ORDER BY si.id ASC;";

  if (prepare_stmt(db, sql, &stmt) == 1)
  {
    goto cleanup;
  }

  int row_count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    row_count++;
  }

  if (row_count == 0)
  {
    result = D_NOT_FOUND;
    goto cleanup;
  }

  *issues = malloc(sizeof(Stock_Issue) * row_count);
  if (!*issues)
  {
    result = D_MEMORY_ALLOC_FAILED;
    goto cleanup;
  }

  sqlite3_reset(stmt);
  int i = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW && i < row_count)
  {
    Stock_Issue *issue = &(*issues)[i];

    issue->issue_id = sqlite3_column_int(stmt, 0);
    issue->item_id = sqlite3_column_int(stmt, 1);

    const unsigned char *code = sqlite3_column_text(stmt, 2);
    const unsigned char *name = sqlite3_column_text(stmt, 3);

    snprintf(issue->item_code, sizeof(issue->item_code), "%s",
             code ? (const char *)code : "");
    snprintf(issue->item_name, sizeof(issue->item_name), "%s",
             name ? (const char *)name : "");

    issue->issued_qty = sqlite3_column_int(stmt, 4);
    issue->total_cost = sqlite3_column_double(stmt, 5);

    i++;
  }

  *count = row_count;
  result = D_SUCCESS;

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  if (result != D_SUCCESS && *issues)
  {
    free(*issues);
    *issues = NULL;
    *count = 0;
  }
  return result;
}

int db_get_stock_addition_by_id(int addition_id, Stock_Addition *addition)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  int result = D_ERROR;

  char *sql = "SELECT sa.id, sa.item_id, i.item_code, i.item_name, "
              "sa.added_qty, sa.unit_cost, sa.unused_qty, "
              "(sa.added_qty * sa.unit_cost) AS total_cost "
              "FROM stock_additions AS sa "
              "JOIN items AS i ON sa.item_id = i.id "
              "WHERE sa.id = ?;";

  if (prepare_stmt(db, sql, &stmt) != 0)
  {
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, addition_id);

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW)
  {
    addition->addition_id = sqlite3_column_int(stmt, 0);
    addition->item_id = sqlite3_column_int(stmt, 1);

    const unsigned char *code = sqlite3_column_text(stmt, 2);
    const unsigned char *name = sqlite3_column_text(stmt, 3);

    snprintf(addition->item_code, sizeof(addition->item_code), "%s",
             code ? (const char *)code : "");
    snprintf(addition->item_name, sizeof(addition->item_name), "%s",
             name ? (const char *)name : "");

    addition->added_qty = sqlite3_column_int(stmt, 4);
    addition->unit_cost = sqlite3_column_double(stmt, 5);
    addition->unused_qty = sqlite3_column_int(stmt, 6);
    addition->total_cost = sqlite3_column_double(stmt, 7);

    result = D_SUCCESS;
  }
  else if (rc == SQLITE_DONE)
  {
    result = D_NOT_FOUND;
  }

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}

int db_delete_stock_addition(Stock_Addition addition)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *delete_stmt = NULL;
  sqlite3_stmt *update_stmt = NULL;
  int result = D_ERROR;

  if (begin_txn(db) != 0)
  {
    goto cleanup;
  }

  // Delete the stock addition
  char *delete_sql = "DELETE FROM stock_additions WHERE id = ?;";

  if (prepare_stmt(db, delete_sql, &delete_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(delete_stmt, 1, addition.addition_id);

  if (step_and_check(db, delete_stmt, 0) != 0)
  {
    if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_FOREIGNKEY)
    {
      result = D_FOREIGNKEY_VIOLATION;
    }
    goto txn_error;
  }

  // Update the item's quantity and value
  char *update_sql = "UPDATE items "
                     "SET current_qty = current_qty - ?, "
                     "total_value = total_value - ? "
                     "WHERE id = ?;";

  if (prepare_stmt(db, update_sql, &update_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(update_stmt, 1, addition.added_qty);
  sqlite3_bind_double(update_stmt, 2, addition.added_qty * addition.unit_cost);
  sqlite3_bind_int(update_stmt, 3, addition.item_id);

  if (step_and_check(db, update_stmt, 0) != 0)
  {
    goto txn_error;
  }

  if (commit_txn(db) != 0)
  {
    goto txn_error;
  }

  result = D_SUCCESS;
  goto cleanup;

txn_error:
  rollback_txn(db);
  goto cleanup;

cleanup:
  if (delete_stmt)
    sqlite3_finalize(delete_stmt);
  if (update_stmt)
    sqlite3_finalize(update_stmt);
  return result;
}

int db_get_stock_issue_by_id(int issue_id, Stock_Issue *issue)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *stmt = NULL;
  int result = D_ERROR;

  char *sql =
      "SELECT si.id, si.item_id, i.item_code, i.item_name, "
      "si.issued_qty, "
      "SUM(siar.issued_qty * sa.unit_cost) AS total_cost "
      "FROM stock_issues AS si "
      "JOIN items AS i ON si.item_id = i.id "
      "JOIN stock_issues_add_relation AS siar ON si.id = siar.stock_issues_id "
      "JOIN stock_additions AS sa ON siar.stock_addition_id = sa.id "
      "WHERE si.id = ? "
      "GROUP BY si.id, si.item_id, i.item_code, i.item_name, si.issued_qty;";

  if (prepare_stmt(db, sql, &stmt) != 0)
  {
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, issue_id);

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW)
  {
    issue->issue_id = sqlite3_column_int(stmt, 0);
    issue->item_id = sqlite3_column_int(stmt, 1);

    const unsigned char *code = sqlite3_column_text(stmt, 2);
    const unsigned char *name = sqlite3_column_text(stmt, 3);

    snprintf(issue->item_code, sizeof(issue->item_code), "%s",
             code ? (const char *)code : "");
    snprintf(issue->item_name, sizeof(issue->item_name), "%s",
             name ? (const char *)name : "");

    issue->issued_qty = sqlite3_column_int(stmt, 4);
    issue->total_cost = sqlite3_column_double(stmt, 5);

    result = D_SUCCESS;
  }
  else if (rc == SQLITE_DONE)
  {
    result = D_NOT_FOUND;
  }

  goto cleanup;

cleanup:
  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}

int db_delete_stock_issue(Stock_Issue issue)
{
  sqlite3 *db = get_db();
  sqlite3_stmt *get_relations_stmt = NULL;
  sqlite3_stmt *delete_relations_stmt = NULL;
  sqlite3_stmt *delete_issue_stmt = NULL;
  sqlite3_stmt *update_additions_stmt = NULL;
  sqlite3_stmt *update_item_stmt = NULL;
  int result = D_ERROR;

  if (begin_txn(db) != 0)
  {
    goto cleanup;
  }

  // Get all relations for this issue to restore used quantities
  char *get_relations_sql = "SELECT stock_addition_id, issued_qty "
                            "FROM stock_issues_add_relation "
                            "WHERE stock_issues_id = ?;";

  if (prepare_stmt(db, get_relations_sql, &get_relations_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(get_relations_stmt, 1, issue.issue_id);

  // Prepare statement to restore used quantities
  char *update_additions_sql = "UPDATE stock_additions "
                               "SET unused_qty = unused_qty + ? "
                               "WHERE id = ?;";

  if (prepare_stmt(db, update_additions_sql, &update_additions_stmt) != 0)
  {
    goto txn_error;
  }

  // Process each relation to restore unused quantities
  while (sqlite3_step(get_relations_stmt) == SQLITE_ROW)
  {
    int addition_id = sqlite3_column_int(get_relations_stmt, 0);
    int issued_qty = sqlite3_column_int(get_relations_stmt, 1);

    sqlite3_bind_int(update_additions_stmt, 1, issued_qty);
    sqlite3_bind_int(update_additions_stmt, 2, addition_id);

    if (step_and_check(db, update_additions_stmt, 0) != 0)
    {
      goto txn_error;
    }

    sqlite3_reset(update_additions_stmt);
  }

  // Delete all relations for this issue
  char *delete_relations_sql = "DELETE FROM stock_issues_add_relation "
                               "WHERE stock_issues_id = ?;";

  if (prepare_stmt(db, delete_relations_sql, &delete_relations_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(delete_relations_stmt, 1, issue.issue_id);

  if (step_and_check(db, delete_relations_stmt, 0) != 0)
  {
    goto txn_error;
  }

  // Delete the stock issue
  char *delete_issue_sql = "DELETE FROM stock_issues WHERE id = ?;";

  if (prepare_stmt(db, delete_issue_sql, &delete_issue_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(delete_issue_stmt, 1, issue.issue_id);

  if (step_and_check(db, delete_issue_stmt, 0) != 0)
  {
    goto txn_error;
  }

  // Update the item's quantity and value
  char *update_item_sql = "UPDATE items "
                          "SET current_qty = current_qty + ?, "
                          "total_value = total_value + ? "
                          "WHERE id = ?;";

  if (prepare_stmt(db, update_item_sql, &update_item_stmt) != 0)
  {
    goto txn_error;
  }

  sqlite3_bind_int(update_item_stmt, 1, issue.issued_qty);
  sqlite3_bind_double(update_item_stmt, 2, issue.total_cost);
  sqlite3_bind_int(update_item_stmt, 3, issue.item_id);

  if (step_and_check(db, update_item_stmt, 0) != 0)
  {
    goto txn_error;
  }

  if (commit_txn(db) != 0)
  {
    goto txn_error;
  }

  result = D_SUCCESS;
  goto cleanup;

txn_error:
  rollback_txn(db);
  goto cleanup;

cleanup:
  if (get_relations_stmt)
    sqlite3_finalize(get_relations_stmt);
  if (delete_relations_stmt)
    sqlite3_finalize(delete_relations_stmt);
  if (delete_issue_stmt)
    sqlite3_finalize(delete_issue_stmt);
  if (update_additions_stmt)
    sqlite3_finalize(update_additions_stmt);
  if (update_item_stmt)
    sqlite3_finalize(update_item_stmt);
  return result;
}

// ----- login.c -----
#include <stdio.h>
#include <string.h>

User_Array user_data = {
    {{"admin", "123", 3}, {"user", "123", 2}, {"read", "123", 1}}, 3};

static User current_user;

int login(void)
{

  FILE *fp = fopen("configs.dat", "rb");

  // if configs.dat does not exist, create it and write the default user creds
  // into the file
  if (fp == NULL)
  {
    fp = fopen("configs.dat", "wb");
    fwrite(&user_data, sizeof(User_Array), 1, fp);
    fclose(fp);
    fp = fopen("configs.dat", "rb");
  }

  // to check if the user struc is read from file
  int read_config_count = fread(&user_data, sizeof(User_Array), 1, fp);

  if (read_config_count < 1)
  {
    fprintf(stderr, "Failed reading config.dat");
    return 1;
  }

  fclose(fp);

  printf("\n");
  printf("================================\n");
  printf("  %s\n", "LOGIN SCREEN");
  printf("================================\n");
  printf("\n");

  char input_user_name[MAX_USER_NAME_LENGTH + 1];
  char input_user_pass[MAX_USER_PASSWORD_LENGTH + 1];

  printf("\nEnter User Name [Max: %d characters]: ", MAX_USER_NAME_LENGTH);
  read_input(input_user_name, sizeof(input_user_name));

  printf("\nEnter Password [Max: %d characters]: ", MAX_USER_PASSWORD_LENGTH);
  read_input(input_user_pass, sizeof(input_user_pass));

  for (int i = 0; i < user_data.user_count; i++)
  {
    if (strcmp(user_data.user_arr[i].username, input_user_name) == 0 &&
        strcmp(user_data.user_arr[i].password, input_user_pass) == 0)
    {
      current_user = user_data.user_arr[i];
      printf("Login successful! User: %s\n", current_user.username);
      wait_for_enter();
      return 1;
    }
  }

  return 0;
}

const User *get_current_user(void) { return &current_user; }

// ----- display_table.c -----
#include <stdio.h>

// Item Headers
void format_item_header(void)
{
  printf("\n%-5s %-15s %-30s\n", "Sn.", "Item Code", "Item Name");
  printf("%-5s %-15s %-30s\n", "----", "----------", "---------");
}

// Item Rows
void format_item_row(const Item *item, int index)
{
  printf("%-5d %-15s %-30s\n", index + 1, item->item_code, item->item_name);
}

// Inventory Header
void format_inventory_header(void)
{
  printf("\n%-5s %-15s %-30s %-12s %-12s\n", "Sn.", "Item Code", "Item Name",
         "Current Qty", "Total Value");
  printf("%-5s %-15s %-30s %-12s %-12s\n", "----", "----------", "---------",
         "-----------", "-----------");
}

// Inventory Row
void format_inventory_row(const Item *item, int index)
{
  printf("%-5d %-15s %-30s %-12d %-12.2f\n", index + 1, item->item_code,
         item->item_name, item->current_qty, item->total_value);
}

// Display Item Rows
void display_item_table(Item *items, int count, void (*print_header)(void),
                        void (*print_row)(const Item *, int))
{
  if (!items || count <= 0)
  {
    printf("No items to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++)
  {
    print_row(&items[i], i);
  }

  printf("\nTotal Items: %d\n", count);
}

void display_selected_item(const Item *item)
{
  printf("Code: [%s]\nName: [%s]\n\n", item->item_code, item->item_name);
}

void display_selected_item_qty(const Item *item)
{
  printf("Code: [%s]\nName: [%s]\nAvailable Qty: [%d]\n\n", item->item_code,
         item->item_name, item->current_qty);
}

void display_selected_addition(const Stock_Addition *addition)
{
  printf("Addition ID   : [%d]\n", addition->addition_id);
  printf("Item Code     : [%s]\n", addition->item_code);
  printf("Item Name     : [%s]\n", addition->item_name);
  printf("Added Qty     : [%d]\n", addition->added_qty);
  printf("Unit Cost     : [%.2f]\n", addition->unit_cost);
  printf("Unused Qty    : [%d]\n", addition->unused_qty);
  printf("Total Cost    : [%.2f]\n\n", addition->total_cost);
}

void display_selected_stock_issue(const Stock_Issue *issue)
{
  printf("Issue ID      : [%d]\n", issue->issue_id);
  printf("Item Code     : [%s]\n", issue->item_code);
  printf("Item Name     : [%s]\n", issue->item_name);
  printf("Issued Qty    : [%d]\n", issue->issued_qty);
  printf("Total Cost    : [%.2f]\n\n", issue->total_cost);
}

// Print Header Formats
// Additions Headers
//  Print header for Stock_Addition list
void format_additions_header(void)
{
  printf("\n%-5s  %-8s  %-15s  %-30s  %-10s  %-10s  %-10s  %-12s\n", "Sn.",
         "Add.ID", "Item Code", "Item Name", "AddedQty", "UnitCost",
         "UnusedQty", "TotalCost");
  printf("%-5s  %-8s  %-15s  %-30s  %-10s  %-10s  %-10s  %-12s\n", "----",
         "-------", "---------", "---------", "--------", "--------",
         "--------", "---------");
}

// Print one row for a Stock_Addition
void format_additions_row(const Stock_Addition *addition, int index)
{
  printf("%-5d  %-8d  %-15s  %-30s  %-10d  %-10.2f  %-10d  %-12.2f\n",
         index + 1, addition->addition_id, addition->item_code,
         addition->item_name, addition->added_qty, addition->unit_cost,
         addition->unused_qty, addition->total_cost);
}

// Display Addition Table
void display_additions_table(Stock_Addition *additions, int count,
                             void (*print_header)(void),
                             void (*print_row)(const Stock_Addition *, int))
{
  if (!additions || count <= 0)
  {
    printf("No Additions to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++)
  {
    print_row(&additions[i], i);
  }

  printf("\nTotal Additions: %d\n", count);
}

// Stock Issues Display

// Print header for StockIssue list
void format_issues_header(void)
{
  printf("\n"
         "%-5s  %-8s  %-15s  %-30s  %-10s  %-12s\n",
         "Sn.", "IssueID", "Item Code", "Item Name", "IssuedQty", "TotalCost");
  printf("%-5s  %-8s  %-15s  %-30s  %-10s  %-12s\n", "----", "--------",
         "---------", "---------", "---------", "---------");
}

// Print one row for a StockIssue
void format_issues_row(const Stock_Issue *issue, int index)
{
  printf("%-5d  %-8d  %-15s  %-30s  %-10d  %-12.2f\n", index + 1,
         issue->issue_id, issue->item_code, issue->item_name, issue->issued_qty,
         issue->total_cost);
}

// Display Issues Table
void display_issues_table(Stock_Issue *issues, int count,
                          void (*print_header)(void),
                          void (*print_row)(const Stock_Issue *, int))
{
  if (!issues || count <= 0)
  {
    printf("No Issues to display.\n");
    return;
  }

  if (print_header)
    print_header();

  for (int i = 0; i < count; i++)
  {
    print_row(&issues[i], i);
  }

  printf("\nTotal Issues: %d\n", count);
}

// ----- item_menu.c -----
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int add_item(void)
{
  char item_code[ITEM_CODE_LENGTH + 1];
  char item_name[ITEM_NAME_LENGTH + 1];

  while (1)
  {

    clear_console();

    printf("Enter Item Code (Must be Unique) [Max: %d characters]: ", ITEM_CODE_LENGTH);
    read_input(item_code, sizeof(item_code));

    printf("Enter Item Name [Max: %d characters]: ", ITEM_NAME_LENGTH);
    read_input(item_name, sizeof(item_name));

    if (item_code[0] == '\0' || item_name[0] == '\0')
    {
      printf("Value Cannot be Blank");
      goto cleanup;
    }

    // Perform Database operation
    int result = db_add_item(item_code, item_name);
    if (result == D_SUCCESS)
    {
      printf("\nAdded New Item [%s] Successfully.\n", item_code);
    }
    else if (result == D_UNIQUE_CONSTRAINT_VIOLATION)
    {
      printf("Item code already exists. Please use a unique code.\n");
      goto cleanup;
    }
    else
    {
      printf("Error adding item.\n");
      goto cleanup;
    }

    const char *add_another_msg = "\nWould you like to add another item ?";
    const char *cancel_msg = "\nDone Adding Items.";

    if (get_user_confirmation(add_another_msg, cancel_msg) != 0)
    {
      break;
    }
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}

int view_items(void)
{

  Item *items = NULL;
  int count = 0;

  int result = db_get_all_items(&items, &count);
  if (result == D_NOT_FOUND)
  {
    printf("No items found in the database.\n");
  }
  else if (result != D_SUCCESS)
  {
    printf("\nDatabase Error\n\n");
  }
  else
  {
    display_item_table(items, count, format_item_header, format_item_row);
  }

  if (items)
    free(items);
  wait_for_enter();
  return 0;
}

int search_item(void)
{
  char input_itm_code[ITEM_CODE_LENGTH + 1];
  Item item;

  printf("Enter Item Code of Item to Search [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(input_itm_code, sizeof(input_itm_code));

  int result = db_get_item_by_code(input_itm_code, &item);
  if (result == D_NOT_FOUND)
  {
    printf("\n\nItem could not be found\n");
  }
  else if (result != D_SUCCESS)
  {
    printf("\nDatabase Error\n\n");
  }
  else
  {
    // Uses the display format item header and row function to display the
    // searched item
    format_item_header();
    format_item_row(&item, 0);
  }

  wait_for_enter();
  return 0;
}

int edit_item(void)
{
  char input_itm_code[ITEM_CODE_LENGTH + 1];
  char new_itm_code[ITEM_CODE_LENGTH + 1];
  char new_itm_name[ITEM_NAME_LENGTH + 1];
  Item item;

  // Check if Item Exists and displays it
  printf("Enter Item Code of Item to edit [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(input_itm_code, sizeof(input_itm_code));

  int result = db_get_item_by_code(input_itm_code, &item);
  if (result != D_SUCCESS)
  {
    printf("Item with code '%s' not found.\n", input_itm_code);
    goto cleanup;
  }

  printf("\nOld Item Details\n");
  display_selected_item(&item);

  printf("\nEnter Updated Item Details;\n\n");

  printf("Enter Updated Item Code [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(new_itm_code, sizeof(new_itm_code));

  printf("Enter Updated Item Name [Max: %d characters]: ", ITEM_NAME_LENGTH);
  read_input(new_itm_name, sizeof(new_itm_name));

  if (new_itm_code[0] == '\0' || new_itm_name[0] == '\0')
  {
    printf("Value Cannot be Blank");
    goto cleanup;
  }

  // Perform Record Update
  result = db_update_item(input_itm_code, new_itm_code, new_itm_name);
  switch (result)
  {
  case D_SUCCESS:
    printf("Record Updated Successfully");
    break;
  case D_NOT_FOUND:
    printf("Item not found for update");
    break;
  case D_UNIQUE_CONSTRAINT_VIOLATION:
    printf("New item code already exists. Please use a unique code.");
    break;
  default:
    printf("Error updating item");
    break;
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}

int delete_item(void)
{
  char input_itm_code[ITEM_CODE_LENGTH + 1];
  Item item;

  printf("Enter Item Code of Item to Delete [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(input_itm_code, sizeof(input_itm_code));

  // Check if Item exist and display
  int result = db_get_item_by_code(input_itm_code, &item);
  if (result != D_SUCCESS)
  {
    printf("Item with code '%s' not found.\n", input_itm_code);
    goto cleanup;
  }

  printf("\nItem Selected for Delete Operation\n");
  display_selected_item(&item);

  // Prompt User if they want to delete the selected item
  char *prompt = "\nConfirm Delete Operation ?\n";
  char *cancel_msg = "\nCancelled Delete Operation.\n";

  if (get_user_confirmation(prompt, cancel_msg) != 0)
  {
    goto cleanup;
  }

  // Perform the Delete Operation
  result = db_delete_item(input_itm_code);
  switch (result)
  {
  case D_SUCCESS:
    printf("Item [%s] Deleted Successfully", input_itm_code);
    break;
  case D_FOREIGNKEY_VIOLATION:
    printf("Item has one or more related transactions. Delete those first.");
    break;
  default:
    printf("Error deleting item");
    break;
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}

// ----- invent_menu.c -----
#include <stdio.h>

int add_stock(void)
{
  char input_itm_code[ITEM_CODE_LENGTH + 1];
  int qty;
  double unit_cost;
  Item item;

  printf("Enter Item Code of Item [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(input_itm_code, sizeof(input_itm_code));

  int result = db_get_item_by_code(input_itm_code, &item);
  if (result != D_SUCCESS)
  {
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

  if (qty <= 0 || unit_cost <= 0)
  {
    printf("\n\nQuantity or Value should be a non zero positive number\n\n");
    goto cleanup;
  }

  printf("Total Cost is : %.2lf \n", qty * unit_cost);

  result = db_add_stock(item.item_id, qty, unit_cost);

  if (result == D_SUCCESS)
  {
    printf("\n\nStock Added Successfully\n");
  }
  else
  {
    printf("\n\nError Adding Stock\n");
  }

  goto cleanup;

cleanup:
  wait_for_enter();
  return 0;
}

int issue_stock(void)
{
  char input_itm_code[ITEM_CODE_LENGTH + 1];
  int issue_qty;
  Item item;

  printf("Enter Item Code of Item [Max: %d characters]: ", ITEM_CODE_LENGTH);
  read_input(input_itm_code, sizeof(input_itm_code));

  int result = db_get_item_by_code(input_itm_code, &item);
  if (result != D_SUCCESS)
  {
    printf("\n\nItem could not be found\n");
    goto cleanup;
  }

  printf("\nItem to Issue Stock\n");

  display_selected_item_qty(&item);

  if (item.current_qty <= 0)
  {
    printf("\nSelected Item has no stock. Add Stock before Issue.\n");
    goto cleanup;
  }

  printf("\nEnter the Stock Issue Details;\n\n");

  printf("Enter Quantity: ");
  scanf("%d", &issue_qty);
  clear_input_buffer();

  if (issue_qty <= 0)
  {
    printf("\n\nQuantity should be a non zero positive number\n\n");
    goto cleanup;
  }

  if (issue_qty > item.current_qty)
  {
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

int view_additions(void)
{
  Stock_Addition *additions = NULL;
  int count = 0;

  int result = db_get_all_stock_additions(&additions, &count);

  switch (result)
  {
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

  if (additions)
  {
    free(additions);
  }

  wait_for_enter();
  return 0;
}

int view_issues(void)
{
  Stock_Issue *issues = NULL;
  int count = 0;

  int result = db_get_all_stock_issues(&issues, &count);

  switch (result)
  {
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

  if (issues)
  {
    free(issues);
  }

  wait_for_enter();
  return 0;
}

int view_inventory(void)
{
  Item *items = NULL;
  int count = 0;

  int result = db_get_all_items(&items, &count);
  if (result == D_NOT_FOUND)
  {
    printf("No items found in Inventory.\n");
  }
  else if (result != D_SUCCESS)
  {
    printf("\nDatabase Error\n\n");
  }
  else
  {
    display_item_table(items, count, format_inventory_header,
                       format_inventory_row);
  }

  if (items)
    free(items);
  wait_for_enter();
  return 0;
}

int delete_stock_addition(void)
{
  int addition_id;
  Stock_Addition addition;

  printf("\n====WARNING!! DELETING STOCK ADDITIONS COULD RESULT IN INCORRECT "
         "FIFO ALLOCATION!!====\n");

  printf("Enter Stock Addition ID to Delete: ");
  scanf("%d", &addition_id);
  clear_input_buffer();

  // Check if addition exists and display
  int result = db_get_stock_addition_by_id(addition_id, &addition);
  if (result != D_SUCCESS)
  {
    printf("Stock Addition with ID '%d' not found.\n", addition_id);
    goto cleanup;
  }

  printf("\nAddition Selected for Deletion\n");
  display_selected_addition(&addition);

  // Check if any quantity has been used
  if (addition.unused_qty != addition.added_qty)
  {
    printf("\nWARNING: This addition has been used. Cannot Delete.\n");
    goto cleanup;
  }

  // Prompt user for confirmation
  char *prompt = "\nConfirm Delete Operation ?\n";
  char *cancel_msg = "\nCancelled Delete Operation.\n";

  if (get_user_confirmation(prompt, cancel_msg) != 0)
  {
    goto cleanup;
  }

  // Perform the delete operation
  result = db_delete_stock_addition(addition);
  switch (result)
  {
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

int delete_stock_issue(void)
{
  int issue_id;
  Stock_Issue issue;

  printf("\n====WARNING!! DELETING STOCK ISSUES COULD RESULT IN INCORRECT FIFO "
         "ALLOCATION!!====\n");

  printf("Enter Stock Issue ID to Delete: ");
  scanf("%d", &issue_id);
  clear_input_buffer();

  // Check if issue exists and display
  int result = db_get_stock_issue_by_id(issue_id, &issue);
  if (result != D_SUCCESS)
  {
    printf("Stock Issue with ID '%d' not found.\n", issue_id);
    goto cleanup;
  }

  printf("\nStock Issue Selected for Delete Operation\n");
  display_selected_stock_issue(&issue);

  // Prompt user for confirmation
  char *prompt = "\nConfirm Delete Operation ?\n";
  char *cancel_msg = "\nCancelled Delete Operation.\n";

  if (get_user_confirmation(prompt, cancel_msg) != 0)
  {
    goto cleanup;
  }

  // Perform the delete operation
  result = db_delete_stock_issue(issue);
  switch (result)
  {
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

// ----- menu.c -----
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
      {"Delete Item", delete_item, 1},
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

// ----- main.c -----
#include <stdio.h>
#include <stdlib.h>

int main(void)
{

  int db_rc = initialize_db();

  if (db_rc == 1)
  {
    printf("Failed to initialize DB");
    goto error_cleanup;
  }

  int login_attempts = 0;
  int rem_logins = MAX_LOGIN_ATTEMPTS;

  do
  {
    clear_console();

    if (login() == 1)
    {
      break;
    }
    else
    {
      login_attempts++;
      rem_logins--;
    }

    printf("\n\nLogin failed. Invalid username or password.\n\n");

    if (rem_logins != 0)
    {
      printf("You have %d more attempts left\n", rem_logins);
      wait_for_enter();
    }

  } while (login_attempts < MAX_LOGIN_ATTEMPTS);

  if (login_attempts >= MAX_LOGIN_ATTEMPTS)
  {
    printf("Max Login Attempts reached, stopping program");
    goto close_cleanup;
  }

  int err = run_main_menu();

  if (err != 0)
    goto error_cleanup;

error_cleanup:
  disconnect_db();
  return 1;

close_cleanup:
  disconnect_db();
  return 0;
}

