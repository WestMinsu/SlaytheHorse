#include "BattleState.h"
#include "Card.h"
#include "InputManager.h"
#include "Player.h"
#include "Enemy.h"
#include "MainMenu.h"
#include <random>
#include "FloatingText.h"

static std::string WrapTextLocal(const std::string& text, size_t maxLineLength)
{
    std::stringstream ss(text);
    std::string word;
    std::string result = "";
    std::string currentLine = "";

    while (ss >> word)
    {
        if (currentLine.length() + word.length() > maxLineLength)
        {
            result += currentLine + "\n";
            currentLine = word;
        }
        else
        {
            if (!currentLine.empty()) currentLine += " ";
            currentLine += word;
        }
    }
    result += currentLine;
    return result;
}

void BattleState::Init(const EngineContext& engineContext)
{
    engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
    engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });

    engineContext.renderManager->RegisterTexture("[Texture]Player", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Player", "[EngineShader]default_texture", { {"u_Texture","[Texture]Player"} });

    engineContext.renderManager->RegisterTexture("[Texture]Enemy", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Enemy", "[EngineShader]default_texture", { {"u_Texture","[Texture]Enemy"} });

    engineContext.soundManager->LoadSound("Card1SFX", "TTS/Card1.mp3", false);
    engineContext.soundManager->LoadSound("Card2SFX", "TTS/Card2.mp3", false);
    engineContext.soundManager->LoadSound("Card3SFX", "TTS/Card3.mp3", false);
    engineContext.soundManager->LoadSound("Card4SFX", "TTS/Card4.mp3", false);
    engineContext.soundManager->LoadSound("Card5SFX", "TTS/Card5.mp3", false);
    engineContext.soundManager->LoadSound("Card6SFX", "TTS/Card6.mp3", false);
    engineContext.soundManager->LoadSound("Card7SFX", "TTS/Card7.mp3", false);
    engineContext.soundManager->LoadSound("Card8SFX", "TTS/Card8.mp3", false);
    engineContext.soundManager->LoadSound("Card9SFX", "TTS/Card9.mp3", false);
    engineContext.soundManager->LoadSound("Card10SFX", "TTS/Card10.mp3", false);


    engineContext.soundManager->LoadSound("HealSFX", "TTS/Heal.mp3", false);
    engineContext.soundManager->LoadSound("HitSFX", "TTS/Hit.mp3", false);
    engineContext.soundManager->LoadSound("DeathSFX", "TTS/Death.mp3", false);
    engineContext.soundManager->LoadSound("EnemyHitSFX", "TTS/EnemyHit.mp3", false);
    engineContext.soundManager->LoadSound("BossHitSFX", "TTS/BossHit.mp3", false);

    battleManager = new BattleManager();
    battleManager->SetupDeck(engineContext);

    auto playerObj = objectManager.AddObject(std::make_unique<Player>(glm::vec2(-300.f, 0.f), glm::vec2(128.f, 128.f)), "[Object]Player");
    player = static_cast<Player*>(playerObj);

    SpawnNextEnemy(engineContext);

    auto inputFieldObj = objectManager.AddObject(std::make_unique<InputField>(glm::vec2(0, 250.f), glm::vec2(300.0f, 150.0f)), "[Object]InputField");
    inputField = static_cast<InputField*>(inputFieldObj);

    inputField->onCommit = [this](const std::string& text, const EngineContext& context)
        {
            this->OnProcessInput(text, context);
        };

    battleManager->DrawCard(engineContext, player->drawCardCnt);

    auto font = engineContext.renderManager->GetFontByTag("[Font]default");
    auto timerTextObj = std::make_unique<TextObject>(font, "Time: 10.0", TextAlignH::Center, TextAlignV::Middle);
    timerText = timerTextObj.get();
    timerText->GetTransform2D().SetPosition({ barBasePos.x, barBasePos.y + 40.0f });
    timerText->SetRenderLayer("[Layer]UIText");
    objectManager.AddObject(std::move(timerTextObj));

    auto timerBarObj = std::make_unique<GameObject>();
    timerBar = timerBarObj.get();
    timerBar->SetMesh(engineContext, "[EngineMesh]default");
    timerBar->SetMaterial(engineContext, "[Material]Button"); 
    timerBar->GetTransform2D().SetPosition(barBasePos);
    timerBar->GetTransform2D().SetScale(barBaseScale);
    timerBar->SetRenderLayer("[Layer]UIText");
    objectManager.AddObject(std::move(timerBarObj));

    currentTurnTime = maxTurnTime;

    auto turnNoticeObj = std::make_unique<TextObject>(font, u8"", TextAlignH::Center, TextAlignV::Middle);
    turnNoticeText = turnNoticeObj.get();
    turnNoticeText->GetTransform2D().SetPosition({ 0.0f, 250.0f });
    turnNoticeText->SetRenderLayer("[Layer]UIText");
    turnNoticeText->GetTransform2D().SetScale({ 1.5f, 1.5f });
    objectManager.AddObject(std::move(turnNoticeObj));

    CreateDeckViewUI(engineContext);
}

void BattleState::Update(float dt, const EngineContext& context)
{
    GameState::Update(dt, context);

    std::vector<Object*> enemies;
    objectManager.FindByTag("[Object]Enemy", enemies);

    if (context.inputManager->IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Camera2D* activeCam = GetActiveCamera();
        glm::vec2 mousePos = context.inputManager->GetMouseWorldPos(activeCam);

        if (isDeckViewOpen)
        {
            // 닫기 버튼 클릭 체크
            // (간단히 AABB 충돌 체크)
            glm::vec2 btnPos = deckViewCloseButton->GetTransform2D().GetPosition();
            glm::vec2 btnScale = deckViewCloseButton->GetTransform2D().GetScale();
            glm::vec2 half = btnScale * 0.5f;

            if (mousePos.x >= btnPos.x - half.x && mousePos.x <= btnPos.x + half.x &&
                mousePos.y >= btnPos.y - half.y && mousePos.y <= btnPos.y + half.y)
            {
                CloseDeckView(context);
                return; // 닫기 버튼 눌렀으면 다른 로직 수행 안함
            }
        }
        else
        {
            // 덱 열기 버튼 클릭 체크
            if (deckButton) // 버튼이 존재한다면
            {
                glm::vec2 btnPos = deckButton->GetTransform2D().GetPosition();
                glm::vec2 btnScale = deckButton->GetTransform2D().GetScale();
                glm::vec2 half = btnScale * 0.5f;

                if (mousePos.x >= btnPos.x - half.x && mousePos.x <= btnPos.x + half.x &&
                    mousePos.y >= btnPos.y - half.y && mousePos.y <= btnPos.y + half.y)
                {
                    OpenDeckView(context);
                    return; // 열기 버튼 눌렀으면 아래 로직 수행 안함
                }
            }
        }
    }

    if (enemies.empty())
    {
        if (currentState != TurnState::NextStageWait && currentState != TurnState::CardSelect && currentRound < 4)
        {
            PrepareNextStageTransition(context);
        }
    }
    else
    {
        Enemy* enemy = static_cast<Enemy*>(enemies[0]);
        if (enemy->GetHealth() <= 0)
        {
            enemy->KillAll();
        }
    }

    if (currentState == TurnState::NextStageWait)
    {
        if (!player->IsDead())
            currentTurnTime -= dt;

        if (player->power > 0)
        {
            player->ModifyPower(-9999, context);
        }
      
        if (currentTurnTime <= 0.0f)
        {
            player->ModifyHealth(-1, context);
            nextStageTargetText = GenerateRandomSpacedText();
            if (nextStageCard)
            {
                nextStageCard->SetCardName(nextStageTargetText);
            }

            currentTurnTime = 2.5f;

            JIN_LOG(u8"실패! 다시 입력: " << nextStageTargetText);

            return;
        }
    }

    if (currentState == TurnState::PlayerTurn)
    {
        if (turnNoticeText)
            turnNoticeText->SetText(u8"");

        if (currentTurnTime > 0.0f && !player->IsDead())
            currentTurnTime -= dt;

        if (currentTurnTime <= 0.0f)
        {
            currentTurnTime = 0.0f;
            if (inputField)
                inputField->SetInteractable(false);

            battleManager->DiscardAllCardFromHand();
            currentState = TurnState::EnemyTurn;
            transitionTimer = 1.5f;

            JIN_LOG(u8"적의 턴 시작!");
        }
    }
    else if (currentState == TurnState::EnemyTurn)
    {
        if (turnNoticeText)
            turnNoticeText->SetText(u8"적 턴 입니다.");

        transitionTimer -= dt;

        if (transitionTimer <= 0.0f)
        {
            currentTurnTime = maxTurnTime;

            std::vector<Object*> enemyObjects;
            objectManager.FindByTag("[Object]Enemy", enemyObjects);

            if (!enemyObjects.empty())
            {
                Enemy* currentEnemy = static_cast<Enemy*>(enemyObjects[0]);
                currentEnemy->Attack(player, currentTurnTime, context);
            }

            currentState = TurnState::PlayerTurn;
            if (inputField)
            {
                inputField->SetInteractable(true);
                inputField->SetFocus(true);
            }
            battleManager->DrawCard(context, player->drawCardCnt);
        }
    }

    if (currentState == TurnState::CardSelect)
    {
        // 마우스 왼쪽 클릭 감지
        if (context.inputManager->IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Camera2D* activeCam = GetActiveCamera();
            glm::vec2 mousePos = context.inputManager->GetMouseWorldPos(activeCam);

            for (size_t i = 0; i < selectionCards.size(); ++i)
            {
                Card* card = selectionCards[i];
                glm::vec4 bounds = card->GetBoundingBox();

                // 마우스가 카드 영역 안에 있는지 확인 (Hit Test)
                if (mousePos.x >= bounds.x && mousePos.x <= bounds.y &&
                    mousePos.y >= bounds.z && mousePos.y <= bounds.w)
                {
                    // 카드 선택됨
                    OnSelectCard(i, context);
                    break;
                }
            }
        }

        // 마우스 호버 효과 (선택적)
        Camera2D* activeCam = GetActiveCamera();
        glm::vec2 mousePos = context.inputManager->GetMouseWorldPos(activeCam);
        for (auto* card : selectionCards)
        {
            glm::vec4 bounds = card->GetBoundingBox();
            bool isMouseOver = (mousePos.x >= bounds.x && mousePos.x <= bounds.y &&
                mousePos.y >= bounds.z && mousePos.y <= bounds.w);
            card->SetHoverState(isMouseOver);
        }
    }

    float ratio = currentTurnTime / maxTurnTime;
    glm::vec4 currentColor = (ratio <= 0.3f) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (timerText)
    {
        if (currentState == TurnState::CardSelect)
        {
            timerText->SetVisibility(false);
        }
        else
        {
            timerText->SetVisibility(true);
            timerText->SetText("Time: " + std::to_string((int)currentTurnTime) + "." + std::to_string((int)(currentTurnTime * 10) % 10));
            timerText->SetColor(currentColor);
        }
    }

    if (timerBar)
    {
        if (currentState == TurnState::CardSelect)
        {
            timerBar->SetVisibility(false);
        }
        else
        {
            timerBar->SetVisibility(true);
            float newWidth = barBaseScale.x * ratio;
            timerBar->GetTransform2D().SetScale({ newWidth, barBaseScale.y });

            float offset = (barBaseScale.x - newWidth) / 2.0f;
            timerBar->GetTransform2D().SetPosition({ barBasePos.x - offset, barBasePos.y });
            timerBar->SetColor(currentColor);
        }
    }

    const auto& currentHand = battleManager->GetHand();
    float spacing = 170.0f;
    float totalWidth = (currentHand.size() - 1) * spacing;
    float startX = -totalWidth / 2.0f;

    Camera2D* activeCam = GetActiveCamera();
    glm::vec2 mousePos = context.inputManager->GetMouseWorldPos(activeCam);

    for (size_t i = 0; i < currentHand.size(); ++i)
    {
        Card* card = currentHand[i];        
        float posX = startX + (i * spacing);
        card->SetBasePosition({ posX, -250.0f });

        glm::vec4 bounds = card->GetBoundingBox();
        bool isMouseOver = (mousePos.x >= bounds.x && mousePos.x <= bounds.y &&
            mousePos.y >= bounds.z && mousePos.y <= bounds.w);

        card->SetHoverState(isMouseOver);
    }
}

void BattleState::Free(const EngineContext& engineContext)
{
    objectManager.FreeAll(engineContext);

    engineContext.renderManager->UnregisterTexture("[Texture]Button", engineContext);
    engineContext.renderManager->UnregisterMaterial("[Material]Button", engineContext);

    /*if (battleManager != nullptr)
    {
        delete battleManager;
        battleManager = nullptr;
    }*/
}

void BattleState::ReturnToMainMenu(const EngineContext& context)
{
    if (context.stateManager != nullptr)
        context.stateManager->ChangeState(std::make_unique<MainMenu>());
}

void BattleState::OnProcessInput(const std::string& text, const EngineContext& context)
{
    if (currentState == TurnState::NextStageWait)
    {
        if (text == nextStageTargetText)
        {
            if (nextStageCard)
            {
                nextStageCard->KillAll();
            }

            nextStageCard = nullptr;
            // SpawnNextEnemy(context);
            // battleManager->DrawCard(context, 5);

            // maxTurnTime = 10.0f;
            // currentTurnTime = maxTurnTime;
            // currentState = TurnState::PlayerTurn;

            PrepareCardSelection(context);

            return;
        }
        else
        {
            player->ModifyHealth(-1, context);

            nextStageTargetText = GenerateRandomSpacedText();
            if (nextStageCard)
            {
                nextStageCard->SetCardName(nextStageTargetText);
            }

            currentTurnTime = 2.5f;

            JIN_LOG(u8"오타! 체력이 1 깎였습니다: " << nextStageTargetText);

            return;
        }
    }

    JIN_LOG("Player typed: " << text);

    Font* font = context.renderManager->GetFontByTag("[Font]default");

    glm::vec2 spawnPos = { 0.0f, 50.0f };

    // [수정됨] const auto& 대신 auto를 사용하여 포인터를 복사합니다.
    // '히히힝' 카드 사용 시 DrawCard(2)가 호출되어 hand 벡터의 메모리 주소가 바뀔 수 있는데(Reallocation),
    // 참조(&)를 사용하면 바뀐 주소를 따라가지 못해 크래시가 발생합니다.
    // 포인터(auto)로 복사해두면 벡터 내부가 바뀌어도 로컬 변수 card는 유효한 객체를 가리킵니다.
    for (auto card : battleManager->GetHand())
    {
        if (card->GetCardName() == text)
        {
            JIN_LOG("Commit Success: " << text);

            // 안전을 위해 이름 미리 저장 (선택 사항이나 권장됨)
            std::string usedCardName = card->GetCardName();

            // 카드 사용 (여기서 드로우가 발생하면 hand 벡터가 재할당될 수 있음)
            card->UseCard(context);

            if (font)
            {
                // card 포인터는 여전히 유효하므로 접근 가능
                std::string msg = u8"사용 성공!\n" + usedCardName;
                auto floatText = std::make_unique<FloatingText>(
                    font, msg, spawnPos, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)
                );

                GetObjectManager().AddObject(std::move(floatText));
            }
            return;
        }
    }

    player->ModifyHealth(-1, context);

    if (font)
    {
        std::string msg = u8"사용 실패!";

        auto floatText = std::make_unique<FloatingText>(
            font, msg, spawnPos, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
        );
        GetObjectManager().AddObject(std::move(floatText));
    }
}

