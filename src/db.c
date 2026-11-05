#include <stdio.h>
#include <string.h>
#include "app.h"

static int execute_sql(sqlite3 *db, const char *sql)
{
    if (sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK)
        return 1;
    printf("[오류] DB 작업 실패: %s\n", sqlite3_errmsg(db));
    return 0;
}

sqlite3 *db_open(void)
{
    sqlite3 *db = NULL;
    if (sqlite3_open("book_rental.db", &db) != SQLITE_OK)
    {
        printf("[오류] DB 연결 실패: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    sqlite3_busy_timeout(db, 1000);

    // 샘플 데이터는 처음 DB를 만들 때만 넣는다. 삭제한 도서를 다시 만들지 않는다.
    sqlite3_stmt *stmt = db_prepare(
        db, "SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'TBL_BOOK';");
    if (!stmt)
        goto fail;
    int result = sqlite3_step(stmt);
    int new_database = result == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (result != SQLITE_ROW && result != SQLITE_DONE)
    {
        printf("[오류] DB 확인 실패: %s\n", sqlite3_errmsg(db));
        goto fail;
    }

    const char *schema =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS TBL_BOOK ("
        "BOOK_ID CHAR(10) PRIMARY KEY, TITLE VARCHAR(50) NOT NULL, AUTHOR VARCHAR(30));"
        "CREATE TABLE IF NOT EXISTS TBL_RENTAL ("
        "RENT_ID INTEGER PRIMARY KEY AUTOINCREMENT, BOOK_ID CHAR(10) NOT NULL, "
        "USER_NAME VARCHAR(30) NOT NULL, RENT_DATE CHAR(8) NOT NULL, RETURN_DATE CHAR(8), "
        "FOREIGN KEY (BOOK_ID) REFERENCES TBL_BOOK(BOOK_ID));";
    if (!execute_sql(db, schema))
        goto fail;

    // 이전 프로젝트 DB에 반납일이 없으면 열만 추가하고 기존 기록은 유지한다.
    stmt = db_prepare(db, "PRAGMA table_info(TBL_RENTAL)");
    if (!stmt)
        goto fail;
    int has_return_date = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
        if (strcmp((const char *)sqlite3_column_text(stmt, 1), "RETURN_DATE") == 0)
            has_return_date = 1;
    sqlite3_finalize(stmt);
    if (result != SQLITE_DONE)
    {
        printf("[오류] 테이블 확인 실패: %s\n", sqlite3_errmsg(db));
        goto fail;
    }
    if (!has_return_date &&
        !execute_sql(db, "ALTER TABLE TBL_RENTAL ADD COLUMN RETURN_DATE CHAR(8);"))
        goto fail;

    // 반납하지 않은 대여는 도서 한 권당 하나만 허용한다.
    if (!execute_sql(db, "CREATE UNIQUE INDEX IF NOT EXISTS one_active_rental "
                         "ON TBL_RENTAL(BOOK_ID) WHERE RETURN_DATE IS NULL;"))
        goto fail;
    if (new_database && !execute_sql(db, "INSERT INTO TBL_BOOK VALUES "
                                         "('B001', 'C언어 클린코드', '데니스'), "
                                         "('B002', 'SQLite 기초', '알렌'), "
                                         "('B003', '알고리즘 입문', '크누스');"))
        goto fail;
    return db;

fail:
    sqlite3_close(db);
    return NULL;
}

sqlite3_stmt *db_prepare(sqlite3 *db, const char *sql)
{
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        printf("[오류] SQL 준비 실패: %s\n", sqlite3_errmsg(db));
    return stmt;
}

// 성공하면 변경된 행 수, 실패하면 -1을 반환한다. stmt는 여기서 해제한다.
int db_write(sqlite3 *db, sqlite3_stmt *stmt)
{
    int result = sqlite3_step(stmt);
    int changed = sqlite3_changes(db);
    if (result != SQLITE_DONE)
    {
        int error = sqlite3_extended_errcode(db);
        if (error == SQLITE_CONSTRAINT_PRIMARYKEY)
            puts("[오류] 이미 등록된 도서 ID입니다.");
        else if (error == SQLITE_CONSTRAINT_UNIQUE)
            puts("[오류] 이미 대여 중인 도서입니다. 반납 후 다시 대여하세요.");
        else if (error == SQLITE_CONSTRAINT_FOREIGNKEY)
            puts("[오류] 도서 ID가 없거나 대여 이력이 있어 처리할 수 없습니다.");
        else
            printf("[오류] 저장 실패: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return result == SQLITE_DONE ? changed : -1;
}

void db_print(sqlite3 *db, sqlite3_stmt *stmt)
{
    if (!stmt)
        return;
    int columns = sqlite3_column_count(stmt);
    for (int i = 0; i < columns; i++)
        printf("%s%s", i == 0 ? "" : " | ", sqlite3_column_name(stmt, i));
    putchar('\n');

    int result;
    int rows = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        for (int i = 0; i < columns; i++)
        {
            const char *text = (const char *)sqlite3_column_text(stmt, i);
            printf("%s%s", i == 0 ? "" : " | ", text ? text : "-");
        }
        putchar('\n');
        rows++;
    }
    if (result != SQLITE_DONE)
        printf("[오류] 조회 실패: %s\n", sqlite3_errmsg(db));
    else if (rows == 0)
        puts("조회 결과가 없습니다.");
    sqlite3_finalize(stmt);
}
