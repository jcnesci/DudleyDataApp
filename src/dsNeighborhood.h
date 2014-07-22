//
//  dsNeighborhood.h
//  Dudley
//
//  Created by Sosolimited on 7/15/14.
//
//

#pragma once

#include "ofMain.h"
#include "dsEvent.h"
#include "ofxPolygonObject.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"
#include "Poco/Timezone.h"

struct neighborhoodStats{
  int nOpen = 0;
  int nClosed = 0;
  float openClosedRatio = 0;
  int nToday = 0;
  int nThisHour = 0;
  int nThisWeek = 0;
  map<string, int> nEventsPerCategory;      // Counts number of events per cateogry in neighborhood.
};

class dsNeighborhood {

public:
  dsNeighborhood();
  ~dsNeighborhood();
  
  string getName();
  int getEventCount(){ return events.size(); }
  int getDayCount(){ return stats.nToday; }
  int getHourCount(){ return stats.nThisHour; }
  int getWeekCount(){ return stats.nThisWeek; }
  int getOpenCount(){ return stats.nOpen; }
  int getClosedCount(){ return stats.nClosed; }
  int getOpenClosedRatio(){ return stats.openClosedRatio; }
  map<string, int> getEventsPerCategory(){ return stats.nEventsPerCategory; }
  int getCategoryCount(string iNeighborhoodName);
  vector<float> getVertsX();
  vector<float> getVertsY();
  float getBound(string iBound);
  ofVec3f getCentroid();
  ofxPolygonObject* getPolygon();
  std::vector<dsEvent*> getEvents(){ return events; }
  
  void setName(string iName);
  void addVertX(float iVertX);
  void addVertY(float iVertY);
  void addBounds(float iLeftBound, float iRightBound, float iBottomBound, float iUpBound);
  void addEvent(dsEvent* iEvent);
  
  void calculateCentroid();
  void calculateStats(dsEvent* iEvent);
  
private:
  
  string name;
  vector<float> vertsX;
  vector<float> vertsY;
  float leftBound;
  float rightBound;
  float bottomBound;
  float topBound;
  ofVec3f centroid;
  ofxPolygonObject *polygon;
  neighborhoodStats stats;
  std::vector<dsEvent*> events;
  
  //TODO
  //polygon
  //events
};
