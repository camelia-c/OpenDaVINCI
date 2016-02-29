/**
 * odrecorder - Tool for recording data
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <string>
#include <iostream>
#include "RecorderModule.h"
#include "core/base/Thread.h"
#include "tools/recorder/DBRecorder.h"
#include "tools/recorder/SharedDataListener.h"
#include "generated/coredata/recorder/RecorderCommand.h"

namespace core { namespace base { class KeyValueDataStore; } }

namespace odrecorder {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::io;
    using namespace tools::recorder;

    DBRecorderModule::DBRecorderModule(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "odrecorder") {}

    DBRecorderModule::~DBRecorderModule() {}

    void DBRecorderModule::setUp() {}

    void DBRecorderModule::tearDown() {}

    void DBRecorderModule::wait() {
        AbstractModule::wait();
    }

    coredata::dmcp::ModuleExitCodeMessage::ModuleExitCode DBRecorderModule::body() {
        // Check if the recorder is remotely controlled.
        bool remoteControl = (getKeyValueConfiguration().getValue<bool>("odrecorder.remoteControl") != 0);

        // database, e.g. testdb , to be read from the configuration file
        const string recorderOutputDB = getKeyValueConfiguration().getValue<string>("odrecorder.outputDB");
        std::cout<< "In DBRecorderModule the outputDB is: " << recorderOutputDB << "\n";
	
        // Size of memory segments.
        const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
        // Number of memory segments.
        const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
        // Run recorder in asynchronous mode to allow real-time recording in background.
        const bool THREADING = true;
        // Dump shared images and shared data?
        const bool DUMP_SHARED_DATA = getKeyValueConfiguration().getValue<uint32_t>("odrecorder.dumpshareddata") == 1;

        // Actual "recording" interface.
        std::cout << "Will create DBRecorder" << "\n";
        DBRecorder dbr(recorderOutputDB, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA);
        std::cout << "DBRecorder successfully created" << "\n";

        // Connect recorder's FIFOQueue to record all containers except for shared images/shared data.
        addDataStoreFor(dbr.getFIFO());

        // Connect recorder's data store that can handle shared data.
        addDataStoreFor(Container::SHARED_DATA, dbr.getDataStoreForSharedData());
        addDataStoreFor(Container::SHARED_IMAGE, dbr.getDataStoreForSharedData());

        // Get key/value-datastore for controlling the odrecorder.
        KeyValueDataStore &kvds = getKeyValueDataStore();

        // If remote control is disabled, simply start recording immediately.
        bool recording = (!remoteControl);
        while (getModuleStateAndWaitForRemainingTimeInTimeslice() == coredata::dmcp::ModuleStateMessage::RUNNING) {
            // Recording queued entries.
            if (recording) {
                if (!dbr.getFIFO().isEmpty()) {
                    dbr.recordQueueEntries();
                }
                else {
                    Thread::usleepFor(500);
                }
            }

            // Check for remote control.
            if (remoteControl) {
                Container container = kvds.get(Container::RECORDER_COMMAND);
                if (container.getDataType() == Container::RECORDER_COMMAND) {
                    coredata::recorder::RecorderCommand rc;
                    rc = container.getData<coredata::recorder::RecorderCommand>();

                    recording = (rc.getCommand() == coredata::recorder::RecorderCommand::RECORD);
                }

                // Discard existing entries.
                if (!recording) {
                    dbr.getFIFO().clear();
                }
            }
        }

        return coredata::dmcp::ModuleExitCodeMessage::OKAY;
    }

} // odrecorder

