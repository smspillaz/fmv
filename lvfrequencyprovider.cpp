/*
    Copyright © 2016 Sam Spilsbury <s@polysquare.org>
    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>

#include <functional>

#include <memory>

#include <libvisual/libvisual.h>
#include <libvisual/lv_common.h>
#include <libvisual/lv_input.h>
#include <libvisual/lv_buffer.h>

#include "lvfrequencyprovider.h"

namespace FMV {

class LVFrequencyProvider :
    public FrequencyProvider
{
private:

    LV::InputPtr visualizerInput;

public:

    LVFrequencyProvider(std::string const &inputPlugin) :
        visualizerInput(LV::Input::load(inputPlugin))
    {
        visualizerInput->realize();
    }

    void workWithCurrentFrequencies(Callback const &callback) override {
        visualizerInput->run();
        auto const &audio = visualizerInput->get_audio();
        auto pcm_buffer = LV::Buffer::create(512 * sizeof(float));
        auto freq_buffer = LV::Buffer::create(256 * sizeof(float));
        const_cast<LV::Audio *>(&audio)->get_sample_mixed_simple(pcm_buffer,
                                                                 2,
                                                                 VISUAL_AUDIO_CHANNEL_LEFT,
                                                                 VISUAL_AUDIO_CHANNEL_RIGHT,
                                                                 nullptr);
        LV::Audio::get_spectrum_for_sample(freq_buffer, pcm_buffer, true);

        callback(static_cast<float *>(freq_buffer->get_data()));
    }
};

std::unique_ptr<FrequencyProvider>
createLVFrequencyProvider(std::string const &audioBackend) {
    return std::unique_ptr<FrequencyProvider>(new LVFrequencyProvider(audioBackend));
}

}
