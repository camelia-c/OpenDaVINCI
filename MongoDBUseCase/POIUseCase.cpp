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
#include <vector>
#include <random>
#include <math.h> 

#include "POIUseCase.h"
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


POIUseCase::POIUseCase(const int32_t &argc, char **argv) :
    DataTriggeredConferenceClientModule(argc, argv, "POIUseCase")
    //,fromDirA(),fromDirB(),fromDirC(),fromDirD()
    {  
     //sensor frequency 10 containers/Sec, allow for 2 minutes
     //fromDirA.resize(1200); 
     //fromDirB.resize(1200); 
     //fromDirC.resize(1200); 
     //fromDirD.resize(1200);     
}

POIUseCase::~POIUseCase() {}

void POIUseCase::setUp() {
    cout << "This method is called before the component's body is executed." << endl;
    //here just when testing
    testWithSyntheticData();   
}

void POIUseCase::tearDown() {
    cout << "This method is called after the program flow returns from the component's body." << endl;    
}

void POIUseCase::nextContainer(Container &c) {  
   
    //these are containers from the current driving; if it's a VehicleData, check it's distance to the POI
    //assume flat surface, as in ODV, and use Euclidian distance 
    if(c.getDataType() == automotive::VehicleData::ID()){ 
            cout << "getting a VEHICLEDATA container" << "\n";
            automotive::VehicleData vd = c.getData<automotive::VehicleData>();	   

            cartesian::Point2 position = vd.getPosition();  
            float* poscoords = position.getP();

            double currentDistToPOI = sqrt((poscoords[0] - POIx)*(poscoords[0] - POIx)+(poscoords[1] - POIy)*(poscoords[1] - POIy));
            if(currentDistToPOI < 100){
                   queryWithinNmetersPOI(POIx,POIy,distToPOI,poscoords[0],poscoords[1],"testcollection7");    
            }

    }

}


//useful: https://github.com/mongodb/mongo-cxx-driver/wiki/Handling-BSON-in-the-new-driver
//assumption: at most one historic travel on each street direction (on 2 street directions historic data can be in same date at different times or in different dates)

