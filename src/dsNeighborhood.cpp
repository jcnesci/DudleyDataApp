//
//  dsNeighborhood.cpp
//  Dudley
//
//  Created by Sosolimited on 7/15/14.
//
//

#include "dsNeighborhood.h"

dsNeighborhood::dsNeighborhood(){
  polygon = NULL;
}

dsNeighborhood::~dsNeighborhood(){
  if (polygon != NULL)
    delete(polygon);
}

void dsNeighborhood::setName(string iName){
  name = iName;
}

void dsNeighborhood::addVertX(float iVertX){
  vertsX.push_back(iVertX);
}
vector<float> dsNeighborhood::getVertsX(){
  return vertsX;
}

void dsNeighborhood::addVertY(float iVertY){
  vertsY.push_back(iVertY);
}
vector<float> dsNeighborhood::getVertsY(){
  return vertsY;
}

void dsNeighborhood::addBounds(float iLeftBound, float iRightBound, float iBottomBound, float iTopBound){
  leftBound = iLeftBound;
  rightBound = iRightBound;
  bottomBound = iBottomBound;
  topBound = iTopBound;
  
  // Now that we have the bounds, calculate the centroid/center of the neighborhood.
  calculateCentroid();
}

float dsNeighborhood::getBound(string iBound){
  if (iBound == "left") { return leftBound; }
  else if (iBound == "right") { return rightBound; }
  else if (iBound == "bottom") { return bottomBound; }
  else if (iBound == "top") { return topBound; }
}

void dsNeighborhood::calculateCentroid(){
  if (leftBound && rightBound && bottomBound && topBound){
    centroid.x = (leftBound + rightBound)/2.0;
    centroid.y = (bottomBound + topBound)/2.0;
    centroid.z = 0;
  } else {
    ofLog() << "dsNeighborhood- ERROR : Cannot calculate centroid!";
  }
}

ofVec3f dsNeighborhood::getCentroid(){
  return centroid;
}

ofxPolygonObject* dsNeighborhood::getPolygon(){

  if (polygon == NULL) {
    ofLogNotice("polygon getting created of size " + ofToString(vertsX.size()));
    polygon = new ofxPolygonObject(vertsX.size());
    polygon->setDrawMode(OF_OUTLINE);
    
    for (int i = 0 ; i < vertsX.size() ; i ++) {
      polygon->setVertexPos(i, ofVec3f(vertsX[i], vertsY[i], 0));
    }
    
  } else {
    ofLogNotice("NO DATA MADE");

  }
  
  return polygon;
}

//
void dsNeighborhood::calculateStats(dsEvent* iEvent){
  // Add to counter vars.
  if (iEvent->getStatus() == "open") { stats.nOpen++; }
  else if (iEvent->getStatus() == "closed") { stats.nClosed++; }
  
  Poco::DateTime now;
  Poco::Timespan diff = now - iEvent->getTime();
  
  int diffDaysFromEventToNow = diff.days();
  if (diffDaysFromEventToNow == 0) { stats.nToday++; }    // If the diff is 0, then it was in this current day.
  
  int diffHoursFromEventToNow = diff.totalHours();    // DEV: is off by 3 hours... timezone problem?
  if (diffHoursFromEventToNow == 0) { stats.nThisHour++; }    // If the diff is 0, then it was in this current hour.
  
	if (diff < Poco::Timespan(7,0,0,0,0)) { stats.nThisWeek++; } // If the diff is less than 7 days add it to the week count.
	
  // Recalculate ratio.
  stats.openClosedRatio = stats.nClosed == 0 ? stats.nOpen/1 : (float)stats.nOpen/(float)stats.nClosed;
  
  // Increment count of event's category.
  ++stats.nEventsPerCategory[iEvent->getCategory()];
  
}

//
int dsNeighborhood::getCategoryCount(string iCategoryName){
  return stats.nEventsPerCategory[iCategoryName];
}

// Return the event categories map as a string, for the UI.
string dsNeighborhood::getEventsPerCategoryString(){
  string epc;
  for(map<string, int>::const_iterator it = stats.nEventsPerCategory.begin(); it != stats.nEventsPerCategory.end(); it++){
    cout<< it->first <<"\t"<< it->second <<endl;
    
    string c = it->first;
    int n = it->second;
    epc = epc + ofToString(c) + " " + ofToString(n) + ", ";
  }
  epc = epc.substr(0, epc.size()-2);        // remove the last ", " we don't need.
  
  return epc;
}
