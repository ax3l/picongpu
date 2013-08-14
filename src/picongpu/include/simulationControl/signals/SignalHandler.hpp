/* 
 * File:   SignalHandler.hpp
 * Author: huebl
 *
 * Created on Juli 8th, 2013 - 06:09pm
 */

#pragma once

#include <iostream> // debug output
#include <signal.h>
#include <pthread.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "dataManagement/DataConnector.hpp"
#include "dataManagement/ISimulationData.hpp"

namespace picongpu
{


    /** \See doc/SIGNALS.md
     */
    class SignalHandler : public ISimulationData
    {
    public:


        /** The lower the number, the higher the priority
         *  For example:
         *    - if DUMP and TERMINATE are set,
         *      the simulation will first dump, then terminate.
         *    - if DUMP, MARKCHECKPOINT and RESTART are set,
         *      the order of execution is: dump -> mark -> restart
         */
        enum sigCommands
        {
            DUMP = 1u, /* create an hdf5 dump */
            MARKCHECKPOINT = 2u, /* mark as checkpoint */
            RSCHECKPOINT = 4u, /* reset checkpoint to n=0 */
            TERMINATE = 8u, /* clean terminate */
            RESTART = 16u, /* restart sim from last checkpoint */
            SLEEP = 32u /* sleep for 15 sec */
        };

        SignalHandler( ) : signalThread( NULL ), signalThreadrunning( true )
        {
            this->resetSignals();
            
            DataConnector::getInstance( ).registerData( *this, SIM_CTRL );
            
            signalThread =
                new boost::thread( boost::bind( &SignalHandler::run, this ) );
        }

        ~SignalHandler( )
        {
            this->stop( );
        }

        void stop( )
        {
            signalThreadrunning = false;
            io_service.stop( );
            signalThread->join( );
        }

        /** Carefull! Do not access more than signal save functions and
         *  volatile sig_atomic_t variables in this function!
         */
        static void asyncHandler( const boost::system::error_code& errCode,
                                  int signal )
        {
            if( !errCode )
            {
                recvSignal[ signal ] = 1;
            }
        }

        uint32_t getCommands( )
        {
            uint32_t cmdMask = 0;
            
            // command: clean terminate
            if( recvSignal[ SIGTERM ] || recvSignal[ SIGINT ] ||
                recvSignal[ SIGHUP ] || recvSignal[ SIGQUIT ] ||
                recvSignal[ SIGABRT ] )
                cmdMask |= TERMINATE;

            // command: mark as checkpoint && create hdf5 dump
            if( recvSignal[ SIGUSR1 ] )
            {
                cmdMask |= DUMP;
                cmdMask |= MARKCHECKPOINT;
            }

            // command: mark as checkpoint && create hdf5 dump
            if( recvSignal[ SIGUSR2 ] )
            {
                cmdMask |= RSCHECKPOINT;
            }

            // command: restart from last check point
            if( recvSignal[ SIGALRM ] )
                cmdMask |= RESTART;

            // command: sleep for 15 sec
            if( recvSignal[ SIGTSTP ] )
                cmdMask |= SLEEP;

            // command: dump hdf5 (do not mark as checkpoint) and clean terminate
            if( recvSignal[ SIGURG ] )
            {
                cmdMask |= DUMP;
                cmdMask |= TERMINATE;
            }

            // command: dump hdf5 (do not mark as checkpoint)
            if( recvSignal[ SIGIO ] )
                cmdMask |= DUMP;
            
            return cmdMask;
        }

        void resetSignals( )
        {
            for( int i = 0; i < NSIG; ++i )
                recvSignal[ i ] = 0;
        }

        void synchronize( )
        {
            return;
        }

        void run( )
        {
            signalThreadrunning = true;

            boost::asio::signal_set signals( io_service );

            signals.add( SIGTERM );
            signals.add( SIGINT );
            signals.add( SIGHUP );
            signals.add( SIGQUIT );
            signals.add( SIGABRT );
            signals.add( SIGUSR1 );
            signals.add( SIGUSR2 );
            signals.add( SIGALRM );
            signals.add( SIGTSTP );
            signals.add( SIGURG );
            signals.add( SIGIO );

            while( signalThreadrunning )
            {
                // Set a handler for the signals
                signals.async_wait( SignalHandler::asyncHandler );

                // wait for something to handle
                io_service.run( );
                // reset to avoid immediate return of run()
                io_service.reset( );
            }

        }

    private:
        // in order of processing priority:
        static volatile sig_atomic_t recvSignal[ NSIG ];

        boost::asio::io_service io_service;
        boost::thread* signalThread;
        bool signalThreadrunning;
    };
    
    volatile sig_atomic_t SignalHandler::recvSignal[ NSIG ];

} // namespace picongpu
