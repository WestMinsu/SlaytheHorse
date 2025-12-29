#include <iostream>
#include "Debug.h"
#include "Engine.h"
#include "BattleState.h" 
#include "GameState.h"
#include "MainMenu.h"

int main(int argc, char* argv[])
{
    JinEngine jinEngine;
    DebugLogger::SetLogLevel(LogLevel::All);
    int width = 1280;
    int height = 720;

    try
    {
        if (argc == 3)
        {
            width = std::stoi(argv[1]);
            height = std::stoi(argv[2]);
        }
        else if (argc != 1)
        {
            JIN_ERR("Usage: ./MyGame [width height]");
            return -1;
        }
    }

    catch (const std::exception&)
    {
        JIN_ERR("Invalid arguments. Width and height must be integers.");
        return -1;
    }

    if (!jinEngine.Init(width, height))
    {
        JIN_ERR("Engine initialization failed.");
        return -1;
    }
    jinEngine.RenderDebugDraws(false);

    auto& ctx = jinEngine.GetEngineContext();

    ctx.renderManager->RegisterRenderLayer("[Layer]Background", 0);
    ctx.renderManager->RegisterRenderLayer("[Layer]UIText", 12);
    ctx.renderManager->RegisterFont("[Font]default1", "Fonts/NotoSans-VariableFont_wdth,wght.ttf", 50);

    ctx.windowManager->SetBackgroundColor({ 0.2, 0.2, 0.4, 1 });

    ctx.stateManager->ChangeState(std::make_unique<BattleState>());

    jinEngine.GetEngineContext().renderManager->RegisterFont("[Font]default", "Fonts/NanumPenScript-Regular.ttf", 50);

    jinEngine.GetEngineContext().stateManager->ChangeState(std::make_unique<MainMenu>());

    jinEngine.Run();

    return 0;
}