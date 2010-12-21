/*
 *  ofxTSPSMockPerson.cpp
 *  openTSPSTester
 *
 *  Created by James George on 6/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTSPSMockPerson.h"

ofxTSPSMockPerson::ofxTSPSMockPerson(int uid, int oid, ofxVec2f centroidOnCanvas, ofRectangle canvas, ofRectangle interactionArea)
	: ofxTSPSPerson(uid, oid), canvas(canvas), centroidOnCanvas(centroidOnCanvas), interactionArea(interactionArea)
{
	moveToLocation = lastPointOnCanvas = centroidOnCanvas;
}

void ofxTSPSMockPerson::update()
{
	lastPointOnCanvas = centroidOnCanvas;
	centroidOnCanvas += (moveToLocation - centroidOnCanvas) * 0.1f;
}

void ofxTSPSMockPerson::setMoveToLocation(ofxVec2f locationOnCanvas)
{
	moveToLocation = locationOnCanvas;
}

bool ofxTSPSMockPerson::isInInteractionArea()
{
	return pointIsInInteractionArea(centroidOnCanvas);
}

bool ofxTSPSMockPerson::didEnterInteractionArea()
{
	return !pointIsInInteractionArea(lastPointOnCanvas) && pointIsInInteractionArea(centroidOnCanvas);	
}

bool ofxTSPSMockPerson::didLeaveInteractionArea()
{
	return pointIsInInteractionArea(lastPointOnCanvas) && !pointIsInInteractionArea(centroidOnCanvas);
}


bool ofxTSPSMockPerson::attemptSelect(ofxVec2f mousePoint)
{
	selected = centroidOnCanvas.distance(mousePoint) < 10;
	return selected;
}

bool ofxTSPSMockPerson::isSelected()
{
	return selected;
}

ofPoint ofxTSPSMockPerson::getCentroidToTransmit()
{
	ofxVec2f centroidToSend = didLeaveInteractionArea() ? lastPointOnCanvas : centroidOnCanvas;
	return ofPoint( ofMap(centroidToSend.x-interactionArea.x, 0, interactionArea.width, 0, 1.0, true),
				    ofMap(centroidToSend.y-interactionArea.y, 0, interactionArea.height, 0, 1.0, true));
}

ofPoint ofxTSPSMockPerson::getCentroidToDraw()
{
	return ofPoint(centroidOnCanvas.x, centroidOnCanvas.y);
}

bool ofxTSPSMockPerson::pointIsInInteractionArea(ofxVec2f point)
{
	return	point.x > interactionArea.x && 
			point.x < interactionArea.x + interactionArea.width &&
			point.y > interactionArea.y && 
			point.y < interactionArea.y + interactionArea.height;
}
