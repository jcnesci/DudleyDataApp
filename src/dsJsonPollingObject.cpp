//
//  dsJsonPollingObject.cpp
//  Dudley
//
//  Created by Sosolimited/JN on 8/6/14.
//
//

#include "dsJsonPollingObject.h"
#include "dsCitizensData.h"       // to complete foward-declaration in substance.

dsJsonPollingObject::dsJsonPollingObject(dsCitizensData* iData){
  data = iData;
}

dsJsonPollingObject::~dsJsonPollingObject(){}

void dsJsonPollingObject::threadedFunction(){

//  lock();
//  timeOfLastPull = data->getTimeOfLastPull();
//  unlock();

  timeOfLastPull = ofGetElapsedTimef();
  
  ofLogNotice("time = "+ ofToString(timeOfLastPull));
  
  while(isThreadRunning()) {
    
    // Pull new data from server at specified interval.
    if (timeOfLastPull) {
      timeSinceLastPull = ofGetElapsedTimef() - timeOfLastPull;
      if (timeSinceLastPull > pollingInterval) {

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        cout << "5 secs" <<endl;
//        string currentStart = dateTimeToString(dateTimeOfLastPull);
//        jsonUrl = baseUrl + "?" + envPull + "=" + currentStart + "&page_size=" + rtPageSize + "&page=" + rtPageNum;
        //        cout << start << endl;
        //        cout << jsonUrl << endl;
//        fetchRealtimeEventJson();
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        lock();
        data->fetchRealtimeEventJson();
        unlock();
        
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        timeOfLastPull = ofGetElapsedTimef();
        
      }
    }
    
  }
}