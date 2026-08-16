#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>
#include <algorithm>

using namespace geode::prelude;

class $modify(APlayLay, PlayLayer) {
    struct Fields {
        std::mt19937 rng{ std::random_device{}() };
        int deathCount = 0;
        float currentGravityMultiplier = 1.0f;
        double originalInterval = 1.0 / 60.0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontMakeObjects) {
        if (!PlayLayer::init(level, useReplay, dontMakeObjects)) return false;

        m_fields->originalInterval = CCDirector::sharedDirector()->getAnimationInterval();

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

    void resetLevel() {

        PlayLayer::resetLevel();
        if (Mod::get()->getSettingValue<bool>("enable-custom-icon") && 
            Mod::get()->getSettingValue<bool>("randomize-icon")) {
            
            std::vector<std::string> icons = { "dog.png", "cat.png", "cube.png" };
            std::string randomChoice = icons[m_fields->rng() % icons.size()];
            
            Mod::get()->setSettingValue<std::string>("custom-icon-choice", randomChoice);
        }
        if (Mod::get()->getSettingValue<bool>("gravity-penalty") && m_player1) {
            m_player1->m_gravityMod = m_fields->currentGravityMultiplier;
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {

        PlayLayer::destroyPlayer(player, obj);
        if (!m_player1 || !m_player1->m_isDead) return;

        auto* f = m_fields.self();
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
        CCDirector::sharedDirector()->setAnimationInterval(m_fields->originalInterval);
    }
};