void POIUseCase::queryWithinNmetersPOI(double x, double y, double distance, double xcar, double ycar, std::string collectionName){
  
   std::cout << "collectionName=" << collectionName << std::endl;
   auto collection = conn["testdb"][collectionName];

   /*
   //just to make sure it sees collection, findOne()
   auto cursoraux = collection.find({});
   for (auto&& doc : cursoraux) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
        break;
    }
    */

   //Query for all VehicleData documents found within 50 m from point of interest (x,y)
   // db.getCollection('testcollection7').find({ 'position.coordinates':{$near:[-1.5585991386402e-09,7.59982919692993],$maxDistance: 5}})
   bsoncxx::builder::stream::document filter_builder;
   filter_builder << "position.coordinates" <<  open_document <<  "$near" << open_array << x << y  <<close_array << "$maxDistance" << distance << close_document; 
   auto cursor = collection.find(filter_builder.view());

   std::cout << "Now will iterate through cursor" << std::endl;

   //The result is a list of documents in ascending order of distance to POI.
   //Iterate through the cursor and place each element in a vector, depending on the direction they come from (based on time). Discard elements with timeSent > their vector's head.
   bool isFirst = true;
   std::string timeSentClosest1A = "2016-04-14 00:00:00.0", timeSentFar1A = "2016-04-14 00:00:00.0"; //dummy value
   std::string timeSentClosest1B = "2016-04-14 00:00:00.0", timeSentFar1B = "2016-04-14 00:00:00.0"; //dummy value
   std::string timeSentClosest1C = "2016-04-14 00:00:00.0", timeSentFar1C = "2016-04-14 00:00:00.0"; //dummy value
   std::string timeSentClosest1D = "2016-04-14 00:00:00.0", timeSentFar1D = "2016-04-14 00:00:00.0"; //dummy value
   std::string dateSentClosestA, dateSentClosestB, dateSentClosestC, dateSentClosestD; 

   int secondOfDayFarA = -1, secondOfDayFarB = -1, secondOfDayFarC = -1, secondOfDayFarD = -1;  //these keep the minute of day (from the timestamp) of the last analyzed container on each direction
   double xFarA = -1, yFarA = -1, xFarB = -1, yFarB = -1, xFarC = -1, yFarC = -1, xFarD = -1, yFarD = -1; //coordinates of fartherest pos prior to POI in distance meters
 
   
   for (auto&& doc : cursor) {
            
      std::cout << "................................ " << std::endl; 
      std::string str = bsoncxx::to_json(doc);
      //std::cout << str << std::endl;
      bsoncxx::document::value val = bsoncxx::from_json(str);
      auto mydoc = val.view();
      bsoncxx::document::element elem = mydoc["timeSent"];
      bsoncxx::v_noabi::array::element elemcoordsX = mydoc["position"]["coordinates"][0];
      bsoncxx::v_noabi::array::element elemcoordsY = mydoc["position"]["coordinates"][1]; 

      if(isFirst){
          isFirst = false;          

          timeSentClosest1A = bsoncxx::to_json(elem.get_value());
          timeSentFar1A = bsoncxx::to_json(elem.get_value());
          //std::cout << timeSentFar1A << "substr(12,2) = " << timeSentFar1A.substr(12,2) << "substr(15,2) = " << timeSentFar1A.substr(15,2) << std::endl;
          //std::cout << timeSentFar1A << "stoi(substr(15,2)) = " << stoi(timeSentFar1A.substr(12,2)) << "stoi(substr(15,2)) = " << stoi(timeSentFar1A.substr(15,2)) << std::endl;
          dateSentClosestA = timeSentClosest1A.substr(0,11);
          
          secondOfDayFarA = stoi(timeSentFar1A.substr(12,2)) * 3600 + stoi(timeSentFar1A.substr(15,2)) * 60 + stoi(timeSentFar1A.substr(18,2)); 
          xFarA = stof(bsoncxx::to_json(elemcoordsX.get_value()));
          yFarA = stof(bsoncxx::to_json(elemcoordsY.get_value()));
          //fromDirA.push_back(timeSentClosest1A.substr(1,timeSentClosest1A.length()-2)); // skip quotes
          std::cout << "STARTED NEW ENTRY (A) timeSentClosest1A =" << timeSentClosest1A << "  timeSentFar1A=" <<  timeSentFar1A << " xFarA=" << xFarA << " yFarA=" << yFarA << std::endl;          
          
      }
      else{
          
          std::string timeSent1 = bsoncxx::to_json(elem.get_value());
	  int second1 = stoi(timeSent1.substr(12,2)) * 3600 + stoi(timeSent1.substr(15,2)) * 60  + stoi(timeSent1.substr(18,2)) ; 
          std::string date1 = timeSent1.substr(0,11);
          //check if timeSent1 belongs to the same direction as one of the other vector last elements, i.e at most 2 min away
          std::cout << "second1= "<< second1 << " secondOfDayFarA= " << secondOfDayFarA << " secondOfDayFarB= " << secondOfDayFarB << " secondOfDayFarC= " << secondOfDayFarC << " secondOfDayFarD= " << secondOfDayFarD << std::endl;
          bool assignedToDir = false;
          if(((date1.compare(dateSentClosestA) == 0) && (abs(secondOfDayFarA - second1) <= 120))){
             if((secondOfDayFarA - second1) >=0) {
                 timeSentFar1A = bsoncxx::to_json(elem.get_value());
                 secondOfDayFarA = second1;
                 xFarA = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                 yFarA = stof(bsoncxx::to_json(elemcoordsY.get_value()));

              //fromDirA.push_back(doc);
              }
              //else discard it
              assignedToDir = true;
              std::cout << "updated  timeSentFar1A=" <<  timeSentFar1A << " xFarA=" << xFarA << " yFarA=" << yFarA << std::endl;
          }
          else if((date1.compare(dateSentClosestB) == 0) && (secondOfDayFarB!=-1) && (abs(secondOfDayFarB - second1) <= 120)){
              if((secondOfDayFarB - second1) >=0) {
                 //fromDirB.push_back(doc);
                 timeSentFar1B = bsoncxx::to_json(elem.get_value());
                 secondOfDayFarB = second1;
                 xFarB = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                 yFarB = stof(bsoncxx::to_json(elemcoordsY.get_value()));
              }
              //else discard it
              assignedToDir = true;
              std::cout << "updated  timeSentFar1B=" <<  timeSentFar1B << " xFarB=" << xFarB << " yFarB=" << yFarB << std::endl;
          }
          else if((date1.compare(dateSentClosestC) == 0) && (secondOfDayFarC!=-1) && (abs(secondOfDayFarC - second1) <= 120)){
               if((secondOfDayFarC - second1) >=0) {
                 //fromDirC.push_back(doc);
                 timeSentFar1C = bsoncxx::to_json(elem.get_value());
                 secondOfDayFarC = second1;
                 xFarC = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                 yFarC = stof(bsoncxx::to_json(elemcoordsY.get_value()));
               }
              //else discard it
               assignedToDir = true;
               std::cout << "updated  timeSentFar1C=" <<  timeSentFar1C << " xFarC=" << xFarC << " yFarC=" << yFarC << std::endl;
          }
          else if((date1.compare(dateSentClosestD) == 0) && (secondOfDayFarD!=-1) && (abs(secondOfDayFarD - second1) <= 120)){
               if((secondOfDayFarD - second1) >=0) {
                 //fromDirD.push_back(doc);
                 timeSentFar1D = bsoncxx::to_json(elem.get_value());
                 secondOfDayFarD = second1;
                 xFarD = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                 yFarD = stof(bsoncxx::to_json(elemcoordsY.get_value()));
              }
              //else discard it
               assignedToDir = true;
	       std::cout << "updated  timeSentFar1D=" <<  timeSentFar1D << " xFarD=" << xFarD << " yFarD=" << yFarD << std::endl;
          }

          //this case happens when the container under analysis belongs to a different direction than the existing ones
          if(!assignedToDir){
            cout << "STILL NOT ASSIGNED TO DIRECTION" << std::endl;
            if(secondOfDayFarB ==-1){
                //fromDirB.push_back(c);
                timeSentClosest1B = bsoncxx::to_json(elem.get_value());
                timeSentFar1B = bsoncxx::to_json(elem.get_value());
                dateSentClosestB = timeSentClosest1B.substr(0,11);
                secondOfDayFarB = second1;
                xFarB = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                yFarB = stof(bsoncxx::to_json(elemcoordsY.get_value()));
                assignedToDir = true;
                std::cout << "STARTED NEW ENTRY (B)  timeSentClosest1B =" << timeSentClosest1B << "  timeSentFar1B=" <<  timeSentFar1B << " xFarB=" << xFarB << " yFarB=" << yFarB << std::endl;
            }
            else if(secondOfDayFarC ==-1){
                //fromDirC.push_back(c);
                timeSentClosest1C = bsoncxx::to_json(elem.get_value());
                timeSentFar1C = bsoncxx::to_json(elem.get_value());
                dateSentClosestC = timeSentClosest1C.substr(0,11); 
                secondOfDayFarC = second1;
                xFarC = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                yFarC = stof(bsoncxx::to_json(elemcoordsY.get_value()));
                assignedToDir = true;
                std::cout << "STARTED NEW ENTRY (C)   timeSentClosest1C =" << timeSentClosest1C << " timeSentFar1C=" <<  timeSentFar1C << " xFarC=" << xFarC << " yFarC=" << yFarC << std::endl;
                
            }
            else if(secondOfDayFarD ==-1){
                //fromDirD.push_back(c);
                timeSentClosest1D = bsoncxx::to_json(elem.get_value());
                timeSentFar1D = bsoncxx::to_json(elem.get_value());
                dateSentClosestD = timeSentClosest1D.substr(0,11); 
                secondOfDayFarD = second1;
                xFarD = stof(bsoncxx::to_json(elemcoordsX.get_value()));
                yFarD = stof(bsoncxx::to_json(elemcoordsY.get_value()));
                assignedToDir = true;
                std::cout << "STARTED NEW ENTRY (D)  timeSentClosest1D =" << timeSentClosest1D << "  timeSentFar1D=" <<  timeSentFar1D << " xFarD=" << xFarD << " yFarD=" << yFarD << std::endl;
            }

          }

          if(!assignedToDir){
              std::cout << "if assignedToDir is still false, than an error happened";
              std::cout << "currently analyzed has timestamp " << timeSent1 << "of which second of day is second1=" << second1 << std::endl;
              std::cout << "---------------- for all directions registered so far the situation is:" << std::endl;
              std::cout << "DIRECTION A: timeSentClosest1A = " << timeSentClosest1A << " secondOfDayFarA = " << secondOfDayFarA << std::endl;
              std::cout << "DIRECTION B: timeSentClosest1A = " << timeSentClosest1B << " secondOfDayFarB = " << secondOfDayFarB << std::endl;
              std::cout << "DIRECTION C: timeSentClosest1A = " << timeSentClosest1C << " secondOfDayFarC = " << secondOfDayFarC << std::endl;
              std::cout << "DIRECTION D: timeSentClosest1A = " << timeSentClosest1D << " secondOfDayFarD = " << secondOfDayFarD << std::endl;
              std::cout << "-----------------------------------------------------------------------" << std::endl;
          }



      }//else (not first container) 
   }//iterate through cursor

   // get which vector is in the travelling direction of the car
   std::string timeSentClosest = timeSentClosest1A.substr(1,timeSentClosest1A.length()-2);
   std::string timeSentFar =  timeSentFar1A.substr(1,timeSentFar1A.length()-2); 
   double distCarA = sqrt((xFarA - xcar)*(xFarA - xcar)+(yFarA - ycar)*(yFarA - ycar));
   std::cout << "On direction A, the distance from car is " << distCarA << std::endl; 
   double distCarMin = distCarA;

   if((xFarB != -1) && (yFarB != -1)) {
       double distCarB = sqrt((xFarB - xcar)*(xFarB - xcar)+(yFarB - ycar)*(yFarB - ycar));
       std::cout << "On direction B, the distance from car is " << distCarB << std::endl; 
       if(distCarB < distCarMin) {
           timeSentClosest = timeSentClosest1B.substr(1,timeSentClosest1B.length()-2);
           timeSentFar =  timeSentFar1B.substr(1,timeSentFar1B.length()-2);
           distCarMin = distCarB; 
       }
   }
   if((xFarC != -1) && (yFarC != -1)) {
       double distCarC = sqrt((xFarC - xcar)*(xFarC - xcar)+(yFarC - ycar)*(yFarC - ycar));
       std::cout << "On direction C, the distance from car is " << distCarC << std::endl; 
       if(distCarC < distCarMin) {
           timeSentClosest = timeSentClosest1C.substr(1,timeSentClosest1C.length()-2);
           timeSentFar =  timeSentFar1C.substr(1,timeSentFar1C.length()-2); 
           distCarMin = distCarC; 
       }
   }
   if((xFarD != -1) && (yFarD != -1)) {
       double distCarD = sqrt((xFarD - xcar)*(xFarD - xcar)+(yFarD - ycar)*(yFarD - ycar));
       std::cout << "On direction D, the distance from car is " << distCarD << std::endl; ; 
       if(distCarD < distCarMin) {
           timeSentClosest = timeSentClosest1D.substr(1,timeSentClosest1D.length()-2);
           timeSentFar =  timeSentFar1D.substr(1,timeSentFar1D.length()-2); 
           distCarMin = distCarD; 
       }
   }

   //Query all SensorBoardData that have timeSent between timeSentFar and timeSentClosest.  
   //db.getCollection('testcollection7').find({$and:[{'containerType': 81},{'timeSent':{$gte:"2016-04-14 21:29:43.734875", $lt:"2016-04-14 21:29:50.93366"}}]}).sort({'timeSent':1})

   std::cout << std::endl << "BETWEEN timeSentFar: " << timeSentFar << " AND  timeSentClosest:"  << timeSentClosest  << std::endl ; 
   mongocxx::options::find opts;
   bsoncxx::builder::stream::document order_builder;
   order_builder << "timeSent" << 1;
   opts.sort(order_builder.view());

   bsoncxx::builder::stream::document filter_builder2;
   filter_builder2 << "$and" << open_array << open_document << "containerType" << 81 << close_document 
                                           << open_document << "timeSent" << open_document  << "$gte" << timeSentFar << "$lt" << timeSentClosest << close_document 
                                           << close_document 
                                           << close_array ;

   auto cursor2 = collection.find(filter_builder2.view(),opts);
   int counter = 0;
   for (auto&& doc2 : cursor2)
   { 
         counter++;
         //let's also print first 3 docs
         if(counter <= 3){
	     std::cout << bsoncxx::to_json(doc2) << std::endl;  //to print them effectively
         }
      
   }
   std::cout << "Counter SensorBoardData documents : " << counter << std::endl;

}

