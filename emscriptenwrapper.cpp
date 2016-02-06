#include <array>

#include <memory>

#include "view.h"
#include "frequencyprovider.h"

#include <emscripten.h>

#include <SDL/SDL.h>

namespace {
    class ModifiableFrequencyProvider :
        public FMV::FrequencyProvider
    {
        public:
            static std::array<float, 255> frequencyData;

        public:
            void workWithCurrentFrequencies(Callback const &cb) override {
                cb(ModifiableFrequencyProvider::frequencyData.data());
            }
    };
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int run_fmv_with_frequency_provider(int width, int height) {
        ModifiableFrequencyProvider::frequencyData.fill(0.0);
        ModifiableFrequencyProvider provider;
        return FMV::runViewLoop(0, nullptr, {width, height}, provider);
    }
    
    EMSCRIPTEN_KEEPALIVE
    void set_frequencies(float *frequencies, size_t length) {
        for (size_t i = 0; i < length; ++i) {
            ModifiableFrequencyProvider::frequencyData[i] = frequencies[i];
        }
    }
}

