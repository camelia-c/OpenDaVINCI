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

#include "DBRecorder.h"
#include <opendavinci/odcore/data/TimeStamp.h>


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>


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

    auto db = conn[targetdbname];    

    //cout << "Received container of type " << c.getDataType() <<
    //                          " sent at " << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() <<
    //                      " received at " << c.getReceivedTimeStamp().getYYYYMMDD_HHMMSSms() << endl;
    
    // if(c.getDataType() == Container::VEHICLEDATA){   //OLD WAY...
    
    if(c.getDataType() == automotive::VehicleData::ID()){ 
            /*  message automotive.VehicleData [id = 39] {
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
            */
            cout << "getting a VEHICLEDATA container" << "\n";

            automotive::VehicleData vd = c.getData<automotive::VehicleData>();	   

            cartesian::Point2 position = vd.getPosition();  
            float* poscoords = position.getP();

            cartesian::Point2 velocity = vd.getVelocity();
            float* velcoords = velocity.getP();

            bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "position" << open_document << "type" << "Point" << "coordinates" 
                                                   <<  open_array << poscoords[0] << poscoords[1] << close_array << close_document 
                                                   << "velocityX" << velcoords[0] << "velocityY" << velcoords[1] 
                                                   << "heading" << vd.getHeading()
                                                   << "absTraveledPath" << vd.getAbsTraveledPath() 
                                                   << "relTraveledPath" << vd.getRelTraveledPath() 
                                                   << "v_log" << vd.getV_log()
                                                   << "v_batt" << vd.getV_batt()
                                                   << "temp" << vd.getTemp() 
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms()
						   << "containerType" << automotive::VehicleData::ID()
                                                   << finalize;

            auto res = db["testcollection7"].insert_one(mydoc.view());	    
     }//if vehicledata
     else if(c.getDataType() == automotive::VehicleControl::ID()){ 
	/*  message automotive.VehicleControl [id = 41] {
		    double speed [id = 1, fourbyteid = 0x0E43596B];
		    double acceleration [id = 2, fourbyteid = 0x0E435991];
		    double steeringWheelAngle [id = 3, fourbyteid = 0x0E435969];
		    bool brakeLights [id = 4, fourbyteid = 0x0E43599B];
		    bool flashingLightsLeft [id = 5, fourbyteid = 0x09823BD7];
		    bool flashingLightsRight [id = 6, fourbyteid = 0x0E435996];
        	  }	    
        */
	cout << "getting a VEHICLECONTROL container" << "\n";
	automotive::VehicleControl vc = c.getData<automotive::VehicleControl>();

        int brakeLights = (vc.getBrakeLights() == true)?1:0;
        int flashingLightsLeft = (vc.getFlashingLightsLeft() == true)?1:0;	
	int flashingLightsRight = (vc.getFlashingLightsRight() == true)?1:0;	
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "speed" << vc.getSpeed() << "acceleration" << vc.getAcceleration()
                                                   << "steeringWheelAngle" << vc.getSteeringWheelAngle() << "brakeLights" << brakeLights
                                                   << "flashingLightsLeft" << flashingLightsLeft << "flashingLightsRight" << flashingLightsRight
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::VehicleControl::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }//if vehiclecontrol
    else if (c.getDataType() == automotive::ForceControl::ID()){
	
	/*  message automotive.ForceControl [id = 33] {
		    double accelerationForce [id = 1, fourbyteid = 0x0E435991];
		    double brakeForce [id = 2, fourbyteid = 0x0E43598D];
		    double steeringForce [id = 3, fourbyteid = 0x0E435969];
		    bool brakeLights [id = 4, fourbyteid = 0x0E43599B];
		    bool flashingLightsLeft [id = 5, fourbyteid = 0x09823BD7];
		    bool flashingLightsRight [id = 6, fourbyteid = 0x0E435996];
		}
        */

	cout << "getting a FORCECONTROL container" << "\n";
	automotive::ForceControl vc = c.getData<automotive::ForceControl>();

	int brakeLights = (vc.getBrakeLights() == true)?1:0;
        int flashingLightsLeft = (vc.getFlashingLightsLeft() == true)?1:0;	
	int flashingLightsRight = (vc.getFlashingLightsRight() == true)?1:0;
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "accelerationForce" << vc.getAccelerationForce() << "brakeForce" << vc.getBrakeForce()
                                                   << "steeringForce" << vc.getSteeringForce() << "brakeLights" << brakeLights
                                                   << "flashingLightsLeft" << flashingLightsLeft << "flashingLightsRight" << flashingLightsRight
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::ForceControl::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

    }
    else if(c.getDataType() == automotive::miniature::SensorBoardData::ID()){
        /* message automotive.miniature.SensorBoardData [id = 81] {
		    uint32 numberOfSensors [id = 1];
		    map<uint32, double> distances [id = 2];
		}
        */
	
        cout << "getting a SENSORBOARD" << "\n"; 
        automotive::miniature::SensorBoardData sbd = c.getData<automotive::miniature::SensorBoardData> ();

	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "MapOfDistances" << open_array << close_array
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::miniature::SensorBoardData::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

        if(!sbd.isEmpty_MapOfDistances()){
		int mapsize = sbd.getSize_MapOfDistances();
		std::map<uint32_t, double> mapdistances = sbd.getMapOfDistances();
		std::map<uint32_t, double>::const_iterator it = mapdistances.begin();
                std::stringstream sstrOfDistances;	
                int counter = 1;	

		while (it != mapdistances.end()) {
		    sstrOfDistances << it->first << "=" << it->second << endl;
                    double d1 = (double)(it->first);
		    double d2 = (double)(it->second);

		    //bsoncxx::builder::stream::document filter_builder, update_builder;
		    bsoncxx::document::value filter_builder = bsoncxx::builder::stream::document{} << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() << "containerType"<< automotive::miniature::SensorBoardData::ID() << finalize;
		    bsoncxx::document::value update_builder = bsoncxx::builder::stream::document{} << "$push" << open_document << "MapOfDistances" << open_document << "direction" << d1 << "value" << d2 << close_document << close_document << finalize;

                    auto res = db["testcollection7"].update_one(filter_builder.view(), update_builder.view());
		    it++;
		    counter++;	
		     
		}		

	}        
  
     }
     else if(c.getDataType() == automotive::carolocup::Sensors::ID()){
	/* message automotive.carolocup.Sensors [id = 87] {
	    uint32 usFront [id = 1];
	    uint32 usRear [id = 2];
	    uint32 irFrontRight [id = 3];
	    uint32 irRearRight [id = 4];
	    uint32 irBackLeft [id = 5];
	    uint32 irBackRight [id = 6];
	    uint32 gyroHeading [id = 7];
	    uint32 wheelRearLeft [id = 8];
	    uint32 wheelRearRight [id = 9];
	    uint32 buttonState [id = 10];
	    uint32 lightReading [id = 11];
	}
	*/
	cout << "getting a CAROLOCUP sensors" << "\n"; 
	automotive::carolocup::Sensors vc = c.getData<automotive::carolocup::Sensors>();

	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "usFront" << (int)vc.getUsFront() << "usRear" << (int)vc.getUsRear()
                                                   << "irFrontRight" << (int)vc.getIrFrontRight() << "irRearRight" << (int)vc.getIrRearRight()
                                                   << "irBackLeft" << (int)vc.getIrBackLeft() << "irBackRight" << (int)vc.getIrBackRight()
						   << "gyroHeading" << (int)vc.getGyroHeading() << "wheelRearLeft" << (int)vc.getWheelRearLeft() << "wheelRearRight" << (int)vc.getWheelRearRight()
						   << "buttonState" << (int)vc.getButtonState() << "lightReading" << (int)vc.getLightReading()	
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::carolocup::Sensors::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 
     }    
     else if(c.getDataType() == automotive::carolocup::Control::ID()){
	/* message automotive.carolocup.Control [id = 88] {
		    uint32 steering [id = 1];
		    uint32 speed [id = 2];
		    uint32 lights [id = 3]; // 0 = no lights, 1 = brake lights, 2 = blinking left, 4 = blinking right, 8 = emergency light
		    uint32 gyroTrigger [id = 4];
		}
		*/
	cout << "getting a CAROLOCUP control" << "\n"; 
	automotive::carolocup::Control vc = c.getData<automotive::carolocup::Control>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "steering" << (int)vc.getSteering() << "speed" << (int)vc.getSpeed()
                                                   << "lights" << (int)vc.getLights() << "gyroTrigger" << (int)vc.getGyroTrigger()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::carolocup::Control::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

      }
      else if(c.getDataType() == automotive::GenericCANMessage::ID()){
	/* message automotive.GenericCANMessage [id = 71] {
		    odcore::data::TimeStamp driverTimeStamp [id = 1]; // TimeStamp from CAN driver.
		    uint64 identifier [id = 2];                       // ID of the CAN message. 
		    uint8 length [id = 3];                            // Number of data bytes (1-8).
		    uint64 data [id = 4];                             // Payload of the CAN message.
		}
		*/
        // omitted the driverTimeStamp field
	cout << "getting a GenericCANMessage" << "\n"; 
	automotive::GenericCANMessage vc = c.getData<automotive::GenericCANMessage>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "identifier" << (long)vc.getIdentifier() << "length" << (int)vc.getLength()
                                                   << "data" << (long)vc.getData() 
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << automotive::GenericCANMessage::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

      }
      else if(c.getDataType() == from::opendlv::proxy::reverefh16::Pedals::ID()){
	/*	message from.opendlv.proxy.reverefh16.Pedals [id = 191] {
	    double accelerationpedalposition [id = 1];
	    double brakepedalposition [id = 2];
	    double torsionbartorque [id = 3];
	}*/

	cout << "getting REVERE pedals" << "\n"; 

	from::opendlv::proxy::reverefh16::Pedals vc = c.getData<from::opendlv::proxy::reverefh16::Pedals>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "accelerationpedalposition" << vc.getAccelerationpedalposition()
                                                   << "brakepedalposition" << vc.getBrakepedalposition()
                                                   << "torsionbartorque" << vc.getTorsionbartorque()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::Pedals::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

      }		
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::AccelerationRequest::ID()){
	/* message from.opendlv.proxy.reverefh16.AccelerationRequest [id = 192] {
	    bool enable_accrequest [id = 1];
	    double accelerationrequest [id = 2];
	} */

	cout << "getting REVERE AccelerationRequest" << "\n"; 

	from::opendlv::proxy::reverefh16::AccelerationRequest vc = c.getData<from::opendlv::proxy::reverefh16::AccelerationRequest>();
	
	int enable_accrequest = (vc.getEnable_accrequest() == true)?1:0;

	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "enable_accrequest" << enable_accrequest
                                                   << "accelerationrequest" << vc.getAccelerationrequest()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::AccelerationRequest::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

      }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::BrakeRequest::ID()){
	/* message from.opendlv.proxy.reverefh16.BrakeRequest [id = 193] {
		    bool enable_brakerequest [id = 1];
		    double brakerequest [id = 2];
		}	
        */
	cout << "getting REVERE BrakeRequest" << "\n"; 

	from::opendlv::proxy::reverefh16::BrakeRequest vc = c.getData<from::opendlv::proxy::reverefh16::BrakeRequest>();
	
	int enable_brakerequest = (vc.getEnable_brakerequest() == true)?1:0;

	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "enable_brakerequest" << enable_brakerequest
                                                   << "brakerequest" << vc.getBrakerequest()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::BrakeRequest::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::SteeringRequest::ID()){
	/* message from.opendlv.proxy.reverefh16.SteeringRequest [id = 194] {
		    bool enable_steerreq [id = 1];
		    double steerreq_rwa [id = 2];
		    double steerreq_deltatrq [id = 3];
		}	
        */
	cout << "getting REVERE SteeringRequest" << "\n"; 

	from::opendlv::proxy::reverefh16::SteeringRequest vc = c.getData<from::opendlv::proxy::reverefh16::SteeringRequest>();
	
	int enable_steerreq = (vc.getEnable_steerreq() == true)?1:0;

	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "enable_steerreq" << enable_steerreq
                                                   << "steerreq_rwa" << vc.getSteerreq_rwa() << "steerreq_deltatrq" << vc.getSteerreq_deltatrq()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::SteeringRequest::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::Axles::ID()){
	/* message from.opendlv.proxy.reverefh16.Axles [id = 195] {
		    double frontaxle1load [id = 1];
		    double driveaxle1load [id = 2];
		    double driveaxle2load [id = 3];
		}	
        */
	cout << "getting REVERE Axles" << "\n"; 

	from::opendlv::proxy::reverefh16::Axles vc = c.getData<from::opendlv::proxy::reverefh16::Axles>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "frontaxle1load" << vc.getFrontaxle1load()
                                                   << "driveaxle1load" << vc.getDriveaxle1load() << "driveaxle2load" << vc.getDriveaxle2load()
						   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::Axles::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::Propulsion::ID()){
	/* message from.opendlv.proxy.reverefh16.Propulsion [id = 196] {
		    double vehiclespeedpropshaft [id = 1];
		}	
        */
	cout << "getting REVERE Propulsion" << "\n"; 

	from::opendlv::proxy::reverefh16::Propulsion vc = c.getData<from::opendlv::proxy::reverefh16::Propulsion>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "vehiclespeedpropshaft" << vc.getVehiclespeedpropshaft()
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::Propulsion::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::VehicleState::ID()){
	/* message from.opendlv.proxy.reverefh16.VehicleState [id = 197] {
		    double acceleration_x [id = 1];
		    double acceleration_y [id = 2];
		    double yawrate [id = 3];
		}	
        */
	cout << "getting REVERE VehicleState" << "\n"; 

	from::opendlv::proxy::reverefh16::VehicleState vc = c.getData<from::opendlv::proxy::reverefh16::VehicleState>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "acceleration_x" << vc.getAcceleration_x()
						   << "acceleration_y" << vc.getAcceleration_y() << "yawrate" << vc.getYawrate()
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::VehicleState::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::Wheels::ID()){
	/* message from.opendlv.proxy.reverefh16.Wheels [id = 198] {
		    double frontaxle1wheelspeedleft [id = 1];
		    double frontaxle1wheelspeedright [id = 2];
		    double driveaxle1wheelspeedleft [id = 3];
		    double driveaxle1wheelspeedright [id = 4];
		    double driveaxle2wheelspeedleft [id = 5];
		    double driveaxle2wheelspeedright [id = 6];
		}	
        */
	cout << "getting REVERE Wheels" << "\n"; 

	from::opendlv::proxy::reverefh16::Wheels vc = c.getData<from::opendlv::proxy::reverefh16::Wheels>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "frontaxle1wheelspeedleft" << vc.getFrontaxle1wheelspeedleft()
						   << "frontaxle1wheelspeedright" << vc.getFrontaxle1wheelspeedright()
                                                   << "driveaxle1wheelspeedleft" << vc.getDriveaxle1wheelspeedleft()
						   << "driveaxle1wheelspeedright" << vc.getDriveaxle1wheelspeedright()
                                                   << "driveaxle2wheelspeedleft" << vc.getDriveaxle2wheelspeedleft()
						   << "driveaxle2wheelspeedright" << vc.getDriveaxle2wheelspeedright()
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::Wheels::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	
     else if(c.getDataType() == from::opendlv::proxy::reverefh16::Steering::ID()){
	/* message from.opendlv.proxy.reverefh16.Steering [id = 199] {
	    double roadwheelangle [id = 1];
	    double steeringwheelangle [id = 2];
	}	
        */
	cout << "getting REVERE Wheels" << "\n"; 

	from::opendlv::proxy::reverefh16::Steering vc = c.getData<from::opendlv::proxy::reverefh16::Steering>();
	
	bsoncxx::document::value mydoc = bsoncxx::builder::stream::document{} << "roadwheelangle" << vc.getRoadwheelangle()
						   << "steeringwheelangle" << vc.getSteeringwheelangle()
                                                   << "timeSent" << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() 
						   << "containerType" << from::opendlv::proxy::reverefh16::Steering::ID()
                                                   << finalize;

        auto res = db["testcollection7"].insert_one(mydoc.view()); 

     }	


}

int32_t main(int32_t argc, char **argv) {
    DBRecorder dtr(argc, argv);

    return dtr.runModule();
}


