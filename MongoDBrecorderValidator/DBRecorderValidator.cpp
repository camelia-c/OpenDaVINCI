/**
 * OpenDaVINCI - Tutorial.
 * Copyright (C) 2015 Christian Berger
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

#include <iostream>
#include <sstream>
#include <vector>
#include <random>

#include "DBRecorderValidator.h"
#include <opendavinci/odcore/data/TimeStamp.h>


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>

//#include "core/base/CommandLineArgument.h"
//#include "core/base/CommandLineParser.h"

#include "opendavinci/generated/odcore/data/image/SharedImage.h"
#include "automotivedata/generated/cartesian/Point2.h"

#include "automotivedata/generated/automotive/VehicleData.h"
#include "automotivedata/generated/automotive/VehicleControl.h"
#include "automotivedata/generated/automotive/ForceControl.h"
#include "automotivedata/generated/automotive/miniature/SensorBoardData.h"
#include "automotivedata/generated/automotive/carolocup/Sensors.h"
#include "automotivedata/generated/automotive/carolocup/Control.h"
#include "automotivedata/generated/automotive/GenericCANMessage.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/Pedals.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/AccelerationRequest.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/BrakeRequest.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/SteeringRequest.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/Axles.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/Propulsion.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/VehicleState.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/Wheels.h"
#include "automotivedata/generated/from/opendlv/proxy/reverefh16/Steering.h"

using namespace std;

// We add some of OpenDaVINCI's namespaces for the sake of readability.
using namespace odcore::base::module;
using namespace odcore::data;

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


DBRecorderValidator::DBRecorderValidator(const int32_t &argc, char **argv) :
    DataTriggeredConferenceClientModule(argc, argv, "DBRecorderValidator"),
    d(0.25),
    gen(rd()),
    samplecontainers(){  
     samplecontainers.resize(sampleSize);     
}

DBRecorderValidator::~DBRecorderValidator() {}

void DBRecorderValidator::setUp() {
    cout << "This method is called before the component's body is executed." << endl;    
}

void DBRecorderValidator::tearDown() {
    cout << "This method is called after the program flow returns from the component's body." << endl;
    validateDataSample();
    
}

void DBRecorderValidator::nextContainer(Container &c) {  
   
   //wip

}

void DBRecorderValidator::validateDataSample() {   

    //wip

}



int32_t main(int32_t argc, char **argv) {
    DBRecorderValidator dtr(argc, argv);

    return dtr.runModule();
}


