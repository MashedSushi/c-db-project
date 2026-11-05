# c-db-project

C와 SQLite로 만든 미니 도서 대여 관리 프로그램입니다.
SQLite와 표 출력용 libfort 소스가 포함되어 있어 두 라이브러리를 별도로 설치할 필요가 없습니다.

## 빠른 실행

소스를 내려받고 VS Code에서 **Makefile이 있는 프로젝트 폴더 전체**를 엽니다.
컴파일러와 Make가 준비된 터미널에서 실행합니다.

```sh
make        # 빌드
make run    # db 폴더에서 실행하여 기존 DB 사용
```

환경별 준비 방법은 아래와 같습니다. VS Code의 C/C++ 확장과 컴파일러는 별도로 설치합니다.

| 환경                    | 준비할 도구                  | 실행 방법                    |
| ----------------------- | ---------------------------- | ---------------------------- |
| macOS + VS Code         | Xcode Command Line Tools     | 프로젝트 루트에서 `make run` |
| Ubuntu/Debian + VS Code | GCC, Make                    | 프로젝트 루트에서 `make run` |
| Windows + VS Code       | MSYS2 UCRT64의 GCC, Make     | MSYS2 터미널에서 `make run`  |
| Windows + Visual Studio | MSVC C/C++ 도구, Windows SDK | 아래 Visual Studio 안내 참고 |

## macOS + VS Code

