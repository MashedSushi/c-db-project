#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"

int read_text(const char *prompt, char *text, int size)
{
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(text, size, stdin))
        return -1;

    if (!strchr(text, '\n'))
    {
        int ch = getchar();
        // Windows 줄바꿈(CRLF)도 한 줄로 처리한다.
        if (ch == '\r')
            ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            printf("[오류] 최대 %d바이트까지 입력할 수 있습니다.\n", size - 1);
            return 0;
        }
    }

    int start = 0;
    int end = (int)strlen(text);
    while (start < end && isspace((unsigned char)text[start]))
        start++;
    while (end > start && isspace((unsigned char)text[end - 1]))
        end--;
    text[end] = '\0';
    memmove(text, text + start, end - start + 1);
    if (text[0] == '\0')
    {
        puts("[오류] 내용을 입력하세요.");
        return 0;
    }
    return 1;
}

int read_number(const char *prompt, int *number)
{
    char text[32];
    int result = read_text(prompt, text, sizeof(text));
    if (result != 1)
        return result;

    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value < 0 || value > INT_MAX)
    {
        puts("[오류] 올바른 정수를 입력하세요.");
        return 0;
    }
    *number = (int)value;
    return 1;
}

int read_date(const char *prompt, char date[9])
{
    int result = read_text(prompt, date, 9);
    if (result != 1)
        return result;

    int year, month, day;
    int valid = strlen(date) == 8;
    for (int i = 0; date[i] != '\0'; i++)
        if (!isdigit((unsigned char)date[i]))
            valid = 0;
    if (valid && sscanf(date, "%4d%2d%2d", &year, &month, &day) == 3 && year > 0 && month >= 1 &&
        month <= 12)
    {
        const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int limit = days[month - 1];
        if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
            limit++;
        if (day >= 1 && day <= limit)
            return 1;
    }
    puts("[오류] 실제 날짜를 YYYYMMDD 형식으로 입력하세요.");
    return 0;
}
