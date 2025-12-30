#ifndef __ENTITY_FRAMED_H__
#define __ENTITY_FRAMED_H__

#include "entity/Entity.h"

namespace opendw
{

/*
 * CLASS: EntityFramed : Entity @ 0x10031D8B8
 */
class EntityFramed : public Entity
{
public:
    /* FUNC: EntityFramed::buildGraphics: @ 0x10016B19A */
    void buildGraphics() override;

    /* FUNC: EntityFramed::finishGraphics @ 0x10016BBA2 */
    void finishGraphics() override;
};

}  // namespace opendw

#endif  // __ENTITY_FRAMED_H__
