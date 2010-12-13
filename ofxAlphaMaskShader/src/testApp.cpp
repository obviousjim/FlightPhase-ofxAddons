/*
 *  ofxAlphaMaskShader.h
 *
 * Created by James George, http://www.jamesgeorge.org
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
 * ofxAlphaMaskShader is not really an addon, but an example
 * of how to use a shader to have one image become the alpha
 * channel of another.
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	//need this for alpha to come through
	ofEnableAlphaBlending();
	
	//we use 3 images, a bottom layer, a top layer, and a mask image
	//the bottom layer is always drawn first
	//then the top layer is drawn over it,using the mask to punch out
	//some of the alpha
	//in this example, they are all the same size. We move the mask
	//to match the mouse so it looks like you can x-ray through the hand
	
	topLayer.loadImage("topLayer.png");
	mask.loadImage("mask.png");
	bottomLayer.loadImage("bottomLayer.png");

	//set the texture parameters for the maks shader. just do this at the beginning
	maskShader.setup("composite.vert", "composite.frag");
	maskShader.begin();
	maskShader.setUniformTexture("Tex0", topLayer.getTextureReference(), 0);
	maskShader.setUniformTexture("Tex1", mask.getTextureReference(), 1);
	maskShader.end();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	//first draw the bottom layer
	bottomLayer.draw(0, 0);
	

	//then draw a quad for the top layer using our composite shader to set the alpha
	maskShader.begin();
	
	//our shader uses two textures, the top layer and the alpha
	//we can load two textures into a shader using the multi texture coordinate extensions
	glActiveTexture(GL_TEXTURE0_ARB);
	topLayer.getTextureReference().bind();

	glActiveTexture(GL_TEXTURE1_ARB);
	mask.getTextureReference().bind();

	//draw a quad the size of the frame
	glBegin(GL_QUADS);
	
	//move the mask around with the mouse by modifying the texture coordinates
	float maskOffset = 15 - mouseY;
	glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
	glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, maskOffset);		
	glVertex2f( 0, 0);

	glMultiTexCoord2d(GL_TEXTURE0_ARB, topLayer.getWidth(), 0);
	glMultiTexCoord2d(GL_TEXTURE1_ARB, mask.getWidth(), maskOffset);		
	glVertex2f( ofGetWidth(), 0);

	glMultiTexCoord2d(GL_TEXTURE0_ARB, topLayer.getWidth(), topLayer.getHeight());
	glMultiTexCoord2d(GL_TEXTURE1_ARB, mask.getWidth(), mask.getHeight() + maskOffset);
	glVertex2f( ofGetWidth(), ofGetHeight());

	glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, topLayer.getHeight());
	glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, mask.getHeight() + maskOffset);		
	glVertex2f( 0, ofGetHeight() );
	
	glEnd();
	
	//deactive and clean up
	glActiveTexture(GL_TEXTURE1_ARB);
	mask.getTextureReference().unbind();
	
	glActiveTexture(GL_TEXTURE0_ARB);
	topLayer.getTextureReference().unbind();
	
	maskShader.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

