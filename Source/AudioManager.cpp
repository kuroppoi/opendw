#include "AudioManager.h"

#define MUSIC_VOLUME_KEY "musicVolume"
#define BUTTON_SFX       "click.ogg"
#define THEME_MUSIC      "theme-v1-loop.ogg"

USING_NS_AX;

namespace opendw
{

static AudioManager* sInstance;

AudioManager* AudioManager::getInstance()
{
    if (!sInstance)
    {
        sInstance = utils::createInstance<AudioManager>();
        AX_SAFE_RETAIN(sInstance);
    }

    return sInstance;
}

void AudioManager::destroyInstance()
{
    AX_SAFE_RELEASE_NULL(sInstance);
}

bool AudioManager::init()
{
    if (!Node::init())
    {
        return false;
    }

    // TODO: load audio settings (volume, etc.)
    _bgmId              = AudioEngine::INVALID_AUDIO_ID;
    _masterVolume       = 1.0F;
    _sfxVolume          = 1.0F;
    _musicVolume        = 1.0F;
    _fadeOutMusicAction = nullptr;
    AudioEngine::preload(THEME_MUSIC);
    return true;
}

void AudioManager::updateTweenAction(float value, std::string_view key)
{
    if (key == MUSIC_VOLUME_KEY)
    {
        auto volume = _masterVolume * _musicVolume * value;
        AudioEngine::setVolume(_bgmId, value);

        if (volume == 0.0F)
        {
            stopMusic();
        }
    }
}

void AudioManager::playSfx(const std::string& file)
{
    auto volume = _masterVolume * _sfxVolume;
    AudioEngine::play2d(file, false, volume);
}

void AudioManager::playButtonSfx()
{
    playSfx(BUTTON_SFX);
}

void AudioManager::playThemeMusic()
{
    playMusic(THEME_MUSIC);
}

void AudioManager::playMusic(const std::string& file)
{
    stopMusic();
    auto volume = _masterVolume * _musicVolume;
    _bgmId = AudioEngine::play2d(file, true, volume);
}

void AudioManager::stopMusic()
{
    stopAction(_fadeOutMusicAction);
    AudioEngine::stop(_bgmId);
    _bgmId              = AudioEngine::INVALID_AUDIO_ID;
    _fadeOutMusicAction = nullptr;
}

void AudioManager::fadeOutMusic(float duration)
{
    stopAction(_fadeOutMusicAction);
    _fadeOutMusicAction = ActionTween::create(duration, MUSIC_VOLUME_KEY, 1.0F, 0.0F);
    runAction(_fadeOutMusicAction);
}

}  // namespace opendw
