/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OPENDAVINCI_CORE_WRAPPER_POSIX_POSIXTCPFACTORY_H_
#define OPENDAVINCI_CORE_WRAPPER_POSIX_POSIXTCPFACTORY_H_

#include "opendavinci/odcore/opendavinci.h"

#include "opendavinci/odcore/wrapper/TCPFactoryWorker.h"
#include "opendavinci/odcore/wrapper/POSIX/POSIXTCPAcceptor.h"
#include "opendavinci/odcore/wrapper/POSIX/POSIXTCPConnection.h"

namespace odcore {
    namespace wrapper {

        using namespace std;

        template <> struct OPENDAVINCI_API TCPFactoryWorker<NetworkLibraryPosix> {
            static odcore::io::tcp::TCPAcceptor* createTCPAcceptor(const uint32_t& port) {
                return new POSIX::POSIXTCPAcceptor(port);
            };

            static odcore::io::tcp::TCPConnection* createTCPConnectionTo(const string& ip, const uint32_t& port) {
                return new POSIX::POSIXTCPConnection(ip, port);
            };
        };
    }
}

#endif /*OPENDAVINCI_CORE_WRAPPER_POSIX_POSIXTCPFACTORY_H_*/
