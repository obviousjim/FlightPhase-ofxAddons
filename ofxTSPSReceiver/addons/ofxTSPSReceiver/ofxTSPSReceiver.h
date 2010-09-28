/***************************************************************************
 *
 *  ofxTSPSReceiver.cpp
 *	ofxTSPSReceiver Part of the
 *  Toolkit for Sensing People in Spaces
 *  A collaboration between Rockwell LAB + IDEO LAB
 *
 *  Addon Created by Brett Renfer
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

#ifndef OFX_TSPS_RECEIVER
#define OFX_TSPS_RECEIVER

//include dependent addons
#include "ofxOsc.h"

//include src
#include "ofxTSPSPerson.h"
#include "ofxTSPSScene.h"

//interface for listener of people events
class ofxTSPSListener {
  public:
	//called when the person enters the system
    virtual void personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called each time the centroid moves (a lot)
	virtual void personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called one frame before the person is removed from the list to let you clean up
    virtual void personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called every frame no matter what.
	virtual void personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene) = 0;
};

class ofxTSPSReceiver : public ofxOscReceiver 
{
  public:
	ofxTSPSReceiver();
	
	void update( ofEventArgs &e );
	void draw( int width, int height );
	void connect( int port );
	
	
	//function to attach testApp
	void setListener( ofxTSPSListener* delegate );
	
	ofxTSPSListener* eventListener;	
	ofxTSPSPerson* personAtIndex(int i);
	ofxTSPSPerson* personWithID(int pid);
	
	int totalPeople();
	ofxTSPSScene* getScene();
	
protected:	
	vector<ofxTSPSPerson*> trackedPeople;
	ofxTSPSScene scene;
	bool bSetup;
	float personTimeout;

	
};

#endif

