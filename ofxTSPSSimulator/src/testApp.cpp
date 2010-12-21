/*
 *  ofxTSPSSimulator 
 *
 * Created by James George, http://www.jamesgeorge.org
 * over a long period of time for a few different projects in collaboration with FlightPhase http://www.flightphase.com 
 *
 **********************************************************
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
#ifdef TARGET_OSX
	ofDisableDataPath();
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)path, PATH_MAX);
	CFRelease(resourcesURL);
	chdir(path);
#endif
	
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	selectedPerson = NULL;
	sender = new ofxTSPSOscSender("localhost", 12000);
	scene = new ofxTSPSScene();

	floorImage = new ofImage();
	floorImage->loadImage("interactionarea.png");
	
	isDragging = false;
	
	uid = 0;
	
	interactionArea = ofRectangle(100,0,640,240);
	canvas = ofRectangle(0,0,ofGetWidth(), ofGetHeight());
						 
}

//--------------------------------------------------------------
void testApp::update(){
	for(int i = 0; i < people.size(); i++){
		ofxTSPSMockPerson* person = people[i];
		person->update();
		if(person->didEnterInteractionArea()){
			sender->personEntered(person, person->getCentroidToTransmit(), interactionArea.width, interactionArea.height, false);						
		}
		else if(person->didLeaveInteractionArea()){
			sender->personWillLeave(person, person->getCentroidToTransmit(), interactionArea.width, interactionArea.height, false);			
		}
		else if(person->isInInteractionArea()){
			scene->timeOfLastPerson = ofGetElapsedTimeMillis();
			sender->personUpdated(person, person->getCentroidToTransmit(), interactionArea.width, interactionArea.height, false);			
		}
		
	}	
	sender->sendScene(scene);

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(255, 255, 255);
	floorImage->draw(interactionArea.x,interactionArea.y, interactionArea.width, interactionArea.height);
	
	ofPushStyle();{
		ofSetRectMode(OF_RECTMODE_CENTER);
		for(int i = 0; i < people.size(); i++){
			ofxTSPSMockPerson* person = people[i];
			if(person->isSelected()){
				ofSetColor(255, 0, 0, 100);
				ofFill();
				ofCircle(person->getCentroidToDraw().x, person->getCentroidToDraw().y, 10);
			}
			
			ofNoFill();
			ofSetColor(155, 155, 155);			
			ofCircle(person->getCentroidToDraw().x, person->getCentroidToDraw().y, 10);
			
			if(person->isInInteractionArea()){
				ofSetColor(100, 255, 100);
			}
			else{
				ofSetColor(255, 100, 100);			
			}
			ofFill();
			ofCircle(person->getCentroidToDraw().x, person->getCentroidToDraw().y,  4);
		}
		
		ofSetColor(0, 0, 0);
		ofDrawBitmapString("Idle Time: " + ofToString(scene->idleTime()/1000.0, 3), 20, ofGetHeight()-20);
		
	}ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if( (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) && selectedPerson != NULL){
		for(int i = people.size() - 1; i >= 0; i--){
			ofxTSPSMockPerson* person = people[i];
			if(person == selectedPerson){
				if(person->isInInteractionArea()){
					sender->personWillLeave(person, person->getCentroidToTransmit(), interactionArea.width, interactionArea.height, false);
				}
				people.erase(people.begin() + i);
				delete person;	
				break;
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if(isDragging){
		selectedPerson->setMoveToLocation( ofxVec2f(x, y) );
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	selectedPerson = NULL;
	ofxVec2f mouse(x,y);
	for(int i = 0; i < people.size(); i++){
		ofxTSPSMockPerson* person = people[i];		
		if(person->attemptSelect(mouse)){
			selectedPerson = person;
			isDragging = true;
		}
	}
	
	if(selectedPerson == NULL){
		ofxTSPSMockPerson* person = new ofxTSPSMockPerson(uid++, people.size(), ofxVec2f(x, y), canvas, interactionArea);
		people.push_back(person);
		if(person->isInInteractionArea()){
			sender->personEntered(person, person->getCentroidToTransmit(), interactionArea.width, interactionArea.height, false);
		}
		selectedPerson = person;
		isDragging = true;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	isDragging = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

