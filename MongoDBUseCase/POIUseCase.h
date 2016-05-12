/**
 * OpenDaVINCI - Point Of Interest Use Case.
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
#include <iomanip>
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include <vector>
#include <random>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

class POIUseCase : public odcore::base::module::DataTriggeredConferenceClientModule {
    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         *
         * @param obj Reference to an object of this class.
         */
        POIUseCase(const POIUseCase &/*obj*/);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         *
         * @param obj Reference to an object of this class.
         * @return Reference to this instance.
         */
        POIUseCase& operator=(const POIUseCase &/*obj*/);

    public:
        /**
         * Constructor.
         *
         * @param argc Number of command line arguments.
         * @param argv Command line arguments.
         */
        POIUseCase(const int32_t &argc, char **argv);

        virtual ~POIUseCase();

        virtual void nextContainer(odcore::data::Container &c);

        virtual void queryWithinNmetersPOI(double x, double y, double dist, double xcar, double ycar, std::string collectionName);

        virtual void testWithSyntheticData();

    private:
        virtual void setUp();

        virtual void tearDown();

    public:
        std::string targetdbname = "testdb";
        mongocxx::instance inst{};
        mongocxx::client conn{mongocxx::uri{}};
        bsoncxx::builder::stream::document document{};

        double POIx = -1.5585991386402e-09;
        double POIy = 7.59982919692993;
        double distToPOI = 5;

	//vectors for possible directions of an intersection; if it's a straight road, only 2 will be used
        /*
        std::vector<std::string> fromDirA;  
        std::vector<std::string fromDirB;
        std::vector<std::string> fromDirC;
        std::vector<std::string> fromDirD;    
        */
};


