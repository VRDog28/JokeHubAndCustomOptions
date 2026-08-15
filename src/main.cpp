#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <random>
#include <algorithm>

using namespace geode::prelude;

// If this doesnt build i will quit making mods

static std::vector<int> getUsedCells() {
    return Mod::get()->getSavedValue<std::vector<int>>("used", {});
}

static void setUsedCells(const std::vector<int>& cells) {
    Mod::get()->setSavedValue("used", cells);
}

static void resetUsedCells() {
    Mod::get()->setSavedValue("used", std::vector<int>{});
}

static void applyOverlays(PlayLayer* layer) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    const int cols = 8;
    const int rows = 4;
    float w = winSize.width  / cols;
    float h = winSize.height / rows;


    for (int cell : getUsedCells()) {
        int col = cell % cols;
        int row = cell / cols;


        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, 255), w, h);
        overlay->setPosition({ col * w, row * h });
        layer->addChild(overlay, 9999);
    }
}

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        std::mt19937 rng{ std::random_device{}() };
    };

    bool init(GJGameLevel* level, bool UseReplay, bool DontCreateDaobjects) {
        if (!PlayLayer::init(level, UseReplay, DontCreateDaobjects)) return false;

        applyOverlays(this);

        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);

        if (!m_player1 || !m_player1->m_isDead) return;

        auto used = getUsedCells();
        if (used.size() >= 32) return;
        int cell;
        Fields* f = m_fields.self();
        do {
            cell = static_cast<int>(f->rng() % 32);
        } while (std::find(used.begin(), used.end(), cell) != used.end());

        used.push_back(cell);
        setUsedCells(used);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        const int cols = 8;
        const int rows = 4;


        float w = winSize.width  / cols;
        float h = winSize.height / rows;


        int col = cell % cols;
        int row = cell / cols;

        auto overlay = CCLayerColor::create(ccc4(0, 0, 0, 255), w, h);
        overlay->setPosition({ col * w, row * h });
        this->addChild(overlay, 9999);
    }
};

class $modify(TheMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;


        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu) return true;

        auto btnSpr = ButtonSprite::create("Reset", "bigFont.fnt", "GJ_button_02.png", 0.8f);
        btnSpr->setScale(0.7f);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSpr,
            this,
            menu_selector(TheMenuLayer::onResetCells)
        );
        btn->setID("reset-cells-btn");

        bottomMenu->addChild(btn);


        if (auto layout = bottomMenu->getLayout()) {
            bottomMenu->updateLayout();
        }

        return true;
    }

    void onResetCells(CCObject*) {
        resetUsedCells();

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto label = CCLabelBMFont::create("Screen reset", "bigFont.fnt");
        
        label->setScale(0.6f);
        label->setPosition({ winSize.width / 2, winSize.height / 2 });
        label->setOpacity(255)
        ;
        this->addChild(label, 999);

        label->runAction(CCSequence::create(
            CCDelayTime::create(1.0f),
            CCFadeOut::create(0.5f),
            CCRemoveSelf::create(),
            nullptr
        ));
    }
};