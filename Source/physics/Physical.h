#ifndef __PHYSICAL_H__
#define __PHYSICAL_H__

#include "physics/PhysicsObject.h"

namespace opendw
{

class PhysicsBaseObject;
class PhysicsBody;
class PhysicsPolyShape;
class PhysicsShape;
class PhysicsSpace;

/*
 * CLASS: Physical : NSObject @ 0x1003175F8
 */
class Physical : public PhysicsObject
{
public:
    typedef std::vector<std::vector<ax::Point>> PhysicsDefinition;

    /* FUNC: Physical::dealloc @ 0x10007BCEE */
    virtual ~Physical() override;

    /* FUNC: Physical::physicalWithTarget: @ 0x10007A670 */
    static Physical* createWithTarget(void* target);

    /* FUNC: Physical::setSpace: @ 0x10007A656 */
    static void setSpace(PhysicsSpace* space) { sGlobalSpace = space; }

    /* FUNC: Physical::initWithTarget: @ 0x10007A6B5 */
    bool initWithTarget(void* target);

    /* FUNC: Physical::addBody @ 0x10007A777 */
    void addBody();

    /* FUNC: Physical::useStaticBody @ 0x10007A80F */
    void useStaticBody();

    /* FUNC: Physical::setLayer: @ 0x10007A8AA */
    void setLayer(uint32_t layer);

    /* FUNC: Physical::setGroup: @ 0x10007AA1D */
    void setGroup(void* group);

    /* FUNC: Physical::shapeBox:offset: @ 0x10007AFDE */
    PhysicsPolyShape* createBoxShape(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::setShapeAsBox:offset: @ 0x10007B13C */
    void setShapeAsBox(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::setShapeFromDefinition:size:offset:rotation:flipped: @ 0x10007B274 */
    void setShapeFromDefinition(const PhysicsDefinition& definition,
                                const ax::Size& size,
                                const ax::Point& offset,
                                float rotation,
                                bool flipped);

    /* FUNC: Physical::bindInternalShape: @ 0x10007B8D6 */
    void bindInternalShape(PhysicsShape* shape);

    /* FUNC: Physical::clearShapes @ 0x10007B92C */
    void clearShapes();

    /* FUNC: Physical::updateChipmunkObjects @ 0x10007B9CD */
    void updatePhysicsObjects();

    /* FUNC: Physical::chipmunkObjects @ 0x10007BDBC */
    const ax::Vector<PhysicsBaseObject*> getPhysicsObjects() override { return _physicsObjects; }

private:
    inline static PhysicsSpace* sGlobalSpace;  // 0x10032EAE8

    void* _target;                                   // Physical::target @ 0x100311D50
    PhysicsBody* _body;                              // Physical::body @ 0x100311D68
    ax::Vector<PhysicsShape*> _internalShapes;       // Physical::internalShapes @ 0x100311D58
    ax::Vector<PhysicsBaseObject*> _physicsObjects;  // Physical::chipmunkObjects @ 0x100311D70
};

}  // namespace opendw

#endif  // __PHYSICAL_H__
