#include "AxUtil.h"

USING_NS_AX;

namespace opendw::ax_util
{

void scheduleOnce(const std::function<void(float)>& callback, void* target, float delay, std::string_view key)
{
    Director::getInstance()->getScheduler()->schedule(callback, target, 0.0F, 0, delay, false, key);
}

void runBlinkSequence(Node* node, const Color3B& fromColor, const Color3B& toColor, float duration)
{
    AX_ASSERT(node);
    auto action = RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(duration * 0.5F, fromColor),
                                                                       TintTo::create(duration * 0.5F, toColor)));
    node->runAction(action);
}

void runFadeSequence(Node* node, float fadeIn, float delay, float fadeOut)
{
    AX_ASSERT(node);
    auto cleanup = CallFunc::create([=]() { node->removeFromParent(); });
    auto action =
        Sequence::create({FadeIn::create(fadeIn), DelayTime::create(delay), FadeOut::create(fadeOut), cleanup});
    node->runAction(action);
}

void fadeOutAndRemove(Node* node, float duration)
{
    AX_ASSERT(node);
    auto cleanup = CallFunc::create([=]() { node->removeFromParent(); });
    auto action  = Sequence::createWithTwoActions(FadeOut::create(duration), cleanup);
    node->runAction(action);
}

bool isNodeVisible(Node* node)
{
    AX_ASSERT(node);
    auto current = node;

    while (current)
    {
        if (!current->isVisible() || current->getOpacity() == 0)
        {
            return false;
        }

        current = current->getParent();
    }

    return true;
}

}  // namespace opendw::ax_util
