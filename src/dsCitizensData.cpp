//
//  dsCitizensData.cpp
//  Dudley
//
//  Created by Sosolimited on 7/11/14.
//
//

#include "dsCitizensData.h"


dsCitizensData::dsCitizensData(){
	
}

void dsCitizensData::setEnvironment(string iEnv, Poco::Timespan iTimeSpan){
  
  environmentType = iEnv;
	
  if (environmentType != "dev_static"){
    // This setting is to use live data from the server.
    
    // Set URL for the big historical fetch of event data.
    baseUrl = "https://mayors24.cityofboston.gov/open311/v2/requests.json";
    initialEnd = dateTimeToString(currentDateTime() - iTimeSpan);
    //	cout << "initial End: " << initialEnd << endl;
    start = "start_date=" + initialEnd + "&end_date=" + dateTimeToString(currentDateTime());
    histPageNum = "1";
    rtPageNum = "1";
    
    if (environmentType == "dev") {
      cout << "System Environment: dev" <<endl;
      rtPageSize = "5";
      histPageSize = "250";
      envPull = "end_date";
      pollingInterval = 5.0;
    } else if (environmentType == "production") {
      cout << "System Environment: production" <<endl;
      rtPageSize = "250";
      histPageSize = "250";
      envPull = "start_date";
      pollingInterval = 5.0;
    } else if (environmentType == "dev_jc_1") {
      cout << "System Environment: dev_jc_1" <<endl;
      rtPageSize = "249";
      histPageSize = "249";
      envPull = "start_date";
      pollingInterval = 5.0;
    }
    
    jsonUrlNoPage = baseUrl + "?" + start + "&page_size=" + histPageSize;
    jsonUrl = jsonUrlNoPage +"&page="+ histPageNum;
    
  } else if (environmentType == "dev_static"){
    // This setting is to use static local data, useful for production.
    //TODO: currently, this setting only shows historical data from a single file, ie. and prevents realtime polling.
    cout << "System Environment: dev_static" <<endl;
    
    jsonUrl = "dudley-june23-750results-manuallycollated.json";
    
  } else {
		cout << "***** error with production setup *****" << endl;
	}
	
//	cout << jsonUrl << endl;
  
	fetchAllJson();

}

dsCitizensData::~dsCitizensData(){}

// IDLE
void dsCitizensData::idle(float iTime){
  
  // After the first historical fetch is complete, we activate polling to look for new realtime data.
  if (pollingActivated == true){
  
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Realtime polling
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (timeOfLastPull) {
      timeSinceLastPull = ofGetElapsedTimef() - timeOfLastPull;
      if (timeSinceLastPull > pollingInterval) {
//        cout << "5 seconds!" <<endl;
        
        string currentStart = dateTimeToString(dateTimeOfLastPull);
        jsonUrl = baseUrl + "?" + envPull + "=" + currentStart + "&page_size=" + rtPageSize + "&page=" + rtPageNum;
        
//        cout << start << endl;
//        cout << jsonUrl << endl;
        fetchRealtimeEventJson();
      }
    }
    
  }
  
}

//
void dsCitizensData::fetchAllJson(){
  fetchGeoJson();       // Get neighborhood data first.
}

// Get geojson file of Boston neighborhoods.
void dsCitizensData::fetchGeoJson(){
  
  if (geojsonBoston.load("boston_neighborhoods.geojson")) {
    ofLog(OF_LOG_NOTICE, "Succeed to load geojson..");
    
    // Copy the neighborhoods from the factory so it also resides here, in this main data object.
    neighborhoods = geojsonBoston.getNeighborhoods();
    
    // Get Open311 "event" data once we have neighborhoods.
    fetchHistoricEventJson();
    
  } else {
    ofLog(OF_LOG_NOTICE, "Failed to load geojson..");
  };
  
}

string dsCitizensData::getEventText(){
  
}

