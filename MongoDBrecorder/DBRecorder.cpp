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

#include "DBRecorder.h"
#include <opendavinci/odcore/data/TimeStamp.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "automotivedata/generated/automotive/VehicleData.h"
#include "automotivedata/generated/cartesian/Point2.h"


using namespace std;

// We add some of OpenDaVINCI's namespaces for the sake of readability.
using namespace odcore::base::module;
using namespace odcore::data;



DBRecorder::DBRecorder(const int32_t &argc, char **argv) :
    DataTriggeredConferenceClientModule(argc, argv, "DBRecorder"){ 
                       
         }

DBRecorder::~DBRecorder() {}

void DBRecorder::setUp() {
    cout << "This method is called before the component's body is executed." << endl;
    //we should be able to read the database name from the configuration file, as we did in DBRecorderModule
    //const string recorderOutputDB = getKeyValueConfiguration().getValue<string>("odrecorder.outputDB");

    std::string targetdbname = "testdb"; 
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};
    bsoncxx::builder::stream::document document{};
    auto collection = conn[targetdbname]["testcollection2"];
    //this is just to ensure that it can read documents fine from MongoDB
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
	std::cout << bsoncxx::to_json(doc) << std::endl;
    } 
}

void DBRecorder::tearDown() {
    cout << "This method is called after the program flow returns from the component's body." << endl;
}

void DBRecorder::nextContainer(Container &c) {
    std::string targetdbname = "testdb"; 
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};
    auto collection = conn[targetdbname]["testcollection2"];
    cout << "Received container of type " << c.getDataType() <<
                              " sent at " << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() <<
                          " received at " << c.getReceivedTimeStamp().getYYYYMMDD_HHMMSSms() << endl;

    if (c.getDataType() == TimeStamp::ID()) {
        TimeStamp ts = c.getData<TimeStamp>();
        cout << "Received the following time stamp: " << ts.toString() << endl;
    }
    if(c.getDataType() == Container::VEHICLEDATA){
            /*
               //from AutomotiveData.odvd
		message automotive.VehicleData [id = 39] {
			cartesian.Point2 position [id = 1];
			cartesian.Point2 velocity [id = 2];
			double heading [id = 3];
			double absTraveledPath [id = 4];
			double relTraveledPath [id = 5];
			double speed [id = 6];
			double v_log [id = 7];
			double v_batt [id = 8];
			double temp [id = 9];
		}
               //from /libautomotivedata/include/generated/automotive
               //from /libautomotivedata/src/generated/automotive
            */
            automotive::VehicleData vd = c.getData<automotive::VehicleData>();
            bsoncxx::builder::stream::document doc{};
	    cout << "getting a VEHICLEDATA container" << "\n";

            cartesian::Point2 position = vd.getPosition();
            doc << "position" << position.toString(); 
            /*  
            //to be modified for spatial data, e.g.                            
              location: {
		 type: "Point",
		 coordinates: [-73.856077, 40.848447]
		 }
            */

            cartesian::Point2 velocity = vd.getVelocity();
            float* coords = velocity.getP();
	    doc << "velocityX" << coords[0] ;
            doc << "velocityY" << coords[1] ;

            doc << "heading" << vd.getHeading();
            doc << "absTraveledPath" << vd.getAbsTraveledPath();
	    doc << "relTraveledPath" << vd.getRelTraveledPath();
	    doc << "v_log" << vd.getV_log();
	    doc << "v_batt" << vd.getV_batt();
	    doc << "temp" << vd.getTemp();

	    cout << "Inserting record: pos " << position.toString() << " abstraveledpath " << vd.getAbsTraveledPath() << "\n";

            collection.insert_one(doc.view());
	    
     }//if vehicledata

}

int32_t main(int32_t argc, char **argv) {
    DBRecorder dtr(argc, argv);

    return dtr.runModule();
}


