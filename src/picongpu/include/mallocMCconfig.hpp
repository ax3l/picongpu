/* 
 * Copyright 2013-2017 Axel Huebl, Felix Schmitt, Heiko Burau, Rene Widera
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

#pragma once

// include the heap with the arguments given in the config
#include "mallocMC/mallocMC_utils.hpp"

// basic files for mallocMC
#include "mallocMC/mallocMC_overwrites.hpp"
#include "mallocMC/mallocMC_hostclass.hpp"

// Load all available policies for mallocMC
#include "mallocMC/CreationPolicies.hpp"
#include "mallocMC/DistributionPolicies.hpp"
#include "mallocMC/OOMPolicies.hpp"
#include "mallocMC/ReservePoolPolicies.hpp"
#include "mallocMC/AlignmentPolicies.hpp"

// configurate the CreationPolicy "Scatter"
struct ScatterConfig
{
    /* 2MiB page can hold around 256 particle frames */
    typedef boost::mpl::int_<2*1024*1024> pagesize;
    /* accessblocks, regionsize and wastefactor are not finale selected
       and might be performance sensitive*/
    typedef boost::mpl::int_<4> accessblocks;
    typedef boost::mpl::int_<8> regionsize;
    typedef boost::mpl::int_<2> wastefactor;
    /* resetfreedpages is used to minimize memory fracmentation while different
       frame sizes were used*/
    typedef boost::mpl::bool_<true> resetfreedpages;
};

// Define a new allocator and call it ScatterAllocator
// which resembles the behaviour of ScatterAlloc
typedef mallocMC::Allocator<
mallocMC::CreationPolicies::Scatter<ScatterConfig>,
mallocMC::DistributionPolicies::Noop,
mallocMC::OOMPolicies::ReturnNull,
mallocMC::ReservePoolPolicies::SimpleCudaMalloc,
mallocMC::AlignmentPolicies::Shrink<>
> ScatterAllocator;

//use ScatterAllocator to replace malloc/free
MALLOCMC_SET_ALLOCATOR_TYPE( ScatterAllocator );
