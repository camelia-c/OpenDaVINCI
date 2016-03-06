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
	    m_dumpSharedData(dumpSharedData) {

	    // Get database name
	    const string dbname(dname); 
	    cout << "In DBRecorder the dbname is: " << dbname << "\n";
            //targetdbname(dname);
            
            //connect to the database
            mongocxx::instance inst{};
            mongocxx::client conn{mongocxx::uri{}};
            bsoncxx::builder::stream::document document{};
            auto collection = conn[dbname]["testcollection"];
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
            mongocxx::client conn{mongocxx::uri{}};
            auto collection = conn[targetdbname]["testcollection"];
	    if (!m_fifo.isEmpty()) {
		uint32_t numberOfEntries = m_fifo.getSize();
		for (uint32_t i = 0; i < numberOfEntries; i++) {
		    Container c = m_fifo.leave();
		    // Filter undefined data as well as recorder commands.
		    if ( (c.getDataType() != Container::UNDEFINEDDATA) &&
		         (c.getDataType() != Container::RECORDER_COMMAND)  &&
		         (c.getDataType() != Container::SHARED_DATA)  &&
		         (c.getDataType() != Container::SHARED_IMAGE) ) {

			  bsoncxx::builder::stream::document document{};

                          document << "hello" << "world";
                          collection.insert_one(document.view());
                          
		          //TODO take fields from c
		     }//if
		}//for
		
	    }//if
	}
   
  } // dbrecorder
} // tools

