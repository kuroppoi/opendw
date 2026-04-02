#ifndef __AX_UTIL_H__
#define __AX_UTIL_H__

#include "axmol.h"

namespace opendw::ax_util
{

/* Easy access to `scheduleOnce` without needing to be a Node object. */
void scheduleOnce(const std::function<void(float)>& callback, void* target, float delay, std::string_view key);

void runFadeSequence(ax::Node* node, float fadeIn, float delay, float fadeOut);

void fadeOutAndRemove(ax::Node* node, float duration = 0.5F);

/* Like `Node::isVisible()`, but also checks the visibility of its ancestors. */
bool isNodeVisible(ax::Node* node);

}  // namespace opendw::ax_util

#endif  // __AX_UTIL_H__
