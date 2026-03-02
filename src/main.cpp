#include <Geode/Geode.hpp>
#include <Geode/modify/ShareCommentLayer.hpp>

using namespace geode::prelude;

std::string getReplacedTemplate(std::string templateName, GJGameLevel* level, int dailyID, int collectedCoins, int coinAmount, bool weekly, bool daily, bool demon) {
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

static ButtonSprite* createButtonSpriteForTemplate(const char* caption, bool recommended) {
	return ButtonSprite::create(
		caption,
		"bigFont.fnt",
		recommended ? "GJ_button_03.png" : "GJ_button_04.png"
	);
}

class SelectTemplatePopup : public geode::Popup {

	GJGameLevel* m_level{};
	ShareCommentLayer* m_shareCommentLayer{};

	int dailyID{};
	bool beaten{};

protected:
	bool init(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
		if (!Popup::init(415.0f, 90.0f)) return false;

		this->m_level = level;
		this->m_shareCommentLayer = commentLayer;
		this->dailyID = dailyID;
		this->beaten = beaten;

		auto layoutMenu = CCMenu::create();
		auto layout = ColumnLayout::create();

		bool demon = level->m_stars >= 10;

		layout->setAxis(geode::Axis::Row);
		layout->setGap(15);
		layout->setGrowCrossAxis(true);

		layoutMenu->setLayout(layout);
		layoutMenu->setID("template-list"_spr);

		auto dailyButton = CCMenuItemSpriteExtra::create(
			createButtonSpriteForTemplate("Daily", daily && !weekly),
			this,
			menu_selector(SelectTemplatePopup::onTemplateButton)
		);

		auto weeklyButton = CCMenuItemSpriteExtra::create(
			createButtonSpriteForTemplate("Weekly", weekly),
			this,
			menu_selector(SelectTemplatePopup::onTemplateButton)
		);

		auto demonButton = CCMenuItemSpriteExtra::create(
			createButtonSpriteForTemplate("Demon", (demon && !weekly)),
			this,
			menu_selector(SelectTemplatePopup::onTemplateButton)
		);

		dailyButton->setID("template-daily-btn"_spr);
		weeklyButton->setID("template-weekly-btn"_spr);
		demonButton->setID("template-demon-btn"_spr);

		dailyButton->setTag(1);
		weeklyButton->setTag(2);
		demonButton->setTag(3);

		layoutMenu->addChild(dailyButton);
		layoutMenu->addChild(weeklyButton);
		layoutMenu->addChild(demonButton);

		this->setTitle("Select Template");
		this->setID("select-template-popup"_spr);
		this->m_mainLayer->addChild(layoutMenu);
		this->m_mainLayer->setPositionY(100.0f);
		this->m_buttonMenu = layoutMenu;

		layoutMenu->updateLayout();
		layoutMenu->setPosition(
			this->m_mainLayer->getContentWidth() / 2.0f,
			this->m_mainLayer->getContentHeight() / 2.0f - 12.0f
		);

		return true;
	}

	void errorAndClose(gd::string errorText) {
		FLAlertLayer::create(
			"Error",
			errorText,
			"Ok"
		)->show();

		this->removeMeAndCleanup();
	}

	void onTemplateButton(CCObject* sender) {
		int tag = sender->getTag();

		if ((tag < 1) || (tag > 3)) {
			this->errorAndClose("<cr>Invalid</c> template button tag.");

			return;
		}

		auto level = this->m_level;
		auto commentLayer = this->m_shareCommentLayer;

		if ((level == nullptr) or (commentLayer == nullptr)) {
			this->errorAndClose("<cr>Invalid</c> popup data.");

			return;
		}

		bool daily = tag == 1;
		bool weekly = tag == 2;
		bool demon = tag == 3;

		bool beaten = this->beaten;

		bool supressPopup = Mod::get()->getSettingValue<bool>("supress_warnings");

		int collectedCoins = getCollectedCoinsViaDict(level);

		std::string templateName = "template_daily";

		switch (tag) {
		case 2:
			templateName = "template_weekly";
			break;
		case 3:
			templateName = "template_demon";
			break;
		default:
			break;
		}

		std::string textToAppend = getReplacedTemplate(
			templateName,
			level,
			this->dailyID,
			collectedCoins,
			level->m_coins,
			weekly,
			daily,
			demon || (level->m_stars >= 10)
		);

		auto currentText = static_cast<std::string>(commentLayer->m_descText);

		currentText.append(textToAppend);

		commentLayer->m_commentInput->setString(currentText);
		commentLayer->updateCharCountLabel();

		if (!supressPopup)
		{
			if (beaten) {
				FLAlertLayer::create(
					"Warning",
					"<cy>Not all variables may have been replaced</c>.",
					"Ok"
				)->show();
			}
			else {
				FLAlertLayer::create(
					"Warning",
					"<cy>Not all variables may have been replaced</c>. You have also <cr>not completed this level yet</c>.",
					"Ok"
				)->show();
			}
		}

		this->removeMeAndCleanup();
	}

public:
	static SelectTemplatePopup* create(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
		auto popup = new SelectTemplatePopup();

		if (popup->init(level, commentLayer, dailyID, beaten, daily, weekly)) {
			popup->autorelease();

			return popup;
		}

		delete popup;

		return nullptr;
	}
};

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

		bool isQuick = Mod::get()->getSettingValue<bool>("use_quick_insert");

		if ((dailyID == 0) && (stars < 10) && isQuick) return true;

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

		button->setID("comment-template-btn"_spr);

		menu->addChild(button);

		m_fields->m_level = level;

		return true;
	}

	void onCommentTemplateButton(CCObject* sender) {
		auto level = m_fields->m_level;
		bool beaten = level->getNormalPercent() == 100;

		int dailyID = static_cast<int>(level->m_dailyID);
		
		bool isQuick = Mod::get()->getSettingValue<bool>("use_quick_insert");

		if (isQuick) {
			bool supressPopup = Mod::get()->getSettingValue<bool>("supress_warnings");

			if (!beaten && !supressPopup) {
				FLAlertLayer::create(
					"Uncompleted Level",
					"You have <cr>not completed this level yet</c>, however the template has been inserted anyways.",
					"Ok"
				)->show();
			}

			int stars = level->m_stars;
			bool demon = stars >= 10;
			bool daily = dailyID > 0;
			bool weekly = daily && demon;
			const char* templateName = "template_daily";

			int collectedCoins = getCollectedCoinsViaDict(level);

			if (weekly) {
				templateName = "template_weekly";
			}
			else if (demon && !weekly) {
				templateName = "template_demon";
			}

			auto currentText = static_cast<std::string>(m_descText);

			currentText.append(
				getReplacedTemplate(
					templateName,
					level,
					dailyID,
					collectedCoins,
					level->m_coins,
					weekly,
					daily,
					demon
				)
			);

			m_commentInput->setString(currentText);
			updateCharCountLabel();
		}
		else {
			int stars = level->m_stars;
			bool demon = stars >= 10;
			bool daily = dailyID > 0;
			bool weekly = daily && demon;

			auto popup = SelectTemplatePopup::create(level, this, dailyID, beaten, daily, weekly);

			popup->show();
		}
	}
};