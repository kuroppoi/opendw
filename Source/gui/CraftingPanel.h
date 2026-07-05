#ifndef __CRAFTING_PANEL_H__
#define __CRAFTING_PANEL_H__

#include "axmol.h"

namespace opendw
{

class CraftingContainer;

/*
 * CLASS: CraftingPanel : CCNode @ 0x10031E948
 */
class CraftingPanel : public ax::Node
{
public:
    CREATE_FUNC(CraftingPanel);

    /* FUNC: CraftingPanel::load @ 0x100181393 */
    void onEnter() override;

private:
    ax::Label* _categoryLabel;  // CraftingPanel::categoryLabel @ 0x1003153D0
    bool _loaded;
};

}  // namespace opendw

#endif  // __CRAFTING_PANEL_H__