void BattleState::SpawnNextEnemy(const EngineContext& context)
{
    currentRound++;

    std::unique_ptr<Enemy> newEnemy;
    switch (currentRound)
    {
    case 1:
        newEnemy = std::make_unique<Enemy>(u8"말", glm::vec2(300.f, 0.f), EnemyType::Normal);
        break;
    case 2:
        newEnemy = std::make_unique<Enemy>(u8"성난 말", glm::vec2(300.f, 0.f), EnemyType::Angry);
        break;
    case 3: 
        newEnemy = std::make_unique<Enemy>(u8"빠른 말", glm::vec2(300.f, 0.f), EnemyType::Fast);
        break;
    case 4:
        newEnemy = std::make_unique<Enemy>(glm::vec2(300.f, 0.f), glm::vec2(256.f, 256.f));
        break;
    default:
        return;
    }

    objectManager.AddObject(std::move(newEnemy), "[Object]Enemy");
}

void BattleState::PrepareNextStageTransition(const EngineContext& context)
{
    if (battleManager != nullptr)
    {
        battleManager->DiscardAllCardFromHand();
    }

    currentState = TurnState::NextStageWait;
    maxTurnTime = 2.5f;
    currentTurnTime = maxTurnTime;

    nextStageTargetText = GenerateRandomSpacedText();

    auto cardObj = std::make_unique<Card>();
    cardObj->SetCardName(nextStageTargetText);
    cardObj->SetCardDescription(u8"다음 단계로 이동합니다");
    cardObj->GetTransform2D().SetPosition({ 0.0f, 0.0f });

    nextStageCard = cardObj.get();
    objectManager.AddObject(std::move(cardObj));

    if (inputField)
    {
        inputField->SetInteractable(true);
        inputField->SetFocus(true);
    }
}

