#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

// -----------------------------------------------------------------------------
// [1] 데이터 구조체
// -----------------------------------------------------------------------------
typedef struct {
    char book_id[32];    // 도서ID (PK)
    char title[64];      // 책 제목
    char author[32];     // 저자
} Book;

typedef struct {
    char book_id[32];    // 도서ID (FK)
    char user_name[32];  // 대출자 이름
    char rent_date[16];  // 대출일자 (YYYYMMDD)
} RentalRecord;

// -----------------------------------------------------------------------------
// [2] DB 추상화 헬퍼
// -----------------------------------------------------------------------------
static int db_execute(sqlite3* db, const char* sql) {
    if (!db || !sql) return 0;
    return sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK;
}

static sqlite3_stmt* db_prepare(sqlite3* db, const char* sql) {
    if (!db || !sql) return NULL;
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return NULL;
    return stmt;
}

sqlite3* init_database(const char* db_name) {
    sqlite3* db = NULL;
    if (sqlite3_open(db_name, &db) != SQLITE_OK) return NULL;

    db_execute(db, "PRAGMA foreign_keys = ON;");

    // 도서 및 대여 테이블 생성
    db_execute(db, "CREATE TABLE IF NOT EXISTS TBL_BOOK ("
        "BOOK_ID CHAR(10) PRIMARY KEY, TITLE VARCHAR(50) NOT NULL, AUTHOR VARCHAR(30));");

    db_execute(db, "CREATE TABLE IF NOT EXISTS TBL_RENTAL ("
        "RENT_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "BOOK_ID CHAR(10) NOT NULL, USER_NAME VARCHAR(30) NOT NULL, RENT_DATE CHAR(8) NOT NULL, "
        "FOREIGN KEY (BOOK_ID) REFERENCES TBL_BOOK(BOOK_ID));");

    // 기본 샘플 도서 등록
    db_execute(db, "INSERT OR IGNORE INTO TBL_BOOK VALUES "
        "('B001', 'C언어 클린코드', '데니스'), "
        "('B002', 'SQLite 기초', '알렌'), "
        "('B003', '알고리즘 입문', '크누스');");

    return db;
}

// -----------------------------------------------------------------------------
// [3] 도서 관련 모듈
// -----------------------------------------------------------------------------
// 신규 도서 등록
int save_book(sqlite3* db, const Book* book) {
    const char* sql = "INSERT INTO TBL_BOOK VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = db_prepare(db, sql);
    if (!stmt) return 0;

    sqlite3_bind_text(stmt, 1, book->book_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, book->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, book->author, -1, SQLITE_STATIC);

    int ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// 전체 도서 목록 조회
void print_all_books(sqlite3* db) {
    const char* sql = "SELECT BOOK_ID, TITLE, AUTHOR FROM TBL_BOOK ORDER BY BOOK_ID ASC;";
    sqlite3_stmt* stmt = db_prepare(db, sql);
    if (!stmt) return;

    printf("\n%-8s | %-20s | %-12s\n", "도서ID", "책 제목", "저자");
    printf("---------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-8s | %-20s | %-12s\n",
            sqlite3_column_text(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

// -----------------------------------------------------------------------------
// [4] 대여 및 통계 모듈
// -----------------------------------------------------------------------------
int save_rental(sqlite3* db, const RentalRecord* rent) {
    const char* sql = "INSERT INTO TBL_RENTAL (BOOK_ID, USER_NAME, RENT_DATE) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = db_prepare(db, sql);
    if (!stmt) return 0;

    sqlite3_bind_text(stmt, 1, rent->book_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, rent->user_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, rent->rent_date, -1, SQLITE_STATIC);

    int ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// INNER JOIN 대여 목록 조회
void print_rental_history_joined(sqlite3* db) {
    const char* sql = "SELECT R.RENT_ID, R.BOOK_ID, B.TITLE, B.AUTHOR, R.USER_NAME, R.RENT_DATE "
        "FROM TBL_RENTAL R "
        "INNER JOIN TBL_BOOK B ON R.BOOK_ID = B.BOOK_ID "
        "ORDER BY R.RENT_ID DESC;";
    sqlite3_stmt* stmt = db_prepare(db, sql);
    if (!stmt) return;

    printf("\n%-6s | %-8s | %-16s | %-10s | %-10s | %-10s\n",
        "대여ID", "도서ID", "책 제목", "저자", "대출자", "대출일");
    printf("-----------------------------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-6d | %-8s | %-16s | %-10s | %-10s | %-10s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_text(stmt, 3),
            sqlite3_column_text(stmt, 4),
            sqlite3_column_text(stmt, 5));
    }
    sqlite3_finalize(stmt);
}

// GROUP BY 통계: 도서별 누적 대여 횟수
void print_book_stats(sqlite3* db) {
    const char* sql = "SELECT B.BOOK_ID, B.TITLE, COUNT(R.RENT_ID) AS RENT_COUNT "
        "FROM TBL_BOOK B "
        "LEFT JOIN TBL_RENTAL R ON B.BOOK_ID = R.BOOK_ID "
        "GROUP BY B.BOOK_ID, B.TITLE "
        "ORDER BY RENT_COUNT DESC;";
    sqlite3_stmt* stmt = db_prepare(db, sql);
    if (!stmt) return;

    printf("\n%-8s | %-20s | %-10s\n", "도서ID", "책 제목", "총 대여 횟수");
    printf("---------------------------------------------------\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-8s | %-20s | %-10d 회\n",
            sqlite3_column_text(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

// -----------------------------------------------------------------------------
// [5] 메인 컨트롤러
// -----------------------------------------------------------------------------
int main(void) {
    sqlite3* db = init_database("book_rental.db");
    if (!db) {
        printf("[오류] 데이터베이스 연결 실패\n");
        return 1;
    }

    int menu = -1;
    while (menu != 0) {
        printf("\n=============================================\n");
        printf("        미니 도서 대여 관리 시스템            \n");
        printf("=============================================\n");
        printf(" 1. 전체 도서 목록 조회\n");
        printf(" 2. 신규 도서 등록\n");
        printf(" 3. 도서 대여 처리\n");
        printf(" 4. 대여 내역 전체 조회 (INNER JOIN)\n");
        printf(" 5. 도서별 대여 통계 (GROUP BY + COUNT)\n");
        printf(" 0. 종료\n");
        printf("---------------------------------------------\n선택 > ");

        if (scanf("%d", &menu) != 1) break;

        if (menu == 1) {
            print_all_books(db);
        }
        else if (menu == 2) {
            Book b;
            printf("\n[도서 등록]\n도서ID 제목 저자: ");
            scanf("%s %s %s", b.book_id, b.title, b.author);
            printf(save_book(db, &b) ? ">> 등록 완료\n" : ">> 등록 실패 (중복 도서ID)\n");
        }
        else if (menu == 3) {
            RentalRecord r;
            printf("\n[도서 대여]\n도서ID 대출자이름 대출일자(YYYYMMDD): ");
            scanf("%s %s %s", r.book_id, r.user_name, r.rent_date);
            printf(save_rental(db, &r) ? ">> 대여 완료\n" : ">> 대여 실패 (존재하지 않는 도서ID)\n");
        }
        else if (menu == 4) {
            print_rental_history_joined(db);
        }
        else if (menu == 5) {
            print_book_stats(db);
        }
    }

    sqlite3_close(db);
    return 0;
}