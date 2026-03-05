#include "../Include.hpp"
#include "../other/Utils.hpp"
#include "../UI/SelectTemplatePopup.hpp"
#include <Geode/modify/ShareCommentLayer.hpp>

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