/**
 * Copyright 2013-2016 Axel Huebl, Heiko Burau, Rene Widera
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

#include "DirSplitting.def"

#include "simulation_defines.hpp"

#include <fields/MaxwellSolver/DirSplitting/DirSplitting.kernel>
#include <math/vector/Int.hpp>
#include <dataManagement/DataConnector.hpp>
#include <fields/FieldB.hpp>
#include <fields/FieldE.hpp>
#include "math/Vector.hpp"
#include <cuSTL/algorithm/kernel/ForeachBlock.hpp>
#include <lambda/Expression.hpp>
#include <cuSTL/cursor/NestedCursor.hpp>
#include <math/vector/TwistComponents.hpp>
#include <math/vector/compile-time/TwistComponents.hpp>
#include <cuSTL/cursor/navigator/compile-time/TwistAxesNavigator.hpp>
#include <cuSTL/cursor/accessor/TwistAxesAccessor.hpp>

namespace picongpu
{
namespace dirSplitting
{
using namespace PMacc;

/** Check Directional Splitting grid and time conditions
 *
 * This is a workaround that the condition check is only
 * triggered if the current used solver is `DirSplitting`
 */
template<typename T_UsedSolver, typename T_Dummy=void>
struct ConditionCheck
{
};

template<typename T_Dummy>
struct ConditionCheck<DirSplitting, T_Dummy>
{
    /* Directional Splitting conditions:
     *
     * using SI units to avoid round off errors
     */
    PMACC_CASSERT_MSG(DirectionSplitting_Set_dX_equal_dt_times_c____check_your_gridConfig_param_file,
                      (SI::SPEED_OF_LIGHT_SI * SI::DELTA_T_SI) == SI::CELL_WIDTH_SI);
    PMACC_CASSERT_MSG(DirectionSplitting_use_cubic_cells____check_your_gridConfig_param_file,
                      SI::CELL_HEIGHT_SI == SI::CELL_WIDTH_SI);
#if (SIMDIM == DIM3)
    PMACC_CASSERT_MSG(DirectionSplitting_use_cubic_cells____check_your_gridConfig_param_file,
                      SI::CELL_DEPTH_SI == SI::CELL_WIDTH_SI);
#endif
};

namespace result_of
{

template<typename T_SpaceTwist, typename T_ComponentTwist, typename T_Cursor>
struct twistVectorForDirSplitting
{
    typedef PMacc::cursor::Cursor<PMacc::cursor::TwistAxesAccessor<T_Cursor, T_ComponentTwist>,
                   PMacc::cursor::CT::TwistAxesNavigator<T_SpaceTwist>,
                   T_Cursor> type;
};

} // result_of

template<typename T_SpaceTwist, typename T_ComponentTwist, typename T_Cursor>
HDINLINE
typename result_of::twistVectorForDirSplitting<T_SpaceTwist, T_ComponentTwist, T_Cursor>::type
twistVectorForDirSplitting(const T_Cursor& cursor)
{
    return typename result_of::twistVectorForDirSplitting<T_SpaceTwist, T_ComponentTwist, T_Cursor>::type(
        PMacc::cursor::TwistAxesAccessor<T_Cursor, T_ComponentTwist>(),
        PMacc::cursor::CT::TwistAxesNavigator<T_SpaceTwist>(),
        cursor);
}

