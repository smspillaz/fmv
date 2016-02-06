#ifndef FMV_LVFREQUENCYPROVIDER_H
#define FMV_LVFREQUENCYPROIVDER_H

#include <memory>

#include <Corrade/Utility/VisibilityMacros.h>

#include "frequencyprovider.h"

namespace FMV {
    CORRADE_VISIBILITY_EXPORT std::unique_ptr<FrequencyProvider>
    createLVFrequencyProvider(std::string const &audioBackend);
}

#endif
