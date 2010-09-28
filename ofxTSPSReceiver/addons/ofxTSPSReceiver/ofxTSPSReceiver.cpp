/***************************************************************************
 *
 *  ofxTSPSReceiver.cpp
 *	ofxTSPSReceiver 
 *  Rockwell LAB + IDEO LAB peopleVision project
 * 
 *  Created by Brett Renfer
 *  Adapted/Polished for by James George ( http://www.jamesgeorge.org )
 *  in collaboration with FlightPhase ( http://www.flightphase.com )
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer as
 * the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NUI GROUP ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BEN WOODHOUSE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include "ofxTSPSReceiver.h"

ofxTSPSReceiver::ofxTSPSReceiver(){
	bSetup = false;
	eventListener = NULL;
	personTimeout = 3.5 * 1000; // in millis
}

void ofxTSPSReceiver::connect( int port ){
	setup(port);
	bSetup = true;
}

void ofxTSPSReceiver::setListener(ofxTSPSListener* listener)
{
	eventListener = listener;
	ofAddListener(ofEvents.update, this, &ofxTSPSReceiver::update);
}

void ofxTSPSReceiver::draw(int width, int height){
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(.5);
	for(int i = 0; i < totalPeople(); i++){
		ofxTSPSPerson* p = personAtIndex(i);
		ofSetColor(0xffffff);
		ofRect(p->boundingRect.x*width, p->boundingRect.y*height, p->boundingRect.width*width, p->boundingRect.height*height);
		ofSetColor(0xff0000);
		ofCircle((p->centroid.x*width)-2, (p->centroid.y*height)-2, 4);
		ofSetColor(0xffff00);
		ofBeginShape();
		for (int j=0; j<p->contour.size(); j++){
			ofVertex(p->contour[j].x*width, p->contour[j].y*height);
		}
		ofEndShape();
	}
	ofPopStyle();
}

ofxTSPSScene* ofxTSPSReceiver::getScene()
{
	return &scene;
}

void ofxTSPSReceiver::update( ofEventArgs &e ){
	
	if(hasWaitingMessages()){
		// check for waiting messages
		while( hasWaitingMessages() ){
			// get the next message
			ofxOscMessage m;
			getNextMessage( &m );
			
			if(m.getAddress() == "TSPS/scene/") {
				scene.idleTime = m.getArgAsFloat(0);
				scene.percentCovered = m.getArgAsFloat(1);
			}
			
			
			if (m.getAddress() == "TSPS/personEntered/" || 
				m.getAddress() == "TSPS/personMoved/" || 
				m.getAddress() == "TSPS/personUpdated/" || 
				m.getAddress() == "TSPS/personWillLeave/"){
				
				int id = m.getArgAsInt32(0);
				bool personIsNew = false;
				ofxTSPSPerson* person = personWithID( id );
				if(person == NULL){
					person = new ofxTSPSPerson(id, trackedPeople.size());
					trackedPeople.push_back(person);
					personIsNew = true;
				}
				person->update( &m );
				
				if(eventListener != NULL){
					if (m.getAddress() == "TSPS/personEntered/" || personIsNew){
						eventListener->personEntered(person, &scene);
					}
					else if (m.getAddress() == "TSPS/personMoved/"){
						eventListener->personMoved(person, &scene);
					}
					else if (m.getAddress() == "TSPS/personUpdated/"){
						eventListener->personUpdated(person, &scene);
						
					}
					else if (m.getAddress() == "TSPS/personWillLeave/"){
						eventListener->personWillLeave(person, &scene);					
					}
				}
				
				if(m.getAddress() == "TSPS/personWillLeave/"){
					for (int i = trackedPeople.size() - 1; i >= 0; i--){
						if (trackedPeople[i]->pid == person->pid){
							trackedPeople.erase(trackedPeople.begin() + i);
							break;
						}
					}
					delete person;				
				}
			}		
		}
	}

	
	//clear orphaned blobs
	for(int i = trackedPeople.size()-1; i >= 0; i--) {
		if(ofGetElapsedTimeMillis()-trackedPeople[i]->updatedAt > personTimeout) {
			trackedPeople.erase(trackedPeople.begin() + i);
			eventListener->personWillLeave(trackedPeople[i], &scene);
		}
	}	
}


//callbacks to get people
ofxTSPSPerson* ofxTSPSReceiver::personAtIndex(int i)
{
	return trackedPeople[i];
}

ofxTSPSPerson* ofxTSPSReceiver::personWithID(int pid)
{
	for (int i = 0; i < trackedPeople.size(); i++){
		if (trackedPeople[i]->pid == pid){
			return trackedPeople[i];
		}
	}
	return NULL;
}	

int ofxTSPSReceiver::totalPeople()
{
	return trackedPeople.size();
}

