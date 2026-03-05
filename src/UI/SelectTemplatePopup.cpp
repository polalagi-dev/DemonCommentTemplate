#include "SelectTemplatePopup.hpp"
#include "../other/Utils.hpp"

bool SelectTemplatePopup::init(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
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

void SelectTemplatePopup::errorAndClose(gd::string errorText) {
	FLAlertLayer::create(
		"Error",
		errorText,
		"Ok"
	)->show();

	this->removeMeAndCleanup();
}

void SelectTemplatePopup::onTemplateButton(CCObject* sender) {
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

SelectTemplatePopup* SelectTemplatePopup::create(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly) {
	auto popup = new SelectTemplatePopup();

	if (popup->init(level, commentLayer, dailyID, beaten, daily, weekly)) {
		popup->autorelease();

		return popup;
	}

	delete popup;

	return nullptr;
}