#include "MultiLabel.h"

#include <regex>

#define ICON_ATLAS "guiv2.png"
#define ICON_CHAR  '\0'  // Character that represents custom icons

USING_NS_AX;

namespace opendw
{

MultiLabel::~MultiLabel()
{
    AX_SAFE_RELEASE(_iconBatchNode);
    AX_SAFE_RELEASE(_reusedIconSprite);
    AX_SAFE_RELEASE(_programState);
}

MultiLabel* MultiLabel::createWithBMFont(std::string_view path, std::string_view text)
{
    auto label = new MultiLabel();

    if (label->setBMFontFilePath(path))
    {
        label->autorelease();
        label->initIcons();
        label->setString(text);
        return label;
    }

    AX_SAFE_DELETE(label);
    return nullptr;
}

void MultiLabel::initIcons()
{
    // Register icon frames
    static const std::unordered_map<std::string, std::string> icons = {
        {":player:", "emoji/person"},
        {":check:", "emoji/check"},
        {":following:", "emoji/person-starred"},
        {":up:", "emoji/up"},
        {":down:", "emoji/down"},
        {":heart:", "emoji/heart"},
        {":gauge:", "emoji/gauge"},
        {":plus:", "emoji/plus"},
        {":question:", "emoji/question-mark"},
        {":keyboard:", "emoji/keyboard"},
        {":touch:", "emoji/touch"},
        {":bullet:", "emoji/bullet"},
        {":gem-red:", "emoji/gem-red"},
        {":cold:", "emoji/cold"},
        {":hunger:", "emoji/hunger"},
        {":thirst:", "emoji/thirst"},
        {":quarter:", "emoji/fraction-quarter"},
        {":half:", "emoji/fraction-half"},
        {":threequarters:", "emoji/fraction-three-quarters"},
        {":crown:", "emoji/crown"}};

    for (auto&& icon : icons)
    {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(icon.second);

        if (frame)
        {
            _iconFrames.insert(icon.first, frame);
        }
    }

    // Create components
    auto texture   = _director->getTextureCache()->addImage(ICON_ATLAS);
    _iconBatchNode = SpriteBatchNode::createWithTexture(texture);
    AX_SAFE_RETAIN(_iconBatchNode);
    _reusedIconSprite = Sprite::create();
    AX_SAFE_RETAIN(_reusedIconSprite);
    _reusedIconSprite->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _reusedIconSprite->setBatchNode(_iconBatchNode);

    // Create program state
    auto programType = backend::ProgramStateRegistry::getInstance()->getProgramType(
        backend::ProgramType::POSITION_TEXTURE_COLOR, texture->getSamplerFlags());
    _programState = new ProgramState(ProgramManager::getInstance()->loadProgram(programType));
    _programState->retain();
    _programState->autorelease();
    _iconQuadCommand.getPipelineDescriptor().programState = _programState;

    // Create fake letter definition for our icon character
    auto height = _fontAtlas->getLineHeight();
    FontLetterDefinition definition{};
    definition.offsetX         = height * 0.75F;
    definition.validDefinition = true;
    definition.xAdvance        = height * 1.5F;
    _fontAtlas->addLetterDefinition(ICON_CHAR, definition);
}

void MultiLabel::setString(std::string_view text)
{
    if (!_rawText.compare(text))
    {
        return;
    }

    _textIcons.clear();
    _rawText = text;
    std::regex regex(":.*?:");
    std::smatch match;
    std::string search(text);
    std::string out;
    auto offset = 0;

    while (std::regex_search(search, match, regex))
    {
        int index   = match.position();
        auto string = std::string(match.str());
        auto prefix = std::string(match.prefix());
        auto it     = _iconFrames.find(string);

        if (it != _iconFrames.end())
        {
            _textIcons.insert(index + offset, it.value());
            out += prefix + ICON_CHAR;
            offset++;
        }
        else
        {
            out += prefix + string;
            offset += string.size();
        }

        offset += prefix.size();
        search = match.suffix();
    }

    out += search;
    _iconsDirty = true;
    Label::setString(out);
}

void MultiLabel::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    Label::draw(renderer, transform, flags);

#if AX_USE_CULLING
    if (_insideBounds)  // Set by Label::draw
#endif
    {
        if (_iconsDirty)
        {
            updateIconQuads();
            updateIconColors();
            _iconsDirty = false;
        }

        auto atlas = _iconBatchNode->getTextureAtlas();

        if (atlas->getTotalQuads() != 0)
        {
            Mat4 projectionMatrix = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
            auto texture          = atlas->getTexture();
            _programState->setUniform(_mvpMatrixLocation, projectionMatrix.m, sizeof(projectionMatrix.m));
            _programState->setTexture(texture->getBackendTexture());
            _iconQuadCommand.init(_globalZOrder, texture, _blendFunc, atlas->getQuads(), atlas->getTotalQuads(),
                                  transform, flags);
            renderer->addCommand(&_iconQuadCommand);
        }
    }
}

void MultiLabel::updateColor()
{
    Label::updateColor();
    updateIconColors();
}

void MultiLabel::updateIconColors()
{
    Color4B color(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

    if (_isOpacityModifyRGB)
    {
        color.r *= _displayedOpacity / 255.0F;
        color.g *= _displayedOpacity / 255.0F;
        color.b *= _displayedOpacity / 255.0F;
    }

    auto atlas = _iconBatchNode->getTextureAtlas();
    auto quads = atlas->getQuads();
    auto count = atlas->getTotalQuads();

    for (ssize_t i = 0; i < atlas->getTotalQuads(); i++)
    {
        quads[i].bl.colors = color;
        quads[i].br.colors = color;
        quads[i].tl.colors = color;
        quads[i].tr.colors = color;
        atlas->updateQuad(quads[i], i);
    }
}

void MultiLabel::updateIconQuads()
{
    auto atlas = _iconBatchNode->getTextureAtlas();
    atlas->removeAllQuads();

    // FIXME: Positioning probably doesn't work properly on all fonts
    for (auto&& icon : _textIcons)
    {
        auto& letterInfo      = _lettersInfo[icon.first];
        auto x                = letterInfo.positionX + _linesOffsetX[letterInfo.lineIndex];
        auto y                = letterInfo.positionY + _letterOffsetY;
        auto index            = static_cast<int>(atlas->getTotalQuads());
        letterInfo.atlasIndex = index;
        auto frame            = icon.second;
        auto& size            = frame->getRect().size;
        auto adjustedScale    = _lineHeight / size.height;
        _reusedIconSprite->setTextureRect(frame->getRect(), frame->isRotated(), frame->getOriginalSize());
        _reusedIconSprite->setScale(_fontScale * MIN(1.0F, adjustedScale));
        _reusedIconSprite->setPosition(x, y - size.height * 0.4F * adjustedScale);
        _iconBatchNode->insertQuadFromSprite(_reusedIconSprite, index);
    }
}

}  // namespace opendw
