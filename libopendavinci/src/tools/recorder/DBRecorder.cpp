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

#include <iostream>

#include "core/base/Serializable.h"
#include "core/base/module/AbstractCIDModule.h"
#include "core/data/Container.h"
#include "core/io/StreamFactory.h"
#include "core/opendavinci.h"
#include "tools/recorder/DBRecorder.h"
#include "tools/recorder/SharedDataListener.h"


//THESE INTRODUCE CIRCULAR DEPENDENCY  libautomotivedata <-> libopendavinci
//#include "automotivedata/generated/automotive/VehicleData.h"
//#include "automotivedata/generated/cartesian/Point2.h"

#include "automotivedata/VehicleData.h"
#include "automotivedata/Point2.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>



namespace tools {
    namespace recorder {

	using namespace std;
	using namespace core::base;
	using namespace core::data;
	using namespace core::io;
	using namespace tools::recorder;


	DBRecorder::DBRecorder(const string &dname, const uint32_t &memorySegmentSize, const uint32_t &numberOfSegments, const bool &threading, const bool &dumpSharedData) :
	    m_fifo(),
	    m_sharedDataListener(NULL),
	    m_outSharedMemoryFile(NULL),
	    m_dumpSharedData(dumpSharedData),
            targetdbname(dname) {

	    // Get database name
	    //const string dbname(dname); 
	    cout << "In DBRecorder the dbname is: " << targetdbname << "\n";
                        
            //connect to the database
            mongocxx::instance inst{};
            mongocxx::client conn{mongocxx::uri{}};
            bsoncxx::builder::stream::document document{};
            auto collection = conn[targetdbname]["testcollection2"];
            //this is just to ensure that it can read documents fine from MongoDB
            auto cursor = collection.find({});

	    for (auto&& doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << std::endl;
	    }            
	    
	    // Create data store for shared memory.
	    m_sharedDataListener = auto_ptr<SharedDataListener>(new SharedDataListener(m_outSharedMemoryFile, memorySegmentSize, numberOfSegments, threading));
	}

	DBRecorder::~DBRecorder() {
	    // Record remaining entries.
	    CLOG1 << "Clearing queue... ";
		recordQueueEntries();
		
	    CLOG1 << "done." << endl;
	}

	FIFOQueue& DBRecorder::getFIFO() {
	    return m_fifo;
	}

	SharedDataListener& DBRecorder::getDataStoreForSharedData() {
	    return *m_sharedDataListener;
	}

	void DBRecorder::store(core::data::Container c) {
	    // Check if the container to be stored is a "regular" data type.
	    if ( (c.getDataType() != Container::UNDEFINEDDATA) &&
		 (c.getDataType() != Container::RECORDER_COMMAND)  &&
		 (c.getDataType() != Container::SHARED_DATA)  &&
		 (c.getDataType() != Container::SHARED_IMAGE) ) {
		getFIFO().enter(c);
		recordQueueEntries();
	    }

	    if (m_dumpSharedData) {
		// ... or a container that describes a shared memory segment.
		if ( (c.getDataType() == Container::SHARED_DATA)  ||
		     (c.getDataType() == Container::SHARED_IMAGE) ) {
		    getDataStoreForSharedData().add(c);
		}
	    }
	}

	void DBRecorder::recordQueueEntries() {
	    mongocxx::instance inst{};
            mongocxx::client conn{mongocxx::uri{}};
            auto collection = conn[targetdbname]["testcollection2"];
	    if (!m_fifo.isEmpty()) {
		uint32_t numberOfEntries = m_fifo.getSize();
		for (uint32_t i = 0; i < numberOfEntries; i++) {
		    Container c = m_fifo.leave();
		    if(c.getDataType() != Container::VEHICLEDATA){
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


                    //commented
		    /*
		    // Filter undefined data as well as recorder commands.
		    if ( (c.getDataType() != Container::UNDEFINEDDATA) &&
		         (c.getDataType() != Container::RECORDER_COMMAND)  &&
		         (c.getDataType() != Container::SHARED_DATA)  &&
		         (c.getDataType() != Container::SHARED_IMAGE) ) {
                            //store the data
		     }//if

                     */


		}//for
		
	    }//if
	}
   
  } // dbrecorder
} // tools

