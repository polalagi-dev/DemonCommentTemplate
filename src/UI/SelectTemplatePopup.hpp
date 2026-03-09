#pragma once

#include "../Include.hpp"

enum TemplateColor {
	Gray = 0,
	Dark = 1,
	Orange = 2,
	Green = 3,
	Teal = 4,
	Cyan = 5,
	Blue = 6,
	Magenta = 7,
	Pink = 8
};

class SelectTemplatePopup : public geode::Popup {

	GJGameLevel* m_level{};
	ShareCommentLayer* m_shareCommentLayer{};

	int m_dailyID{};
	bool m_beaten{};
	bool m_daily{};
	bool m_weekly{};
	bool m_demon{};

protected:
	bool init(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly);
	CCMenu* createTemplateListEntry(std::string commentTemplate, std::string templateName, TemplateColor color, float parentWidth, float padding);
	CCMenu* createTemplateList(float parentWidth, float parentHeight, float padding);
	void errorAndClose(gd::string errorText);
	void onTemplateButton(CCObject* sender);

public:
	static SelectTemplatePopup* create(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly);
};