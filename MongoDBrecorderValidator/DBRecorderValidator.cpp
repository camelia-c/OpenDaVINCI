/**
 * OpenDaVINCI - db recorder validator.
 * Copyright (C) 2016 Camelia Ciolac
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
   
    cout << "current container =" << currentCount << "but startContainer=" << startContainer << "and currentSampleSize =" << currentSampleCount << " while sampleSize=" << sampleSize << endl;
    currentCount++;
    if((currentCount >= startContainer)&&(currentSampleCount < sampleSize)){            

            cout << "------------  possible candidate, if containertype matches our interest" << endl;
	    //test if it is the type of container, do a Bernoulli trial whether to include it or not in the sample
	    if(c.getDataType() == testedContainerType){
                cout << " --------------it's the containerType of our interest, do a Bernoulli trial" << endl;
		if (d(gen)){
			samplecontainers.push_back(c);
                        currentSampleCount++; 
                        cout << " CONTAINER RETAINED !" << endl;
		}
	    }
    }

}

void DBRecorderValidator::validateDataSample() {   

    auto db = conn[targetdbname];
    int i = 1;
    int countBadContainers = 0;
    if(currentSampleCount >= 1){
	    for(odcore::data::Container c : samplecontainers){
                //there should be only one type of containers, of type testedContainerType

		cout << i << ") in sample is container of type " << c.getDataType() << " sent at " << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() << std::endl;
		i++;

		// query MongoDB for this container (by type and time)- it should find exactly one, exactly with this data, otherwise mark as different in output  
		bsoncxx::builder::stream::document filter_builder;
		filter_builder << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() << "containerType" << c.getDataType() ; //<< finalize;      
		auto cursor = db["testcollection7"].find(filter_builder.view());

		int counter_answers = 0;
		for (auto&& doc : cursor) {
			counter_answers ++;
			std::cout << bsoncxx::to_json(doc) << std::endl;
			//check for equality in all fields, possible extension


		}       

		if(counter_answers != 1){
			std::cout << "FOR THIS CONTAINER, SOMETHING WRONG, NUMBER OF RESULTS != 1" << std::endl;
                        countBadContainers++;
		}
	    }//for

            cout << "End result: out of total = " << currentSampleCount << " bad were countBadContainers =" << countBadContainers << endl;
    }//if
    else{
        cout << "currentSampleCount = 0 " << endl;
    }

}



int32_t main(int32_t argc, char **argv) {
    DBRecorderValidator dtr(argc, argv);

    return dtr.runModule();
}


