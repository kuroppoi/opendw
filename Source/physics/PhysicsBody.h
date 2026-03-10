#ifndef __PHYSICS_BODY_H__
#define __PHYSICS_BODY_H__

#include "chipmunk/chipmunk_structs.h"

#include "physics/PhysicsObject.h"

namespace opendw
{

class PhysicsSpace;

/*
 * CLASS: ChipmunkBody : NSObject @ 0x1003172D8
 */
class PhysicsBody : public PhysicsBaseObject
{
public:
    /* FUNC: ChipmunkBody::dealloc @ 0x10006FB09 */
    virtual ~PhysicsBody() override;

    /* FUNC: ChipmunkBody::bodyWithMass:andMoment: @ 0x10006F915 */
    static PhysicsBody* createWithMass(float mass, float moment);

    /* FUNC: ChipmunkBody::initWithMass:andMoment: @ 0x10006FA09 */
    bool initWithMass(float mass, float moment);

    /* FUNC: ChipmunkBody::addToSpace: @ 0x10006FE7C */
    void addToSpace(PhysicsSpace* space) override;

    /* FUNC: ChipmunkBody::removeFromSpace: @ 0x10006FE97 */
    void removeFromSpace(PhysicsSpace* space) override;

    /* FUNC: ChipmunkBody::setType: @ 0x10006FBC1 */
    void setType(cpBodyType type);

    /* FUNC: ChipmunkBody::type @ 0x10006FBB0 */
    cpBodyType getType() const;

    /* FUNC: ChipmunkBody::setMass: @ 0x10006FBE5 */
    void setMass(float mass);

    /* FUNC: ChipmunkBody::mass @ 0x10006FDB4 */
    float getMass() const;

    /* FUNC: ChipmunkBody::setMoment: @ 0x10006FC07 */
    void setMoment(float moment);

    /* FUNC: ChipmunkBody::moment @ 0x10006FBF6 */
    float getMoment() const;

    /* FUNC: ChipmunkBody::body @ 0x10006FBA0 */
    cpBody* getBody() const { return (cpBody*)&_body; }

    /* FUNC: ChipmunkBody::setUserData: @ 0x10006FFA8 */
    void setUserData(void* userData) { _userData = userData; }

    /* FUNC: ChipmunkBody::userData @ 0x10006FF97 */
    void* getUserData() const { return _userData; }

private:
    cpBody _body;     // ChipmunkBody::_body @ 0x100311B68
    void* _userData;  // ChipmunkBody::_userData @ 0x100311B70
};

}  // namespace opendw

#endif  // __PHYSICS_BODY_H__
