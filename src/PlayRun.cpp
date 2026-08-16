#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <cmath>
#include <algorithm>

using namespace geode::prelude;

class $modify(RunawayMenuLayer, MenuLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* playBtn = nullptr;
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto mainMenu = this->getChildByID("main-menu")) {
            if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(mainMenu->getChildByID("play-button"))) {
                m_fields->playBtn = btn;
            }
        }

        this->schedule(schedule_selector(RunawayMenuLayer::stepRunaway));

        return true;
    }

    void stepRunaway(float dt) {
        if (!Mod::get()->getSettingValue<bool>("enable-runaway-play")) return;
        // this was a bit hard to make lots of math 
        auto* btn = m_fields->playBtn;
        if (!btn || !btn->getParent()) return;

        CCPoint mouseposition = geode::cocos::getMousePos();
        CCPoint localpoint = btn->getParent()->convertToNodeSpace(mouseposition);
        CCPoint curpos = btn->getPosition();

        float dx = curpos.x - localpoint.x;
        float dy = curpos.y - localpoint.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        const float escapeRadius = 130.0f;
        // used ai for this part because i couldnt do it myself
        if (dist < escapeRadius && dist > 0.001f) {
            float fleeFactor = (escapeRadius - dist) * 0.35f;
            float dirX = dx / dist;
            float dirY = dy / dist;

            CCPoint targetPos = curpos + CCPoint(dirX * fleeFactor, dirY * fleeFactor);

            auto winSize = CCDirector::sharedDirector()->getWinSize();
            CCPoint menuPos = btn->getParent()->getPosition();

            CCSize btnSize = btn->getContentSize();
            float halfW = (btnSize.width * 0.5f) * btn->getScaleX();
            float halfH = (btnSize.height * 0.5f) * btn->getScaleY();

            float minX = -menuPos.x + halfW;
            float maxX = winSize.width - menuPos.x - halfW;
            float minY = -menuPos.y + halfH;
            float maxY = winSize.height - menuPos.y - halfH;

            targetPos.x = std::clamp(targetPos.x, minX, maxX);
            targetPos.y = std::clamp(targetPos.y, minY, maxY);

            btn->setPosition(targetPos);
        }
    }
};