1. [Xcode Command Line Tools](https://developer.apple.com/documentation/xcode/installing-the-command-line-tools/)를 설치합니다. 이미 설치되어 있으면 생략합니다.

    ```sh
    xcode-select --install
    ```

2. 설치가 끝나면 VS Code 터미널을 새로 열고 확인합니다.

    ```sh
    gcc --version
    make --version
    ```

3. 프로젝트 루트에서 실행합니다.

    ```sh
    make run
    ```

Xcode 도구의 `gcc` 명령은 Apple Clang을 실행합니다. 현재 프로젝트는 이 환경에서 검증했습니다.
실행 파일을 지우고 다시 빌드하려면 `make clean` 다음 `make run`을 실행합니다.

## Ubuntu/Debian + VS Code

터미널에서 GCC와 Make가 포함된 개발 도구를 설치합니다.
다른 Linux 배포판에서는 해당 배포판의 패키지 관리자로 GCC와 Make를 설치합니다.

```sh
sudo apt update
sudo apt install build-essential
```

프로젝트 루트에서 `make run`을 실행합니다.
[Ubuntu 컴파일러 설치 안내](https://help.ubuntu.com/community/InstallingCompilers)

## Windows + VS Code: MSYS2 사용

아래 안내는 Windows x64 환경 기준입니다.

1. [MSYS2](https://www.msys2.org/)를 설치하고 시작 메뉴에서 **MSYS2 UCRT64**를 엽니다.
2. 아래 명령으로 업데이트합니다. 터미널을 닫으라는 안내가 나오면 UCRT64를 다시 열고 같은 명령을 다시 실행합니다.

    ```sh
    pacman -Syu
    ```

3. GCC와 Make를 설치합니다.

    ```sh
    pacman -S --needed mingw-w64-ucrt-x86_64-gcc make
    gcc --version
    make --version
    ```

4. UCRT64 터미널에서 프로젝트 폴더로 이동해 실행합니다. 경로는 자신의 저장 위치로 바꿉니다.

    ```sh
    cd /c/work/c-db-project
    make run
    ```

Windows의 `C:\work\c-db-project`는 MSYS2에서 `/c/work/c-db-project`로 표현합니다.
경로에 공백이 있으면 `cd "/c/Users/My Name/Downloads/c-db-project"`처럼 따옴표로 감쌉니다.
이 방식의 `make run`은 MSYS2 터미널에서 실행합니다.

### VS Code 안에서 MSYS2 터미널 열기

명령 팔레트에서 **Preferences: Open User Settings (JSON)**을 열고 아래 항목을 기존 설정에 합칩니다.
MSYS2 설치 경로가 다르면 `C:\\msys64` 부분을 바꿉니다.

```json
{
    "terminal.integrated.profiles.windows": {
        "MSYS2 UCRT64": {
            "path": "cmd.exe",
            "args": ["/c", "C:\\msys64\\msys2_shell.cmd -defterm -here -no-start -ucrt64"]
        }
    }
}
```

터미널의 새 터미널 메뉴에서 **MSYS2 UCRT64**를 선택한 뒤 `make run`을 실행합니다.
[MSYS2의 VS Code 터미널 설정 안내](https://www.msys2.org/docs/ides-editors/#visual-studio-code)

Windows에서 생성된 실행 파일은 `src/cdb.exe`입니다. 실행 파일을 지우려면 MSYS2에서
`rm -f src/cdb.exe`를 실행합니다. 다른 PC에서는 소스로 다시 빌드합니다.

## Windows + Visual Studio: MSVC 사용

Visual Studio Installer에서 **C++를 사용한 데스크톱 개발** 워크로드와 Windows SDK를 설치합니다.
아래 안내는 C11을 지원하는 Visual Studio 2022 이상을 기준으로 합니다.
GCC용 Makefile 대신 MSVC로 같은 C 소스를 빌드합니다.

### 개발자 명령 프롬프트에서 실행

시작 메뉴에서 **x64 Native Tools Command Prompt for VS**를 열고 실행합니다.
경로는 자신의 프로젝트 위치로 바꿉니다.

```bat
cd /d "C:\work\c-db-project"
if not exist x64 mkdir x64
cl /nologo /TC /std:c11 /utf-8 /D_CRT_SECURE_NO_WARNINGS /Fox64\ /Fex64\cdb.exe src\main.c src\input.c src\db.c src\book.c src\rental.c src\sqlite3.c src\fort.c
cd db
chcp 65001
..\x64\cdb.exe
```

- `/TC`, `/std:c11`: 소스를 C11로 컴파일합니다.
- `/utf-8`: 한글 소스와 문자열을 UTF-8로 컴파일합니다.
- `/D_CRT_SECURE_NO_WARNINGS`: 현재 사용한 표준 C 함수의 MSVC CRT 경고를 해제합니다. 입력 길이와 날짜 검사는 코드에서 수행합니다.
- `chcp 65001`: 실행 전에 Windows 콘솔의 한글 입력·출력을 UTF-8로 맞춥니다.
- `x64` 폴더: 실행 파일과 중간 빌드 파일을 저장합니다. Git 관리에서 제외되어 있습니다.

[C11 설정](https://learn.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version),
[UTF-8 컴파일 설정](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8),
[CRT 경고 설정](https://learn.microsoft.com/en-us/cpp/c-runtime-library/security-features-in-the-crt),
[콘솔 코드 페이지](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/chcp)

### Visual Studio IDE에서 프로젝트 만들기

현재 저장소에는 `.sln`이나 `.vcxproj` 파일이 없으므로 새 **C++ 빈 프로젝트**를 만듭니다.
프로젝트는 저장소 밖에 만들고, **추가 > 기존 항목**으로 아래 파일만 추가합니다.

```text
src/main.c
src/input.c
src/db.c
src/book.c
src/rental.c
src/sqlite3.c
src/fort.c
src/app.h
src/sqlite3.h
src/fort.h
```

루트에 있는 `main.c`는 별도 연결 테스트 코드이므로 이 프로젝트에 함께 추가하지 않습니다.
프로젝트 속성에서 **모든 구성 / 모든 플랫폼**에 다음을 설정합니다.

| 속성                             | 설정                                                            |
| -------------------------------- | --------------------------------------------------------------- |
| C/C++ > 고급 > 컴파일 형식       | C 코드로 컴파일 `/TC`                                           |
| C/C++ > 언어 > C 언어 표준       | ISO C11 `/std:c11`                                              |
| C/C++ > 명령줄 > 추가 옵션       | `/utf-8` 추가                                                   |
| C/C++ > 전처리기 > 전처리기 정의 | `_CRT_SECURE_NO_WARNINGS` 추가, 기존 정의 유지                  |
| 링커 > 시스템 > 하위 시스템      | 콘솔 `/SUBSYSTEM:CONSOLE`                                       |
| 디버깅 > 작업 디렉터리           | 내려받은 저장소의 `db` 전체 경로, 예: `C:\work\c-db-project\db` |

빌드 후 실행합니다. IDE 디버깅 콘솔에서 한글이 깨지면 위의 개발자 명령 프롬프트 방식으로
컴파일하고 `chcp 65001` 적용 후 실행합니다.

## DB 파일과 실행 경로

도서·대여 데이터는 `db/book_rental.db`에 보관합니다. C 소스와 헤더는 `src/`에 있습니다.
프로그램은 **현재 작업 폴더**의 `book_rental.db`를 엽니다.
기존 데이터를 사용하려면 작업 폴더가 저장소의 `db`여야 합니다.

- `make run`은 자동으로 `db`로 이동합니다.
- 실행 파일을 직접 실행한다면 먼저 `db`로 이동합니다.
- 다른 폴더에서 실행하면 그 폴더에 새 DB가 생성될 수 있습니다.

DB가 없으면 테이블과 샘플 도서 3권을 생성합니다.
이전 형식의 도서 DB에는 반납일 열을 추가하며 기존 도서·대여 기록을 유지합니다.

## 사용 방법

입력은 항목마다 한 줄씩 받으므로 제목과 이름에 공백을 사용할 수 있습니다.
날짜는 `YYYYMMDD` 형식으로 입력하고, 조회 표에서는 `YYYY-MM-DD` 형식으로 표시합니다.
검색은 ID·제목·저자의 일부로 가능합니다.
메뉴 1~5는 목록·등록·대여·이력·통계, 6~10은 검색·수정·삭제·반납·현재 대여 목록입니다.
메뉴는 처음 한 번 표시하며, 이후에는 `선택 >`에서 번호를 입력합니다.
종료하려면 `0`을 입력합니다.

도서 등록 예시는 다음과 같습니다. 각 줄을 입력하고 Enter를 누릅니다.

```text
2
B100
C 언어 심화
김 작가
```

반납은 대여 ID로 처리하며, 대여 이력이 있는 도서는 기록 보존을 위해 삭제할 수 없습니다.

목록·검색·대여 이력·통계는 테두리와 열 정렬이 있는 표로 출력합니다.
한글과 영문을 함께 표시해도 열 너비를 맞추며, 숫자는 오른쪽으로 정렬하고 조회 건수를 표시합니다.
고정폭 글꼴을 사용하는 터미널에서 실행합니다.

표 출력에는 [libfort v0.5.2 태그의 소스](https://github.com/seleznevae/libfort/tree/v0.5.2)를 사용합니다.
`src/fort.c`와 `src/fort.h`를 포함했으며, 각 파일의 MIT 라이선스 고지를 유지합니다.

## 자주 발생하는 문제

| 증상                                 | 확인할 내용                                                                                       |
| ------------------------------------ | ------------------------------------------------------------------------------------------------- |
| `make` 또는 `gcc`를 찾을 수 없음     | 해당 환경의 개발 도구 설치 후 터미널을 새로 엽니다. Windows에서는 UCRT64를 사용합니다.            |
| `cl`을 찾을 수 없음                  | 일반 터미널 대신 Visual Studio의 x64 개발자 명령 프롬프트를 엽니다.                               |
| `main.c`만 실행했더니 연결 오류 발생 | 앱의 여러 C 파일을 함께 빌드해야 합니다. VS Code에서는 `make run`을 사용합니다.                   |
| 기존 도서나 대여 기록이 보이지 않음  | 실행 시 작업 폴더가 `db`인지 확인합니다.                                                          |
| Windows에서 한글이 깨짐              | MSVC의 `/utf-8` 설정과 실행 콘솔의 `chcp 65001`을 확인합니다. MSYS2는 UCRT64 터미널을 사용합니다. |
| 코드 수정이 반영되지 않음            | 실행 중인 프로그램을 `0`으로 종료하고 다시 빌드합니다.                                            |
| 이미 대여 중인 도서라는 오류         | 10번에서 현재 대여 목록을 확인하고 9번에서 반납 후 대여합니다.                                    |

## 코드 구성과 포맷

`src/main.c`는 메뉴, `input.c`는 입력 검사, `db.c`는 DB 연결과 공통 SQL 처리,
`book.c`는 도서 CRUD·검색, `rental.c`는 대여·반납·통계를 담당합니다.
공통 구조체와 함수 선언은 `src/app.h`에 있습니다.

VS Code에서 Prettier와 C/C++ 확장을 사용하면 저장 시 자동으로 포맷합니다.
JSON·Markdown·YAML은 `.prettierrc`, C는 `.clang-format` 설정을 따릅니다.
들여쓰기는 공백 4칸, 줄 길이는 100자를 기준으로 하며 SQLite와 libfort 원본은 포맷에서 제외합니다.

## 검증 범위

macOS에서 빌드, CRUD·검색·대여·반납·통계와 입력 검증을 테스트했습니다.
Linux와 Windows의 실제 실행은 아직 검증하지 않았으며, 위 안내는 각 도구의 공식 문서를 기준으로 작성했습니다.
