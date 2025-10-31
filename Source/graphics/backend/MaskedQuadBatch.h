#ifndef __MASKED_QUAD_BATCH_H__
#define __MASKED_QUAD_BATCH_H__

#include "axmol.h"

#include "graphics/backend/MaskedQuadCommand.h"

namespace opendw
{

/*
 * CLASS: CCTextureAtlasMasked : CCTextureAtlas @ 0x1003180C0
 */
class MaskedQuadBatch : public ax::Object
{
public:
    typedef MaskedQuadCommand::Quad Quad;

    ~MaskedQuadBatch() override;

    static MaskedQuadBatch* createWithCapacity(ssize_t capacity);

    bool initWithCapacity(ssize_t capacity);

    void setQuad(ssize_t index, const Quad& quad);
    void insertQuad(ssize_t index, const Quad& quad);
    void removeQuad(ssize_t index);

    void clear();

    bool setCapacity(ssize_t capacity);
    ssize_t getCapacity() const { return _capacity; }
    ssize_t getTotalQuads() const { return _totalQuads; }

    Quad* getQuads() const { return _quads; }

    void setDirty(bool dirty) { _dirty = dirty; }
    bool isDirty() const { return _dirty; }

private:
    ssize_t _capacity;
    ssize_t _totalQuads;
    Quad* _quads;
    bool _dirty;
};

}  // namespace opendw

#endif  // __MASKED_QUAD_BATCH_H__
