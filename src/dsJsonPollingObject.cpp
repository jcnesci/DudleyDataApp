//
//  dsJsonPollingObject.cpp
//  Dudley
//
//  Created by Sosolimited/JN on 8/6/14.
//
//

//TODO: add locks and unlocks where necessary. Will they be necessary in citizensData too?

#include "dsJsonPollingObject.h"
#include "dsCitizensData.h"       // to complete foward-declaration in substance.

dsJsonPollingObject::dsJsonPollingObject(dsCitizensData* iData){
  data = iData;
}

dsJsonPollingObject::~dsJsonPollingObject(){}

void dsJsonPollingObject::threadedFunction(){

//  lock();
  timeOfLastPull = data->getTimeOfLastPull();
//  unlock();
  ofLogNotice("time = "+ ofToString(timeOfLastPull));
  
  while(isThreadRunning()) {
    
    // Pull new data from server at specified interval.
    if (timeOfLastPull) {
      timeSinceLastPull = ofGetElapsedTimef() - timeOfLastPull;
      if (timeSinceLastPull > pollingInterval) {

          // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        cout << "5 secs" <<endl;
        string currentStart = data->getDateTimeOfLastPullString();

        //TODO: finish creating getter methods for baseUrl, etc.
        //        data->setJsonUrl(baseUrl + "?" + envPull + "=" + currentStart + "&page_size=" + rtPageSize + "&page=" + rtPageNum);
        
        cout << currentStart << endl;
//        cout << jsonUrl << endl;
        
        // Try to fetch new data since last pull.
        lock();
        data->fetchRealtimeEventJson();
        unlock();
        
        // Update timers.
        timeOfLastPull = data->getTimeOfLastPull();
        ofLogNotice("time = "+ ofToString(timeOfLastPull));
        
      }
    }
    
  }
}