#include "Utils.hpp"

const std::string MONTH_NAMES[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

std::string getReplacedTemplate(std::string templateName, GJGameLevel* level, int dailyID, int collectedCoins, int coinAmount, bool weekly, bool daily, bool demon) {
	bool useCurlyBraces = Mod::get()->getSettingValue<bool>("use_curly_braces");

	// yes this code sucks but I do not know a better way of doing it in C++
	std::string attemptCountVar = "AttemptCount";
	std::string weeklyIDVar = "WeeklyID";
	std::string dailyIDVar = "DailyID";
	std::string demonCountVar = "DemonCount";
	std::string coinCountVar = "CoinCount";
	std::string timeH12Var = "TimeH12";
	std::string timeH24Var = "TimeH24";
	std::string timeMVar = "TimeM";
	std::string timeSVar = "TimeS";
	std::string timeDayVar = "TimeDay";
	std::string timeMonthVar = "TimeMonth";
	std::string timeMonthNameVar = "TimeMonName";
	std::string timeYearVar = "TimeYear";
	std::string timeAMPMVar = "TimeAMPM";

	if (useCurlyBraces) {
		attemptCountVar = "{AttemptCount}";
		weeklyIDVar = "{WeeklyID}";
		dailyIDVar = "{DailyID}";
		demonCountVar = "{DemonCount}";
		coinCountVar = "{CoinCount}";
		timeH12Var = "{TimeH12}";
		timeH24Var = "{TimeH24}";
		timeMVar = "{TimeM}";
		timeSVar = "{TimeS}";
		timeDayVar = "{TimeDay}";
		timeMonthVar = "{TimeMonth}";
		timeMonthNameVar = "{TimeMonName}";
		timeYearVar = "{TimeYear}";
		timeAMPMVar = "{TimeAMPM}";
	}

	auto commentTemplate = Mod::get()->getSettingValue<std::string>(templateName);

	auto replaced = utils::string::replace(commentTemplate, attemptCountVar, std::to_string(level->m_attempts));

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

	auto now = time(nullptr);
	auto localTime = std::localtime(&now);
	auto day = std::to_string(localTime->tm_mday);
	auto month = std::to_string(localTime->tm_mon + 1);
	auto monthName = MONTH_NAMES[localTime->tm_mon];
	auto year = std::to_string(localTime->tm_year + 1900);
	auto hour24 = std::to_string(localTime->tm_hour);
	auto hour12i = localTime->tm_hour;
	auto ampm = "AM";
	auto minute = std::to_string(localTime->tm_min);
	auto second = std::to_string(localTime->tm_sec);

	if (hour12i > 12) {
		ampm = "PM";
		hour12i -= 12;
	}

	if (hour12i == 0) {
		ampm = "AM";
		hour12i = 12;
	}

	std::string hour12 = std::to_string(hour12i);

	replaced = utils::string::replace(replaced, timeDayVar, day);
	replaced = utils::string::replace(replaced, timeMonthVar, month);
	replaced = utils::string::replace(replaced, timeMonthNameVar, monthName);
	replaced = utils::string::replace(replaced, timeYearVar, year);
	replaced = utils::string::replace(replaced, timeH12Var, hour12);
	replaced = utils::string::replace(replaced, timeH24Var, hour24);
	replaced = utils::string::replace(replaced, timeMVar, minute);
	replaced = utils::string::replace(replaced, timeSVar, second);
	replaced = utils::string::replace(replaced, timeAMPMVar, ampm);

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

// TODO to be removed for v1.1.0

ButtonSprite* createButtonSpriteForTemplate(const char* caption, bool recommended) {
	return ButtonSprite::create(
		caption,
		"bigFont.fnt",
		recommended ? "GJ_button_03.png" : "GJ_button_04.png"
	);
}