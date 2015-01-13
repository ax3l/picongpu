/**
 * Copyright 2014 Axel Huebl
 *
 * This file is part of PIConGPU.
 *
 * PIConGPU is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PIConGPU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PIConGPU.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <cuda_runtime.h>

#include <simulation_defines.hpp>
//include <mpi.h>
#include "communication/manager_common.h"

#include <boost/python.hpp>

using namespace PMacc;
using namespace picongpu;

char const* greet()
{
return "hello, world";
}

using namespace boost::python;

BOOST_PYTHON_MODULE(pypicongpu)
{
    def("greet", greet);
    using namespace picongpu::simulation_starter;
    class_<SimStarter>("SimStarter")
          .def("parseConfigs", &SimStarter::load)
          .def("load", &SimStarter::load)
          .def("start", &SimStarter::start)
          .def("unload" &SimStarter::unload)
    ;
};
