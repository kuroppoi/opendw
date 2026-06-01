#ifndef __TABS_BAR_DELEGATE_H__
#define __TABS_BAR_DELEGATE_H__

#include "axmol.h"

namespace opendw
{

class TabsBar;

class TabsBarDelegate
{
public:
    virtual void onTabSelected(TabsBar* source, ssize_t index) = 0;
};

}  // namespace opendw

#endif  // __TABS_BAR_DELEGATE_H__
