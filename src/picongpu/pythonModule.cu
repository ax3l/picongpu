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

#include <Python.h>
#include <mpi.h>

#include <simulation_defines.hpp>
#include "communication/manager_common.h"

#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/stl_iterator.hpp>

#include <vector>
#include <iterator>
#include <string>
#include <iostream>

char const* greet()
{
return "hello, world";
}

using namespace boost::python;

class WrappedSimStarter : public picongpu::simulation_starter::SimStarter
{
  public:
    WrappedSimStarter( PyObject* self ) : m_self(self)
    {}

    void parse( list l )
    {
        int argc = len(l);
        stl_input_iterator<std::string> it_l(l), end_l;

        std::vector<char> vargv;
        std::vector<size_t> len;
        for( ; it_l != end_l; ++it_l )
        {
            std::cout << "l: " << it_l->c_str() << std::endl;

            // store len incl. NULL terminator
            len.push_back( it_l->size() + 1 );

            for( size_t i = 0; i < it_l->size(); ++i )
                vargv.push_back( it_l->c_str()[i] );

            // add NULL
            vargv.push_back( '\0' );
        }

        char** ptr = new char*[argc];
        int pos = 0;
        for( int i = 0; i < argc; ++i )
        {
            ptr[i] = &vargv.at(pos);
            pos += len.at(i);
        }

        picongpu::simulation_starter::SimStarter::parseConfigs(argc, ptr);
        delete [] ptr;
    }

  private:
    PyObject* const m_self;
};

BOOST_PYTHON_MODULE(pypicongpu)
{
    def("greet", greet);
    using namespace picongpu::simulation_starter;

    class_<SimStarter, WrappedSimStarter, boost::noncopyable >("SimStarter")
        .def("parseConfigs", &WrappedSimStarter::parse)
        .def("load", &SimStarter::load)
        .def("unload", &SimStarter::unload)
        .def("start", &SimStarter::start)
    ;
};
