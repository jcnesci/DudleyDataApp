//
//  dsUIObject.cpp
//  Dudley
//
//  Created by Sosolimited on 7/30/14.
//
//

#include "dsUIObject.h"
#include <iostream>

dsUIObject::dsUIObject(dsCitizensData* iData)
{
  data = iData;
  bool isVisible = true;
  
  // Initialize "hack" variables for window bar and screen width corrections in Soso universe
  windowBarHack = 0;
  screenWidthHack = 0;
  
  // Reassign mouse callback to flip y-coords
  ofAddListener(ofEvents().mouseReleased, this, &dsUIObject::onMouseReleased);
  ofAddListener(ofEvents().mousePressed, this, &dsUIObject::onMousePressed);
  ofAddListener(ofEvents().mouseMoved, this, &dsUIObject::onMouseMoved);
  ofAddListener(ofEvents().mouseDragged, this, &dsUIObject::onMouseDragged);
  
}

dsUIObject::~dsUIObject()
{
  delete UI;
  delete numEventsLabel;
  delete timeToNextPullLabel;
  delete numNewEventsLabel;
  
  delete neighborhoodDropdown;
  delete neighborhoodOpenLabel;
  delete neighborhoodClosedLabel;
  delete neighborhoodRatioLabel;
  delete neighborhoodHourLabel;
  delete neighborhoodTodayLabel;
  delete neighborhoodWeekLabel;

  delete categoryDropdown;
  delete categoryOpenLabel;
  delete categoryClosedLabel;
  delete categoryRatioLabel;
  delete categoryHourLabel;
  delete categoryTodayLabel;
  delete categoryWeekLabel;

}

