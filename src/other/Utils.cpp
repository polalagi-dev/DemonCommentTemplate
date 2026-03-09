#include "Utils.hpp"

std::string getReplacedTemplate(std::string stringToReplace, GJGameLevel* level, int dailyID, int collectedCoins, int coinAmount, bool weekly, bool daily, bool demon) {
	bool useCurlyBraces = Mod::get()->getSettingValue<bool>("use_curly_braces");

	std::string attemptCountVar = "AttemptCount";
	std::string weeklyIDVar = "WeeklyID";
	std::string dailyIDVar = "DailyID";
	std::string demonCountVar = "DemonCount";
	std::string coinCountVar = "CoinCount";

	if (useCurlyBraces) {
		attemptCountVar = "{AttemptCount}";
		weeklyIDVar = "{WeeklyID}";
		dailyIDVar = "{DailyID}";
		demonCountVar = "{DemonCount}";
		coinCountVar = "{CoinCount}";
	}

	auto replaced = utils::string::replace(stringToReplace, attemptCountVar, std::to_string(level->m_attempts));

	if (weekly) {
		replaced = utils::string::replace(replaced, weeklyIDVar, std::to_string(dailyID - 100000));
	}
	else if (daily) {
		replaced = utils::string::replace(replaced, dailyIDVar, std::to_string(dailyID));
	}

	if (demon) {
		int demons = GameStatsManager::sharedState()->getStat("5");

		replaced = utils::string::replace(replaced, demonCountVar, std::to_string(demons));
	}

	replaced = utils::string::replace(replaced, coinCountVar, (coinAmount > 0) ? std::to_string(collectedCoins) : "N/A");

	return replaced;
}

int getCollectedCoinsViaDict(GJGameLevel* level) {
	auto statsManager = GameStatsManager::sharedState();

	auto coinDict = statsManager->m_pendingUserCoins;
	auto coinDict2 = statsManager->m_verifiedUserCoins;

	int collectedCoins = 0;

	for (int i = 1; i <= level->m_coins; i++)
	{
		auto key = level->getCoinKey(i);

		if ((coinDict->objectForKey(key) == nullptr) && (coinDict2->objectForKey(key) == nullptr)) continue;

		collectedCoins++;
	}

	return collectedCoins;
}