#ifndef __PHYSICAL_H__
#define __PHYSICAL_H__

#include "physics/ChipmunkObject.h"

namespace opendw
{

class ChipmunkBaseObject;
class ChipmunkBody;
class ChipmunkPolyShape;
class ChipmunkShape;
class ChipmunkSpace;

/*
 * CLASS: Physical : NSObject @ 0x1003175F8
 */
class Physical : public ChipmunkObject
{
public:
    typedef std::vector<std::vector<ax::Point>> PhysicsDefinition;

    /* FUNC: Physical::dealloc @ 0x10007BCEE */
    virtual ~Physical() override;

    /* FUNC: Physical::physicalWithTarget: @ 0x10007A670 */
    static Physical* createWithTarget(void* target);

    /* FUNC: Physical::setSpace: @ 0x10007A656 */
    static void setSpace(ChipmunkSpace* space) { sGlobalSpace = space; }

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
    ChipmunkPolyShape* createBoxShape(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::setShapeAsBox:offset: @ 0x10007B13C */
    void setShapeAsBox(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::setShapeFromDefinition:size:offset:rotation:flipped: @ 0x10007B274 */
    void setShapeFromDefinition(const PhysicsDefinition& definition,
                                const ax::Size& size,
                                const ax::Point& offset,
                                float rotation,
                                bool flipped);

    /* FUNC: Physical::bindInternalShape: @ 0x10007B8D6 */
    void bindInternalShape(ChipmunkShape* shape);

    /* FUNC: Physical::clearShapes @ 0x10007B92C */
    void clearShapes();

    /* FUNC: Physical::updateChipmunkObjects @ 0x10007B9CD */
    void updateChipmunkObjects();

    /* FUNC: Physical::chipmunkObjects @ 0x10007BDBC */
    const ax::Vector<ChipmunkBaseObject*> getChipmunkObjects() override { return _chipmunkObjects; }

private:
    inline static ChipmunkSpace* sGlobalSpace;  // 0x10032EAE8

    void* _target;                                     // Physical::target @ 0x100311D50
    ChipmunkBody* _body;                               // Physical::body @ 0x100311D68
    ax::Vector<ChipmunkShape*> _internalShapes;        // Physical::internalShapes @ 0x100311D58
    ax::Vector<ChipmunkBaseObject*> _chipmunkObjects;  // Physical::chipmunkObjects @ 0x100311D70
};

}  // namespace opendw

#endif  // __PHYSICAL_H__
