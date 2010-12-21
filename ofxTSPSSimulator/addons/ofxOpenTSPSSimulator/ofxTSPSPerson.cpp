/***************************************************************************
 *
 *  ofxPerson.cpp
 *  Rockwell LAB + IDEO LAB peopleVision project
 * 
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
 
 * Web: http://code.google.com/p/peoplevision/
 *
 * (...)
 *
 ***************************************************************************/
#include "ofxTSPSPerson.h"


//ofxTSPSPerson::ofxTSPSPerson(int pid, int oid, ofxCvBlob blob)
//: pid(pid),
//  oid(oid),
//  age(0),
//  hasHaar(false),
//  haarRect(ofRectangle(0,0,0,0)),
//  opticalFlowVectorAccumulation(ofPoint(0,0)),
//  centroid(blob.centroid),
//  lastCentroid(blob.centroid),
//  framesWithoutHaar(0),
//  customAttributes(NULL)
//{
//	update(blob, false);
//}

ofxTSPSPerson::ofxTSPSPerson(int pid, int oid)
:	pid(pid),
	oid(oid),
	age(0),
	hasHaar(false),
	haarRect(ofRectangle(0,0,0,0)),
	opticalFlowVectorAccumulation(ofPoint(0,0)),
	centroid(ofPoint(0,0)),
	lastCentroid(ofPoint(0,0)),
	framesWithoutHaar(0),
	customAttributes(NULL)
{};

ofxTSPSPerson::~ofxTSPSPerson()
{

}

ofRectangle ofxTSPSPerson::getBoundingRectNormalized(float videoWidth, float videoHeight)
{
		return ofRectangle(boundingRect.x/videoWidth,
						   boundingRect.y/videoHeight,
						   boundingRect.width/videoWidth,
						   boundingRect.height/videoHeight);
}

ofPoint ofxTSPSPerson::getCentroidNormalized(float videoWidth, float videoHeight)
{
	return ofPoint(centroid.x / videoWidth, centroid.y / videoHeight);
}

ofPoint ofxTSPSPerson::getVelocityNormalized(float videoWidth, float videoHeight)
{
	return ofPoint(velocity.x / videoWidth, velocity.y / videoHeight);
}


