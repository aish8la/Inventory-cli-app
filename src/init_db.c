#include "init_db.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "sqlite_helpers.h"

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
    char *get_init_flag = "SELECT flag_value FROM flags WHERE flag_name = 'not_initial_run';";

    if(open_db(&db) != 0) {
        exit(1);
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

int initialize_db(void) {

    /*check init returns 0 if the current run is not the first time so this check will return without running
    database initialization*/
    if(!check_init_flag()) {
        return 0;
    }

    sqlite3 *db;
    int rc;
    char *err_msg = NULL;
    char *init_item_table = "CREATE TABLE IF NOT EXISTS items (" //The first query creates an item table
                        "id INTEGER PRIMARY KEY NOT NULL,"
                        "item_code TEXT UNIQUE NOT NULL,"
                        "item_name TEXT);"
                        "CREATE TABLE IF NOT EXISTS flags (flag_name TEXT UNIQUE, flag_value INT);"//This query creates flags table
                        "INSERT INTO flags (flag_name, flag_value) VALUES ('not_initial_run', 1)";

    if(open_db(&db) != 0) {
        exit(1);
    }

    if(run_sql(db, init_item_table) != 0) {
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);
    db = NULL;
    return 0;
}