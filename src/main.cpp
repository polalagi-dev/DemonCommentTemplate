#include <Geode/Geode.hpp>
#include <Geode/modify/ShareCommentLayer.hpp>
#include <string>

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

		auto* currentScene = CCScene::get();

		if (currentScene == nullptr) return true;

		auto* levelInfoLayer = currentScene->getChildByType<LevelInfoLayer>(0);

		if (levelInfoLayer == nullptr) return true;

		auto* level = levelInfoLayer->m_level;
		CCLayer* layer = static_cast<CCLayer*>(this->getChildByIndex(0));
		
		if (level == nullptr) return true;
		if (layer == nullptr) return true;

		auto stars = static_cast<int>(level->m_stars);
		auto dailyID = static_cast<int>(level->m_dailyID);

		if ((dailyID == 0) && (stars < 10)) return true;

		CCMenu* menu = layer->getChildByType<CCMenu>(0);

		if (menu == nullptr) return true;

		auto button = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Insert Template", 0.6f),
			this,
			menu_selector(HookedShareCommentLayer::onCommentTemplateButton)
		);

		button->setPosition(-0.0f, -10.0f);
		button->setID("comment-template-btn"_spr);

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

		if (weekly) {
			templateName = "template_weekly";
		}
		else if (demon && !weekly) {
			templateName = "template_demon";
		}

		std::string commentTemplate = Mod::get()->getSettingValue<std::string>(templateName);

		std::string replaced = replaceString(commentTemplate, "{attempts}", std::to_string(level->m_attempts));

		if (weekly) {
			replaced = replaceString(replaced, "{weekly}", std::to_string(dailyID - 100000));
		}
		else if (daily) {
			replaced = replaceString(replaced, "{daily}", std::to_string(dailyID));
		}

		if (demon) {
			auto* gameStatsManager = GameStatsManager::get();
			auto* stats = gameStatsManager->m_playerStats;
			auto* demonsObject = static_cast<CCString*>(stats->objectForKey("5")); // m_playerStats index for demons beaten
			
			if (demonsObject != nullptr) {
				int demons = std::stoi(demonsObject->getCString());

				replaced = replaceString(replaced, "{demons}", std::to_string(demons));
			}
		}

		std::string currentText = static_cast<std::string>(m_descText);

		currentText.append(replaced);

		m_commentInput->setString(currentText);
		updateCharCountLabel();
	}
};