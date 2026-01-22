#include <Geode/Geode.hpp>
#include <Geode/modify/ShareCommentLayer.hpp>

using namespace geode::prelude;

std::string replaceString(std::string original, std::string replace, std::string with) {
	size_t pos = original.find(replace);

	if (pos != std::string::npos) {
		original.replace(pos, replace.length(), with);
	}

	return original;
}

class $modify(HookedShareCommentLayer, ShareCommentLayer) {
	struct Fields {
		GJGameLevel* m_level;
	};

	bool init(gd::string title, int charLimit, CommentType type, int ID, gd::string desc) {
		if (!ShareCommentLayer::init(title, charLimit, type, ID, desc)) return false;

		auto currentScene = CCScene::get();

		if (currentScene == nullptr) return true;

		auto levelInfoLayer = currentScene->getChildByType<LevelInfoLayer>(0);

		if (levelInfoLayer == nullptr) return true;

		auto level = levelInfoLayer->m_level;
		auto layer = this->getChildByIndex(0);
		
		if (level == nullptr) return true;
		if (layer == nullptr) return true;

		auto stars = static_cast<int>(level->m_stars);
		auto dailyID = static_cast<int>(level->m_dailyID);

		if ((dailyID == 0) && (stars < 10)) return true;

		auto menu = layer->getChildByType<CCMenu>(0);

		if (menu == nullptr) return true;

		bool useOldUI = Mod::get()->getSettingValue<bool>("use_old_ui");
		CCSprite* buttonSprite = nullptr;

		if (useOldUI) {
			buttonSprite = ButtonSprite::create("Insert Template", 0.6f);
		}
		else {
			buttonSprite = CCSprite::create("Button.png"_spr);
			buttonSprite->setScale(0.75f);
			buttonSprite->setColor({ 0, 0, 0 });
			buttonSprite->setOpacity(105);
		}

		auto button = CCMenuItemSpriteExtra::create(
			buttonSprite,
			this,
			menu_selector(HookedShareCommentLayer::onCommentTemplateButton)
		);

		if (useOldUI) button->setPosition(0.0f, -10.0f);
		else button->setPosition(133.0f, 36.5f);

		if (Loader::get()->isModLoaded("geode.node-ids")) {
			button->setID("comment-template-btn"_spr);
		}

		menu->addChild(button);

		m_fields->m_level = level;

		return true;
	}

	void onCommentTemplateButton(CCObject* sender) {
		auto level = m_fields->m_level;
		bool beaten = level->getNormalPercent() == 100;

		if (!beaten) {
			FLAlertLayer::create(
				"Uncompleted Level",
				"You have <cr>not completed this level yet</c>, however the template has been inserted anyways.",
				"Ok"
			)->show();
		}

		int dailyID = static_cast<int>(level->m_dailyID);
		int stars = level->m_stars;
		bool demon = stars >= 10;
		bool daily = dailyID > 0;
		bool weekly = daily && demon;
		const char* templateName = "template_daily";

		int collectedCoins = GameStatsManager::sharedState()->getCollectedCoinsForLevel(level);

		if (weekly) {
			templateName = "template_weekly";
		}
		else if (demon && !weekly) {
			templateName = "template_demon";
		}

		auto commentTemplate = Mod::get()->getSettingValue<std::string>(templateName);

		auto replaced = replaceString(commentTemplate, "AttemptCount", std::to_string(level->m_attempts));

		if (weekly) {
			replaced = replaceString(replaced, "WeeklyID", std::to_string(dailyID - 100000));
		}
		else if (daily) {
			replaced = replaceString(replaced, "DailyID", std::to_string(dailyID));
		}

		if (demon) {
			int demons = GameStatsManager::sharedState()->getStat("5");

			replaced = replaceString(replaced, "DemonCount", std::to_string(demons));
		}

		replaced = replaceString(replaced, "CoinCount", std::to_string(collectedCoins));

		auto currentText = static_cast<std::string>(m_descText);

		currentText.append(replaced);

		m_commentInput->setString(currentText);
		updateCharCountLabel();
	}
};