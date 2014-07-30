//
//  dsCitizensData.h
//  Dudley
//
//  Created by Sosolimited on 7/11/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxJSONElement.h"
#include "dsNeighborhoodFactory.h"
#include "dsCategory.h"
#include "dsEvent.h"
#include "dsCitizensDataSubscriber.h"
#include "ofxObject.h"

//#include "ofxGeoJSON.h"

#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Timestamp.h"
#include "Poco/Timezone.h"
#include <ctime>

//class dsCategory;   // forward-declaration.

class dsCitizensData:public ofxObject{

public:

	int day, month, year;
	
  dsCitizensData();
  ~dsCitizensData();
	
	void setEnvironment(string iEnv, Poco::Timespan iTimeSpan);

  void fetchAllJson();
  void fetchEventJson();
  void fetchGeoJson();
  void startPolling();
  
  int										getNumEvents() {return events.size(); }
  dsNeighborhoodFactory getGeoJson(){ return geojsonBoston; }
  int										getNumNeighborhoods() { return geojsonBoston.getNeighborhoodCount(); }
  ofVec3f								getCentroid();
  dsCategory*						getCategoryByName(string iCategoryName);
  dsNeighborhood*				getNeighborhoodByName(string iNeighborhoodName);
  int										getNumEventsForNeighborhood(string iNeighborhoodName, string iCategoryName="");
	string								getEventCategory(int index);
  string								getNeighborhoodName(int index);
	string								getEventTime(int index);
	
  ofVec3f								getEventCoords(int index);

  void									draw();
	
  dsCategory*						addCategoryToVector(string iCategoryName);
  
	Poco::DateTime				dateParser(string iTime);
	string								dateTimeToString(Poco::DateTime iDateTime);
  
  //DEV fcts
  void									printCategoryCounter();
  void									printCategoryContents();
  void									printNeighborhoodContents();
  
  void									updateSubscribers(dsEvent* iEvent);
  void									addEventSubscriber(dsCitizensDataSubscriber* iSubscriber);
  
private:
	
	string								baseUrl;
	string								start;
	string								pageSize;
	string								pageNum;
	string								initialEnd;
	string								envPull;

	void									idle(float iTime);
	float									timeOfLastPull;
	Poco::DateTime				dateTimeOfLastPull;
	Poco::DateTime				currentDateTime();
	
  string																	jsonUrl;          // Contains the Open311 JSON query string originally passed to this class.
  ofxJSONElement													jsonResults;			// Contains the raw Open311 data
  dsNeighborhoodFactory										geojsonBoston;		// Creates neighborhood objects from geojson of Boston.
  
  std::vector<dsEvent*>										events;						// Contains the Open311 data transformed into objects.
  std::vector<dsCategory*>								categories;
  std::vector<dsNeighborhood*>						neighborhoods;

  map<string, int>												categoryCounter;

  std::vector<dsCitizensDataSubscriber*>	eventSubscribers;
  
};