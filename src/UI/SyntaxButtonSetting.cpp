#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class SyntaxButton : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SyntaxButton>();
        auto root = checkJson(json, "syntax-button");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);

        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }

    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class SyntaxButtonNode : public SettingNodeV3 {
    ButtonSprite* m_buttonSprite;
    CCMenuItemSpriteExtra* m_button;

    bool init(std::shared_ptr<SyntaxButton> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;

        m_buttonSprite = ButtonSprite::create("Show", "goldFont.fnt", "GJ_button_01.png", .8f);
        m_buttonSprite->setScale(0.75f);
        m_button = CCMenuItemSpriteExtra::create(
            m_buttonSprite, this, menu_selector(SyntaxButtonNode::onButton)
        );
        this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
        this->getButtonMenu()->setContentWidth(60);
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);

        return true;
    }

    void updateState(CCNode* invoker) override {
        SettingNodeV3::updateState(invoker);

        auto shouldEnable = this->getSetting()->shouldEnable();
        m_button->setEnabled(shouldEnable);
        m_buttonSprite->setCascadeColorEnabled(true);
        m_buttonSprite->setCascadeOpacityEnabled(true);
        m_buttonSprite->setOpacity(shouldEnable ? 255 : 155);
        m_buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
    }

    void onButton(CCObject*) {
        auto alert = FLAlertLayer::create(
            "Template Syntax",
            "<cg>AttemptCount</c> - Replaces with the amount of attempts on the level\n<cg>DemonCount</c> - Replaces with the current amount of demons beaten.\n<cg>DailyID</c> - Replaces with the current amount of daily levels beaten.\n<cg>WeeklyID</c> - Replaces with the current amount of weekly levels beaten.\n<cg>CoinCount</c> - Replaces with the amount of completed coins. N/A if the levels doesn't have coins.\n<cg>TimeH12, TimeH24, TimeM, TimeS, TimeAMPM, TimeDay, TimeMonth, TimeMonName, TimeYear</c> - 12-Hour Hour, 24-Hour Hour, Minute, Second, AM/PM, Day, Month, Short Month Name, Year (Local Time)",
            "Ok"
        );

        alert->setScale(0.7f);
        alert->setOpacity(0);

        alert->show();
    }

    void onCommit() override {}
    void onResetToDefault() override {}

public:
    static SyntaxButtonNode* create(std::shared_ptr<SyntaxButton> setting, float width) {
        auto ret = new SyntaxButtonNode();

        if (ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }
};

SettingNodeV3* SyntaxButton::createNode(float width) {
    return SyntaxButtonNode::create(
        std::static_pointer_cast<SyntaxButton>(shared_from_this()),
        width
    );
}

$on_mod(Loaded) {
    (void)Mod::get()->registerCustomSettingType("syntax-button", &SyntaxButton::parse);
}