#ifndef FMV_VIEW_H
#define FMV_VIEW_H

#include <Corrade/Utility/VisibilityMacros.h>

namespace FMV {
    class FrequencyProvider;

    CORRADE_VISIBILITY_EXPORT int runViewLoop(int argc, char **argv, FrequencyProvider &);
}

#endif
