#include "SelectTemplatePopup.hpp"
#include "../other/Utils.hpp"

bool SelectTemplatePopup::init(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
	if (!Popup::init(320.0f, 220.0f)) return false;

	bool demon = level->m_stars >= 10;

	this->m_level = level;
	this->m_shareCommentLayer = commentLayer;
	this->m_dailyID = dailyID;
	this->m_beaten = beaten;
	this->m_daily = daily;
	this->m_weekly = weekly;
	this->m_demon = demon;

	const float padding = 10.0f;

	auto mainWidth = this->m_mainLayer->getContentWidth();
	auto mainHeight = this->m_mainLayer->getContentHeight();

	auto templateList = SelectTemplatePopup::createTemplateList(mainWidth, mainHeight, padding);

	this->setTitle("Template Manager");
	this->setID("select-template-popup"_spr);
	this->m_mainLayer->addChild(templateList);
	this->m_mainLayer->setPositionY(130.0f);

	templateList->setID("template-list"_spr);

	return true;
}

CCMenu* SelectTemplatePopup::createTemplateListEntry(std::string commentTemplate, std::string name, TemplateColor color, float parentWidth, float padding) {
	auto entry = CCMenu::create();
	auto background = NineSlice::create("square02_001.png");

	entry->setPosition({ padding, padding });
	entry->setContentSize({ parentWidth * 0.5f - padding * 2.0f, 20.0f });

	background->setScale(0.5f);
	background->setContentSize({ entry->getContentWidth() * 2.0f, entry->getContentHeight() * 2.0f });
	background->setAnchorPoint({ 0, 0 });
	background->setOpacity(60);

	entry->addChild(background);

	return entry;
}

CCMenu* SelectTemplatePopup::createTemplateList(float parentWidth, float parentHeight, float padding) {
	auto list = CCMenu::create();
	auto background = NineSlice::create("square02_001.png");

	list->setPosition({ padding, padding });
	list->setContentSize({ parentWidth - padding * 2.0f, parentHeight - padding * 4.5f });

	background->setScale(0.5f);
	background->setContentSize({ list->getContentWidth() * 2.0f, list->getContentHeight() * 2.0f });
	background->setAnchorPoint({ 0, 0 });
	background->setOpacity(60);

	auto scroll = geode::ScrollLayer::create(list->getContentSize(), true, true);

	scroll->setLayout(geode::ScrollLayer::createDefaultListLayout());

	list->addChild(background);
	list->addChild(scroll);
	scroll->m_contentLayer->addChild(SelectTemplatePopup::createTemplateListEntry("a", "b", TemplateColor::Teal, list->getContentWidth(), 2.5f));
	scroll->updateLayout();
	scroll->scrollToTop();
	scroll->setContentHeight(200.0f);
	scroll->setID("template-list-scroll-layer"_spr);

	list->updateLayout();

	return list;
}

void SelectTemplatePopup::errorAndClose(gd::string errorText) {
	FLAlertLayer::create(
		"Error",
		errorText,
		"Ok"
	)->show();

	this->removeMeAndCleanup();
}

void SelectTemplatePopup::onTemplateButton(CCObject* sender) {
	auto level = this->m_level;
	auto commentLayer = this->m_shareCommentLayer;

	if ((level == nullptr) or (commentLayer == nullptr)) {
		this->errorAndClose("<cr>Invalid</c> popup data.");

		return;
	}

	bool daily = this->m_daily;
	bool weekly = this->m_weekly;
	bool demon = this->m_demon;

	bool beaten = this->m_beaten;

	bool supressPopup = Mod::get()->getSettingValue<bool>("supress_warnings");

	int collectedCoins = getCollectedCoinsViaDict(level);

	std::string textToAppend = getReplacedTemplate(
		"?",
		level,
		this->m_dailyID,
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

SelectTemplatePopup* SelectTemplatePopup::create(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
	auto popup = new SelectTemplatePopup();

	if (popup->init(level, commentLayer, dailyID, beaten, daily, weekly)) {
		popup->autorelease();

		return popup;
	}

	delete popup;

	return nullptr;
}