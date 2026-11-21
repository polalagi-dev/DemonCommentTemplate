#pragma once

#include "../Include.hpp"

class SelectTemplatePopup : public geode::Popup {

	GJGameLevel* m_level{};
	ShareCommentLayer* m_shareCommentLayer{};

	int dailyID{};
	bool beaten{};

protected:
	bool init(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly);
	void errorAndClose(std::string errorText);
	void onTemplateButton(CCObject* sender);

public:
	static SelectTemplatePopup* create(GJGameLevel* level, ShareCommentLayer* commentLayer, int dailyID, bool beaten, bool daily, bool weekly);
};