// Is used to get new data by polling every few seconds. Loop durationd defined in idle loop.
void dsCitizensData::fetchRealtimeEventJson(){
  
  bool parsingSuccessful = jsonResults.open(jsonUrl);
	int initialEventSize = events.size();
  
	if (parsingSuccessful) {
		
//    cout  << "---------------- Successfully parsed JSON" << endl;
		
		// cout << jsonResults.getRawString() << endl;
    
		if (jsonResults.size() > 0) {
			
//			cout << "New event(s): " << jsonResults.size() << endl;
			
      
			//  Create an event for each piece of data.
			for(int i=0; i<jsonResults.size(); i++) {
				// Event attributes based on default Open311 data attributes.
				dsEvent* e = new dsEvent(
                                  i,
                                  jsonResults[i]["updated_datetime"].asString(),
                                  jsonResults[i]["status"].asString(),
                                  jsonResults[i]["lat"].asFloat(),
                                  jsonResults[i]["long"].asFloat(),
                                  jsonResults[i]["service_name"].asString(),
                                  jsonResults[i]["description"].asString()
																 );
				events.push_back(e);
				// Add a few custom attributes of our own.
				e->setTime(dateParser(jsonResults[i]["updated_datetime"].asString()));
				e->setNeighborhood(geojsonBoston.getNeighborhoodForPoint(e->getLon(), e->getLat()));
				// Add current category to the category vector.
				dsCategory* c = addCategoryToVector(e->getCategory());
				// Add reference to this event in the category object.
				c->addEvent(e);
				// Add reference to this event in the neighborhood object.
				dsNeighborhood* n = getNeighborhoodByName(e->getNeighborhood());
				if (e->getNeighborhood() != "unknown"){
					n->addEvent(e);
				}
        
				// DEV
//				cout << "---------------------------------------------- events["<< i <<"]" << endl;
//				cout << "          id: "<< e->getId() << endl;
//				cout << "        Time: "<< dateTimeToString( e->getTime() ) << endl;
//				cout << "      Status: "<< e->getStatus() << endl;
//				cout << "         Lat: "<< e->getLat() << endl;
//				cout << "         Lon: "<< e->getLon() << endl;
//				cout << "Neighborhood: "<< e->getNeighborhood() << endl;
//				cout << "    Category: "<< e->getCategory() << endl;
				
			}
			
			//DEV
			//printCategoryCounter();
			//printCategoryContents();
			//printNeighborhoodContents();
			
			// Save to file : pretty print
//			if(!jsonResults.save("example_output_pretty.json",true)) {
//        cout << "example_output_pretty.json written unsuccessfully." << endl;
//			} else {
//        cout << "example_output_pretty.json written successfully." << endl;
//			}
//			// Save to file : raw
//			if(!jsonResults.save("example_output_fast.json",false)) {
//        cout << "example_output_pretty.json written unsuccessfully." << endl;
//			} else {
//        cout << "example_output_pretty.json written successfully." << endl;
//			}
			
			

			cout << "Total Event Size: " << events.size() << endl;
			if (initialEventSize != 0) {
				for (int i = initialEventSize; i < events.size(); i++) {
					updateSubscribers(events[i]);
				}
			}

      cout << "dsCitizensData::fetchRealtimeEventJson- # new events: " << jsonResults.size() << endl;
      cout << "dsCitizensData::fetchRealtimeEventJson- # total events: " << events.size() << endl;
      cout << "dsCitizensData::fetchRealtimeEventJson- jsonUrl: " << jsonUrl << endl;

			//updateSubscribers();
      
		} else {
      cout << "dsCitizensData::fetchRealtimeEventJson- # new events: NONE" << endl;
		}
    
    numNewEventsFromLastPull = events.size() - initialEventSize;
		
	} else {
		cout  << "---------------- Failed to parse JSON" << endl;
	}
	
	// setting the current time for pulling reference
	timeOfLastPull = ofGetElapsedTimef();
	dateTimeOfLastPull = currentDateTime();
  
}

