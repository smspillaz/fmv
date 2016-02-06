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
    frequencies.fill(0.5);

    cb(frequencies.data());
}

}

std::unique_ptr<FMV::FrequencyProvider>
FMV::createDummyFrequencyProvider() {
    return std::unique_ptr<FMV::FrequencyProvider>(new DummyFrequencyProvider());
}
