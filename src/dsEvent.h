//
//  dsEvent.h
//  Dudley
//
//  Created by Sosolimited on 7/17/14.
//
//

#pragma once

#include "ofMain.h"

#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Timestamp.h"
#include <ctime>

class dsEvent {
  
public:
  dsEvent(int iId, string iTimestring, string iStatus, float iLat, float iLon, string iCategory, string iDescription);
  ~dsEvent();
  
  int							getId(){ return id; }
  Poco::DateTime	getTime(){ return time; }
  string					getTimeString(){ return timeString; }
  string					getStatus(){ return status; }
  float						getLat(){ return lat; }
  float						getLon(){ return lon; }
  string					getNeighborhood(){ return neighborhood; }
  string					getCategory(){ return category; }
  string					getDescription(){ return description; }
  void						setTime(Poco::DateTime iTime){ time = iTime; }
  void						setNeighborhood(string iNeighborhood){ neighborhood = iNeighborhood; }
  
  string					getNeighborhoodName(int index) { return neighborhood; }
  ofVec3f					getEventCoords();
	string					getEventCategory(int index) {return category;}

  
private:
	
  // Default Open311 event attributes.
  int			id;
  string	timeString;
  string	status;
  float		lat;
  float		lon;
  string	category;
  string	description;
  
  // Our additional custom attributes.
  Poco::DateTime	time;
  string					neighborhood;

};