class DirSplitting : private ConditionCheck<fieldSolver::FieldSolver>
{
private:
    template<typename SpaceTwist, typename OrientationTwist,typename JSpaceTwist,typename CursorE, typename CursorB, typename CursorJ, typename GridSize>
    void propagate(CursorE cursorE, CursorB cursorB,CursorJ cursorJ,CursorE old_cursorE, CursorB old_cursorB, GridSize gridSize) const
    {
        using namespace cursor::tools;
        using namespace PMacc::math;

        typedef typename CT::shrinkTo<SpaceTwist,simDim>::type SpaceTwistSimDim;
        PMACC_AUTO(gridSizeTwisted, twistComponents<
            SpaceTwistSimDim
        >(gridSize));

        /* twist components of the supercell */
        typedef typename PMacc::math::CT::make_Vector<simDim,bmpl::integral_c<int,0> >::type ZeroVector;

        typedef typename PMacc::math::CT::AssignIfInRange<
            ZeroVector,
            bmpl::integral_c<int,0>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::x>::type
        >::type VectorWith_X;

        typedef typename PMacc::math::CT::AssignIfInRange<
            VectorWith_X,
            bmpl::integral_c<int,1>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::y>::type
        >::type VectorWith_XY;

        typedef typename PMacc::math::CT::AssignIfInRange<
            VectorWith_XY,
            bmpl::integral_c<int,2>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::z>::type
        >::type BlockDim;


        PMacc::math::Size_t<simDim> zoneSize(gridSizeTwisted);
        zoneSize.x()=BlockDim::x::value;

        algorithm::kernel::ForeachBlock<BlockDim> foreach;
        foreach(zone::SphericZone<simDim>(zoneSize),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorE)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorB)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorJ)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(old_cursorE)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(old_cursorB)),
                DirSplittingKernel<BlockDim,JSpaceTwist>((int)gridSizeTwisted.x()));
    }



    template<typename SpaceTwist, typename OrientationTwist,int dir,typename CursorE, typename CursorB, typename CursorJ, typename GridSize>
    void propagateX(CursorE cursorE, CursorB cursorB,CursorJ cursorJ,CursorE old_cursorE, CursorB old_cursorB, GridSize gridSize) const
    {
        using namespace cursor::tools;
        using namespace PMacc::math;

        typedef typename CT::shrinkTo<SpaceTwist,simDim>::type SpaceTwistSimDim;
        PMACC_AUTO(gridSizeTwisted, twistComponents<
            SpaceTwistSimDim
        >(gridSize));

        /* twist components of the supercell */
        typedef typename PMacc::math::CT::make_Vector<simDim,bmpl::integral_c<int,0> >::type ZeroVector;

        typedef typename PMacc::math::CT::AssignIfInRange<
            ZeroVector,
            bmpl::integral_c<int,0>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::x>::type
        >::type VectorWith_X;

        typedef typename PMacc::math::CT::AssignIfInRange<
            VectorWith_X,
            bmpl::integral_c<int,1>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::y>::type
        >::type VectorWith_XY;

        typedef typename PMacc::math::CT::AssignIfInRange<
            VectorWith_XY,
            bmpl::integral_c<int,2>,
            typename PMacc::math::CT::At<SuperCellSize,typename SpaceTwist::z>::type
        >::type BlockDim;


        PMacc::math::Size_t<simDim> zoneSize(gridSizeTwisted);
        //zoneSize.x()=BlockDim::x::value;

        algorithm::kernel::ForeachBlock<BlockDim> foreach;
        foreach(zone::SphericZone<simDim>(zoneSize),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorE)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorB)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(cursorJ)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(old_cursorE)),
                cursor::make_NestedCursor(twistVectorForDirSplitting<SpaceTwistSimDim, OrientationTwist>(old_cursorB)),
                DirSplittingKernelX<BlockDim,dir>((int)gridSizeTwisted.x()));
    }
public:
    DirSplitting(MappingDesc) {}

    void update_afterCurrent(uint32_t currentStep) const
    {
        typedef SuperCellSize GuardDim;

        DataConnector &dc = Environment<>::get().DataConnector();

        FieldE& fieldE = dc.getData<FieldE > (FieldE::getName(), true);
        FieldB& fieldB = dc.getData<FieldB > (FieldB::getName(), true);
        FieldJ& fieldJ = dc.getData<FieldJ > (FieldJ::getName(), true);

        using namespace cursor::tools;



        if (laserProfile::INIT_TIME > float_X(0.0))
            dc.getData<FieldE > (FieldE::getName(), true).laserManipulation(currentStep);

        __setTransactionEvent(fieldE.asyncCommunication(__getTransactionEvent()));
        __setTransactionEvent(fieldB.asyncCommunication(__getTransactionEvent()));

/*  enable this to fill the double buffer with the same values like the original field
        __startOperation(ITask::TASK_HOST);
        __startOperation(ITask::TASK_CUDA);
        fieldE.sync();
        fieldB.sync();
        fieldE.reset(currentStep);
        fieldB.reset(currentStep);
*/

        BOOST_AUTO(fieldE_coreBorder,
            fieldE.getGridBuffer().getDeviceBuffer().
                   cartBuffer().view(GuardDim().toRT(),
                                     -GuardDim().toRT()));
        BOOST_AUTO(fieldB_coreBorder,
            fieldB.getGridBuffer().getDeviceBuffer().
            cartBuffer().view(GuardDim().toRT(),
                              -GuardDim().toRT()));

        BOOST_AUTO(old_fieldE_coreBorder,
            fieldE.getGridBuffer2().getDeviceBuffer().
                   cartBuffer().view(GuardDim().toRT(),
                                     -GuardDim().toRT()));
        BOOST_AUTO(old_fieldB_coreBorder,
            fieldB.getGridBuffer2().getDeviceBuffer().
            cartBuffer().view(GuardDim().toRT(),
                              -GuardDim().toRT()));

        BOOST_AUTO(fieldJ_coreBorder,
            fieldJ.getGridBuffer().getDeviceBuffer().
            cartBuffer().view(GuardDim().toRT(),
                              -GuardDim().toRT()));

        typedef PMacc::math::CT::Int<0,1,2> Orientation_XJ;
        typedef PMacc::math::CT::Int<0,1,2> Space_XJ;

        PMacc::math::Size_t<simDim> gridSize = fieldE_coreBorder.size();

        typedef PMacc::math::CT::Int<0,1,2> Orientation_X;
        typedef PMacc::math::CT::Int<0,1,2> Space_X;
        typedef PMacc::math::CT::Int<0,2,1> JDir_X;
        propagate<Space_X,Orientation_X,JDir_X>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);

