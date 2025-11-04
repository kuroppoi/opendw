#ifndef __AX_UTIL_H__
#define __AX_UTIL_H__

#include "axmol.h"

namespace opendw::ax_util
{

void runFadeSequence(ax::Node* node, float fadeIn, float delay, float fadeOut);

/* Like `Node::isVisible()`, but also checks the visibility of its ancestors. */
bool isNodeVisible(ax::Node* node);

}  // namespace opendw::ax_util

#endif  // __AX_UTIL_H__