// Gets the first big fetch of event data for a given timespan in the past.
void dsCitizensData::fetchHistoricEventJson(){
  
  bool parsingSuccessful = jsonResults.open(jsonUrl);
  
	if (parsingSuccessful) {
		
    cout  << "---------------- Successfully parsed JSON" << endl;
//    cout << jsonResults.getRawString() << endl;
    
		if (jsonResults.size() > 0) {
			
//			cout << "dsCitizensData::fetchHistoricEventJson- jsonResults.size: " << jsonResults.size() << endl;
			
      
			// #1) --- Create an event for each piece of data.
			for(int i=0; i<jsonResults.size(); i++) {
				// Event attributes based on default Open311 data attributes.
				dsEvent* e = new dsEvent(
                                  i,
                                  jsonResults[i]["updated_datetime"].asString(),
                                  jsonResults[i]["status"].asString(),
                                  jsonResults[i]["lat"].asFloat(),
                                  jsonResults[i]["long"].asFloat(),
                                  jsonResults[i]["service_name"].asString(),
                                  jsonResults[i]["description"].asString()
																 );
				events.push_back(e);
				// Add a few custom attributes of our own.
				e->setTime(dateParser(jsonResults[i]["updated_datetime"].asString()));
				e->setNeighborhood(geojsonBoston.getNeighborhoodForPoint(e->getLon(), e->getLat()));
				// Add current category to the category vector.
				dsCategory* c = addCategoryToVector(e->getCategory());
				// Add reference to this event in the category object.
				c->addEvent(e);
				// Add reference to this event in the neighborhood object.
				dsNeighborhood* n = getNeighborhoodByName(e->getNeighborhood());
				if (e->getNeighborhood() != "unknown"){
					n->addEvent(e);
				}
				
				// DEV
//				cout << "---------------------------------------------- events["<< i <<"]" << endl;
//				cout << "          id: "<< e->getId() << endl;
//				cout << "        Time: "<< dateTimeToString( e->getTime() ) << endl;
//				cout << "      Status: "<< e->getStatus() << endl;
//				cout << "         Lat: "<< e->getLat() << endl;
//				cout << "         Lon: "<< e->getLon() << endl;
//				cout << "Neighborhood: "<< e->getNeighborhood() << endl;
//				cout << "    Category: "<< e->getCategory() << endl;
				
			}
			
			//DEV
			//printCategoryCounter();
			//printCategoryContents();
			//printNeighborhoodContents();
			
			// Save to file : pretty print
			if(!jsonResults.save("example_output_pretty.json",true)) {
        cout << "example_output_pretty.json written unsuccessfully." << endl;
			} else {
        cout << "example_output_pretty.json written successfully." << endl;
			}
			// Save to file : raw
//			if(!jsonResults.save("example_output_fast.json",false)) {
//        cout << "example_output_pretty.json written unsuccessfully." << endl;
//			} else {
//        cout << "example_output_pretty.json written successfully." << endl;
//			}
			
      if (environmentType != "dev_static"){
        
        // #2) --- Paging: if we just hit the maximum 250 results per page (allowed by server), get the next page of data.
        if (jsonResults.size() >= 250){
          //        cout<< "HISTORICAL FETCH - PAGE "+ histPageNum +" * * * * * * * * * * * * * * * * * * * * * * * * * * *";
          histPageNum = ofToString(ofToInt(histPageNum) + 1);
          jsonUrl = jsonUrlNoPage +"&page="+ histPageNum;
          fetchHistoricEventJson();
        }
        // else, we have gotten all the data for the requested timespan, we can begin realtime polling.
        else {
          cout << "dsCitizensData::fetchHistoricEventJson- Total Event Size: " << events.size() << endl;
          cout << "dsCitizensData::fetchHistoricEventJson- jsonUrl: " << jsonUrl << endl;
          
          timeOfLastPull = ofGetElapsedTimef();        // setting the current time for realtime polling.
          dateTimeOfLastPull = currentDateTime();
          pollingActivated = true;
        }
        
      }
      
		} else {
//			cout << "dsCitizensData::fetchHistoricEventJson- No results." << endl;
		}
		
	} else {
		cout  << "ERROR- dsCitizensData::fetchHistoricEventJson- Failed to parse JSON" << endl;
	}
  
}

dsNeighborhood* dsCitizensData::getNeighborhoodByName(string iNeighborhoodName){
  for (auto neighborhood : neighborhoods){
    if(neighborhood->getName() == iNeighborhoodName){
      return neighborhood;
    }
  }
}

dsCategory* dsCitizensData::getCategoryByName(string iCategoryName){
  for (auto category : categories){
    if(category->getName() == iCategoryName){
      return category;
    }
  }
}

// Adds the provided category to the categories vector if we haven't seen it already, and returns the category object.
dsCategory* dsCitizensData::addCategoryToVector(string iCategoryName){
  
  dsCategory* cat;
  if(!categoryCounter[iCategoryName]){
    cat = new dsCategory(iCategoryName);
    categories.push_back(cat);
  } else {
    cat = getCategoryByName(iCategoryName);
  }
  
  ++categoryCounter[iCategoryName];
  
  return cat;
}

