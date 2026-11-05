#include <stdio.h>
#include "app.h"

void list_books(sqlite3 *db)
{
    const char *sql = "SELECT B.BOOK_ID AS 도서ID, B.TITLE AS 제목, B.AUTHOR AS 저자, "
                      "CASE WHEN EXISTS (SELECT 1 FROM TBL_RENTAL R "
                      "WHERE R.BOOK_ID = B.BOOK_ID AND R.RETURN_DATE IS NULL) "
                      "THEN '대여 중' ELSE '대여 가능' END AS 상태 "
                      "FROM TBL_BOOK B ORDER BY B.BOOK_ID;";
    db_print(db, db_prepare(db, sql));
}

void search_books(sqlite3 *db)
{
    char keyword[128], pattern[130];
    if (read_text("검색어 (도서 ID·제목·저자): ", keyword, sizeof(keyword)) != 1)
        return;
    // LIKE 검색을 위해 검색어 양쪽에 %를 붙인다.
    snprintf(pattern, sizeof(pattern), "%%%s%%", keyword);
    sqlite3_stmt *stmt = db_prepare(
        db, "SELECT BOOK_ID AS 도서ID, TITLE AS 제목, AUTHOR AS 저자 FROM TBL_BOOK "
            "WHERE BOOK_ID LIKE ?1 OR TITLE LIKE ?1 OR AUTHOR LIKE ?1 ORDER BY BOOK_ID;");
    if (!stmt)
        return;
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    db_print(db, stmt);
}

// 등록과 수정은 같은 항목을 입력받고, 실행할 SQL만 다르다.
static void save_book(sqlite3 *db, const char *sql, const char *message)
{
    Book book;
    if (read_text("도서 ID: ", book.book_id, sizeof(book.book_id)) != 1 ||
        read_text("제목: ", book.title, sizeof(book.title)) != 1 ||
        read_text("저자: ", book.author, sizeof(book.author)) != 1)
        return;
    sqlite3_stmt *stmt = db_prepare(db, sql);
    if (!stmt)
        return;
    sqlite3_bind_text(stmt, 1, book.book_id, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, book.title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, book.author, -1, SQLITE_TRANSIENT);
    int changed = db_write(db, stmt);
    if (changed > 0)
        puts(message);
    else if (changed == 0)
        puts("[오류] 해당 도서가 없습니다.");
}

void add_book(sqlite3 *db)
{
    save_book(db, "INSERT INTO TBL_BOOK (BOOK_ID, TITLE, AUTHOR) VALUES (?1, ?2, ?3);",
              "등록 완료");
}

void update_book(sqlite3 *db)
{
    save_book(db, "UPDATE TBL_BOOK SET TITLE = ?2, AUTHOR = ?3 WHERE BOOK_ID = ?1;", "수정 완료");
}

void delete_book(sqlite3 *db)
{
    char book_id[32];
    if (read_text("삭제할 도서 ID (대여 이력이 있는 도서는 삭제 불가): ", book_id,
                  sizeof(book_id)) != 1)
        return;
    sqlite3_stmt *stmt = db_prepare(db, "DELETE FROM TBL_BOOK WHERE BOOK_ID = ?;");
    if (!stmt)
        return;
    sqlite3_bind_text(stmt, 1, book_id, -1, SQLITE_TRANSIENT);
    int changed = db_write(db, stmt);
    if (changed > 0)
        puts("삭제 완료");
    else if (changed == 0)
        puts("[오류] 해당 도서가 없습니다.");
}
