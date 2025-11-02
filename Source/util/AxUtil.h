#ifndef __AX_UTIL_H__
#define __AX_UTIL_H__

#include "axmol.h"

namespace opendw::ax_util
{

void runFadeSequence(ax::Node* node, float fadeIn, float delay, float fadeOut);

}  // namespace opendw::ax_util

#endif  // __AX_UTIL_H__