void dsUIObject::setup()
{
  // Define the canvas for the Left Sidebar.
  int canvas_width = 280;
  int canvas_height = 600;
  UI = new ofxUICanvas(0, 0, canvas_width, canvas_height);
  UI->setTheme(OFX_UI_THEME_MACOSX);    //OFX_UI_THEME_GRAYDAY
  
  // Disable app event callbacks because we do it from within render tree (ofxObject-style)
  UI->disableAppEventCallbacks();
  
  // Disable mouse event callbacks because we have to flip the y-coords.
  UI->disableMouseEventCallbacks();
  
  buildUIreadouts();
  
  // ------------------------------------------------------------
  // UI items.
  // ------------------------------------------------------------
  
  UI->setWidgetFontSize(OFX_UI_FONT_SMALL);
  
  // Canvas title.
  UI->addLabel("[1] Dudley Data App", OFX_UI_FONT_MEDIUM);
  //
  UI->addSpacer();
  // Updating FPS label.
  UI->addFPS(OFX_UI_FONT_SMALL);
  //
  UI->addSpacer();
  //
  numEventsLabel = NULL;
  numEventsLabel = new ofxUILabel("events: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(numEventsLabel);
  timeToNextPullLabel = NULL;
  timeToNextPullLabel = new ofxUILabel("next poll in: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(timeToNextPullLabel);
  numNewEventsLabel = NULL;
  numNewEventsLabel = new ofxUILabel("new events from last poll: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(numNewEventsLabel);
  //
  UI->addSpacer();
  //
  UI->addLabel("Neighborhood Stats", OFX_UI_FONT_MEDIUM);
  vector<string> neighborhoods = data->getNeighborhoodNames();
  neighborhoodDropdown = UI->addDropDownList("Select neighborhood", neighborhoods);
  neighborhoodDropdown->setAutoClose(true);
  neighborhoodDropdown->setShowCurrentSelected(true);
  for (auto t : neighborhoodDropdown->getToggles()){
    t->setColorBack(ofxUIColor(0, 255));      // Set list item bg to black.
  }
  
  neighborhoodOpenLabel = NULL;
  neighborhoodOpenLabel = new ofxUILabel("open: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodOpenLabel);
  
  neighborhoodClosedLabel = NULL;
  neighborhoodClosedLabel = new ofxUILabel("closed: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodClosedLabel);
  
  neighborhoodRatioLabel = NULL;
  neighborhoodRatioLabel = new ofxUILabel("ratio: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodRatioLabel);

  neighborhoodHourLabel = NULL;
  neighborhoodHourLabel = new ofxUILabel("this hour: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodHourLabel);
  
  neighborhoodTodayLabel = NULL;
  neighborhoodTodayLabel = new ofxUILabel("today: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodTodayLabel);

  neighborhoodWeekLabel = NULL;
  neighborhoodWeekLabel = new ofxUILabel("this week: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(neighborhoodWeekLabel);
  //
  UI->addSpacer();
  //
  UI->addLabel("Category Stats", OFX_UI_FONT_MEDIUM);
  vector<string> categories = data->getCategoryNames();
  categoryDropdown = UI->addDropDownList("Select category", categories);
  categoryDropdown->setAutoClose(true);
  categoryDropdown->setShowCurrentSelected(true);
  for (auto t : categoryDropdown->getToggles()){
    t->setColorBack(ofxUIColor(0, 255));      // Set list item bg to black.
  }

  categoryOpenLabel = NULL;
  categoryOpenLabel = new ofxUILabel("open: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryOpenLabel);
  
  categoryClosedLabel = NULL;
  categoryClosedLabel = new ofxUILabel("closed: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryClosedLabel);
  
  categoryRatioLabel = NULL;
  categoryRatioLabel = new ofxUILabel("ratio: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryRatioLabel);
  
  categoryHourLabel = NULL;
  categoryHourLabel = new ofxUILabel("this hour: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryHourLabel);
  
  categoryTodayLabel = NULL;
  categoryTodayLabel = new ofxUILabel("today: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryTodayLabel);
  
  categoryWeekLabel = NULL;
  categoryWeekLabel = new ofxUILabel("this week: ", OFX_UI_FONT_SMALL);
  UI->addWidgetDown(categoryWeekLabel);
  //
  UI->addSpacer();
  // Buttons
  UI->addToggle("show latest events", true);
  UI->addToggle("show neighborhood labels", false);
  //
  UI->addSpacer();
  //
  UI->addLabel("Events/minute, last 60 minutes", OFX_UI_FONT_MEDIUM);
  UI->addLabel("min: "+ ofToString(data->getMinEventsPerMinute()) +", max: "+ ofToString(data->getMaxEventsPerMinute()), OFX_UI_FONT_SMALL);
  graphEpmBuffer = getGraphData();
  graphEpm = UI->addMovingGraph("eventsPerMinute", graphEpmBuffer, graphEpmBuffer.size(), 0.0, 2.0);
  
  //DEV_jn
//  string textString = "This widget is a text area widget. Use this when you need to display a paragraph of text. It takes care of formatting the text to fit the block.";
//  gui1->addTextArea("textarea", textString, OFX_UI_FONT_SMALL);
}

vector<float> dsUIObject::getGraphData(){
  vector<float> graphData = data->getEventsPerMinuteInLastHour();
  reverse(graphData.begin(), graphData.end());      // Must reverse the data to have first event at the end (to the right).
  
  return graphData;
}

void dsUIObject::buildUIreadouts(){
  int ofAppCornerOffset = 20;
  int readoutsHeight = 300;
  int readoutsWidth = ofGetWidth() - (ofAppCornerOffset*2);
  int readoutsY = ofGetHeight()-readoutsHeight - (ofAppCornerOffset*2);
  // Define the canvas for the event readouts.
  UIreadouts = new ofxUICanvas(0, readoutsY, readoutsWidth, readoutsHeight);
  UIreadouts->setTheme(OFX_UI_THEME_MACOSX);
  UIreadouts->disableAppEventCallbacks();
  UIreadouts->disableMouseEventCallbacks();
  UIreadouts->setWidgetFontSize(OFX_UI_FONT_SMALL);
  
  UIreadouts->addLabel("[2] Readouts", OFX_UI_FONT_MEDIUM);
  UIreadouts->addSpacer();
  
  // Get last few events to print text to UI as readout.
  int numLastEventsDesired = 5;
  vector<dsEvent*>::const_iterator first = data->getEvents().end() - numLastEventsDesired;
  vector<dsEvent*>::const_iterator last = data->getEvents().end();
  lastEventsForReadout.assign(first, last);
  
  
  ofxUIScrollableCanvas *scrollZone = new ofxUIScrollableCanvas(0,readoutsY,readoutsWidth,readoutsHeight);
  
}

void dsUIObject::idle(float iTime)
{
  
}

// Continously update the following items.
void dsUIObject::update()
{
  
  //DEV_jn
//  graphEpm->addPoint(buffer[0]);
//  for(int i = 0; i < 256; i++) { buffer[i] = ofNoise(i/100.0, ofGetElapsedTimef()); }
  
  // Do something to the updating label (so it updates)
  if (numEventsLabel){ numEventsLabel->setLabel("events: "+ ofToString(data->getNumEvents())); }
  if (timeToNextPullLabel){ timeToNextPullLabel->setLabel("next poll in: "+ ofToString(data->getTimeToNextPull()) +" secs"); }
  if (numNewEventsLabel){ numNewEventsLabel->setLabel("new events from last poll: "+ ofToString(data->getNumNewEvents())); }
  
  // Update the UI (ofxUICanvas)
  UI->update();
  UIreadouts->update();
  
}

// Update the dropdowns when they are interacted with.
void dsUIObject::updateDropdown(string iDropdownName, string iSelectedItem){
  if (iDropdownName == "Select neighborhood"){
    dsNeighborhood* curNeighborhood = data->getNeighborhoodByName(iSelectedItem);

    neighborhoodOpenLabel->setLabel("open: "+ ofToString( curNeighborhood->getOpenCount() ));
    neighborhoodClosedLabel->setLabel("closed: "+ ofToString( curNeighborhood->getClosedCount() ));
    neighborhoodRatioLabel->setLabel("ratio: "+ ofToString( curNeighborhood->getOpenClosedRatio(), 2 ));
    neighborhoodHourLabel->setLabel("this hour: "+ ofToString( curNeighborhood->getHourCount() ));
    neighborhoodTodayLabel->setLabel("today: "+ ofToString( curNeighborhood->getDayCount() ));
    neighborhoodWeekLabel->setLabel("this week: "+ ofToString( curNeighborhood->getWeekCount() ));
  } else if (iDropdownName == "Select category"){
    dsCategory* curCategory = data->getCategoryByName(iSelectedItem);
    
    categoryOpenLabel->setLabel("open: "+ ofToString( curCategory->getOpenCount() ));
    categoryClosedLabel->setLabel("closed: "+ ofToString( curCategory->getClosedCount() ));
    categoryRatioLabel->setLabel("ratio: "+ ofToString( curCategory->getOpenClosedRatio(), 2 ));
    categoryHourLabel->setLabel("this hour: "+ ofToString( curCategory->getHourCount() ));
    categoryTodayLabel->setLabel("today: "+ ofToString( curCategory->getDayCount() ));
    categoryWeekLabel->setLabel("this week: "+ ofToString( curCategory->getWeekCount() ));
  }

}

void dsUIObject::render()
{
  if (isVisible){
    ofPushMatrix();
    
    // Flip y-coords to draw correctly in Soso universe.
    ofScale(1, -1, 1);
    
    UI->draw();
    UIreadouts->draw();
    
    ofPopMatrix();
  }
}

void dsUIObject::exit()
{
  UI->exit();
  UIreadouts->exit();
}

// Mouse callbacks that flip the y to bring interactions into soso universe.

void dsUIObject::onMouseReleased(ofMouseEventArgs & iArgs)
{
  windowBarHack = (0 - ofGetHeight() )/2.0;
  screenWidthHack = (0 - ofGetWidth() )/2.0;
  
  UI->mouseReleased(iArgs.x - getTrans().x + screenWidthHack, iArgs.y+getTrans().y + windowBarHack, iArgs.button);
  
}

void dsUIObject::onMousePressed(ofMouseEventArgs & iArgs)
{
  windowBarHack = (0 - ofGetHeight() )/2.0;
  screenWidthHack = (0 - ofGetWidth() )/2.0;
  
  UI->mousePressed(iArgs.x - getTrans().x + screenWidthHack, iArgs.y+getTrans().y + windowBarHack, iArgs.button);
}

void dsUIObject::onMouseMoved(ofMouseEventArgs & iArgs)
{
  windowBarHack = (0 - ofGetHeight() )/2.0;
  screenWidthHack = (0 - ofGetWidth() )/2.0;
  
  UI->mouseMoved(iArgs.x - getTrans().x + screenWidthHack, iArgs.y+getTrans().y + windowBarHack);
}

void dsUIObject::onMouseDragged(ofMouseEventArgs & iArgs)
{
  windowBarHack = (0 - ofGetHeight() )/2.0;
  screenWidthHack = (0 - ofGetWidth() )/2.0;
  
  UI->mouseDragged(iArgs.x - getTrans().x + screenWidthHack, iArgs.y+getTrans().y + windowBarHack, iArgs.button);
}