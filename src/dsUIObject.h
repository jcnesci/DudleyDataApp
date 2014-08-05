//
//  dsUIObject.h
//  Dudley
//
//  Created by Sosolimited on 7/30/14.
//
//

# pragma once

#include "ofxObject.h"
#include "ofxUI.h"                        // ofxUI.h addon is dependent on ofxXmlSettings.h addon
#include "ofxXmlSettings.h"
#include "dsCitizensData.h"

class dsUIObject : public ofxObject {
  
public:
  dsUIObject(dsCitizensData* iData);
  ~dsUIObject();
  
  void                setup();
  void                idle(float iTime);
  void                update();
  
  void                render();
  
  void                exit();
  
  void                onMouseReleased(ofMouseEventArgs & iArgs);
  void                onMousePressed(ofMouseEventArgs & iArgs);
  void                onMouseMoved(ofMouseEventArgs & iArgs);
  void                onMouseDragged(ofMouseEventArgs & iArgs);
  
  void                toggleVisible() { isVisible = !isVisible; }
  void                setVisible(bool iIsVisible) { isVisible = iIsVisible; }
  
  void                buildUIreadouts();
  
  void                updateDropdown(string iDropdownName, string iNeighborhoodName);
  vector<float>       getGraphData();
  
public:
  
  ofxUICanvas*          UI;
  ofxUILabel*           demoLabel;
  ofxUILabel*           numEventsLabel;
  ofxUILabel*           timeToNextPullLabel;
  ofxUILabel*           numNewEventsLabel;
  ofxUIDropDownList*    neighborhoodDropdown;
  ofxUILabel*           neighborhoodOpenLabel;
  ofxUILabel*           neighborhoodClosedLabel;
  ofxUILabel*           neighborhoodRatioLabel;
  ofxUILabel*           neighborhoodHourLabel;
  ofxUILabel*           neighborhoodTodayLabel;
  ofxUILabel*           neighborhoodWeekLabel;
  ofxUITextArea*        neighborhoodCategoriesTextArea;
  ofxUIDropDownList*    categoryDropdown;
  ofxUILabel*           categoryOpenLabel;
  ofxUILabel*           categoryClosedLabel;
  ofxUILabel*           categoryRatioLabel;
  ofxUILabel*           categoryHourLabel;
  ofxUILabel*           categoryTodayLabel;
  ofxUILabel*           categoryWeekLabel;

  ofxUIMovingGraph*     graphEpm;
  vector<float>         graphEpmBuffer;
  
  ofxUIScrollableCanvas*          UIreadouts;
  vector<dsEvent*>      lastEventsForReadout;
  
private:
  
  dsCitizensData*       data;               // Pointer to all data to get statistics in the UI.
  
  bool                  isVisible;
  
  int                   windowBarHack;
  int                   screenWidthHack;
  
  //Slider Values and default positions
  float                 sliderValR = 100;
  float                 sliderValG = 150;
  float                 sliderValB = 200;
  int                   sliderValRes = 5;
  float                 sliderValScale = 100;
  
};