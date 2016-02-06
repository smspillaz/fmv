#ifndef FMV_FREQUENCYPROVIDER_H
#define FMV_FREQUENCYPROVIDER_H

#include <functional>

namespace FMV {

class FrequencyProvider {
public:
    typedef std::function<void(float *)> Callback;

    virtual ~FrequencyProvider() = 0;
    virtual void workWithCurrentFrequencies(Callback const &) = 0;
};

}

#endif
