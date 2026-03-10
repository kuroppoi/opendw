#ifndef __PHYSICS_SPACE_H__
#define __PHYSICS_SPACE_H__

#include "chipmunk/chipmunk.h"
#include "axmol.h"

#include "physics/PhysicsObject.h"  // FIXME: ax::Vector

namespace opendw
{

class PhysicsBody;
class PhysicsShape;

/*
 * CLASS: PhysicsSpace : NSObject @ 0x100317BE8
 */
class PhysicsSpace : public ax::Object
{
public:
    /* FUNC: ChipmunkSpace::dealloc @ 0x*/
    virtual ~PhysicsSpace() override;

    CREATE_FUNC(PhysicsSpace);

    /* FUNC: ChipmunkSpace::initWithSpace: @ 0x100098AF1 */
    bool initWithSpace(cpSpace* space);

    /* FUNC: ChipmunkSpace::init @ 0x100098C07 */
    bool init();

    /* FUNC: ChipmunkSpace::step: @ 0x100099FCA */
    void update(float deltaTime);

    /* FUNC: ChipmunkSpace::freeSpace @ 0x100098C2E */
    void freeSpace();

    /* FUNC: ChipmunkSpace::setGravity: @ 0x100098D19 */
    void setGravity(const ax::Vec2& gravity);

    /* FUNC: ChipmunkSpace::add: @ 0x1000990FA */
    void add(PhysicsObject* object);

    /* FUNC: ChipmunkSpace::remove: @ 0x1000992F4 */
    void remove(PhysicsObject* object);

    /* FUNC: ChipmunkSpace::addBody: @ 0x100099FDF */
    void addBody(PhysicsBody* body);

    /* FUNC: ChipmunkSpace::removeBody: @ 0x10009A03E */
    void removeBody(PhysicsBody* body);

    /* FUNC: ChipmunkSpace::addShape: @ 0x10009A09D */
    void addShape(PhysicsShape* shape);

    /* FUNC: ChipmunkSpace::removeShape: @ 0x10009A0FC */
    void removeShape(PhysicsShape* shape);

    /* FUNC: ChipmunkSpace::space @ 0x100098CC7 */
    cpSpace* getSpace() const { return _space; }

    /* FUNC: ChipmunkSpace::staticBody @ 0x100098E5A */
    PhysicsBody* getStaticBody() const { return _staticBody; }

private:
    cpSpace* _space;                       // ChipmunkSpace::_space @ 0x1003124F8
    PhysicsBody* _staticBody;              // ChipmunkSpace::_staticBody @ 0x100312500
    ax::Vector<PhysicsObject*> _children;  // ChipmunkSpace::_children @ 0x1003124E8
};

}  // namespace opendw

#endif  // __PHYSICS_SPACE_H__
