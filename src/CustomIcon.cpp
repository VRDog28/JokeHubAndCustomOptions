#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
// this was the hardest script to make in my whole entire codebase
class $modify(CustomIconPlayer, PlayerObject) {
    struct Fields {
        CCSprite* customSprite = nullptr;
        std::string loadedIconFile;
    };

    bool init(int playerindex, int vehicletype, GJBaseGameLayer* layer, PlayLayer* playLayer, bool isfirst) {
        if (!PlayerObject::init(playerindex, vehicletype, layer, playLayer, isfirst)) {
            return false;
        }

        bool enabled = Mod::get()->getSettingValue<bool>("enable-custom-icon");
        std::string iconFile = Mod::get()->getSettingValue<std::string>("custom-icon-choice");

        bool isCube = (!m_isShip && !m_isBall && !m_isBird && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing);

        if (!enabled || !isCube) {

            if (m_fields->customSprite) {
                m_fields->customSprite->setVisible(false);
            }
            if (m_iconSprite) m_iconSprite->setOpacity(255);
            if (m_iconSpriteSecondary) m_iconSpriteSecondary->setOpacity(255);
            if (m_iconGlow) m_iconGlow->setOpacity(255);
            return true;
        }
  

        if (m_iconSprite) m_iconSprite->setOpacity(0);
        if (m_iconSpriteSecondary) m_iconSpriteSecondary->setOpacity(0);
        if (m_iconGlow) m_iconGlow->setOpacity(0);
        if (m_iconSpriteWhitener) m_iconSpriteWhitener->setOpacity(0);

        if (!m_fields->customSprite || m_fields->loadedIconFile != iconFile) {
            if (m_fields->customSprite) {
                m_fields->customSprite->removeFromParent();
                m_fields->customSprite = nullptr;
            }

            std::string prefixname = fmt::format("{}/{}", Mod::get()->getID(), iconFile);
            CCSprite* spr = CCSprite::create(prefixname.c_str());

            if (spr && !spr->getTexture()) {
                spr = nullptr;
            }

            if (spr) {
                auto sz = spr->getContentSize();
                if (sz.width > 0 && sz.height > 0) {
                    spr->setScale(30.0f / std::max(sz.width, sz.height));
                }

                spr->setPosition(CCPoint(0.0f, 0.0f));

                if (m_mainLayer) {
                    m_mainLayer->addChild(spr, 99);
                } else {
                    this->addChild(spr, 99);
                }

                m_fields->customSprite = spr;
                m_fields->loadedIconFile = iconFile;
            }
        }

        if (m_fields->customSprite) {
            m_fields->customSprite->setVisible(true);
        }

        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);

        bool enabled = Mod::get()->getSettingValue<bool>("enable-custom-icon");
        std::string iconFile = Mod::get()->getSettingValue<std::string>("custom-icon-choice");

        bool isCube = (!m_isShip && !m_isBall && !m_isBird && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing);

        if (!enabled || !isCube) {
            if (m_fields->customSprite) {
                m_fields->customSprite->setVisible(false);
            }
            if (m_iconSprite) m_iconSprite->setOpacity(255);
            if (m_iconSpriteSecondary) m_iconSpriteSecondary->setOpacity(255);
            if (m_iconGlow) m_iconGlow->setOpacity(255);
            return;
        }

        if (m_iconSprite) m_iconSprite->setOpacity(0);
        if (m_iconSpriteSecondary) m_iconSpriteSecondary->setOpacity(0);
        if (m_iconGlow) m_iconGlow->setOpacity(0);
        if (m_iconSpriteWhitener) m_iconSpriteWhitener->setOpacity(0);

        if (!m_fields->customSprite || m_fields->loadedIconFile != iconFile) {
            if (m_fields->customSprite) {
                m_fields->customSprite->removeFromParent();
                m_fields->customSprite = nullptr;
            }

            std::string prefixname = fmt::format("{}/{}", Mod::get()->getID(), iconFile);
            CCSprite* spr = CCSprite::create(prefixname.c_str());

            if (spr && !spr->getTexture()) {
                spr = nullptr;
            }

            if (spr) {
                auto sz = spr->getContentSize();
                if (sz.width > 0 && sz.height > 0) {
                    spr->setScale(30.0f / std::max(sz.width, sz.height));
                }

                spr->setPosition(CCPoint(0.0f, 0.0f));

                if (m_mainLayer) {
                    m_mainLayer->addChild(spr, 99);
                } else {
                    this->addChild(spr, 99);
                }

                m_fields->customSprite = spr;
                m_fields->loadedIconFile = iconFile;
            }
        }

        if (m_fields->customSprite) {
            m_fields->customSprite->setVisible(true);
        }
    }
};