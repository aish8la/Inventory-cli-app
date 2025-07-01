#include "sqlite_helpers.h"
#include "sqlite3.h"
#include <stdio.h>

int open_db(sqlite3 **ppDb) {
    int rc;

    rc = sqlite3_open("data.db", ppDb);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Database could not be opened: %s\n", sqlite3_errmsg(*ppDb));
        sqlite3_close(*ppDb);
        return 1;
    }

    return 0;
}

int run_sql(sqlite3 *db, const char *sql) {
    int rc;
    char *err_msg = NULL;

    rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", err_msg);
        sqlite3_free(err_msg);
        err_msg = NULL;
        return 1;
    }

    return 0;
}

int run_sql_with_cb(
    sqlite3 *db,
    const char *sql,
    int (*callback)(void*,int,char**,char**),
    void *cbdata
) {

    int rc;
    char *err_msg = NULL;

    rc = sqlite3_exec(db, sql, callback, cbdata, &err_msg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL Error: %s\n", err_msg);
        sqlite3_free(err_msg);
        err_msg = NULL;
        return 1;
    }

    return 0;

}

int prepare_stmt(sqlite3 *db, char *sql, sqlite3_stmt **stmt) {
    
    int rc = sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    return 0;
}

    