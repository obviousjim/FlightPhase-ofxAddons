/**
 *  ofxProjectorBlend
 *  
 * Copyright 2010 (c) James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
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
 * 
 * This class is an openFrameworks wrapper for this technique:
 * from http://local.wasp.uwa.edu.au/~pbourke/texture_colour/edgeblend/
 * Original Core Image filter converted by James George ( http://www.jamesgeorge.org/ )
 * for ofxProjectorBlend addon for openFrameworks ( http://www.openframeworks.cc )
 *
 * Originally used in the Infiniti MOI project for projector blending ( http://vimeo.com/14588336 )
 *
 * This shader is capable of blending on all sides, but ofxProjectorBlend currently
 * just uses the shader for two projections at once.
 *
 */


#ifndef OFX_PROJECTOR_BLEND
#define OFX_PROJECTOR_BLEND

//Uncomment this to access the addGuiPage() method
//that will add a simple page to your SimpleGUI allowing
//control of the projector blend through the interface
//#define USE_SIMPLE_GUI

#include "ofxFBOTexture.h"
#include "ofShader.h"

#ifdef USE_SIMPLE_GUI
#include "ofxSimpleGuiToo.h"
#endif


class ofxProjectorBlend
{
  public:
	ofxProjectorBlend();
	void setup(int resolutionWidth, int resolutionHeight, int pixelOverlap);
	void begin();
	void end();
	//void draw(float x, float y, float w, float h);
	void draw();
	
	float getCanvasWidth();
	float getCanvasHeight();
	
	//useful function for setting a standby image that is blended across
	void setStandbyImage(ofImage* standbyImage);

	void fadeInStandby();
	void fadeOutStandby();
	
	bool showBlend;
	bool showStandBy;
	
	//variables that control the blend
	float blendPower;
	float gamma;
	float luminance;
	
#ifdef USE_SIMPLE_GUI
	void addGuiPage();
#endif
	
  protected:
	
	float fullTextureWidth;
	float singleChannelWidth;
	float height;
	float pixelOverlap;
	
	ofShader* blendShader;
	ofxFBOTexture* fullTexture;
	ofxFBOTexture* leftChannel;
	ofxFBOTexture* rightChannel;
	
	bool fadePreviousStandby;
	ofImage* standbyImage;	
	ofImage* previousStandby;
	float timeChanged;
};

#endif