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
#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Timestamp.h"
#include "Poco/Timezone.h"
#include <ctime>

class dsCitizensData:public ofxObject{

public:

	int day, month, year;
	
  dsCitizensData();
  ~dsCitizensData();
	
	void setEnvironment(string iEnv, Poco::Timespan iTimeSpan);

  void fetchAllJson();
  void fetchGeoJson();
  void fetchHistoricEventJson();
  void fetchRealtimeEventJson();
  
  dsNeighborhoodFactory     getGeoJson(){ return geojsonBoston; }
  vector<dsEvent*>          getEvents(){ return events; }
  vector<dsCategory*>       getCategories(){ return categories; }
  vector<dsNeighborhood*>   getNeighbohoods(){ return neighborhoods; }
  
  int                   getNumNewEvents();
  int										getNumEvents();
  vector<float>         getEventsPerMinuteInLastHour();
  float                 getMaxEventsPerMinute();
  float                 getMinEventsPerMinute();
  
  ofVec3f								getCentroid();
  int                   getTimeToNextPull();
  
	string								getEventCategory(int index);
	string								getEventTime(int index);
  ofVec3f								getEventCoords(int index);
  
  int										getNumNeighborhoods() { return geojsonBoston.getNeighborhoodCount(); }
  string								getNeighborhoodName(int index);
  dsNeighborhood*				getNeighborhoodByName(string iNeighborhoodName);
  int										getNumEventsForNeighborhood(string iNeighborhoodName, string iCategoryName="");
  vector<string>        getNeighborhoodNames();
  vector<int>           getNeighborhoodEventsNumRange();
  
  dsCategory*						getCategoryByName(string iCategoryName);
  vector<string>        getCategoryNames();
  
  void									draw();
	
  dsCategory*						addCategoryToVector(string iCategoryName);
  
	Poco::DateTime				dateParser(string iTime);
	string								dateTimeToString(Poco::DateTime iDateTime);
  
  void									updateSubscribers(dsEvent* iEvent);
  void									addEventSubscriber(dsCitizensDataSubscriber* iSubscriber);
  
  string                getEventText();
  
	//DEV fcts
  void									printCategoryCounter();
  void									printCategoryContents();
  void									printNeighborhoodContents();
  
private:
  
  void									idle(float iTime);
	Poco::DateTime				currentDateTime();
  
private:
	
  string                environmentType;
  bool                  pollingActivated = false;
  float                 pollingInterval;
  float									timeOfLastPull;
	Poco::DateTime				dateTimeOfLastPull;
  int                   timeSinceLastPull = 0;
  int                   numNewEventsFromLastPull;
  
	string								baseUrl;
  string                jsonUrlNoPage;
  string								jsonUrl;          // Contains the Open311 JSON query string originally passed to this class.
	string								start;
  string                histPageNum;      // Page number to get for historical data.
  string                histPageSize;     // Page size for historical data.
	string								rtPageSize;       // Page size for realtime polling.
	string								rtPageNum;        // Page number to get, for realtime polling.
	string								initialEnd;
	string								envPull;
	
  ofxJSONElement													jsonResults;			// Contains the raw Open311 data
  dsNeighborhoodFactory										geojsonBoston;		// Creates neighborhood objects from geojson of Boston.
  
  vector<dsEvent*>                        events;						// Contains the Open311 data transformed into objects.
  vector<dsCategory*>                     categories;
  vector<dsNeighborhood*>                 neighborhoods;

  map<string, int>												categoryCounter;
  vector<float>                           eventsPerMinuteInLastHour;    // For UI graph.

  vector<dsCitizensDataSubscriber*>       eventSubscribers;
  
};