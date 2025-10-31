#include "MaskedQuadBatch.h"

#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

MaskedQuadBatch::~MaskedQuadBatch()
{
    AX_SAFE_FREE(_quads);
}

MaskedQuadBatch* MaskedQuadBatch::createWithCapacity(ssize_t capacity)
{
    CREATE_INIT(MaskedQuadBatch, initWithCapacity, capacity);
}

bool MaskedQuadBatch::initWithCapacity(ssize_t capacity)
{
    AXASSERT(!_quads, "Reinitialization is not allowed");
    setCapacity(capacity);
    return true;
}

void MaskedQuadBatch::setQuad(ssize_t index, const Quad& quad)
{
    AXASSERT(index >= 0 && index < _capacity, "Index is out of bounds");
    _totalQuads = MAX(_totalQuads, index + 1);
    _quads[index] = quad;
    _dirty        = true;
}

void MaskedQuadBatch::insertQuad(ssize_t index, const Quad& quad)
{
    AXASSERT(index >= 0 && index < _capacity, "Index is out of bounds");
    AXASSERT(_totalQuads < _capacity, "Capacity has been reached");
    auto quadsToMove = _totalQuads - index;

    if (quadsToMove)
    {
        memmove(&_quads[index + 1], &_quads[index], sizeof(Quad) * quadsToMove);
    }

    _totalQuads++;
    _dirty = true;
}

void MaskedQuadBatch::removeQuad(ssize_t index)
{
    AXASSERT(index >= 0 && index < _totalQuads, "Index is out of bounds");
    auto quadsToMove = _totalQuads - index - 1;

    if (quadsToMove)
    {
        memmove(&_quads[index], &_quads[index + 1], sizeof(Quad) * quadsToMove);
    }

    _totalQuads--;
    _dirty = true;
}

void MaskedQuadBatch::clear()
{
    memset(_quads, 0, _capacity * sizeof(Quad));
    _totalQuads = 0;
    _dirty      = true;
}

bool MaskedQuadBatch::setCapacity(ssize_t capacity)
{
    AXASSERT(capacity >= 0, "Capacity can't be negative");

    if (capacity == _capacity)
    {
        return true;
    }

    Quad* quads    = nullptr;
    auto allocSize = capacity * sizeof(Quad);

    if (!_quads)
    {
        quads = reinterpret_cast<Quad*>(malloc(allocSize));

        if (quads)
        {
            memset(quads, 0, allocSize);
        }
    }
    else
    {
        quads  = reinterpret_cast<Quad*>(realloc(_quads, allocSize));
        _quads = nullptr;

        if (quads && capacity > _capacity)
        {
            memset(quads + _capacity, 0, (capacity - _capacity) * sizeof(Quad));
        }
    }

    if (!quads)
    {
        AXLOGE("[MaskedQuadBatch] Allocation for {} capacity failed", capacity);
        AX_SAFE_FREE(quads);
        AX_SAFE_FREE(_quads);
        _capacity   = 0;
        _totalQuads = 0;
    }

    _quads      = quads;
    _capacity   = capacity;
    _totalQuads = MIN(_totalQuads, _capacity);
    _dirty      = true;
    return true;
}

}  // namespace opendw
