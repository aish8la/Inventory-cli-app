#ifndef SQLITE_HELPERS_H
#define SQLITE_HELPERS_H

#include "sqlite3.h"


int open_db(sqlite3 **ppDb);
int run_sql(sqlite3 *db, const char *sql);

int run_sql_with_cb(
    sqlite3 *db,
    const char *sql,
    int (*callback)(void*,int,char**,char**),
    void *cbdata
);

#endif