#include "init_db.h"
#include <stdio.h>
#include "sqlite3.h"

int initialize_db(void) {
    sqlite3 *db;
    int rc;
    char *err_msg = NULL;
    char *init_item_table = "CREATE TABLE IF NOT EXISTS items ("
                        "id INTEGER PRIMARY KEY NOT NULL,"
                        "item_code TEXT UNIQUE NOT NULL,"
                        "item_name TEXT);";

    rc = sqlite3_open("data.db", &db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Database could not be opened: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_exec(db, init_item_table, NULL, NULL, &err_msg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", err_msg);
        sqlite3_free(err_msg);
        err_msg = NULL;
        return 1;
    }

    sqlite3_close(db);
    db = NULL;
    return 0;
}