#pragma once

#include "../Include.hpp"

std::string getReplacedTemplate(std::string templateName, GJGameLevel* level, int dailyID, int collectedCoins, int coinAmount, bool weekly, bool daily, bool demon);

int getCollectedCoinsViaDict(GJGameLevel* level);

ButtonSprite* createButtonSpriteForTemplate(const char* caption, bool recommended);