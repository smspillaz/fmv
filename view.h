#ifndef FMV_VIEW_H
#define FMV_VIEW_H

#include <Corrade/Utility/VisibilityMacros.h>
#include <Magnum/Math/Vector2.h>

namespace FMV {
    class FrequencyProvider;

    CORRADE_VISIBILITY_EXPORT int runViewLoop(int argc,
                                              char **argv,
                                              Magnum::Math::Vector2<Magnum::Int> const &,
                                              FrequencyProvider &);
}

#endif