// FOR DEV: Prints out the categories.
void dsCitizensData::printCategoryCounter(){
//  cout<< "* * * * * * * * * * * * * * * * * * * * * * * * * PRINTING CATEGORY COUNTER" <<endl;
  for(map<string, int>::const_iterator it = categoryCounter.begin(); it != categoryCounter.end(); it++){
    cout<< it->first <<"\t"<< it->second <<endl;
  }
}

void dsCitizensData::printCategoryContents(){
//  cout<< "* * * * * * * * * * * * * * * * * * * * * * * * * PRINTING CATEGORY CONTENTS" <<endl;
  for(auto category : categories){
    cout<< "name : " << category->getName() <<endl;
    cout<< "#events : " << category->getEvents().size() <<endl;
  }
}

void dsCitizensData::printNeighborhoodContents(){
//  cout<< "* * * * * * * * * * * * * * * * * * * * * * * * * PRINTING NEIGHBORHOOD CONTENTS" <<endl;
  for(auto neighborhood : neighborhoods){
    cout<< "name : " << neighborhood->getName() <<endl;
    cout<< "#events : " << neighborhood->getEvents().size() <<endl;
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Time Tools
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// String to Poco::DateTime
Poco::DateTime dsCitizensData::dateParser(string iTime) {
	
	Poco::DateTimeParser parser;
	Poco::DateTime time;
	int tzd;
	parser.parse(Poco::DateTimeFormat::ISO8601_FORMAT, iTime, time, tzd);
//	cout << "UTC Time Diff: " << tzd << endl;
	return (time);
}

// Poco::DateTime to String, matching server format
string dsCitizensData::dateTimeToString(Poco::DateTime iDateTime) {
	
	int tzBoston;
	Poco::DateTime bostonTime;
	
	if (Poco::Timezone::dst() == 3600) {
		//cout << "day light savings in affect" << endl;
    tzBoston = -14400;
		bostonTime = iDateTime - Poco::Timespan(0,4,0,0,0);
	} else {
		//cout << "standard time, no DST." << endl;
		tzBoston = -18000;
		bostonTime = iDateTime - Poco::Timespan(0,5,0,0,0);
	}
	
	string s = (Poco::DateTimeFormatter::format(bostonTime, Poco::DateTimeFormat::ISO8601_FORMAT, tzBoston));
	return s;
	
}

// Get current local Poco::DateTime
Poco::DateTime dsCitizensData::currentDateTime() {

	Poco::DateTime current = Poco::DateTime::DateTime();
	cout << dateTimeToString( current ) << endl;
	
	return current;
	
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Return the geojson.
//dsNeighborhoodFactory dsCitizensData::getGeoJson(){
//  return geojsonBoston;
//}

ofVec3f dsCitizensData::getEventCoords(int index) {
  
  if (index < events.size()) {
    ofVec3f xyz(events[index]->getLon(), events[index]->getLat(), 0);
    return xyz;
  }
  else
    return ofVec3f(0,0,0);
  
}

ofVec3f dsCitizensData::getCentroid() {
  
  ofVec3f centroid;
  
  for (auto e : events ) {
    centroid.x += e->getLon()/((float)events.size());
    centroid.y += e->getLat()/((float)events.size());
    
  }
  return centroid;
}

//debug function for making sure there is data, uses basic oF drawing tools
void dsCitizensData::draw() {
  
  //ofLogNotice("drawing");
  ofSetColor(250, 0, 0);
  for (int i = 0 ; i < events.size() ; i++) {
    
    ofCircle(400 + 400.0*(events[i]->getLon() - 42.34), 500 -400.0*(events[i]->getLat() + 72.06), 2);
    //ofLogNotice(ofToString(i));
  }
}

// Returns the number of events for a certain neighborhood, and optionally, specific to a category in said neighborhood.
int dsCitizensData::getNumEventsForNeighborhood(string iNeighborhoodName, string iCategoryName){
  for (auto n : neighborhoods){
    if (n->getName() == iNeighborhoodName){
      
      if (iCategoryName.empty()){ return n->getEventCount(); }
      else { return n->getCategoryCount(iCategoryName); }
      
    }
  }
}

//
string dsCitizensData::getEventCategory(int index) {
	if (index >= 0 && index < events.size()) {
		return events[index]->getCategory();
	}
}
string dsCitizensData::getNeighborhoodName(int index) {
	if (index >= 0 && index < events.size()) {
		return events[index]->getNeighborhood();
	}
}
string dsCitizensData::getEventTime(int index) {
	if (index >= 0 && index < events.size()) {
		return events[index]->getTimeString();
	}
}

//
void dsCitizensData::addEventSubscriber(dsCitizensDataSubscriber* iSubscriber){
  eventSubscribers.push_back(iSubscriber);
}

//
void dsCitizensData::updateSubscribers(dsEvent* iEvent){
	//newEvents
  for (auto es : eventSubscribers){
    es->handleNewEvent(iEvent);
  }
}

int dsCitizensData::getNumEvents() {
  if (!events.empty()){
    return events.size();
  } else {
    return 0;
  }
}

int dsCitizensData::getTimeToNextPull(){
  if(pollingActivated){
    return (pollingInterval - timeSinceLastPull);
  } else {
    return 0;
  }
}

int dsCitizensData::getNumNewEvents(){
  return numNewEventsFromLastPull;
}

vector<string> dsCitizensData::getNeighborhoodNames(){
  vector<string> names;
  for (auto n : neighborhoods){
    names.push_back(n->getName());
  }
  return names;
}

vector<string> dsCitizensData::getCategoryNames(){
  vector<string> names;
  for (auto c : categories){
    names.push_back(c->getName());
  }
  return names;
}

// Returns range of the minimum and maximum number of events per neighborhoods.
vector<int> dsCitizensData::getNeighborhoodEventsNumRange(){
  float min = 100000000, max = 0;
  for (auto n : neighborhoods){
    if (n->getEventCount() < min){ min = n->getEventCount(); }
    else if (n->getEventCount() > max){ max = n->getEventCount(); }
  }
  vector<int> range;
  range.push_back(min);
  range.push_back(max);
  
  return range;
}

// Compute array of number of events per minute for the last 60 minutes. Index 0 is most recent event minute, index 59 is 60 minutes ago.
vector<float> dsCitizensData::getEventsPerMinuteInLastHour(){
  
  if(eventsPerMinuteInLastHour.empty()){
    
    for(int i = 0; i < 60; i++){
      eventsPerMinuteInLastHour.push_back(0.0);
    }
    
    // Get time 60 minutes (1 hour) before the most recent event.
    Poco::DateTime startTime = events.front()->getTime();
    Poco::DateTime endTime = startTime - Poco::Timespan(0,0,60,0,0);       // 60 minutes.
    
    //  ofLogNotice("- - - - - - - - startTime: "+ dateTimeToString(startTime));
    //  ofLogNotice("- - - - - - - - endTime: "+ dateTimeToString(endTime));
    
    // Find events within the last 60 minutes and increment count for that minute in counter vector.
    for(int i = 0; i < events.size(); i++){
      if(events[i]->getTime() > endTime){
        //      ofLogNotice("- - - - - - - - - - - - : "+ ofToString(i));
        //      ofLogNotice("- - - - e.timeString = "+ events[i]->getTimeString());
        
        int index = Poco::Timespan(startTime - events[i]->getTime()).totalMinutes();
        
        //      ofLogNotice("- - - - index = "+ ofToString(index));
        
        eventsPerMinuteInLastHour[index] = eventsPerMinuteInLastHour[index]+1;
      }
    }
    
  }
  return eventsPerMinuteInLastHour;
}

float dsCitizensData::getMaxEventsPerMinute(){
  if (eventsPerMinuteInLastHour.empty()){
    getEventsPerMinuteInLastHour();
  }
  return ( *std::max_element(eventsPerMinuteInLastHour.begin(), eventsPerMinuteInLastHour.end()) );
}

float dsCitizensData::getMinEventsPerMinute(){
  if (eventsPerMinuteInLastHour.empty()){
    getEventsPerMinuteInLastHour();
  }
  return ( *std::min_element(eventsPerMinuteInLastHour.begin(), eventsPerMinuteInLastHour.end()) );
}
