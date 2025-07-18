#include "init_db.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "sqlite_helpers.h"

static sqlite3 *db_instance = NULL; 

int connect_db(const char *db_name) {

    if(db_instance != NULL) {
        return 0;
    }

    int rc = sqlite3_open(db_name, &db_instance);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Database could not be opened: %s\n", sqlite3_errmsg(db_instance));
        db_instance = NULL;
        wait_for_enter();
        return 1;
    }

    return 0;
}


sqlite3 *get_db(void) {
    return db_instance;
}

void disconnect_db(void) {
    if (db_instance != NULL) {
        sqlite3_close(db_instance);
        db_instance = NULL;
    }
}

//This is a callback for sqlite_exec to check 1 flag value from the database and write it to the var address give to the sqlite_exec function
int flag_value_callback(void *data, int argc, char **argv, char **arg_col_name) {
    int *flag_var = (int *)data; //typecasting the generic pointer into a integer pointer
    if(argc > 0 && argv[0]) {
        *flag_var = atoi(argv[0]);
    } else {
        *flag_var = 0;
    }

    return 0;
}

//returns 0 if the current run is not the first time running (by checking the flags table). returns 1 in case of sqlite errors or first time running; 
int check_init_flag(void) {
    sqlite3 *db;
    int flag_value = 0;
    char *set_init_flag = "CREATE TABLE IF NOT EXISTS flags ("
                        "flag_name  TEXT UNIQUE,"
                        "flag_value INT"
                        ");"
                        "INSERT OR IGNORE INTO flags (flag_name, flag_value) "
                        "VALUES ('not_initial_run', 0);";

    char *get_init_flag ="SELECT flag_value FROM flags WHERE flag_name = 'not_initial_run';";

    if(open_db(&db) != 0) {
        return 1;
    }

    if(run_sql(db, set_init_flag) != 0) {
        sqlite3_close(db);
        return 1;
    }

    if(run_sql_with_cb(db, get_init_flag, flag_value_callback, &flag_value) != 0) {
        sqlite3_close(db);
        return 1;
    }
  
    sqlite3_close(db);
    db = NULL;

    if(flag_value) {
        return 0;
    } else {
        return 1;
    }

}

//These are the initial queries to run
const char *initial_queries[] = {
    //1) The first query creates an item table
    "CREATE TABLE IF NOT EXISTS items (" 
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "item_code TEXT UNIQUE NOT NULL,"
    "item_name TEXT,"
    "current_qty INTEGER DEFAULT 0,"
    "total_value REAL DEFAULT 0.0);",

    //2)This creates the item addition table
    "CREATE TABLE IF NOT EXISTS stock_additions ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "added_qty INTEGER CHECK(added_qty != 0),"
    "unit_cost REAL DEFAULT 0,"
    "unused_qty INTEGER,"
    "item_id INTEGER NOT NULL);",

    //3)This creates the item issue table
    "CREATE TABLE IF NOT EXISTS stock_issues ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "issued_qty INTEGER CHECK(issued_qty != 0),"
    "stock_addition_id INTEGER NOT NULL);",

    //4)This query updates flag value
    "UPDATE flags SET flag_value = 1 WHERE flag_name = 'not_initial_run';",

    //5)These queries insert items
    "INSERT INTO items (item_code, item_name) VALUES "
    "('ITM-001', 'PENCIL'),"
    "('ITM-002', 'ERASER'),"
    "('ITM-003', 'MARKER'),"
    "('ITM-004', 'RULER'),"
    "('ITM-005', 'SHARPENER');"
};

int initialize_db(void) {

    /*check init returns 0 if the current run is not the first time so this check will return without running
    database initialization*/
    if(!check_init_flag()) {
        return 0;
    }

    sqlite3 *db;

    if(open_db(&db) != 0) {
        return 1;
    }

    int query_count = sizeof(initial_queries) / sizeof(initial_queries[0]);

    for (int i = 0; i < query_count; i++) {
        if(run_sql(db, initial_queries[i]) != 0) {
            sqlite3_close(db);
            return 1;
        }
    }

    sqlite3_close(db);
    db = NULL;
    return 0;
}