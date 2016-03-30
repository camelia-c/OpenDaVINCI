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
#include "automotivedata/generated/automotive/VehicleControl.h"


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
}

void DBRecorder::tearDown() {
    cout << "This method is called after the program flow returns from the component's body." << endl;
}

void DBRecorder::nextContainer(Container &c) {

    auto collection3 = conn[targetdbname]["testcollection3"];
    auto collection4 = conn[targetdbname]["testcollection4"];
    

    cout << "Received container of type " << c.getDataType() <<
                              " sent at " << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() <<
                          " received at " << c.getReceivedTimeStamp().getYYYYMMDD_HHMMSSms() << endl;

    if (c.getDataType() == TimeStamp::ID()) {
        TimeStamp ts = c.getData<TimeStamp>();
        cout << "Received the following time stamp: " << ts.toString() << endl;
    }
    
    // if(c.getDataType() == Container::VEHICLEDATA){   //OLD WAY...

    if(c.getDataType() == automotive::VehicleData::ID()){ 
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

            collection3.insert_one(doc.view());
	    
     }//if vehicledata
     else if(c.getDataType() == automotive::VehicleControl::ID()){ 
	/* from AutomotiveData.odvd
	    message automotive.VehicleControl [id = 41] {
		    double speed [id = 1, fourbyteid = 0x0E43596B];
		    double acceleration [id = 2, fourbyteid = 0x0E435991];
		    double steeringWheelAngle [id = 3, fourbyteid = 0x0E435969];
		    bool brakeLights [id = 4, fourbyteid = 0x0E43599B];
		    bool flashingLightsLeft [id = 5, fourbyteid = 0x09823BD7];
		    bool flashingLightsRight [id = 6, fourbyteid = 0x0E435996];
        	  }
	    
        */
	automotive::VehicleControl vc = c.getData<automotive::VehicleControl>();
        bsoncxx::builder::stream::document doc{};
	cout << "getting a VEHICLECONTROL container" << "\n";
	
	doc << "speed" << vc.getSpeed();
	doc << "acceleration" << vc.getAcceleration();	
	doc << "steeringWheelAngle" << vc.getSteeringWheelAngle();	

	collection4.insert_one(doc.view());

     }//if vehiclecontrol

}

int32_t main(int32_t argc, char **argv) {
    DBRecorder dtr(argc, argv);

    return dtr.runModule();
}


