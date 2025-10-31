#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "audio/AudioEngine.h"
#include "axmol.h"

namespace opendw
{

/*
 * FUNC: AudioEngine : NSObject @ 0x100318340
 */
class AudioManager : public ax::Node, public ax::ActionTweenDelegate
{
public:
    static AudioManager* getInstance();
    static void destroyInstance();

    /* FUNC: AudioEngine::init @ 0x1000B1018 */
    bool init() override;

    void updateTweenAction(float value, std::string_view key) override;

    /* FUNC: AudioEngine::playSample: @ 0x1000B3A2F */
    void playSfx(const std::string& file);
    void playButtonSfx();

    /* FUNC: AudioEngine::playThemeMusic @ 0x1000B4248 */
    void playThemeMusic();

    void playMusic(const std::string& file);

    /* FUNC: AudioEngine::stopMusic @ 0x1000B42BA */
    void stopMusic();

    /* FUNC: AudioEngine::fadeOutMusic @ 0x1000B4273 */
    void fadeOutMusic(float duration = 2.0F);

private:
    AUDIO_ID _bgmId;
    float _masterVolume;
    float _sfxVolume;
    float _musicVolume;
    ax::Action* _fadeOutMusicAction;
};

}  // namespace opendw

#endif  // __AUDIO_MANAGER_H__
