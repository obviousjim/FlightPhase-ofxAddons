/***************************************************************************
 *
 *  ofxPerson.h
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

//11/23/2009 modified by James George
//Added:
//	blob-based constuctor
//	persistent id
//	ghost frame protection from Haar objects
//	normalized accessors
//	countour points
//	optical flow calculations

//3/8/2009 - BR
//	changed to work as addon
//	- update via ofxTSPSPerson
//	- added optical flow velocity

//9/20/2010
//James George
//Updated as addon to be released

#ifndef OFX_PERSON_OBJECT
#define OFX_PERSON_OBJECT

#include "ofMain.h"
#include "ofxOsc.h"
class ofxTSPSPerson
{
  public: 
	ofxTSPSPerson();
	ofxTSPSPerson(int pid, int oid);
	
	~ofxTSPSPerson();
	
	void update(ofxOscMessage* m);
	ofPoint denormalizedCentroid(float width, float height); //used to return screen-space center points
	vector<ofPoint> denormalizedContour(float width, float height); //used to return screen-space contours
	
	//this can be a pointer to whatever you want to store in this person
	void* customAttributes;

	ofPoint opticalFlowVectorAccumulation; //total amount of optical flow within the area of the person

	int pid; //persistent id from frame to frame
	int oid; //ordered id, used for TUIO messaging
	int age; //how many frames has this person been in the system                
	
	ofPoint centroid; //center of mass of the person
	ofRectangle boundingRect; //enclosing area
	vector<ofPoint> contour; //shape contour
	ofPoint velocity; //most recent movement of centroid
	ofPoint opticalFlowVelocity; //accumulated optical flow velocity (set explicitly)
	float area; //area as a scalar size

	long updatedAt;
};

#endif