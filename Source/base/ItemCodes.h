#ifndef __ITEM_CODES_H__
#define __ITEM_CODES_H__

#include <stdint.h>

namespace opendw::item_codes
{

enum : uint16_t
{
    BASE_EARTH          = 2,
    BASE_LIMESTONE      = 4,
    BASE_MAW            = 5,
    BASE_PIPE           = 6,
    PLUGGED_MAW         = 7,
    PLUGGED_PIPE        = 8,
    BASE_WATER          = 9,
    BASE_EARTH_ACCENT   = 15,
    LIQUID_WATER        = 192,
    RUBY_PLAQUE         = 399,
    EARTH               = 512,
    EARTH_DUG           = 519,
    GLASS               = 599,
    BALLOON             = 607,
    BALLOON_STRIPED     = 678,
    DAGUERREOTYPE_SMALL = 754,
    DAGUERREOTYPE_LARGE = 755,
    GIANT_CLOCK         = 761,
    LANDSCAPE           = 797,
    MIXING_BARREL       = 811,
    MECHANICAL_PIPE     = 860,
    WINE_PRESS          = 863,
    GECK_TUB            = 880,
    GECK_COG_LARGE      = 886,
    GECK_COG_SMALL      = 887,
    COMPOSTER_CHAMBER   = 894,
    COMPOSTER_TURBINE   = 899,
    LANDMARK_PLAQUE     = 916,
    MECHANICAL_SIGN     = 919,
    RECYCLER_CHAMBER    = 927,
    RECYCLER_GEAR       = 929,
    EXPIATOR_FACE       = 1003,
    EXPIATOR_GEAR       = 1006,
    HELL_DISH           = 1010
};

}  // namespace opendw::item_codes

#endif  // __ITEM_CODES_H__
