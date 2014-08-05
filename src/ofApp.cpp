#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
  //
  ref = dsGraphicsRef::instance();
  ofDisableSetupScreen();             // Disable the of setupScreen because now each scene has a custom renderer.

  scene = new ofxScene(ofGetWidth(), ofGetHeight());
  scene->setBackgroundColor(10, 10, 10);
  ofEnableAntiAliasing();
	
  // ------------------------------------------------------------
  // Data stuff.
  // ------------------------------------------------------------
  
	citizensData = new dsCitizensData();
	scene->getRoot()->addChild(citizensData);         // Required to have its idle loop work.
  
	// ---- Dev or Production ----
	env = "dev_static"; // set to "dev" or "production" -- dev pull 5 events every 5 seconds.
	initialGrab = Poco::Timespan(7,0,0,0,0); // Initial pull amount days,hr,min,sec,milsec
	setupEnv(env, initialGrab);
  
  // ------------------------------------------------------------
  // Graphic layers stuff.
  // ------------------------------------------------------------
  
  // Draws the map of Boston neighborhoods.
  neighborhoodLayer = new dsNeighborhoodLayer();
  neighborhoodLayer->buildNeighborhoods(citizensData);
  scene->getRoot()->addChild(neighborhoodLayer);
  neighborhoodLayer->setZ(0);
	
  //
  realtimeLayer = new dsRealtimeLayer();
  citizensData->addEventSubscriber(realtimeLayer);
	scene->getRoot()->addChild(realtimeLayer);
  realtimeLayer->setZ(1);
  
  //
  historicalLayer = new dsHistoricalLayer(citizensData);
  scene->getRoot()->addChild(historicalLayer);         // Required to have its idle loop work.
  historicalLayer->setZ(2);
  
  // ------------------------------------------------------------
  // GUI stuff.
  // ------------------------------------------------------------
  
  // Build UIObject
  UIObject = new dsUIObject(citizensData);
  UIObject->setup();
  scene->getRoot()->addChild(UIObject);
  UIObject->setZ(100);
  UIObject->setVisible(true);
  
  int cornerOffset = 20;
  
  int UI_x = -ofGetWindowWidth()/2 + cornerOffset;
  int UI_y = ofGetWindowHeight()/2 - cornerOffset;
  UIObject->setTrans(UI_x, UI_y, 0);
  
  // Add Event Listeners
  ofAddListener(UIObject->UI->newGUIEvent, this, &ofApp::UIEvent);
  
}

//--------------------------------------------------------------
void ofApp::update(){
  
  scene->update(ofGetElapsedTimef());
  
  UIObject->update(); // This could also be done with an idle() method in UIObject.

}

//--------------------------------------------------------------
void ofApp::draw(){
  
  ofBackground(255, 255, 255);
  ofSetColor(250,0,0);
  scene->draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	//animate events in relative to their time
	if (key == 'a') {
		eventLayer->animateEvent(citizensData);
	} else if (key == 's') {
    historicalLayer->animateByEventRate(citizensData, 10.0); // Passing Events and Length
	} else if (key == 'f') {
    ofToggleFullscreen();
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::setupEnv(string iEnv, Poco::Timespan iTimeSpan) {
	citizensData->setEnvironment(iEnv, iTimeSpan);
}

//--------------------------------------------------------------
// Monitor UI events to update the appropriate values based on input.
void ofApp::UIEvent(ofxUIEventArgs &e){
  
  string name = e.widget->getName();
  
  if (name == "Select neighborhood" || name == "Select category"){
    ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
    vector<ofxUIWidget *> &selected = ddlist->getSelected();
    for(int i = 0; i < selected.size(); i++){
      ofLogNotice() << "SELECTED: " << selected[i]->getName() << endl;
      UIObject->updateDropdown(name, selected[i]->getName());
    }
  } else if (name == "show latest events"){
    ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    historicalLayer->showLastFewEvents(toggle->getValue());
  } else if (name == "show neighborhood labels"){
    ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
    historicalLayer->showCentroidLabels(toggle->getValue());
  }
  
}

