#include "HangulAutomata.h"
#include <cctype> // isalpha, isupper

// =========================================================================================
//  데이터 테이블 정의 (링커 에러 방지용 더미 정의)
//  실제 로직은 아래의 로컬 배열을 사용합니다.
// =========================================================================================
const int HangulAutomata::KEY_TO_CHO[256] = { 0 };
const int HangulAutomata::KEY_TO_JUNG[256] = { 0 };
const int HangulAutomata::KEY_TO_JONG[256] = { 0 };

// =========================================================================================
//  실제 매핑 데이터 (a-z 순서)
// =========================================================================================

// 초성 매핑 (a~z)
// a(ㅁ)=6, b(ㅠ)=-1, c(ㅊ)=14, d(ㅇ)=11, e(ㄷ)=3, f(ㄹ)=5, g(ㅎ)=18, h(ㅗ)=-1 ...
static const int MAP_CHO_LOWER[26] = {
    6, -1, 14, 11, 3, 5, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, 7, 0, 2, 9, -1, 17, 12, 16, -1, 15
};

// 초성 매핑 (A~Z, Shift 키)
// Q(ㅃ)=8, W(ㅉ)=13, E(ㄸ)=4, R(ㄲ)=1, T(ㅆ)=10, O(ㅒ)=-1, P(ㅖ)=-1
static const int MAP_CHO_UPPER[26] = {
    -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 1, -1, 10, -1, -1, 13, -1, -1, -1
};

// 중성 매핑 (a~z)
// h(ㅗ)=8, j(ㅓ)=4, k(ㅏ)=0, l(ㅣ)=20 ... b(ㅠ)=17, n(ㅜ)=13, m(ㅡ)=18
static const int MAP_JUNG_LOWER[26] = {
    -1, 17, -1, -1, -1, -1, -1, 8, 2, 4, 0, 20, 18, 13, 1, 5, -1, -1, -1, -1, 6, -1, -1, -1, 12, -1
};

// 중성 매핑 (A~Z) - O(ㅒ)=3, P(ㅖ)=7
static const int MAP_JUNG_UPPER[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

// 종성 매핑 (a~z) - 0은 없음, 1(ㄱ)부터 시작
// a(ㅁ)=16, s(ㄴ)=4, d(ㅇ)=21, f(ㄹ)=8, g(ㅎ)=27 ...
static const int MAP_JONG_LOWER[26] = {
    16, -1, 23, 21, 7, 8, 27, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, 1, 4, 19, -1, 26, 20, 25, -1, 24
};

// 종성 매핑 (A~Z) - Shift+R(ㄲ)=2, Shift+T(ㅆ)=20
static const int MAP_JONG_UPPER[26] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, 20, -1, -1, -1, -1, -1, -1
};


// =========================================================================================
//  구현
// =========================================================================================

HangulAutomata::HangulAutomata()
{
}

