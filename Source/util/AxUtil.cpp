#include "AxUtil.h"

USING_NS_AX;

namespace opendw::ax_util
{

void runFadeSequence(Node* node, float fadeIn, float delay, float fadeOut)
{
    AX_ASSERT(node);
    auto cleanup = CallFunc::create([=]() { node->removeFromParent(); });
    auto action =
        Sequence::create({FadeIn::create(fadeIn), DelayTime::create(delay), FadeOut::create(fadeOut), cleanup});
    node->runAction(action);
}

bool isNodeVisible(Node* node)
{
    AX_ASSERT(node);
    auto current = node;

    while (current)
    {
        if (!current->isVisible())
        {
            return false;
        }

        current = current->getParent();
    }

    return true;
}

}  // namespace opendw::ax_util
