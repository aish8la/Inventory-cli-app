#ifndef SQLITE_HELPERS_H
#define SQLITE_HELPERS_H

#include "sqlite3.h"


int run_sql(sqlite3 *db, const char *sql);

int run_sql_with_cb(
    sqlite3 *db,
    const char *sql,
    int (*callback)(void*,int,char**,char**),
    void *cbdata
);

int prepare_stmt(sqlite3 *db, char *sql, sqlite3_stmt **stmt);

int step_and_check(sqlite3 *db, sqlite3_stmt *stmt, int expect_row);

int begin_txn(sqlite3 *db);
int rollback_txn(sqlite3 *db);
int commit_txn(sqlite3 *db);

#endif