void BattleState::ModifyCurrentTurnTime(int amount)
{
    currentTurnTime += amount;
}

std::string BattleState::GenerateRandomSpacedText()
{

    std::vector<std::string> syllables = { u8"다", u8"음", u8"단", u8"계", u8"로" };

    std::string result = syllables[0];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (size_t i = 1; i < syllables.size(); ++i)
    {
        if (dis(gen))
            result += " ";

        result += syllables[i];
    }

    return result;
}

void BattleState::PrepareCardSelection(const EngineContext& context)
{
    currentState = TurnState::CardSelect; // 상태 변경

    // 입력 필드 비활성화 (선택 도중 타이핑 방지)
    if (inputField)
    {
        inputField->SetInteractable(false);
    }

    // 1. 인덱스 풀 생성 (0, 1번 제외, 2번부터 끝까지)
    std::vector<int> indices;
    for (size_t i = 2; i < GlobalCardTemplates.size(); ++i)
    {
        indices.push_back(static_cast<int>(i));
    }

    // 2. 셔플 (중복 방지를 위해 인덱스를 섞음)
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    // 3. 앞에서부터 3개 선택하여 카드 생성
    float startX = -300.0f; // 왼쪽 위치
    float gap = 300.0f;     // 간격

    for (int i = 0; i < 3 && i < indices.size(); ++i)
    {
        int cardIdx = indices[i];
        const auto& cardData = GlobalCardTemplates[cardIdx];

        // 카드 객체 생성
        auto cardObj = std::make_unique<Card>();
        cardObj->SetCardName(cardData.name);

        // [수정] 18글자마다 줄바꿈을 적용하여 설명을 설정함
        std::string wrappedDesc = WrapTextLocal(cardData.description, 18);
        cardObj->SetCardDescription(wrappedDesc);

        // 화면 중앙 배치를 위한 좌표 설정
        float posX = startX + (i * gap);
        cardObj->GetTransform2D().SetPosition({ posX, 0.0f }); // Y=0 중앙
        cardObj->SetVisibilityAll(true);

        // 관리 리스트에 추가
        selectionCards.push_back(cardObj.get());

        // ObjectManager에 등록 (화면에 그려짐)
        objectManager.AddObject(std::move(cardObj));
    }

    JIN_LOG("보상 카드 선택 단계 진입: 3장 생성 완료");
}

