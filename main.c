#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

// 데이터베이스 연결 및 테스트를 수행하는 함수
int verify_database_connection(const char* db_filename) {
    sqlite3* db = NULL;
    int result_code = sqlite3_open(db_filename, &db);

    if (result_code != SQLITE_OK) {
        printf("Database connection failed: %s\n", sqlite3_errmsg(db));
        if (db != NULL) {
            sqlite3_close(db);
        }
        return 1;
    }

    printf("SQLite3 connected successfully!\n");
    sqlite3_close(db);
    return 0;
}

int main(void) {
    return verify_database_connection("test.db");
}