#include "AudioManager.h"

#include "util/MapUtil.h"

#define MUSIC_VOLUME_KEY "musicVolume"
#define AUDIO_FORMAT     "ogg"
#define BUTTON_SFX       "click"
#define THEME_MUSIC      "theme-v1-loop"

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
    AudioEngine::preload(std::format("{}.{}", THEME_MUSIC, AUDIO_FORMAT));
    return true;
}

void AudioManager::configure(const ValueMap& config)
{
    _config = config;  // Create copy
}

void AudioManager::update(float deltaTime)
{
    for (auto& entry : _autoLoopMap)
    {
        auto& loopLayer = entry.second;
        playLoopLayer(entry.first, loopLayer.level, loopLayer.gain);
    }
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

AUDIO_ID AudioManager::playSfx(const std::string& name, float pitch, float gain)
{
    auto volume = _masterVolume * _sfxVolume * gain;
    auto file   = name + "." + AUDIO_FORMAT;
    auto track  = AudioEngine::play2d(file, false, volume);
    AudioEngine::setPitch(track, pitch);
    return track;
}

AUDIO_ID AudioManager::playSfx(const std::string& name, const std::string& variant, float pitchRange, float gain)
{
    auto path        = std::format("{}.{}", name, variant);
    auto defaultPath = std::format("{}.default", name);
    auto& options    = map_util::getArray(_config, path, map_util::getArray(_config, defaultPath));

    if (options.empty())
    {
        return AudioEngine::INVALID_AUDIO_ID;
    }

    auto option = options[rand() % options.size()].asString();
    auto pitch  = random(1.0F - pitchRange * 0.5F, 1.0F + pitchRange * 0.5F);
    auto track  = playSfx(option, pitch, gain);
    return track;
}

void AudioManager::playButtonSfx()
{
    playSfx(BUTTON_SFX, 1.0F, 0.3F);
}

void AudioManager::playThemeMusic()
{
    playMusic(THEME_MUSIC);
}

void AudioManager::playMusic(const std::string& name)
{
    stopMusic();
    auto volume = _masterVolume * _musicVolume;
    auto file   = name + "." + AUDIO_FORMAT;
    _bgmId      = AudioEngine::play2d(file, true, volume);
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

void AudioManager::playLoopLayer(const std::string& name, float level, float gain)
{
    auto& samples = map_util::getArray(_config, name);
    auto count    = samples.size();

    if (count == 0)
    {
        return;
    }

    auto maxIndex = count - 1;
    auto sampleA  = (int)(maxIndex * level);
    auto sampleB  = MIN(sampleA + 1, maxIndex);

    if (count > 1)
    {
        level = clampf(level, 0.0F, 1.0F - FLT_EPSILON);  // BUGFIX: To prevent 1.0 % 1.0 = 0.0
        level = fmodf(level, 1.0F / maxIndex) * maxIndex;
    }

    for (int i = 0; i < count; i++)
    {
        auto sample = samples[i].asString();

        // Mute the track if it's outside of the level range
        if (i != sampleA && i != sampleB)
        {
            if (_loopMap.contains(name))
            {
                auto track = _loopMap[sample];
                AudioEngine::setVolume(track, 0.0F);
            }

            continue;
        }

        // sampleA can be the same as sampleB, so watch out
        auto modifier = i != sampleB ? 1.0F - level : level;
        auto file     = sample + "." + AUDIO_FORMAT;
        auto volume   = _masterVolume * _sfxVolume * modifier * gain;
        auto track    = _loopMap.contains(sample) ? _loopMap[sample] : AudioEngine::play2d(file, true);
        AudioEngine::setVolume(track, volume);
        _loopMap[sample] = track;
    }
}

void AudioManager::setAutoLoopLayer(const std::string& name, float level, float gain)
{
    _autoLoopMap[name] = {level, gain};
}

void AudioManager::clearLoopLayers()
{
    for (auto& entry : _loopMap)
    {
        AudioEngine::stop(entry.second);
    }

    _loopMap.clear();
    _autoLoopMap.clear();
}

}  // namespace opendw
