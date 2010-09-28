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

#include "ofxProjectorBlend.h"

#define FADE_TIME .5

ofxProjectorBlend::ofxProjectorBlend()
{
	showBlend = true;
}

void ofxProjectorBlend::setup(int resolutionWidth, int resolutionHeight, int _pixelOverlap)
{
	height = resolutionHeight;	
	pixelOverlap = _pixelOverlap;
	singleChannelWidth = resolutionWidth;
	fullTextureWidth = resolutionWidth*2 - pixelOverlap;

	fullTexture = new ofxFBOTexture();
	fullTexture->allocate(fullTextureWidth, height, GL_RGB, 0);

	leftChannel = new ofxFBOTexture();
	rightChannel = new ofxFBOTexture();
	leftChannel->allocate(singleChannelWidth, height, GL_RGB, 0);
	rightChannel->allocate(singleChannelWidth, height, GL_RGB, 0);
	
	blendShader = new ofShader();
	blendShader->loadShader("shader/SmoothEdgeBlend");
	
	timeChanged = -FADE_TIME;
	
	showStandBy = false;
	standbyImage = NULL;
	previousStandby = NULL;
	fadePreviousStandby = false;
}

void ofxProjectorBlend::begin()
{
	fullTexture->begin();
	ofPushStyle();
}

void ofxProjectorBlend::fadeInStandby()
{
	fadePreviousStandby = showStandBy;	
	showStandBy = true;
	timeChanged = ofGetElapsedTimef();
}

void ofxProjectorBlend::fadeOutStandby()
{
	if(!showStandBy) return;
	showStandBy = false;
	timeChanged = ofGetElapsedTimef();	
}

void ofxProjectorBlend::end()
{
	fullTexture->end();
	ofPopStyle();
	
	ofPushStyle();{
		ofEnableAlphaBlending();
		ofSetRectMode(OF_RECTMODE_CORNER);

		ofBaseDraws *toDraw;
		ofBaseDraws *fadeOut;
		if(showStandBy){
			toDraw = standbyImage;
			if(fadePreviousStandby){
				fadeOut = previousStandby;
			}
			else {
				fadeOut = fullTexture;
			}
		}
		else {
			toDraw = fullTexture;
			fadeOut = standbyImage;
		}
		
		float fadePoint = ofMap(ofGetElapsedTimef()-timeChanged, 0, FADE_TIME, 0, 1.0, false);
		leftChannel->begin();
		if(fadePoint > 0.0 && fadePoint < 1.0){
			fadeOut->draw(0,0, getCanvasWidth(), getCanvasHeight());
			ofSetColor(255, 255, 255, fadePoint*255);
			toDraw->draw(0,0, getCanvasWidth(), getCanvasHeight());
			ofSetColor(255, 255, 255);
		}
		else {
			toDraw->draw(0,0, getCanvasWidth(), getCanvasHeight());
		}

		leftChannel->end();

		rightChannel->begin();
		if(fadePoint > 0.0 && fadePoint < 1.0){
			fadeOut->draw(-singleChannelWidth+pixelOverlap, 0, getCanvasWidth(), getCanvasHeight());
			ofSetColor(255, 255, 255, fadePoint*255);
			toDraw->draw(-singleChannelWidth+pixelOverlap, 0, getCanvasWidth(), getCanvasHeight());
			ofSetColor(255, 255, 255);
		}
		else {
			toDraw->draw(-singleChannelWidth+pixelOverlap, 0, getCanvasWidth(), getCanvasHeight());
		}
		rightChannel->end();
		
	}ofPopStyle();
}

float ofxProjectorBlend::getCanvasWidth()
{
	return fullTextureWidth;
}

float ofxProjectorBlend::getCanvasHeight()
{
	return height;
}

void ofxProjectorBlend::setStandbyImage(ofImage* _standbyImage)
{
	previousStandby = standbyImage;
	standbyImage = _standbyImage;
}

void ofxProjectorBlend::draw()
{
	ofPushStyle();
	ofSetRectMode(OF_RECTMODE_CENTER);
	
	//center it within the OF canvas, maintaining aspect ratio
	float ofAspect = 1.0*ofGetWidth()/ofGetHeight();  //16x9 would be 1.77777
	float canvasAspect = singleChannelWidth*2/height;  //2560x960 2.6667

	//cout << "canvas aspect is " << canvasAspect << " of aspect is " << ofAspect << " OF: " << ofGetWidth() << "x" << ofGetHeight() << endl;
	
	float w,h;
	if(ofAspect < canvasAspect){
		w = ofGetWidth();
		h = -w/canvasAspect;
	}
	else {
		h = -ofGetHeight();
		w = -h*canvasAspect;
	}
				   
	float x = ofGetWidth()/2.0;
	float y = ofGetHeight()/2.0;
	
	ofSetColor(0, 0, 0);
	ofRect(x, y, ofGetWidth(), ofGetHeight());
	ofSetColor(255, 255, 255);

	if(showBlend){

		glActiveTexture(GL_TEXTURE0);	
		blendShader->setShaderActive(true);
		
		blendShader->setUniform("Tex0", 0);
		blendShader->setUniform("width", singleChannelWidth);
		blendShader->setUniform("height", height);
		
		blendShader->setUniform("OverlapLeft", 0.0f);
		blendShader->setUniform("OverlapRight", pixelOverlap);
		blendShader->setUniform("OverlapTop", 0.0f);
		blendShader->setUniform("OverlapBottom", 0.0f);
		
		blendShader->setUniform("BlackOutLeft", 0.0f);
		blendShader->setUniform("BlackOutRight", 0.0f);
		blendShader->setUniform("BlackOutTop", 0.0f);
		blendShader->setUniform("BlackOutBottom", 0.0f);
		
		blendShader->setUniform("BlendPower", blendPower);
		blendShader->setUniform("SomeLuminanceControl", luminance);
		blendShader->setUniform("GammaCorrection", gamma, gamma, gamma);
		blendShader->setUniform("SolidEdgeEnable", 0.0f);
		
		blendShader->setUniform("SolidEdgeColor", 0.0f, 0.0f, 0.0f, 1.0f);
		
		leftChannel->draw(x-w/4, y, w/2, h);
		
		blendShader->setUniform("OverlapLeft", pixelOverlap);
		blendShader->setUniform("OverlapRight", 0.0f);
		
		rightChannel->draw(x+w/4, y, w/2, h);
		
		blendShader->setShaderActive(false);
	}
	else{
		//calculate the real single channel width based on the height
		w = -singleChannelWidth * ( h / getCanvasHeight());
		float scaledPixelOverlap = pixelOverlap *  w / singleChannelWidth;
		leftChannel->draw( x-w/2+scaledPixelOverlap/2, y, w, h);
		rightChannel->draw(x+w/2-scaledPixelOverlap/2, y, w, h);
	}
	
	ofPopStyle();
}

#ifdef USE_SIMPLE_GUI
void ofxProjectorBlend::addGuiPage()
{
	gui.addPage("Projector Blend");
	gui.addToggle("Show Blend", showBlend);
	gui.addToggle("Show Standby", showStandBy);
	
	gui.addSlider("Blend Power", blendPower, 0.0, 4.0);
	gui.addSlider("Gamma", gamma, 0.0, 4.0);
	gui.addSlider("Luminance", luminance, 0.0, 4.0);
		
	gui.page("Projector Blend").setXMLName("../../../ProjectorBlendSettings.xml");
}	

#endif

