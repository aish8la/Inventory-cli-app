#ifndef QUERY_HANDLERS_H
#define QUERY_HANDLERS_H

#include "sqlite3.h"

void display_table(sqlite3 *db, sqlite3_stmt *stmt, void (*display_header)(void), void (*display_row)(sqlite3_stmt *stmt));

#endif