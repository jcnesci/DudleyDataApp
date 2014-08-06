//
//  dsJsonPollingObject.h
//  Dudley
//
//  Created by Sosolimited/JN on 8/6/14.
//
//

#pragma once

#include "ofMain.h"

// forwards declaration
class dsCitizensData;

class dsJsonPollingObject : public ofThread {
  
public:
  
  dsJsonPollingObject(dsCitizensData* iData);
  ~dsJsonPollingObject();
  
protected:
  
  void threadedFunction();
  
private:
  
  dsCitizensData*        data;
  
  float                 pollingInterval = 5;
  float									timeOfLastPull;
  int                   timeSinceLastPull = 0;
  
};