// 헬퍼: 문자 c에 해당하는 매핑 값을 찾아주는 함수
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
    // 1. 초성만 있는 경우 (자음 낱자)
    if (cho != -1 && jung == -1) {
        const int CHO_UNICODE[] = {
            0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142,
            0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B,
            0x314C, 0x314D, 0x314E
        };
        return (wchar_t)CHO_UNICODE[cho];
    }

    // 2. 완성형 한글
    if (cho != -1 && jung != -1) {
        int jongVal = (jong != -1) ? jong : 0;
        return (wchar_t)(0xAC00 + (cho * 588) + (jung * 28) + jongVal);
    }

    // 3. 중성만 있는 경우 (모음 낱자)
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

    // 현재 조합 중인 글자 계산
    int cho = nCho; int jung = nJung; int jong = nJong;

    if (cho != -1 || jung != -1) {
        wchar_t temp = MakeHangul(cho, jung, jong); // const 제거됨에 따라 내부 호출 가능하면 좋으나, static helper가 아님.
        // MakeHangul은 멤버 함수이므로 복사해서 쓰거나 로직 동일하게 수행

        // (위의 MakeHangul 로직 복사)
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

// ------------------------------------------------------------
// 핵심: 키 입력 처리 (State Machine)
// ------------------------------------------------------------
void HangulAutomata::Type(char c)
{
    // [수정됨] 헬퍼 함수를 통해 정확한 인덱스 매핑 사용
    int inCho = GetMapValue(c, MAP_CHO_LOWER, MAP_CHO_UPPER);
    int inJung = GetMapValue(c, MAP_JUNG_LOWER, MAP_JUNG_UPPER);
    int inJong = GetMapValue(c, MAP_JONG_LOWER, MAP_JONG_UPPER);

    // 특수문자나 공백 (매핑되지 않은 문자)
    if (inCho == -1 && inJung == -1) {
        CommitState();
        completeText += (wchar_t)c;
        return;
    }

    // 1. 초성도 없는 상태 (새 글자 시작)
    if (nCho == -1 && nJung == -1) {
        if (inCho != -1) {
            nCho = inCho; // 초성 입력
        }
        else if (inJung != -1) {
            nJung = inJung; // 모음만 입력 (중성 상태로)
        }
        return;
    }

    // 2. 초성만 있는 상태
    if (nCho != -1 && nJung == -1) {
        if (inJung != -1) {
            nJung = inJung; // 자음+모음 -> 글자 조합 시작
        }
        else if (inCho != -1) {
            // 초성+자음 (예: ㄱ+ㄴ) -> 앞 글자(ㄱ) 완료처리하고 뒤 글자(ㄴ) 새로 시작
            CommitState();
            nCho = inCho;
        }
        return;
    }

    // 3. 초성+중성 있는 상태 (종성 올 차례)
    if (nCho != -1 && nJung != -1 && nJong == -1) {
        if (inJong != -1) {
            // 자음 입력됨 -> 종성으로 붙임 (예: 가+ㄱ=각)
            // 단, 'ㄸ, ㅉ, ㅃ' 같은 건 종성으로 못 오지만 inJong 테이블에서 -1 처리됨
            nJong = inJong;
        }
        else if (inJung != -1) {
            // 모음 입력됨 -> 복모음 확인 (예: 고+ㅏ=과)
            int combined = CombineJung(nJung, inJung);
            if (combined != -1) {
                nJung = combined; // 합쳐짐
            }
            else {
                // 안 합쳐짐 -> 앞 글자 완료, 새 글자(모음) 시작 (예: 가+ㅏ=가아)
                CommitState();
                nJung = inJung;
            }
        }
        else if (inCho != -1) {
            // 종성으로 쓸 수 없는 자음이 옴 (거의 없지만 ㄸ 같은 경우)
            CommitState();
            nCho = inCho;
        }
        return;
    }

    // 4. 초성+중성+종성 있는 상태
    if (nCho != -1 && nJung != -1 && nJong != -1) {
        if (inJung != -1) {
            // [중요] 연음 법칙 (예: 각+ㅏ -> 가가)
            int curJong = 0, nextCho = 0;
            SeparateJong(nJong, curJong, nextCho);

            // 1. 현재 글자(종성 빠짐) 완성해서 커밋
            // 임시 저장
            int tempCho = nextCho;
            int tempJung = inJung; // 새로 들어온 모음

            nJong = (curJong == 0) ? -1 : curJong;
            CommitState();

            // 2. 떼어낸 받침을 초성으로 새 글자 세팅
            nCho = tempCho;
            nJung = tempJung;
        }
        else if (inJong != -1) {
            // 자음 입력 -> 겹받침 시도 (예: 간+ㄱ=삯)
            int combined = CombineJong(nJong, inJong);
            if (combined != -1) {
                nJong = combined;
            }
            else {
                // 겹받침 불가 -> 앞 글자 완료, 새 글자(초성) 시작
                CommitState();
                nCho = GetMapValue(c, MAP_CHO_LOWER, MAP_CHO_UPPER); // inJong값 말고 초성값으로 세팅
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
    // 1. 종성 있음 -> 겹받침이면 분리, 홑받침이면 제거
    if (nJong != -1) {
        // 겹받침 분해 로직 구현 생략 (간단히 제거)
        // 디테일한 구현을 위해선 CombineJong의 역매핑이 필요함
        nJong = -1;
    }
    // 2. 중성 있음 -> 복모음이면 분리? -> 간단히 제거
    else if (nJung != -1) {
        nJung = -1;
    }
    // 3. 초성 있음 -> 제거
    else if (nCho != -1) {
        nCho = -1;
    }
    // 4. 아무것도 없으면 -> completeText에서 한 글자 뺌
    else if (!completeText.empty()) {
        completeText.pop_back();
    }
}

// ------------------------------------------------------------
// 복잡한 한글 결합 규칙 (하드코딩)
// ------------------------------------------------------------

int HangulAutomata::CombineJung(int j1, int j2) {
    // ㅗ(8) + ㅏ(0) = ㅘ(9)
    // ㅗ(8) + ㅐ(1) = ㅙ(10)
    // ㅗ(8) + ㅣ(20) = ㅚ(11)
    if (j1 == 8) {
        if (j2 == 0) return 9;
        if (j2 == 1) return 10;
        if (j2 == 20) return 11;
    }
    // ㅜ(13) + ㅓ(4) = ㅝ(14)
    // ㅜ(13) + ㅔ(5) = ㅞ(15)
    // ㅜ(13) + ㅣ(20) = ㅟ(16)
    if (j1 == 13) {
        if (j2 == 4) return 14;
        if (j2 == 5) return 15;
        if (j2 == 20) return 16;
    }
    // ㅡ(18) + ㅣ(20) = ㅢ(19)
    if (j1 == 18 && j2 == 20) return 19;

    return -1; // 결합 불가
}

int HangulAutomata::CombineJong(int j1, int j2) {
    // ㄱ(1) + ㅅ(19) = ㄳ(3)
    if (j1 == 1 && j2 == 19) return 3;
    // ㄴ(4) + ㅈ(22) = ㄵ(5)
    // ㄴ(4) + ㅎ(27) = ㄶ(6)
    if (j1 == 4) {
        if (j2 == 22) return 5;
        if (j2 == 27) return 6;
    }
    // ㄹ(8) + ㄱ(1) = ㄺ(9)
    // ㄹ(8) + ㅁ(16) = ㄻ(10)
    // ㄹ(8) + ㅂ(17) = ㄼ(11)
    // ㄹ(8) + ㅅ(19) = ㄽ(12)
    // ㄹ(8) + ㅌ(25) = ㄾ(13)
    // ㄹ(8) + ㅍ(26) = ㄿ(14)
    // ㄹ(8) + ㅎ(27) = ㅀ(15)
    if (j1 == 8) {
        if (j2 == 1) return 9;
        if (j2 == 16) return 10;
        if (j2 == 17) return 11;
        if (j2 == 19) return 12;
        if (j2 == 25) return 13;
        if (j2 == 26) return 14;
        if (j2 == 27) return 15;
    }
    // ㅂ(17) + ㅅ(19) = ㅄ(18)
    if (j1 == 17 && j2 == 19) return 18;

    return -1;
}

void HangulAutomata::SeparateJong(int jong, int& outCurJong, int& outNextCho) {
    // 겹받침인 경우 쪼개서 앞은 종성, 뒤는 초성으로
    // 홑받침인 경우 앞은 0(종성없음), 뒤는 초성으로

    // 매핑 테이블 (종성 인덱스 -> 초성 인덱스 변환용)
    int jToC[30];
    for (int i = 0; i < 30; i++) jToC[i] = -1;
    jToC[1] = 0; jToC[2] = 1; jToC[4] = 2; jToC[7] = 3; jToC[8] = 5; jToC[16] = 6;
    jToC[17] = 7; jToC[19] = 9; jToC[20] = 10; jToC[21] = 11; jToC[22] = 12; jToC[23] = 13;
    jToC[24] = 14; jToC[25] = 16; jToC[26] = 17; jToC[27] = 18;

    int first = jong;
    int second = 0; // 0이면 없음

    switch (jong) {
    case 3: first = 1; second = 19; break; // ㄳ -> ㄱ,ㅅ
    case 5: first = 4; second = 22; break; // ㄵ -> ㄴ,ㅈ
    case 6: first = 4; second = 27; break; // ㄶ -> ㄴ,ㅎ
    case 9: first = 8; second = 1; break;  // ㄺ -> ㄹ,ㄱ
    case 10: first = 8; second = 16; break; // ㄻ -> ㄹ,ㅁ
    case 11: first = 8; second = 17; break; // ㄼ -> ㄹ,ㅂ
    case 12: first = 8; second = 19; break; // ㄽ -> ㄹ,ㅅ
    case 13: first = 8; second = 25; break; // ㄾ -> ㄹ,ㅌ
    case 14: first = 8; second = 26; break; // ㄿ -> ㄹ,ㅍ
    case 15: first = 8; second = 27; break; // ㅀ -> ㄹ,ㅎ
    case 18: first = 17; second = 19; break; // ㅄ -> ㅂ,ㅅ
    default: second = jong; first = 0; break;
    }

    outCurJong = first;
    outNextCho = jToC[second];
}