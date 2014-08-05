//
//  dsEvent.cpp
//  Dudley
//
//  Created by Sosolimited on 7/17/14.
//
//

#include "dsEvent.h"

// Constructor params are only the default event attributes of the Open311 data.
dsEvent::dsEvent(int iId, string iTimestring, string iStatus, float iLat, float iLon, string iCategory, string iDescription){
  
  id = iId;
  timeString = iTimestring;
  status = iStatus;
  lat = iLat;
  lon = iLon;
  category = iCategory;
  description = iDescription;
}

dsEvent::~dsEvent(){}


