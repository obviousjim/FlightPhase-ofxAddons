#ifndef _TEST_APP
#define _TEST_APP

/**
 *  testApp.h
 *
 *	ofxQTKitveVideoGrabber example project
 *
 *  Created by James George ( http://www.jamesgeorge.org ) on 6/15/10.
 *  In collaboration with FlightPhase ( http://www.flightphase.com )
 *
 *  Copyright (c) 2010 
 */

#include "ofMain.h"
#include "ofxQTKitVideoGrabber.h"

#define CAM_WIDTH 320
#define CAM_HEIGHT 240

class testApp : public ofBaseApp{
	
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	ofxQTKitVideoGrabber	vidGrabber;	
	unsigned char * 	videoInverted;
	ofTexture			videoTexture;
	int 				camWidth;
	int 				camHeight;
};

#endif	
