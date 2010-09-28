/***************************************************************************
 *
 *  ofxPerson.cpp
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

#include "ofxTSPSPerson.h"


ofxTSPSPerson::ofxTSPSPerson(int pid, int oid)
:	pid(pid),
	oid(oid),
	age(0),
	opticalFlowVectorAccumulation(ofPoint(0,0)),
	centroid(ofPoint(0,0)),
	customAttributes(NULL)
{};

ofxTSPSPerson::~ofxTSPSPerson()
{

}

void ofxTSPSPerson::update(ofxOscMessage* m)
{
	age = m->getArgAsInt32(1);
	centroid.x = m->getArgAsFloat(2);
	centroid.y = m->getArgAsFloat(3);
	velocity.x = m->getArgAsFloat(4);
	velocity.y = m->getArgAsFloat(5);
	boundingRect.x = m->getArgAsFloat(6);
	boundingRect.y = m->getArgAsFloat(7);
	boundingRect.width= m->getArgAsFloat(8);
	boundingRect.height = m->getArgAsFloat(9);
	opticalFlowVelocity.x = m->getArgAsFloat(10);
	opticalFlowVelocity.y = m->getArgAsFloat(11);	
	
	if (m->getNumArgs() > 12){
		contour.clear();
		for (int i = 12; i < m->getNumArgs(); i += 2){
			contour.push_back(ofPoint(m->getArgAsFloat(i), m->getArgAsFloat(i+1)));
		}
	}
	
	updatedAt = ofGetElapsedTimeMillis();
}

ofPoint ofxTSPSPerson::denormalizedCentroid(float width, float height)
{
	return ofPoint(centroid.x*width, centroid.y*height);
}


vector<ofPoint> ofxTSPSPerson::denormalizedContour(float width, float height)
{
	
	vector<ofPoint> denormalized;
	for( int i = 0; i < contour.size(); i++){
		ofPoint p;
		p.x = contour[i].x*width;
		p.y = contour[i].y*height;
		denormalized.push_back(p);
	}
	
	cout << "num points in contour " << denormalized.size() << endl;
	return denormalized;
}
