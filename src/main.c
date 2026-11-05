#include <stdio.h>
#include "app.h"

int main(void)
{
    sqlite3 *db = db_open();
    if (!db)
        return 1;

    puts("\n=== 미니 도서 대여 관리 ===\n"
         "1. 전체 도서 목록\n"
         "2. 도서 등록\n"
         "3. 도서 대여\n"
         "4. 대여·반납 이력\n"
         "5. 도서별 대여 통계\n"
         "6. 도서 검색 (ID·제목·저자)\n"
         "7. 도서 정보 수정\n"
         "8. 도서 삭제\n"
         "9. 도서 반납\n"
         "10. 현재 대여 목록\n"
         "0. 종료");

    while (1)
    {
        int menu;
        int result = read_number("선택 > ", &menu);
        if (result < 0 || (result == 1 && menu == 0))
            break;
        if (result == 0)
            continue;

        switch (menu)
        {
        case 1:
            list_books(db);
            break;
        case 2:
            add_book(db);
            break;
        case 3:
            rent_book(db);
            break;
        case 4:
            print_rentals(db, 0);
            break;
        case 5:
            print_stats(db);
            break;
        case 6:
            search_books(db);
            break;
        case 7:
            update_book(db);
            break;
        case 8:
            delete_book(db);
            break;
        case 9:
            return_book(db);
            break;
        case 10:
            print_rentals(db, 1);
            break;
        default:
            puts("[오류] 0~10 중에서 선택하세요.");
        }
    }

    sqlite3_close(db);
    return 0;
}
