#pragma once
#include <string>
#include <vector>

class HangulAutomata
{
public:
    HangulAutomata();

    // 키보드 문자 입력 처리 (a~z, A~Z, 공백 등)
    void Type(char c);

    // 백스페이스 처리
    void Backspace();

    // 입력 초기화
    void Clear();

    // 확정된 문자열 반환 (엔터 칠 때 사용)
    std::wstring GetCompleteText() const { return completeText; }

    // 현재 조합 중인 상태까지 포함하여 전체 텍스트 반환 (화면 표시용)
    std::wstring GetDisplayString() const;

    // 현재 버퍼 내용을 확정하고 버퍼 비움 (엔터 처리용)
    std::wstring Flush();

private:
    std::wstring completeText; // 이미 확정된 글자들

    // 현재 조합 중인 자모 인덱스 (-1이면 없음)
    int nCho = -1;
    int nJung = -1;
    int nJong = -1;

    // 현재 상태에서 조합된 한글 글자 반환
    wchar_t MakeHangul(int cho, int jung, int jong) const;

    // 조합 중인 글자를 completeText로 넘기고 상태 초기화
    void CommitState();

    // ---------------------------------------------------------
    // 매핑 및 분해/결합 로직
    // ---------------------------------------------------------

    // 영문 키 -> 초성 인덱스 (0~18, 없으면 -1)
    static const int KEY_TO_CHO[256];
    // 영문 키 -> 중성 인덱스 (0~20, 없으면 -1)
    static const int KEY_TO_JUNG[256];
    // 영문 키 -> 종성 인덱스 (1~27, 없으면 -1. 0은 종성없음)
    static const int KEY_TO_JONG[256];

    // 종성 분리 (겹받침 -> 홑받침 + 초성) 
    // 예: ㄳ + ㅏ -> ㄱ + 사
    // 반환값: 분리된 받침의 종성 인덱스 (남는건 다음 초성으로)
    void SeparateJong(int jong, int& outCurJong, int& outNextCho);

    // 중성 결합 (단모음 + 단모음 -> 복모음)
    // 예: ㅗ + ㅏ -> ㅘ
    // 반환값: 합쳐진 중성 인덱스 (실패 시 -1)
    int CombineJung(int jung1, int jung2);

    // 종성 결합 (홑받침 + 자음 -> 겹받침)
    // 예: ㄱ + ㅅ -> ㄳ
    // 반환값: 합쳐진 종성 인덱스 (실패 시 -1)
    int CombineJong(int jong1, int jong2);
};