#if (SENTOKU==1) //prpagateX adds current
        propagateX<Space_XJ,Orientation_XJ, 1>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);
#endif
        __setTransactionEvent(fieldE.asyncCommunication(__getTransactionEvent()));
        __setTransactionEvent(fieldB.asyncCommunication(__getTransactionEvent()));

        typedef PMacc::math::CT::Int<1,2,0> Orientation_Y;
        typedef PMacc::math::CT::Int<1,0,2> Space_Y;
        typedef PMacc::math::CT::Int<0,1,2> JDir_Y;
        propagate<Space_Y,Orientation_Y,JDir_Y>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);
#if (SENTOKU==1)
        propagateX<Space_XJ,Orientation_XJ, 0>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);
#endif
        __setTransactionEvent(fieldE.asyncCommunication(__getTransactionEvent()));
        __setTransactionEvent(fieldB.asyncCommunication(__getTransactionEvent()));

#if (SENTOKU==1)

        propagateX<Space_XJ,Orientation_XJ, 2>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);


        __setTransactionEvent(fieldE.asyncCommunication(__getTransactionEvent()));
        __setTransactionEvent(fieldB.asyncCommunication(__getTransactionEvent()));
#endif

#if (SIMDIM==DIM3)
        //! \todo: currently 3D: check this code if someone enable 3D
        typedef PMacc::math::CT::Int<2,0,1> Orientation_Z;
        typedef PMacc::math::CT::Int<2,0,1> Space_Z;
        typedef PMacc::math::CT::Int<0,2,1> JDir_Z;
        propagate<Space_Z,Orientation_Z,JDir_Z>(
                  fieldE_coreBorder.origin(),
                  fieldB_coreBorder.origin(),
                  fieldJ_coreBorder.origin(),
                  old_fieldE_coreBorder.origin(),
                  old_fieldB_coreBorder.origin(),
                  gridSize);

        __setTransactionEvent(fieldE.asyncCommunication(__getTransactionEvent()));
        __setTransactionEvent(fieldB.asyncCommunication(__getTransactionEvent()));
#endif
    }

    void update_beforeCurrent(uint32_t) const
    {
    /*    DataConnector &dc = Environment<>::get().DataConnector();

        FieldE& fieldE = dc.getData<FieldE > (FieldE::getName(), true);
        FieldB& fieldB = dc.getData<FieldB > (FieldB::getName(), true);

        EventTask eRfieldE = fieldE.asyncCommunication(__getTransactionEvent());
        EventTask eRfieldB = fieldB.asyncCommunication(__getTransactionEvent());
        __setTransactionEvent(eRfieldE);
        __setTransactionEvent(eRfieldB);

        dc.releaseData(FieldE::getName());
        dc.releaseData(FieldB::getName());
     * */
    }

    static PMacc::traits::StringProperty getStringProperties()
    {
        PMacc::traits::StringProperty propList( "name", "DS" );
        return propList;
    }
};

} // dirSplitting

} // picongpu
