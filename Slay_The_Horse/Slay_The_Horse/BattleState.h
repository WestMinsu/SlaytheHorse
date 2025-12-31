#pragma once
#include "GameState.h"
#include "BattleManager.h"
#include "InputField.h"
#include "Player.h"
#include "Enemy.h"

enum class TurnState
{
    PlayerTurn,
    EnemyTurn,
    Transition,
    NextStageWait,
    CardSelect,
    BossDialogue,
    Ending
};

struct DialogueData
{
    std::string bossLine;
    std::string playerResponse;
};

class BattleState : public GameState
{
public:
    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;
    void Free(const EngineContext& engineContext) override;
    void ReturnToMainMenu(const EngineContext& context);

    InputField* inputField = nullptr;

    Player* player = nullptr;

    BattleManager* battleManager = nullptr;;

    void ModifyCurrentTurnTime(int amount);

    void StartEnding(const EngineContext& context);
    void TriggerShake(float duration, float intensity);
private:
    void OnProcessInput(const std::string& text, const EngineContext& context);

    TextObject* timerText = nullptr;
    GameObject* timerBar = nullptr;

    float maxTurnTime = 10.0f;
    float currentTurnTime = 10.0f;

    const glm::vec2 barBaseScale = { 400.0f, 20.0f };
    const glm::vec2 barBasePos = { 0.0f, 120.0f };

    TurnState currentState = TurnState::PlayerTurn;
    Enemy* enemy = nullptr;
    float transitionTimer = 0.0f;
    TextObject* turnNoticeText = nullptr;

    int currentRound = 0;
    void SpawnNextEnemy(const EngineContext& context);

    std::string nextStageTargetText; 
    Card* nextStageCard = nullptr;   

    void PrepareNextStageTransition(const EngineContext& context);
    std::string GenerateRandomSpacedText();

    bool isSpawning = false;

    std::vector<DialogueData> bossDialogues;
    int currentDialogueIdx = 0;
    bool isBossBattle = false;
    void StartBossDialogue(const EngineContext& context);
    void ProcessDialogueInput(const std::string& text, const EngineContext& context);
    void UpdateTimerUI();
    void UpdateCardLayout(const EngineContext& context);
    std::vector<Card*> selectionCards;
    void PrepareCardSelection(const EngineContext& context);
    void OnSelectCard(int cardIndex, const EngineContext& context);

    bool isDeckViewOpen = false;       // 덱 보기 창이 열려있는지 여부
    GameObject* deckButton = nullptr;  // 우측 상단 '덱' 버튼 배경
    TextObject* deckButtonText = nullptr; // '덱' 텍스트

    GameObject* deckViewOverlay = nullptr;     // 반투명 검정 배경
    GameObject* deckViewCloseButton = nullptr; // 닫기 버튼 (흰색)

    std::vector<Card*> deckViewCards;  // 덱 보기 화면에 띄워진 임시 카드들

    // [추가] 덱 보기 관련 함수
    void CreateDeckViewUI(const EngineContext& context); // 초기화 시 UI 생성
    void OpenDeckView(const EngineContext& context);     // 창 열기
    void CloseDeckView(const EngineContext& context);    // 창 닫기

    float shakeTimer = 0.0f;     
    float shakeIntensity = 0.0f; 
    glm::vec2 originalCamPos = { 0.0f, 0.0f };
};