void BattleState::OnSelectCard(int cardIndex, const EngineContext& context)
{
    if (cardIndex < 0 || cardIndex >= selectionCards.size()) return;

    Card* selectedUI = selectionCards[cardIndex];
    std::string cardName = selectedUI->GetCardName();

    // 1. 선택한 카드를 실제 플레이어 덱에 추가
    // BattleManager::AddCard는 CardData를 받으므로 이름으로 원본 데이터를 찾습니다.
    int templateIdx = GetCardIndex(cardName);
    if (templateIdx != -1)
    {
        battleManager->AddCard(GlobalCardTemplates[templateIdx], context);
        JIN_LOG("덱에 카드 추가됨: " << cardName);
    }

    // 2. 선택지 카드들 모두 제거 (Kill)
    for (auto* card : selectionCards)
    {
        card->KillAll();
    }
    selectionCards.clear();

    // 3. 다음 라운드(전투) 시작 로직 (원래 OnProcessInput에 있던 로직 이동)
    SpawnNextEnemy(context);
    battleManager->DrawCard(context, 5); // 기본 드로우

    maxTurnTime = 10.0f;
    currentTurnTime = maxTurnTime;

    currentState = TurnState::PlayerTurn;

    // 입력 필드 다시 활성화
    if (inputField)
    {
        inputField->SetInteractable(true);
        inputField->SetFocus(true);
    }
}

