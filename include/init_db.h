#ifndef INIT_DB_H
#define INIT_DB_H

#include "sqlite3.h"

int initialize_db(void);

int connect_db(const char *db_name);
sqlite3 *get_db(void);
void disconnect_db(void);

#endif