void POIUseCase::testWithSyntheticData(){
   //inserted synthetic data, directly from RoboMongo
   double xcar = 50;
   double ycar = 15; 
   POIx = 50;
   POIy = 50;
   distToPOI = 20; 
   queryWithinNmetersPOI(POIx,POIy,distToPOI,xcar,ycar,"syntheticdata");  
} 

/*
//initial implementation, that worked for one way street, one time historic data, called as queryWithin50mPOI(-1.5585991386402e-09,7.59982919692993,5);
void POIUseCase::queryWithin50mPOI(double x, double y, double distance){

   auto collection = conn["testdb"]["testcollection7"];
   //Query for all VehicleData documents found within 5 m from point of interest (x,y)
   // db.getCollection('testcollection7').find({ 'position.coordinates':{$near:[-1.5585991386402e-09,7.59982919692993],$maxDistance: 5}})
   bsoncxx::builder::stream::document filter_builder;
   filter_builder << "position.coordinates" <<  open_document <<  "$near" << open_array << x << y  <<close_array << "$maxDistance" << distance << close_document; 
   auto cursor = collection.find(filter_builder.view());

   //The first element (document) in the result list (which comes ordered by distance to point) is the one situated closest from (x,y). Store this timeSent value in a variable called timeSentClosest.
   bool isFirst = true;
   std::string timeSentClosest1 = "2016-04-14 00:00:00.0"; //dummy value
   for (auto&& doc : cursor) {
      if(isFirst){
          isFirst = false;
          std::string str = bsoncxx::to_json(doc);
          bsoncxx::document::value val = bsoncxx::from_json(str);
          auto mydoc = val.view();
          bsoncxx::document::element elem = mydoc["timeSent"];
          timeSentClosest1 = bsoncxx::to_json(elem.get_value());
      }
   }

   //Iterate through the result list (which comes ordered by distance to point) to identify  the lst document having timeSent < timeSentClosest . Store this timeSent value in a variable called timeSentFar.
   auto cursor1 = collection.find(filter_builder.view());
   std::string timeSentFar1 = "2016-04-14 00:00:00.0"; //dummy value
   for (auto&& doc : cursor1) {
          std::string str = bsoncxx::to_json(doc);
          bsoncxx::document::value val = bsoncxx::from_json(str);
          auto mydoc = val.view();
          bsoncxx::document::element elem = mydoc["timeSent"];
          std::string timeSentCurrent = bsoncxx::to_json(elem.get_value());
          if(timeSentCurrent.compare(timeSentClosest1) < 0){               
               timeSentFar1 = timeSentCurrent; 
          }
          
   }
   std::string timeSentFar = timeSentFar1.substr(1,timeSentFar1.length()-2);  
   std::string timeSentClosest = timeSentClosest1.substr(1,timeSentClosest1.length()-2); 
   std::cout << std::endl << "BETWEEN timeSentFar: " << timeSentFar << " AND  timeSentClosest:"  << timeSentClosest  << std::endl ; 

   //Query all SensorBoardData that have timeSent between timeSentFar and timeSentClosest.  
   //db.getCollection('testcollection7').find({$and:[{'containerType': 81},{'timeSent':{$gte:"2016-04-14 21:29:43.734875", $lt:"2016-04-14 21:29:50.93366"}}]}).sort({'timeSent':1})
   mongocxx::options::find opts;
   bsoncxx::builder::stream::document order_builder;
   order_builder << "timeSent" << 1;
   opts.sort(order_builder.view());

   bsoncxx::builder::stream::document filter_builder2;
   filter_builder2 << "$and" << open_array << open_document << "containerType" << 81 << close_document 
                                           << open_document << "timeSent" << open_document  << "$gte" << timeSentFar << "$lt" << timeSentClosest << close_document 
                                           << close_document 
                                           << close_array ;

   auto cursor2 = collection.find(filter_builder2.view(),opts);
   int counter = 0;
   for (auto&& doc2 : cursor2)
   { 
         counter++;
         //let's also print first 3 docs
         if(counter <= 3){
	     std::cout << bsoncxx::to_json(doc2) << std::endl;  //to print them effectively
         }
      
   }
   std::cout << "Counter SensorBoardData documents : " << counter << std::endl;

}

*/




int32_t main(int32_t argc, char **argv) {
    POIUseCase dtr(argc, argv);

    return dtr.runModule();
}


