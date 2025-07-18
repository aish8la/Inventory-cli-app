#include "sqlite_helpers.h"
#include "sqlite3.h"
#include <stdio.h>
#include "utilities.h"

int open_db(sqlite3 **ppDb) {
    int rc;

    rc = sqlite3_open("data.db", ppDb);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Database could not be opened: %s\n", sqlite3_errmsg(*ppDb));
        sqlite3_close(*ppDb);
        wait_for_enter();
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
        wait_for_enter();
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
        wait_for_enter();
        return 1;
    }

    return 0;

}

int prepare_stmt(sqlite3 *db, char *sql, sqlite3_stmt **stmt) {
    
    int rc = sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Sqlite Error: %s\n", sqlite3_errmsg(db));
        wait_for_enter();
        return 1;
    }

    return 0;
}

int step_and_check(sqlite3 *db, sqlite3_stmt *stmt, int expect_row) {
    int rc = sqlite3_step(stmt);

    if (expect_row) {
        if (rc == SQLITE_DONE) {
            printf("No matching records found.\n");
            return 1;
        } else if (rc != SQLITE_ROW) {
            fprintf(stderr, "SQLite Error: %s\n", sqlite3_errmsg(db));
            return -1;
        }
    } else {
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "SQLite Error: %s\n", sqlite3_errmsg(db));
            return -1;
        }
    }

    return 0;
}

int begin_txn(sqlite3 *db) {
    if(run_sql(db, "BEGIN TRANSACTION;") != 0) {
        return 1;
    }
    return 0;
}

int rollback_txn(sqlite3 *db) {
        if(run_sql(db, "ROLLBACK;") != 0) {
        return 1;
    }
    return 0;
}

int commit_txn(sqlite3 *db) {
    if(run_sql(db, "COMMIT;") != 0) {
        rollback_txn(db);
        return 1;
    }
    return 0;
}