#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	citizensData = new dsCitizensData();
	
	// ---- Dev or Production ----
	env = "dev"; // set to "dev" or "production" -- dev pull 5 events every 5 seconds.
	initialGrab = Poco::Timespan(1,0,0,0,0); // Initial pull amount days,hr,min,sec,milsec
	setupEnv(env, initialGrab);
	// ---------------------------
	
  
  ref = dsGraphicsRef::instance();
  
	// Disable the of setupScreen because now each scene has a custom renderer.
  ofDisableSetupScreen();
  
  scene = new ofxScene(ofGetWidth(), ofGetHeight());
  scene->setBackgroundColor(10, 10, 10);
	
  //TODO: make request be from 7 days ago (start with this, we need at least 1 week of data). Paging will be necessary.
	//https://mayors24.cityofboston.gov/open311/v2/requests.json?start_date=[SEVEN DAYS AGO]-08:00&page_size=250&page=1
	//  Setting start and end might help when trying to find specific times
	//  start_date=2014-07-16T05:00:00-08:00&end_date=2014-07-16T15:22:00-08:00&
  
  scene->getRoot()->addChild(citizensData);

  // Draws/animates circles for each event and neighborhood centroids.
	eventLayer = new dsEventLayer();
  citizensData->addEventSubscriber(eventLayer);
  eventLayer->buildEvents(citizensData);
  scene->getRoot()->addChild(eventLayer);

  // Draws the map of Boston neighborhoods.
  neighborhoodLayer = new dsNeighborhoodLayer();
  neighborhoodLayer->buildNeighborhoods(citizensData);
  scene->getRoot()->addChild(neighborhoodLayer);
  
  
}

//--------------------------------------------------------------
void ofApp::update(){
  
  scene->update(ofGetElapsedTimef());

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
		eventLayer->animateByEventRate(citizensData, 10.0); // Passing Events and Length
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
