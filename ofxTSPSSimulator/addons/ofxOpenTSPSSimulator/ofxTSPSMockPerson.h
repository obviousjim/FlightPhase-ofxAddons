/*
 *  ofxTSPSMockPerson.h
 *  openTSPSTester
 *
 *  Created by James George on 6/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef OFX_TSPS_MOCKPERSON
#define OFX_TSPS_MOCKPERSON

#include "ofMain.h"
#include "ofxTSPSPerson.h"
#include "ofxVectorMath.h"

class ofxTSPSMockPerson : public ofxTSPSPerson
{
  public:
	ofxTSPSMockPerson(int uid, int oid, ofxVec2f centroidOnCanvas, ofRectangle canvas, ofRectangle interactionArea);
		
	void update();
	void setMoveToLocation(ofxVec2f locationOnCanvas);
	
	bool attemptSelect(ofxVec2f mousePoint);
	bool isSelected();
	
	bool isInInteractionArea();
	
	//call *after* update to see changes
	bool didEnterInteractionArea();
	bool didLeaveInteractionArea();
	
	ofPoint getCentroidToTransmit();
	ofPoint getCentroidToDraw();

  protected:
	bool pointIsInInteractionArea(ofxVec2f point);
	
	ofRectangle canvas;
	ofRectangle interactionArea;
	ofxVec2f moveToLocation;
	ofxVec2f centroidOnCanvas;
	ofxVec2f lastPointOnCanvas;
	bool selected;
};
#endif;