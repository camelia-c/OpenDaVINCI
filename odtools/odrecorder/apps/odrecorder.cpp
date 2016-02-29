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
#include "RecorderModule.h"
#include "core/base/CommandLineArgument.h"
#include "core/base/CommandLineParser.h"

/*
#include "core/dmcp/connection/Client.h"
#include "core/dmcp/discoverer/Client.h"
#include "core/exceptions/Exceptions.h"
#include "generated/coredata/dmcp/ModuleDescriptor.h"
#include "generated/coredata/dmcp/ModuleStateMessage.h"
*/
int32_t main(int32_t argc, char **argv) {
    //initial version
    // we parse the command line arguments to see what recorder to launch: file or database
    
    core::base::CommandLineParser cmdParser;
    cmdParser.addCommandLineArgument("id");
    cmdParser.addCommandLineArgument("cid");
    cmdParser.addCommandLineArgument("freq");
    cmdParser.addCommandLineArgument("verbose");
    cmdParser.addCommandLineArgument("profiling");
    cmdParser.addCommandLineArgument("realtime");
    cmdParser.addCommandLineArgument("recorderdest");
    cmdParser.parse(argc, argv);
    core::base::CommandLineArgument cmdArgumentID = cmdParser.getCommandLineArgument("id");
    core::base::CommandLineArgument cmdArgumentCID = cmdParser.getCommandLineArgument("cid");
    core::base::CommandLineArgument cmdArgumentFREQ = cmdParser.getCommandLineArgument("freq");
    core::base::CommandLineArgument cmdArgumentVERBOSE = cmdParser.getCommandLineArgument("verbose");
    core::base::CommandLineArgument cmdArgumentPROFILING = cmdParser.getCommandLineArgument("profiling");
    core::base::CommandLineArgument cmdArgumentREALTIME = cmdParser.getCommandLineArgument("realtime");
    core::base::CommandLineArgument cmdArgumentRECORDERDEST = cmdParser.getCommandLineArgument("recorderdest");
    
    if (cmdArgumentRECORDERDEST.isSet()) {
        std::string value = cmdArgumentRECORDERDEST.getValue<std::string>();
        std::string fileval ("file");
        std::string dbval ("database");
        if (value.compare(dbval) == 0){
             //have a database recorder, as requested
             odrecorder::DBRecorderModule r(argc, argv);
             return r.runModule();
        }
        else{
             //have a file recorder, as default even for misspelled or wrong options
             odrecorder::RecorderModule r(argc, argv);
             return r.runModule();
        }
    }
    else{
        // by default have a file recorder
	odrecorder::RecorderModule r(argc, argv);
        return r.runModule();
    }
   

   /*ModuleDescriptor md(getName(), getIdentifier(), myVersion, getFrequency());
   try {
            // Try to get configuration from DMCP server.
            m_dmcpClient = SharedPointer<connection::Client>(new connection::Client(md, m_serverInformation));
            m_dmcpClient->setSupercomponentStateListener(this);
            m_dmcpClient->initialize();

            // Get configuration from DMCP client.
            m_keyValueConfiguration KeyValueConfiguration(); 
            m_keyValueConfiguration = m_dmcpClient->getConfiguration();
        } catch (ConnectException& e) {
            CLOG1 << "(ClientModule) connecting to supercomponent failed: " << e.getMessage() << endl;
            return coredata::dmcp::ModuleExitCodeMessage::SERIOUS_ERROR;
        }
   
   const std::string value = m_keyValueConfiguration.getValue<std::string>("odrecorder.recorderdest");
   std::string fileval ("file");
   std::string dbval ("database");
   if (value.compare(dbval) == 0){
	     //have a database recorder, as requested
	     odrecorder::DBRecorderModule r(argc, argv);
	     return r.runModule();
    }
    else{
	     //have a file recorder, as default even for misspelled or wrong options
	     odrecorder::RecorderModule r(argc, argv);
	     return r.runModule();
    }  
    */

}
