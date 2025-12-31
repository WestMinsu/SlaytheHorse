#include "InputField.h"
#include "EngineContext.h"
#include "InputManager.h"
#include "Collider.h"
#include "GameState.h"
#include <Windows.h> 
#include <string>

std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();

    // 1. 필요한 버퍼 크기 계산
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    // 2. 문자열 버퍼 확보
    std::string strTo(size_needed, 0);

    // 3. 실제 변환 수행
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

// 생성자: 테두리 두께 초기화 추가 (기본값 5.0f 가정)
InputField::InputField(const glm::vec2& pos, const glm::vec2& size)
    : initPos(pos), initSize(size)
{
}

void InputField::Init(const EngineContext& context)
{
    // 1. 바깥쪽 사각형 (테두리 역할, 흰색)
    SetMesh(context, "[EngineMesh]default");
    SetMaterial(context, "[Material]Button");

    GetTransform2D().SetPosition(initPos);
    GetTransform2D().SetScale(initSize);
    SetColor({ 1.f, 1.f, 1.f, 1.f });

    auto collider = std::make_unique<AABBCollider>(this, glm::vec2(1.0f, 1.0f));
    SetCollider(std::move(collider));

    // 2. 안쪽 사각형 (입력 배경, 검정색)
    auto innerObj = std::make_unique<GameObject>();
    innerObj->SetMesh(context, "[EngineMesh]default");
    innerObj->SetMaterial(context, "[Material]Button");

    // 테두리 두께만큼 작게 설정
    glm::vec2 innerSize = initSize - glm::vec2(borderThickness * 2.0f);
    innerObj->GetTransform2D().SetScale(innerSize);
    innerObj->GetTransform2D().SetPosition(initPos);

    // 깊이 설정 (테두리보다 앞에)
    innerObj->GetTransform2D().SetDepth(GetTransform2D().GetDepth() + 0.01f);

    innerObj->SetColor({ 0.f, 0.f, 0.f, 1.0f }); // 검정 배경

    // UI 모드 동기화
    if (ShouldIgnoreCamera())
        innerObj->SetIgnoreCamera(true, GetReferenceCamera());

    innerObject = static_cast<GameObject*>(context.stateManager->GetCurrentState()->GetObjectManager().AddObject(
        std::move(innerObj), GetTag() + "_Inner"
    ));

    // 3. 텍스트 오브젝트 (흰색)
    Font* font = context.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        auto tObj = std::make_unique<TextObject>(font, "Enter text...", TextAlignH::Left, TextAlignV::Middle);

        // 안쪽 사각형 기준 왼쪽 정렬 위치 계산
        tObj->GetTransform2D().SetPosition(initPos + glm::vec2(-innerSize.x * 0.45f, 0.0f));
        tObj->GetTransform2D().SetDepth(GetTransform2D().GetDepth() + 0.02f); // 가장 위

        if (ShouldIgnoreCamera())
            tObj->SetIgnoreCamera(true, GetReferenceCamera());

        tObj->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 흰색 글자

        textObject = static_cast<TextObject*>(
            context.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(tObj), GetTag() + "_Text")
            );
    }

    // 초기 상호작용 상태 적용
    SetInteractable(isInteractable);
}

void InputField::Update(float dt, const EngineContext& context)
{
    // 위치 동기화 (입력창이 움직일 경우를 대비)
    if (innerObject)
    {
        innerObject->GetTransform2D().SetPosition(GetTransform2D().GetPosition());
    }

    if (textObject && innerObject)
    {
        glm::vec2 innerSize = innerObject->GetTransform2D().GetScale();
        glm::vec2 newTextPos = GetTransform2D().GetPosition() + glm::vec2(-innerSize.x * 0.45f, 0.0f);
        textObject->GetTransform2D().SetPosition(newTextPos);
    }

    // 비활성화 상태면 입력 처리 안 함
    if (!isInteractable) return;

    InputManager* input = context.inputManager;
    Camera2D* camera = context.stateManager->GetCurrentState()->GetActiveCamera();

    glm::vec2 mousePos = ShouldIgnoreCamera() ? input->GetMousePos() : input->GetMouseWorldPos(camera);

    // 마우스 클릭 포커스 처리
    if (input->IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (GetCollider() && GetCollider()->CheckPointCollision(mousePos))
        {
            SetFocus(true);
        }
        else
        {
            SetFocus(false);
        }
    }

    // [핵심 수정 사항] 텍스트 표시 로직 (오토마타 사용)
    // 오토마타에서 현재 조합 중인 문자열을 가져옴 (wstring)
    std::wstring displayW = automata.GetDisplayString();
    std::string displayStr = WStringToString(displayW);

    if (isFocused)
    {
        HandleTyping(context);

        // 커서 깜박임
        cursorTimer += dt;
        if (cursorTimer >= 0.5f)
        {
            cursorTimer = 0.0f;
            showCursor = !showCursor;
        }

        // 현재 조합 중인 텍스트 + 커서 표시
        if (textObject)
            textObject->SetText(displayStr + (showCursor ? "|" : ""));
    }
    else
    {
        // 포커스가 없을 때
        if (textObject)
            textObject->SetText(displayStr.empty() ? u8"클릭 후 입력..." : displayStr);
    }
}

void InputField::SetInteractable(bool interactable)
{
    isInteractable = interactable;

    if (!isInteractable)
    {
        SetFocus(false);
        SetColor({ 1.f, 1.f, 1.f, 0.f });
        if (innerObject) innerObject->SetColor({ 0.f, 0.f, 0.f, 0.f });
        if (textObject) textObject->SetColor({ 0.5f, 0.5f, 0.5f, 0.0f });   // 글자도 어둡게
    }
    else
    {
        SetColor({ 1.f, 1.f, 1.f, 1.f });
        if (innerObject) innerObject->SetColor({ 0.f, 0.f, 0.f, 1.f });
        if (textObject) textObject->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });   // 흰색
    }
}

void InputField::SetFocus(bool focus)
{
    isFocused = focus;
    cursorTimer = 0.0f;
    showCursor = true;

    if (isInteractable && innerObject)
    {
        if (isFocused) innerObject->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        else innerObject->SetColor({ 0.f, 0.f, 0.f, 1.0f });
    }
}

void InputField::HandleTyping(const EngineContext& context)
{
    InputManager* input = context.inputManager;

    // 1. 엔터 (전송)
    if (input->IsKeyPressed(KEY_ENTER))
    {
        // 오토마타 버퍼를 비우고 확정된 문자열을 가져옴
        std::wstring complete = automata.Flush();
        std::string finalStr = WStringToString(complete);

        if (!finalStr.empty())
        {
            if (onCommit) onCommit(finalStr, context);
        }
        return;
    }

    // 2. 백스페이스 (지우기)
    if (input->IsKeyPressed(KEY_BACKSPACE))
    {
        automata.Backspace();
        return;
    }

    // 3. 영문 문자 입력 (a~z -> 오토마타가 한글로 변환)
    for (int key = KEY_A; key <= KEY_Z; ++key)
    {
        if (input->IsKeyPressed(key))
        {
            bool shift = input->IsKeyDown(KEY_LEFT_SHIFT) || input->IsKeyDown(KEY_RIGHT_SHIFT);
            char c = shift ? (char)key : (char)(key + 32); // 대문자/소문자 처리
            automata.Type(c);
        }
    }

    // 4. 스페이스
    if (input->IsKeyPressed(KEY_SPACE))
    {
        automata.Type(' ');
    }
}