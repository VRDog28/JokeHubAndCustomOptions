#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <random>
#include <algorithm>
// Includes
using namespace geode::prelude;

// Hopfully this gets accepted

// Play Layer
class $modify(APlayLay, PlayLayer) {
    struct Fields {
        std::mt19937 rng{ std::random_device{}() };
        int deathCount = 0;
        float currentGravityMultiplier = 1.0f;
    };
    // Initializes
    bool init(GJGameLevel* level, bool useReplay, bool DontMakeObjects) {
        if (!PlayLayer::init(level, useReplay, DontMakeObjects)) return false;

        if (Mod::get()->getSettingValue<bool>("screen-break")) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            int cols = static_cast<int>(Mod::get()->getSettingValue<int64_t>("grid-cols"));
            int rows = std::max(1, cols / 2);
            float w = winSize.width / cols;
            float h = winSize.height / rows;

            for (int cell : Mod::get()->getSavedValue<std::vector<int>>("used", {})) {
                auto overlay = CCLayerColor::create(ccc4(0, 0, 0, 255), w, h);
                overlay->setPosition(CCPoint((cell % cols) * w, (cell / cols) * h));
                this->addChild(overlay, 9999);
            }
        }
        return true;
    }
    // Makes it so that gravity persists across attempts
    void resetLevel() {
        PlayLayer::resetLevel();
        if (Mod::get()->getSettingValue<bool>("gravity-penalty") && m_player1) {
            m_player1->m_gravityMod = m_fields->currentGravityMultiplier;
        }
    }
    // If the player dies apply the things
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);
        if (!m_player1 || !m_player1->m_isDead) return;

        Fields* f = m_fields.self();
        f->deathCount++;

        if (Mod::get()->getSettingValue<bool>("screen-break")) {

            int cols = static_cast<int>(Mod::get()->getSettingValue<int64_t>("grid-cols"));

            int rows = std::max(1, cols / 2);
            int maxCells = cols * rows;

            auto used = Mod::get()->getSavedValue<std::vector<int>>("used", {});


            if (static_cast<int>(used.size()) < maxCells) {
                int cell;
                do {
                    cell = static_cast<int>(f->rng() % maxCells);
                } while (std::find(used.begin(), used.end(), cell) != used.end());

                used.push_back(cell);
                Mod::get()->setSavedValue("used", used);

                auto winSize = CCDirector::sharedDirector()->getWinSize();
                float w = winSize.width / cols;
                float h = winSize.height / rows;

                auto overlay = CCLayerColor::create(ccc4(0, 0, 0, 255), w, h);
                overlay->setPosition(CCPoint((cell % cols) * w, (cell / cols) * h));
                overlay->setOpacity(0);
                overlay->runAction(CCFadeIn::create(0.4f));
                this->addChild(overlay, 9999);
            }
        }

        if (Mod::get()->getSettingValue<bool>("fps-penalty")) {
            int penalty = static_cast<int>(Mod::get()->getSettingValue<int64_t>("fps-drop"));
            int newCap = std::max(10, 60 - f->deathCount * penalty);
            CCDirector::sharedDirector()->setAnimationInterval(1.0 / newCap);
        }

        if (Mod::get()->getSettingValue<bool>("gravity-penalty") && m_player1) {
            float gravDrop = static_cast<float>(Mod::get()->getSettingValue<double>("gravity-drop"));
            f->currentGravityMultiplier = std::max(0.1f, f->currentGravityMultiplier - gravDrop);
            m_player1->m_gravityMod = f->currentGravityMultiplier;
        }
    }

    void onQuit() {
        PlayLayer::onQuit();
        CCDirector::sharedDirector()->setAnimationInterval(1.0 / 60.0);
    }
};
// UI for buttons and settings etc
class $modify(AMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto bottomMenu = this->getChildByID("bottom-menu");

        auto settingsLabel = CCLabelBMFont::create("Settings", "bigFont.fnt");
        settingsLabel->setScale(0.32f);

        auto settingsSpr = CircleButtonSprite::create(
            settingsLabel,
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
        auto label = CCLabelBMFont::create("successfully reset!", "bigFont.fnt"); // reset message 
        label->setScale(0.6f);
        label->setPosition(CCPoint(winSize.width / 2, winSize.height / 2));
        label->setOpacity(255);
        this->addChild(label, 999);

        label->runAction(CCSequence::create(
            CCDelayTime::create(1.0f),
            CCFadeOut::create(0.4f),
            CCRemoveSelf::create(),
            nullptr
        ));
    }
};
