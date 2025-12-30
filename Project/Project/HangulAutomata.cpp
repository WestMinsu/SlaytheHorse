#include "HangulAutomata.h"
#include <cctype> // isalpha, isupper

// =========================================================================================
//  데이터 테이블 정의 (링커 에러 방지용 더미 정의)
// =========================================================================================
const int HangulAutomata::KEY_TO_CHO[256] = { 0 };
const int HangulAutomata::KEY_TO_JUNG[256] = { 0 };
const int HangulAutomata::KEY_TO_JONG[256] = { 0 };

// =========================================================================================
//  실제 매핑 데이터 (a-z 순서)
// =========================================================================================

// 초성 매핑 (a~z)
static const int MAP_CHO_LOWER[26] = {
    6, -1, 14, 11, 3, 5, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 0, 2, 9, -1, 17, 12, 16, -1, 15
};

// 초성 매핑 (A~Z, Shift 키)
static const int MAP_CHO_UPPER[26] = {
    -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 1, -1, 10, -1, -1, 13, -1, -1, -1
};

// 중성 매핑 (a~z)
static const int MAP_JUNG_LOWER[26] = {
    -1, 17, -1, -1, -1, -1, -1, 8, 2, 4, 0, 20, 18, 13, 1, 5, -1, -1, -1, -1, 6, -1, -1, -1, 12, -1
};

