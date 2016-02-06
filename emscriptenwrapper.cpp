#include <cmath>

#include <array>

#include <memory>

#include "view.h"
#include "frequencyprovider.h"

#include <emscripten.h>

#include <SDL/SDL.h>
#include <cstdio>

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

    std::array<float, 255> ModifiableFrequencyProvider::frequencyData;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int run_fmv_with_frequency_provider(int width, int height) {
        ModifiableFrequencyProvider::frequencyData.fill(0.0);
        ModifiableFrequencyProvider provider;
        return FMV::runViewLoop(0, nullptr, {width, height}, provider);
    }
    
    EMSCRIPTEN_KEEPALIVE
    void set_frequencies(float *frequencies, float minDB, float range) {
        printf("%f  - %f\n", range, minDB);
        for (size_t i = 0; i < ModifiableFrequencyProvider::frequencyData.size(); ++i) {
            ModifiableFrequencyProvider::frequencyData[i] = (frequencies[i] - minDB) / (range * 2);
        }
    }
}

