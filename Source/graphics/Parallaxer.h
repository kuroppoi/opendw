#ifndef __PARALLAXER_H__
#define __PARALLAXER_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: Parallaxer : CCNode @ 0x10031CEB8
 */
class Parallaxer : public ax::Node
{
public:
    ~Parallaxer() override;

    virtual bool init() override;

    /* FUNC: Parallaxer::onEnter @ 0x10014FFE8 */
    void onEnter() override;

    /* FUNC: Parallaxer::step: @ 0x100150215 */
    void update(float deltaTime) override;
    void updateChildrenInNode(ax::Node* node, float deltaTime);

    /* FUNC: Parallaxer::rebuild @ 0x1001506B1 */
    virtual void rebuild() { clear(); };

    /* FUNC: Parallaxer::clear @ 0x1001507F4 */
    virtual void clear() {};

    /* FUNC: Parallaxer::stepChildren: @ 0x100150237 */
    virtual void updateChildren(float deltaTime);

    /* FUNC: Parallaxer::updateColors: @ 0x1001506B7 */
    virtual void updateColors(float deltaTime) {}

    /* FUNC: Parallaxer::screenSizeDidChange: @ 0x1001506BD */
    void onWindowResized();

    void setViewPosition(const ax::Point& position) { _viewPosition = position; }
    const ax::Point& getViewPosition() const { return _viewPosition; }

    void setViewScale(float scale) { _viewScale = scale; }
    float getViewScale() const { return _viewScale; }

protected:
    ax::LayerGradient* _gradient;  // Parallaxer::gradient @ 0x1003149B8
    ax::EventListener* _windowListener;
    ax::Point _viewPosition;
    float _viewScale = 1.0F;
};

}  // namespace opendw

#endif  // __PARALLAXER_H__
