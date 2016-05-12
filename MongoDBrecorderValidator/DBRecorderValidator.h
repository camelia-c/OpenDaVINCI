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
#include <iomanip>
#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include <vector>
#include <random>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

class DBRecorderValidator : public odcore::base::module::DataTriggeredConferenceClientModule {
    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         *
         * @param obj Reference to an object of this class.
         */
        DBRecorderValidator(const DBRecorderValidator &/*obj*/);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         *
         * @param obj Reference to an object of this class.
         * @return Reference to this instance.
         */
        DBRecorderValidator& operator=(const DBRecorderValidator &/*obj*/);

    public:
        /**
         * Constructor.
         *
         * @param argc Number of command line arguments.
         * @param argv Command line arguments.
         */
        DBRecorderValidator(const int32_t &argc, char **argv);

        virtual ~DBRecorderValidator();

        virtual void nextContainer(odcore::data::Container &c);

        virtual void validateDataSample();


    private:
        virtual void setUp();

        virtual void tearDown();

    public:
        std::string targetdbname = "testdb";
        mongocxx::instance inst{};
        mongocxx::client conn{mongocxx::uri{}};
        bsoncxx::builder::stream::document document{};

        std::vector<odcore::data::Container> samplecontainers;  

        int testedContainerType = 39;
        double bernoulliParam = 0.4;
        int startContainer = 1;
        int sampleSize = 20;
        int currentCount = 0;
        int currentSampleCount = 0;

	std::random_device rd;
	std::mt19937 gen;
        std::bernoulli_distribution d;

};


