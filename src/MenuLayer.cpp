#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;
// boring ui stuff
class $modify(AMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto bottomMenu = this->getChildByID("bottom-menu");

        auto settingtext = CCLabelBMFont::create("Settings", "bigFont.fnt");
        settingtext->setScale(0.32f);

        auto settingsSpr = CircleButtonSprite::create(
            settingtext,
            CircleBaseColor::Green,
            CircleBaseSize::Medium
        );

        auto settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSpr, this,
            menu_selector(AMenuLayer::onOpenSettings)
        );
        settingsBtn->setID("screencrack-settings-btn");

        if (bottomMenu) {
            bottomMenu->addChild(settingsBtn);
            bottomMenu->updateLayout();
        }

        auto resetSpr = ButtonSprite::create("Reset", "bigFont.fnt", "GJ_button_06.png", 0.5f);
        resetSpr->setScale(0.5f);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            resetSpr, this,
            menu_selector(AMenuLayer::onResetScreen)
        );
        resetBtn->setID("screencrack-reset-btn");

        auto resetMenu = CCMenu::create();
        resetMenu->setID("screencrack-reset-menu");
        resetMenu->addChild(resetBtn);

        float posY = bottomMenu ? (bottomMenu->getPositionY() - 26.0f) : 18.0f;

        resetMenu->setPosition(CCPoint(winSize.width / 2, posY));

        this->addChild(resetMenu);
        return true;
    }

    void onOpenSettings(CCObject*) {
        openSettingsPopup(Mod::get());
    }

    void onResetScreen(CCObject*) {
        Mod::get()->setSavedValue("used", std::vector<int>{});

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("successfully reset!", "bigFont.fnt");


        label->setScale(0.6f);
        label->setPosition(CCPoint(winSize.width / 2, winSize.height / 2));
        label->setOpacity(255);
        this->addChild(label, 999);
        //the very cool effect
        label->runAction(CCSequence::create(CCDelayTime::create(1.0f),CCFadeOut::create(0.4f),CCRemoveSelf::create(),nullptr));
    }
};