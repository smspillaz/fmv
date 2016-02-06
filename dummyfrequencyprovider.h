#ifndef FMV_DUMMYFREQUENCYPROVIDER_H
#define FMV_DUMMYFREQUENCYPROVIDER_H

#include <memory>

#include <Corrade/Utility/VisibilityMacros.h>

#include "frequencyprovider.h"

namespace FMV {

std::unique_ptr<FrequencyProvider>
CORRADE_VISIBILITY_EXPORT createDummyFrequencyProvider();

}

#endif