// [BattleState.cpp]

void BattleState::CreateDeckViewUI(const EngineContext& context)
{
    // 1. 우측 상단 '덱' 버튼 (검정 사각형)
    auto btnObj = std::make_unique<GameObject>();
    btnObj->SetMesh(context, "[EngineMesh]default");
    btnObj->SetMaterial(context, "[Material]Button"); // 혹은 색상만 쓰는 머티리얼
    btnObj->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // 검정색

    // 위치: 우측 상단 (해상도에 따라 조정 필요, 여기선 800x600 기준 예시)
    // 카메라 좌표계 기준 우측 상단: (350, 250) 정도
    btnObj->GetTransform2D().SetPosition({ 350.0f, 250.0f });
    btnObj->GetTransform2D().SetScale({ 80.0f, 40.0f });
    btnObj->GetTransform2D().SetDepth(0.9f); // UI가 위에 오도록 깊이 설정
    btnObj->SetRenderLayer("[Layer]UIText"); // UI 레이어 사용

    deckButton = btnObj.get();
    objectManager.AddObject(std::move(btnObj));

    // 2. '덱' 텍스트
    auto font = context.renderManager->GetFontByTag("[Font]default");
    auto txtObj = std::make_unique<TextObject>(font, u8"덱", TextAlignH::Center, TextAlignV::Middle);
    txtObj->GetTransform2D().SetPosition({ 350.0f, 250.0f });
    txtObj->GetTransform2D().SetDepth(0.95f); // 버튼보다 더 위에
    txtObj->SetRenderLayer("[Layer]UIText");
    txtObj->SetColor(glm::vec4(1.0f)); // 흰색 글씨

    deckButtonText = txtObj.get();
    objectManager.AddObject(std::move(txtObj));


    // 3. 덱 보기 오버레이 (반투명 검정 배경) - 초기엔 안보임
    auto overlayObj = std::make_unique<GameObject>();
    overlayObj->SetMesh(context, "[EngineMesh]default");
    overlayObj->SetMaterial(context, "[Material]Button");
    overlayObj->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));

    // [수정] 크기를 {700, 500} -> {1000, 700} 으로 확대 (화면 해상도에 맞춰 조절하세요)
    overlayObj->GetTransform2D().SetScale({ 1000.0f, 700.0f });

    overlayObj->GetTransform2D().SetDepth(0.96f);
    overlayObj->SetRenderLayer("[Layer]UIText");
    overlayObj->SetVisibility(false);

    deckViewOverlay = overlayObj.get();
    objectManager.AddObject(std::move(overlayObj));


    // 4. 닫기 버튼 (위치 이동)
    auto closeBtnObj = std::make_unique<GameObject>();
    closeBtnObj->SetMesh(context, "[EngineMesh]default");
    closeBtnObj->SetMaterial(context, "[Material]Button");
    closeBtnObj->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // [수정] 오버레이가 커졌으므로 버튼 위치도 우상단 끝으로 이동
    // 오버레이 반너비(500) - 여유분(40) = 460, 반높이(350) - 여유분(40) = 310
    closeBtnObj->GetTransform2D().SetPosition({ 460.0f, 310.0f });

    closeBtnObj->GetTransform2D().SetScale({ 40.0f, 40.0f });
    closeBtnObj->GetTransform2D().SetDepth(0.97f);
    closeBtnObj->SetRenderLayer("[Layer]UIText");
    closeBtnObj->SetVisibility(false);

    deckViewCloseButton = closeBtnObj.get();
    objectManager.AddObject(std::move(closeBtnObj));
}

