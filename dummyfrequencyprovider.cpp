#include <stdlib.h>

#include <cmath>

#include <array>

#include <functional>

#include <memory>

#include "frequencyprovider.h"
#include "dummyfrequencyprovider.h"

namespace {

class DummyFrequencyProvider :
    public FMV::FrequencyProvider
{
public:

    typedef std::function<void(float *)> Callback;

    void workWithCurrentFrequencies(Callback const &);
};

void
DummyFrequencyProvider::workWithCurrentFrequencies(Callback const &cb) {
    std::array<float, 255> frequencies;

    for (size_t i = 0; i < frequencies.size(); ++i) {
        frequencies[i] = (rand() % 1000) / 3000.0f;
    }

    cb(frequencies.data());
}

}

std::unique_ptr<FMV::FrequencyProvider>
FMV::createDummyFrequencyProvider() {
    return std::unique_ptr<FMV::FrequencyProvider>(new DummyFrequencyProvider());
}
