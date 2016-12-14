/**
 * Copyright 2015-2016 Erik Zenker, Alexander Grund
 *
 * This file is part of libPMacc.
 *
 * libPMacc is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libPMacc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with libPMacc.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "PMaccFixture.hpp"

// STL
#include <stdint.h> /* uint8_t */
#include <iostream> /* cout, endl */
#include <string>

// BOOST
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/int.hpp>

// MPI
#include <mpi.h> /* MPI_Init, MPI_Finalize */

// PMacc
#include <Environment.hpp>
#include <dimensions/DataSpace.hpp>
#include <memory/buffers/HostBufferIntern.hpp>
#include <memory/buffers/HostBuffer.hpp>
#include <memory/buffers/DeviceBufferIntern.hpp>
#include <memory/buffers/DeviceBuffer.hpp>
#include <dimensions/DataSpace.hpp>
#include "pmacc_types.hpp" /* DIM1,DIM2,DIM3 */


/*******************************************************************************
 * Configuration
 ******************************************************************************/


/**
 * Defines for which numbers of elements a
 * test should be verfied e.g. the size
 * of a host or device buffer.
 */
template<typename T_Dim>
std::vector<size_t> getElementsPerDim(){
    std::vector<size_t> nElements;
    std::vector<size_t> nElementsPerDim;

    // Elements total
    nElements.push_back(1);
    nElements.push_back(1 * 1000);
    nElements.push_back(1 * 1000 * 1000);
    nElements.push_back(1 * 1000 * 1000 * 10);

    // Elements per dimension
    for(size_t i = 0; i < nElements.size(); ++i){
        nElementsPerDim.push_back(std::pow(nElements[i], static_cast<double>(1)/static_cast<double>(T_Dim::value)));

    }
    return nElementsPerDim;
}


/**
 * Definition of a list of dimension types. This
 * List is used to test memory operations in
 * each dimension setup automatically. For this
 * purpose boost::mpl::for_each is used.
 */
typedef ::boost::mpl::list<boost::mpl::int_<DIM1>,
                           boost::mpl::int_<DIM2>,
                           boost::mpl::int_<DIM3> > Dims;


/*******************************************************************************
 * Test Suites
 ******************************************************************************/
typedef PMaccFixture<TEST_DIM> MyPMaccFixture;
BOOST_GLOBAL_FIXTURE(MyPMaccFixture);

BOOST_AUTO_TEST_SUITE( memory )

  BOOST_AUTO_TEST_SUITE( HostBufferIntern )
  #include "HostBufferIntern/copyFrom.hpp"
  #include "HostBufferIntern/reset.hpp"
  #include "HostBufferIntern/setValue.hpp"
  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
