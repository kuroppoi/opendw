#ifndef __PHYSICAL_H__
#define __PHYSICAL_H__

#include "chipmunk/chipmunk_types.h"

#include "physics/ChipmunkObject.h"

namespace opendw
{

class ChipmunkBaseObject;
class ChipmunkBody;
class ChipmunkPolyShape;
class ChipmunkShape;
class ChipmunkSpace;

enum class CollisionType : cpCollisionType
{
    NONE,
    ENTITY
};

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
    static Physical* createWithTarget(ax::Object* target);

    /* FUNC: Physical::setSpace: @ 0x10007A656 */
    static void setSpace(ChipmunkSpace* space) { sGlobalSpace = space; }

    /* FUNC: Physical::initWithTarget: @ 0x10007A6B5 */
    bool initWithTarget(ax::Object* target);

    /* FUNC: Physical::addBody @ 0x10007A777 */
    void addBody();

    /* FUNC: Physical::useStaticBody @ 0x10007A80F */
    void useStaticBody();

    /* FUNC: Physical::addToSpace @ 0x10007B949 */
    void addToSpace();

    /* FUNC: Physical::setLayer: @ 0x10007A8AA */
    void setLayer(uint32_t layer);

    /* FUNC: Physical::setGroup: @ 0x10007AA1D */
    void setGroup(void* group);

    /* FUNC: Physical::setCollisionType: @ 0x10007ABAA */
    void setCollisionType(CollisionType type);

    /* FUNC: Physical::setPosition: @ 0x10007AD1D */
    void setPosition(const ax::Point& position);

    /* FUNC: Physical::position @ 0x10007AE6C */
    ax::Point getPosition() const;

    /* FUNC: Physical::setVelocity: @ 0x10007ACCB */
    void setVelocity(const ax::Vec2& velocity);

    /* FUNC: Physical::velocity @ 0x10007AE89 */
    ax::Vec2 getVelocity() const;

    /* FUNC: Physical::setShapeAsCircle:offset: @ 0x10007AEA6 */
    void setShapeAsCircle(float radius, const ax::Point& offset);

    /* FUNC: Physical::shapeBox:offset: @ 0x10007AFDE */
    ChipmunkPolyShape* createBoxShape(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::setShapeAsBox:offset: @ 0x10007B13C */
    void setShapeAsBox(const ax::Size& size, const ax::Point& offset);

    /* FUNC: Physical::addShapeAsBox:offset: @ 0x10007B228 */
    void addBoxShape(const ax::Size& size, const ax::Point& offset);

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

    /* FUNC: Physical::body @ 0x10007BDCD */
    ChipmunkBody* getBody() const { return _body; }

    /* FUNC: Physical::setRotates: @ 0x10007BE0C */
    void setRotates(bool rotates) { _rotates = rotates; }

private:
    inline static ChipmunkSpace* sGlobalSpace;  // 0x10032EAE8

    ax::Object* _target;                               // Physical::target @ 0x100311D50
    ChipmunkBody* _body;                               // Physical::body @ 0x100311D68
    ax::Vector<ChipmunkShape*> _internalShapes;        // Physical::internalShapes @ 0x100311D58
    ax::Vector<ChipmunkBaseObject*> _chipmunkObjects;  // Physical::chipmunkObjects @ 0x100311D70
    bool _rotates;                                     // Physical::rotates @ 0x100311D60
};

}  // namespace opendw

#endif  // __PHYSICAL_H__