void BattleState::OpenDeckView(const EngineContext& context)
{
    isDeckViewOpen = true;
    deckViewOverlay->SetVisibility(true);
    deckViewCloseButton->SetVisibility(true);

    if (inputField) inputField->SetInteractable(false);

    // 1. 모든 카드 리스트 합치기 (덱 + 버려진 카드 + 핸드)
    std::vector<Card*> allCards = battleManager->GetDeck();       // 덱 복사
    const auto& discardPile = battleManager->GetDiscardPile();    // 버려진 카드 참조
    const auto& hand = battleManager->GetHand();                  // [추가] 핸드 참조

    // 덱 뒤에 버려진 카드 추가
    allCards.insert(allCards.end(), discardPile.begin(), discardPile.end());
    // [추가] 그 뒤에 핸드 카드 추가
    allCards.insert(allCards.end(), hand.begin(), hand.end());

    // 2. 카드 배치 및 생성 (이전 요청사항인 넓은 간격 적용)
    float startX = -320.0f; // 시작 X 위치 (왼쪽으로 이동)
    float startY = 200.0f;  // 시작 Y 위치 (위쪽으로 이동)
    float gapX = 160.0f;    // 가로 간격 확대
    float gapY = 220.0f;    // 세로 간격 확대
    int cols = 5;           // 한 줄에 5장

    for (size_t i = 0; i < allCards.size(); ++i)
    {
        Card* originalCard = allCards[i];

        // 보여주기용 임시 카드 생성
        auto cardObj = std::make_unique<Card>();
        cardObj->SetCardName(originalCard->GetCardName());

        // 줄바꿈 등 설명 설정
        std::string desc = originalCard->GetCardDescription();
        cardObj->SetCardDescription(desc);

        // 그리드 좌표 계산
        int col = i % cols;
        int row = i / cols;
        float x = startX + (col * gapX);
        float y = startY - (row * gapY);

        // 위치 및 레이어 설정
        cardObj->GetTransform2D().SetPosition({ x, y });
        cardObj->GetTransform2D().SetDepth(0.98f); // 오버레이보다 위에
        cardObj->SetRenderLayer("[Layer]UIText");

        // 관리 리스트에 추가 및 엔진에 등록
        deckViewCards.push_back(cardObj.get());
        objectManager.AddObject(std::move(cardObj));
    }
}

void BattleState::CloseDeckView(const EngineContext& context)
{
    isDeckViewOpen = false;
    deckViewOverlay->SetVisibility(false);
    deckViewCloseButton->SetVisibility(false);

    // 입력 필드 다시 활성화 (플레이어 턴이라면)
    if (inputField && currentState == TurnState::PlayerTurn)
    {
        inputField->SetInteractable(true);
        inputField->SetFocus(true);
    }

    // 보여주기용 카드들 제거
    for (auto* card : deckViewCards)
    {
        card->KillAll();
    }
    deckViewCards.clear();
}