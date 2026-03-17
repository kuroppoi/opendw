#ifndef __CHIPMUNK_SPACE_H__
#define __CHIPMUNK_SPACE_H__

#include "chipmunk/chipmunk.h"
#include "axmol.h"

namespace opendw
{

class ChipmunkBody;
class ChipmunkObject;
class ChipmunkShape;

/*
 * CLASS: ChipmunkSpace : NSObject @ 0x100317BE8
 */
class ChipmunkSpace : public ax::Object
{
public:
    typedef std::function<bool(cpArbiter*, ChipmunkSpace*)> CollisionBegin;
    typedef std::function<bool(cpArbiter*, ChipmunkSpace*)> CollisionPreSolve;
    typedef std::function<void(cpArbiter*, ChipmunkSpace*)> CollisionPostSolve;
    typedef std::function<void(cpArbiter*, ChipmunkSpace*)> CollisionSeparate;

    struct CollisionHandler
    {
        ChipmunkSpace* space;
        CollisionBegin beginFunc;
        CollisionPreSolve preSolveFunc;
        CollisionPostSolve postSolveFunc;
        CollisionSeparate separateFunc;
    };

    /* FUNC: ChipmunkSpace::dealloc @ 0x*/
    virtual ~ChipmunkSpace() override;

    CREATE_FUNC(ChipmunkSpace);

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

     /* FUNC: ChipmunkSpace::addCollisionHandler:typeA:typeB:begin:preSolve:postSolve:separate: @ 0x100098FEF */
    void addCollisionHandler(cpCollisionType typeA,
                             cpCollisionType typeB,
                             CollisionBegin begin,
                             CollisionPreSolve preSolve,
                             CollisionPostSolve postSolve,
                             CollisionSeparate separate);

    /* FUNC: ChipmunkSpace::add: @ 0x1000990FA */
    void add(ChipmunkObject* object);

    /* FUNC: ChipmunkSpace::remove: @ 0x1000992F4 */
    void remove(ChipmunkObject* object);

    /* FUNC: ChipmunkSpace::addBody: @ 0x100099FDF */
    void addBody(ChipmunkBody* body);

    /* FUNC: ChipmunkSpace::removeBody: @ 0x10009A03E */
    void removeBody(ChipmunkBody* body);

    /* FUNC: ChipmunkSpace::addShape: @ 0x10009A09D */
    void addShape(ChipmunkShape* shape);

    /* FUNC: ChipmunkSpace::removeShape: @ 0x10009A0FC */
    void removeShape(ChipmunkShape* shape);

    /* FUNC: ChipmunkSpace::bodies @ 0x100099E2E */
    std::vector<ChipmunkBody*> getBodies() const;

    /* FUNC: ChipmunkSpace::space @ 0x100098CC7 */
    cpSpace* getSpace() const { return _space; }

    /* FUNC: ChipmunkSpace::staticBody @ 0x100098E5A */
    ChipmunkBody* getStaticBody() const { return _staticBody; }

private:
    cpSpace* _space;                        // ChipmunkSpace::_space @ 0x1003124F8
    ChipmunkBody* _staticBody;              // ChipmunkSpace::_staticBody @ 0x100312500
    ax::Vector<ChipmunkObject*> _children;  // ChipmunkSpace::_children @ 0x1003124E8
    std::vector<CollisionHandler*> _collisionHandlers;
};

}  // namespace opendw

#endif  // __CHIPMUNK_SPACE_H__
