#include <Geode/Geode.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <fmod.hpp>

// Hopefully this gets approved
// After getting rejected 3 times


using namespace geode::prelude;

class $modify(CustomDeathSoundEngine, FMODAudioEngine) {
    struct Fields {
        std::vector<std::pair<FMOD::Sound*, FMOD::Channel*>> pendcustomsounds;
    };

    int playEffect(gd::string path, float speed, float p2, float volume) {

        bool enabled = Mod::get()->getSettingValue<bool>("enable-custom-death-sound");

        if (enabled && std::string(path.c_str()).find("explode_11.ogg") != std::string::npos) {\

            std::string soundChoice = Mod::get()->getSettingValue<std::string>("custom-death-sound-choice");
            std::string prefixname = fmt::format("{}/{}", Mod::get()->getID(), soundChoice);
            std::string resolvPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(prefixname.c_str(), false);

            if (!resolvPath.empty()) {
                if (m_system) {
                    FMOD::Sound* sound = nullptr;

                    if (m_system->createSound(resolvPath.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK || !sound) log::warn("debug1 (fail create death sound): '{}'", resolvPath); // debug1 remove later*
                    else {
                        FMOD::Channel* channel = nullptr;
                        m_system->playSound(sound, nullptr, false, &channel);
                        if (channel) {
                            channel->setVolume(volume);
                        }
                        m_fields->pendcustomsounds.push_back({ sound, channel });
                    }
                }
                return 0;
            }
        }

        return FMODAudioEngine::playEffect(path, speed, p2, volume);
    }

    void update(float dt) {
        FMODAudioEngine::update(dt);

        auto& pending = m_fields->pendcustomsounds;

        for (auto it = pending.begin(); it != pending.end();) {
            bool playing = false;
            if (it->second) it->second->isPlaying(&playing);

            if (!playing) {
                if (it->first) it->first->release();
                it = pending.erase(it);
            } else {
                ++it;
            }
        }
    }

};