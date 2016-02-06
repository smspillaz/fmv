#include <memory>

#include "view.h"
#include "dummyfrequencyprovider.h"
#include "frequencyprovider.h"

#include <emscripten.h>

#include <SDL/SDL.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int run_fmv_with_frequency_provider(int width, int height) {
        std::shared_ptr<FMV::FrequencyProvider> provider(FMV::createDummyFrequencyProvider());
        return FMV::runViewLoop(0, nullptr, {width, height}, *provider);
    }
}