// 중성 매핑 (A~Z)
static const int MAP_JUNG_UPPER[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

// 종성 매핑 (a~z) 
// [수정 완료] w(22번째 인덱스)를 20(ㅆ)에서 22(ㅈ)로 변경했습니다.
static const int MAP_JONG_LOWER[26] = {
    16, -1, 23, 21, 7, 8, 27, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, 1, 4, 19, -1, 26, 22, 25, -1, 24
};

// 종성 매핑 (A~Z)
static const int MAP_JONG_UPPER[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, 20, -1, -1, -1, -1, -1, -1
};


// =========================================================================================
//  구현
// =========================================================================================

HangulAutomata::HangulAutomata()
{
}

int GetMapValue(char c, const int* lowerMap, const int* upperMap)
{
    if (c >= 'a' && c <= 'z')
        return lowerMap[c - 'a'];

    if (c >= 'A' && c <= 'Z')
        return upperMap[c - 'A'];

    return -1;
}

void HangulAutomata::Clear()
{
    completeText.clear();
    nCho = -1; nJung = -1; nJong = -1;
}

std::wstring HangulAutomata::Flush()
{
    CommitState();
    std::wstring res = completeText;
    Clear();
    return res;
}

wchar_t HangulAutomata::MakeHangul(int cho, int jung, int jong) const
{
    if (cho != -1 && jung == -1) {
        const int CHO_UNICODE[] = {
            0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142,
            0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B,
            0x314C, 0x314D, 0x314E
        };
        return (wchar_t)CHO_UNICODE[cho];
    }

    if (cho != -1 && jung != -1) {
        int jongVal = (jong != -1) ? jong : 0;
        return (wchar_t)(0xAC00 + (cho * 588) + (jung * 28) + jongVal);
    }

    if (cho == -1 && jung != -1) {
        const int JUNG_UNICODE[] = {
            0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156,
            0x3157, 0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E,
            0x315F, 0x3160, 0x3161, 0x3162, 0x3163
        };
        return (wchar_t)JUNG_UNICODE[jung];
    }

    return 0;
}

void HangulAutomata::CommitState()
{
    wchar_t c = MakeHangul(nCho, nJung, nJong);
    if (c != 0) completeText += c;
    nCho = -1; nJung = -1; nJong = -1;
}

std::wstring HangulAutomata::GetDisplayString() const
{
    std::wstring disp = completeText;

    int cho = nCho; int jung = nJung; int jong = nJong;

    if (cho != -1 || jung != -1) {
        // 내부 로직 복사 (MakeHangul)
        wchar_t temp = 0;
        if (cho != -1 && jung == -1) {
            const int CHO_UNICODE[] = { 0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E };
            temp = (wchar_t)CHO_UNICODE[cho];
        }
        else if (cho == -1 && jung != -1) {
            const int JUNG_UNICODE[] = { 0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157, 0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E, 0x315F, 0x3160, 0x3161, 0x3162, 0x3163 };
            temp = (wchar_t)JUNG_UNICODE[jung];
        }
        else if (cho != -1 && jung != -1) {
            int jongVal = (jong != -1) ? jong : 0;
            temp = (wchar_t)(0xAC00 + (cho * 588) + (jung * 28) + jongVal);
        }

        if (temp != 0) disp += temp;
    }
    return disp;
}

void HangulAutomata::Type(char c)
{
    // 1. 기본 매핑 시도 (대문자면 UPPER 테이블, 소문자면 LOWER 테이블 조회)
    int inCho = GetMapValue(c, MAP_CHO_LOWER, MAP_CHO_UPPER);
    int inJung = GetMapValue(c, MAP_JUNG_LOWER, MAP_JUNG_UPPER);
    int inJong = GetMapValue(c, MAP_JONG_LOWER, MAP_JONG_UPPER);

    // [버그 수정] Shift 키(대문자)가 눌렸지만, 쌍자음/이중모음이 아닌 경우 처리
    // 예: Shift+'a'('A')는 특수 한글이 없으므로 -1이 반환됨 -> 소문자 'a'('ㅁ')로 다시 조회해야 함
    if (std::isupper(c))
    {
        // 셋 다 매핑되지 않았다면 소문자로 변환해서 재시도
        if (inCho == -1 && inJung == -1 && inJong == -1)
        {
            char lowerC = std::tolower(c);
            inCho = GetMapValue(lowerC, MAP_CHO_LOWER, MAP_CHO_UPPER);
            inJung = GetMapValue(lowerC, MAP_JUNG_LOWER, MAP_JUNG_UPPER);
            inJong = GetMapValue(lowerC, MAP_JONG_LOWER, MAP_JONG_UPPER);
        }
    }

    // 여전히 매핑되지 않았다면(특수문자, 공백 등) 영문/기호 그대로 출력
    if (inCho == -1 && inJung == -1) {
        CommitState();
        completeText += (wchar_t)c;
        return;
    }

    // ... (이하 기존 로직 동일) ...

    if (nCho == -1 && nJung == -1) {
        if (inCho != -1) {
            nCho = inCho;
        }
        else if (inJung != -1) {
            nJung = inJung;
        }
        return;
    }

    // ... (나머지 Type 함수 내용 그대로 유지) ...
    // 아래 코드는 기존 코드의 나머지 부분입니다 (복사해서 붙여넣으세요)

    if (nCho != -1 && nJung == -1) {
        if (inJung != -1) {
            nJung = inJung;
        }
        else if (inCho != -1) {
            CommitState();
            nCho = inCho;
        }
        return;
    }

    if (nCho != -1 && nJung != -1 && nJong == -1) {
        if (inJong != -1) {
            nJong = inJong;
        }
        else if (inJung != -1) {
            int combined = CombineJung(nJung, inJung);
            if (combined != -1) {
                nJung = combined;
            }
            else {
                CommitState();
                nJung = inJung;
            }
        }
        else if (inCho != -1) {
            CommitState();
            nCho = inCho;
        }
        return;
    }

    if (nCho != -1 && nJung != -1 && nJong != -1) {
        if (inJung != -1) {
            int curJong = 0, nextCho = 0;
            SeparateJong(nJong, curJong, nextCho);

            int tempCho = nextCho;
            int tempJung = inJung;

            nJong = (curJong == 0) ? -1 : curJong;
            CommitState();

            nCho = tempCho;
            nJung = tempJung;
        }
        else if (inJong != -1) {
            int combined = CombineJong(nJong, inJong);
            if (combined != -1) {
                nJong = combined;
            }
            else {
                CommitState();
                nCho = GetMapValue(c, MAP_CHO_LOWER, MAP_CHO_UPPER);
            }
        }
        else if (inCho != -1) {
            CommitState();
            nCho = inCho;
        }
        return;
    }
}

void HangulAutomata::Backspace()
{
    if (nJong != -1) {
        nJong = -1;
    }
    else if (nJung != -1) {
        nJung = -1;
    }
    else if (nCho != -1) {
        nCho = -1;
    }
    else if (!completeText.empty()) {
        completeText.pop_back();
    }
}

int HangulAutomata::CombineJung(int j1, int j2) {
    if (j1 == 8) {
        if (j2 == 0) return 9;
        if (j2 == 1) return 10;
        if (j2 == 20) return 11;
    }
    if (j1 == 13) {
        if (j2 == 4) return 14;
        if (j2 == 5) return 15;
        if (j2 == 20) return 16;
    }
    if (j1 == 18 && j2 == 20) return 19;
    return -1;
}

int HangulAutomata::CombineJong(int j1, int j2) {
    if (j1 == 1 && j2 == 19) return 3;
    if (j1 == 4) {
        if (j2 == 22) return 5;
        if (j2 == 27) return 6;
    }
    if (j1 == 8) {
        if (j2 == 1) return 9;
        if (j2 == 16) return 10;
        if (j2 == 17) return 11;
        if (j2 == 19) return 12;
        if (j2 == 25) return 13;
        if (j2 == 26) return 14;
        if (j2 == 27) return 15;
    }
    if (j1 == 17 && j2 == 19) return 18;
    return -1;
}

void HangulAutomata::SeparateJong(int jong, int& outCurJong, int& outNextCho) {
    // 겹받침 매핑
    // ... (이전과 동일)

    // [중요 수정사항]
    // 종성 인덱스(1~27)를 초성 인덱스(0~18)로 변환하는 테이블입니다.
    // 기존 코드에서 ㅊ(23), ㅋ(24)의 매핑이 잘못되어 있었습니다.

    int jToC[30];
    for (int i = 0; i < 30; i++) jToC[i] = -1;

    jToC[1] = 0;  // ㄱ
    jToC[2] = 1;  // ㄲ
    jToC[4] = 2;  // ㄴ
    jToC[7] = 3;  // ㄷ
    jToC[8] = 5;  // ㄹ
    jToC[16] = 6; // ㅁ
    jToC[17] = 7; // ㅂ
    jToC[19] = 9; // ㅅ
    jToC[20] = 10; // ㅆ
    jToC[21] = 11; // ㅇ
    jToC[22] = 12; // ㅈ

    // [수정된 부분]
    jToC[23] = 14; // ㅊ (기존 13(ㅉ) -> 14(ㅊ)로 수정)
    jToC[24] = 15; // ㅋ (기존 14(ㅊ) -> 15(ㅋ)로 수정)

    jToC[25] = 16; // ㅌ
    jToC[26] = 17; // ㅍ
    jToC[27] = 18; // ㅎ

    int first = jong;
    int second = 0;

    switch (jong) {
    case 3: first = 1; second = 19; break; // ㄳ
    case 5: first = 4; second = 22; break; // ㄵ
    case 6: first = 4; second = 27; break; // ㄶ
    case 9: first = 8; second = 1; break;  // ㄺ
    case 10: first = 8; second = 16; break; // ㄻ
    case 11: first = 8; second = 17; break; // ㄼ
    case 12: first = 8; second = 19; break; // ㄽ
    case 13: first = 8; second = 25; break; // ㄾ
    case 14: first = 8; second = 26; break; // ㄿ
    case 15: first = 8; second = 27; break; // ㅀ
    case 18: first = 17; second = 19; break; // ㅄ
    default: second = jong; first = 0; break;
    }

    outCurJong = first;
    outNextCho = jToC[second];
}