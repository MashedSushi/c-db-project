#include <stdio.h>
#include "app.h"

void rent_book(sqlite3 *db)
{
    Rental rent;
    if (read_text("대여할 도서 ID: ", rent.book_id, sizeof(rent.book_id)) != 1 ||
        read_text("대출자 이름: ", rent.user_name, sizeof(rent.user_name)) != 1 ||
        read_date("대여일 (YYYYMMDD): ", rent.rent_date) != 1)
        return;
    sqlite3_stmt *stmt =
        db_prepare(db, "INSERT INTO TBL_RENTAL (BOOK_ID, USER_NAME, RENT_DATE) VALUES (?, ?, ?);");
    if (!stmt)
        return;
    sqlite3_bind_text(stmt, 1, rent.book_id, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, rent.user_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, rent.rent_date, -1, SQLITE_TRANSIENT);
    if (db_write(db, stmt) > 0)
        puts("대여 완료");
}

void return_book(sqlite3 *db)
{
    int rent_id;
    char date[9];
    if (read_number("반납할 대여 ID (대여 목록에서 확인): ", &rent_id) != 1)
        return;
    if (rent_id == 0)
    {
        puts("[오류] 대여 ID는 1 이상이어야 합니다.");
        return;
    }
    if (read_date("반납일 (YYYYMMDD): ", date) != 1)
        return;
    sqlite3_stmt *stmt =
        db_prepare(db, "UPDATE TBL_RENTAL SET RETURN_DATE = ?2 "
                       "WHERE RENT_ID = ?1 AND RETURN_DATE IS NULL AND RENT_DATE <= ?2;");
    if (!stmt)
        return;
    sqlite3_bind_int(stmt, 1, rent_id);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_TRANSIENT);
    int changed = db_write(db, stmt);
    if (changed > 0)
        puts("반납 완료");
    else if (changed == 0)
        puts("[오류] 대여 ID, 반납 여부, 반납일이 대여일보다 이르지 않은지 확인하세요.");
}

void print_rentals(sqlite3 *db, int active_only)
{
    sqlite3_stmt *stmt =
        db_prepare(db, "SELECT R.RENT_ID AS 대여ID, B.BOOK_ID AS 도서ID, B.TITLE AS 제목, "
                       "R.USER_NAME AS 대출자, "
                       "substr(R.RENT_DATE, 1, 4) || '-' || substr(R.RENT_DATE, 5, 2) || '-' || "
                       "substr(R.RENT_DATE, 7, 2) AS 대여일, "
                       "COALESCE(substr(R.RETURN_DATE, 1, 4) || '-' || "
                       "substr(R.RETURN_DATE, 5, 2) || '-' || substr(R.RETURN_DATE, 7, 2), "
                       "'대여 중') AS 반납일 "
                       "FROM TBL_RENTAL R INNER JOIN TBL_BOOK B ON R.BOOK_ID = B.BOOK_ID "
                       "WHERE ?1 = 0 OR R.RETURN_DATE IS NULL ORDER BY R.RENT_ID DESC;");
    if (!stmt)
        return;
    sqlite3_bind_int(stmt, 1, active_only);
    db_print(db, stmt);
}

void print_stats(sqlite3 *db)
{
    const char *sql =
        "SELECT B.BOOK_ID AS 도서ID, B.TITLE AS 제목, COUNT(R.RENT_ID) AS 누적대여횟수 "
        "FROM TBL_BOOK B LEFT JOIN TBL_RENTAL R ON B.BOOK_ID = R.BOOK_ID "
        "GROUP BY B.BOOK_ID, B.TITLE ORDER BY 누적대여횟수 DESC, B.BOOK_ID;";
    db_print(db, db_prepare(db, sql));
}
