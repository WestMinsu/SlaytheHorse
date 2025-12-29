#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Card.h"
#include <iostream>

class BattleManager
{
public:
    void SetupDeck();
    void ShowAllCardsInDeck();

private:
    std::vector<std::unique_ptr<Card>> deck;
};