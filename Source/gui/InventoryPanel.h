#ifndef __INVENTORY_PANEL_H__
#define __INVENTORY_PANEL_H__

#include "axmol.h"

namespace opendw
{

class ItemContainer;

/*
 * CLASS: InventoryPanel : CCNode @ 0x10031E8F8
 */
class InventoryPanel : public ax::Node
{
public:
    CREATE_FUNC(InventoryPanel);

    /* FUNC: InventoryPanel::load @ 0x100180753 */
    void onEnter() override;

private:
    ax::Label* _categoryLabel;
    bool _loaded;
};

}  // namespace opendw

#endif  // __INVENTORY_PANEL_H__
