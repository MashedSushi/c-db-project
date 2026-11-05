#ifndef APP_H
#define APP_H

#include "sqlite3.h"

typedef struct
{
    char book_id[32];
    char title[128];
    char author[64];
} Book;

typedef struct
{
    char book_id[32];
    char user_name[64];
    char rent_date[9];
} Rental;

// 입력 함수: 성공 1, 잘못된 입력 0, 입력 종료 -1
int read_text(const char *prompt, char *text, int size);
int read_number(const char *prompt, int *number);
int read_date(const char *prompt, char date[9]);

// 도서 1 : N 대여 이력. RETURN_DATE가 NULL이면 현재 대여 중이다.
sqlite3 *db_open(void);
sqlite3_stmt *db_prepare(sqlite3 *db, const char *sql);
int db_write(sqlite3 *db, sqlite3_stmt *stmt);
void db_print(sqlite3 *db, sqlite3_stmt *stmt);

void list_books(sqlite3 *db);
void search_books(sqlite3 *db);
void add_book(sqlite3 *db);
void update_book(sqlite3 *db);
void delete_book(sqlite3 *db);
void rent_book(sqlite3 *db);
void return_book(sqlite3 *db);
void print_rentals(sqlite3 *db, int active_only);
void print_stats(sqlite3 *db);

#endif
