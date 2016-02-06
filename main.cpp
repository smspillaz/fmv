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

#include <memory>

#include <libvisual/libvisual.h>

#include "frequencyprovider.h"
#include "lvfrequencyprovider.h"
#include "view.h"

int main(int argc, char **argv)
{
    LV::System::init(argc, argv);
    std::shared_ptr<FMV::FrequencyProvider> provider(FMV::createLVFrequencyProvider("mplayer"));
    int code = FMV::runViewLoop(argc, argv, {800, 600}, *provider);
    LV::System::destroy();

    return code;
}
