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
    struct LoopLayer
    {
        float level;
        float gain;
    };

    static AudioManager* getInstance();
    static void destroyInstance();

    /* FUNC: AudioEngine::init @ 0x1000B1018 */
    bool init() override;

    /* FUNC: AudioEngine::load: @ 0x1000B2FE0 */
    void configure(const ax::ValueMap& config);

    /* FUNC: AudioEngine::step: @ 0x1000B2C71 */
    void update(float deltaTime) override;
    void updateTweenAction(float value, std::string_view key) override;

    /* FUNC: AudioEngine::playSample:group:pitch:pan:gain: @ 0x1000B3A79 */
    AUDIO_ID playSfx(const std::string& name, float pitch = 1.0F, float gain = 1.0F);

    /* FUNC: AudioEngine::playSfx:variant:pitchRange:pan:gain: @ 0x1000B3DDA */
    AUDIO_ID playSfx(const std::string& name, const std::string& variant, float pitchRange = 0.0F, float gain = 1.0F);

    void playButtonSfx();

    /* FUNC: AudioEngine::playThemeMusic @ 0x1000B4248 */
    void playThemeMusic();
    void playMusic(const std::string& name);

    /* FUNC: AudioEngine::stopMusic @ 0x1000B42BA */
    void stopMusic();

    /* FUNC: AudioEngine::fadeOutMusic @ 0x1000B4273 */
    void fadeOutMusic(float duration = 2.0F);

    /* FUNC: AudioEngine::playLoopLayer:level:gain: @ 0x1000B3ED4 */
    void playLoopLayer(const std::string& name, float level, float gain);

    /* FUNC: AudioEngine::setAutoLoopLayer:level:gain: @ 0x1000B41B0 */
    void setAutoLoopLayer(const std::string& name, float level, float gain);
    void clearLoopLayers();

private:
    ax::ValueMap _config;                           // AudioEngine::config @ 0x100312820
    std::map<std::string, AUDIO_ID> _loopMap;       // AudioEngine::loopMap @ 0x1003127A0
    std::map<std::string, LoopLayer> _autoLoopMap;  // AudioEngine::autoLoopMap @ 0x1003127A8
    AUDIO_ID _bgmId;
    float _masterVolume;
    float _sfxVolume;
    float _musicVolume;
    ax::Action* _fadeOutMusicAction;
};

}  // namespace opendw

#endif  // __AUDIO_MANAGER_H__
