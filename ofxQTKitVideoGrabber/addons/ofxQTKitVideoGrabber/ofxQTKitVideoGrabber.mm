/*
 * ofxQTKitVideoGrabber.cpp
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
 * ofxQTKitVideoGrabber works exactly the same as the standard ofMovieGrabber
 * but uses the QTKit Objective-C Libraries to drive the video display.
 * These libraries are naturally GPU enabled, multi-threaded, as well
 * as supporting more Quicktime capture codecs such as HDV.
 *
 * You will need to add the QTKit.framework and CoreVide.framework
 * to the openFrameworks Xcode project
 *
 * Requires Mac OS 10.5 or greater
 */

#include "ofxQTKitVideoGrabber.h"
#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>
#import <QuickTime/QuickTime.h>

static inline void argb_to_rgb(unsigned char* src, unsigned char* dst, int numPix)
{
	for(int i = 0; i < numPix; i++){
		memcpy(dst, src+1, 3);
		src+=4;
		dst+=3;
	}	
}

@interface QTKitVideoGrabber : QTCaptureVideoPreviewOutput
{
    QTCaptureSession *session;
	QTCaptureDeviceInput *videoDeviceInput;
	NSInteger width, height;
	NSInteger deviceID;
	
	CVImageBufferRef cvFrame;
	ofTexture* texture;
	unsigned char* pixels;	

	BOOL isRunning;
	BOOL hasNewFrame;
	BOOL isFrameNew;
	BOOL useTexture;
	BOOL verbose;
}

@property(nonatomic, readonly) NSInteger height;
@property(nonatomic, readonly) NSInteger width;
@property(readwrite) NSInteger deviceID;
@property(retain) QTCaptureSession* session;
@property(nonatomic, retain) QTCaptureDeviceInput* videoDeviceInput;
@property(nonatomic, readonly) BOOL isRunning;
@property(readonly) unsigned char* pixels;
@property(readonly) ofTexture* texture;
@property(readonly) BOOL isFrameNew;
@property(nonatomic, readwrite) BOOL verbose;
@property(nonatomic, readwrite) BOOL useTexture;

+ (void) listDevices;

- (id) initWithWidth:(NSInteger)width 
			  height:(NSInteger)height 
			  device:(NSInteger)deviceID
		usingTexture:(BOOL)_useTexture;

- (void) outputVideoFrame:(CVImageBufferRef)videoFrame 
		 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
		   fromConnection:(QTCaptureConnection *)connection;

- (bool) setSelectedVideoDevice:(QTCaptureDevice *)selectedVideoDevice;

- (void) videoSettings;

- (void) update;

- (void) stop;

@end


@implementation QTKitVideoGrabber
@synthesize width, height;
@synthesize deviceID;
@synthesize session;
@synthesize videoDeviceInput;
@synthesize pixels;
@synthesize texture;
@synthesize isFrameNew;
@synthesize verbose;
@synthesize useTexture;

+ (void) listDevices
{
	NSLog(@"ofxQTKitVideoGrabber devices %@", 
		  [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
		   arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]]);
	
}

- (id) initWithWidth:(NSInteger)_width height:(NSInteger)_height device:(NSInteger)_deviceID usingTexture:(BOOL)_useTexture
{
	if(self = [super init]){
		//configure self
		width = _width;
		height = _height;
		
		//instance variables
		cvFrame = NULL;
		hasNewFrame = false;
		texture = NULL;
		self.useTexture = _useTexture;
		
		[self setPixelBufferAttributes: [NSDictionary dictionaryWithObjectsAndKeys: 
										 [NSNumber numberWithInt: kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
										 [NSNumber numberWithInt:width], kCVPixelBufferWidthKey, 
										 [NSNumber numberWithInt:height], kCVPixelBufferHeightKey, 
										 [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey,
										 nil]];	
		
		pixels = (unsigned char*)calloc(sizeof(char), _width*_height*3);
		
		//init the session
		self.session = [[[QTCaptureSession alloc] init] autorelease];
		
		NSError* error;
		bool success = [self.session addOutput:self error:&error];
		if( !success ){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error creating capture session");
			return nil;
		}
		
		deviceID = -1;		
		[self setDeviceID:_deviceID];
		
		[self startSession];
	}
	return self;
}


- (void) startSession
{
	//start the session
	NSLog(@"starting video session");
	[session startRunning];
	
}

- (void) setDeviceID:(NSInteger)_deviceID
{	
	if(deviceID != _deviceID){
		
		//get video device
		NSArray* videoDevices = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] 
								 arrayByAddingObjectsFromArray:[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeMuxed]];
		
		if(verbose) ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Device List:  %s", [[videoDevices description] cString]);
		
		// Try to open the new device
		if(_deviceID >= videoDevices.count){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error device ID out of range");
			return;
		}		
		QTCaptureDevice* selectedVideoDevice = [videoDevices objectAtIndex:_deviceID];
		if([self setSelectedVideoDevice:selectedVideoDevice]){
			deviceID = _deviceID;
		}
	}
}

- (bool) setSelectedVideoDevice:(QTCaptureDevice *)selectedVideoDevice
{
	BOOL success = YES;	
	if (self.videoDeviceInput) {
		// Remove the old device input from the session and close the device
		[self.session removeInput:videoDeviceInput];
		[[self.videoDeviceInput device] close];
		[videoDeviceInput release];
		videoDeviceInput = nil;
	}
	
	if (selectedVideoDevice) {
		NSError *error = nil;
		
		// Try to open the new device
		success = [selectedVideoDevice open:&error];
		if(success){
			// Create a device input for the device and add it to the session
			self.videoDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:selectedVideoDevice];
			
			success = [self.session addInput:self.videoDeviceInput error:&error];
			if(verbose) ofLog(OF_LOG_VERBOSE, "ofxQTKitVideoGrabber -- Attached camera %s", [[selectedVideoDevice description] cString]);
		}
	}
	
	if(!success) ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber - ERROR - Error adding device to session");	

	return success;
}

//Frame from the camera
//this tends to be fired on a different thread, so keep the work really minimal
- (void) outputVideoFrame:(CVImageBufferRef)videoFrame 
		 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
		   fromConnection:(QTCaptureConnection *)connection
{
	CVImageBufferRef toRelease;	
	@synchronized(self){
		toRelease = cvFrame;
		CVBufferRetain(videoFrame);
		cvFrame = videoFrame;
		hasNewFrame = YES;
		if(toRelease != NULL){
			CVBufferRelease(toRelease);
		}
	}	
}

- (void) setUseTexture:(BOOL)_useTexture
{
	if(_useTexture && texture == NULL){
		texture = new ofTexture();
		texture->allocate(self.width, self.height, GL_RGB);
	}
	useTexture = _useTexture;
}

- (void) update
{
	@synchronized(self){
		if(hasNewFrame){
			CVPixelBufferLockBaseAddress(cvFrame, 0);
			unsigned char* src = (unsigned char*)CVPixelBufferGetBaseAddress(cvFrame);
			
			//I wish this weren't necessary, but
			//in my tests the only performant & reliabile
			//pixel format for QTCapture is k32ARGBPixelFormat, 
			//to my knowledge there is only RGBA format
			//available to gl textures
			
			//convert pixels from ARGB to RGB			
			argb_to_rgb(src, pixels, width*height);
			if(self.useTexture){
				texture->loadData(pixels, width, height, GL_RGB);
			}
			CVPixelBufferUnlockBaseAddress(cvFrame, 0);
			hasNewFrame = NO;
			isFrameNew = YES;
		}
		else{
			isFrameNew = NO;
		}
	}	
}

/**
 * JG:
 * This is experimental and doesn't quite work yet --
 *
 * Bring up the oldschool video setting dialog.
 * It just gets a pointer to the underlying SequenceGrabber
 * component from within QTKit.
 * this doesn't seem to work for all cameras, for example my macbook iSight the pointer is null
 * but it does work with the Macam driver for the PS3Eye which is the important one at the moment
 */
- (void) videoSettings
{
	NSDictionary* attr = [self.videoDeviceInput.device deviceAttributes];
	if (attr == NULL) {
		ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Warning: Video Settings not available for this camera");
		return;
	}
	
	NSValue* sgnum = [attr objectForKey:QTCaptureDeviceLegacySequenceGrabberAttribute];
	if (sgnum == NULL) {
		ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Warning: Video Settings not available for this camera");
		return;
	}
	
	
	OSErr err;
	SeqGrabComponent sg = (SeqGrabComponent)[sgnum pointerValue];
	SGChannel chan;
	OSType type;
	
	err = SGPause (sg, true);
	if(err){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Could not pause for video settings");
	}
	
	static SGModalFilterUPP gSeqGrabberModalFilterUPP = NewSGModalFilterUPP(SeqGrabberModalFilterUPP);
	err = SGGetIndChannel(sg, 1, &chan, &type );
	if (err == noErr){
		ComponentResult result = SGSettingsDialog(sg, chan, 0, NULL, 0, gSeqGrabberModalFilterUPP, 0 );
		if(result != noErr){
			ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Error in Sequence Grabber Dialog");
		}
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Could not init channel");
	}
	
	SGPause(sg, false);
	
}

- (void) stop
{
	if(self.isRunning){
		[self.session stopRunning];
	}	
	
	self.session = nil;
	
	free(pixels);
	if(texture != NULL){
		delete texture;
	}
}


- (BOOL) isRunning
{
	return self.session && self.session.isRunning;
}

@end

//C++ Wrapper class:
ofxQTKitVideoGrabber::ofxQTKitVideoGrabber()
{
	deviceID = 0;
	grabber = NULL;
	isInited = false;
	bUseTexture = true;
}

ofxQTKitVideoGrabber::~ofxQTKitVideoGrabber()
{
	if(isInited){
		close();		
	}
}

void ofxQTKitVideoGrabber::setDeviceID(int _deviceID)
{
	if(isInited){
		grabber.deviceID = _deviceID;
		deviceID = grabber.deviceID;
	}
	else{
		deviceID = _deviceID;
	}		
}

void ofxQTKitVideoGrabber::initGrabber(int w, int h)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	grabber = [[QTKitVideoGrabber alloc] initWithWidth:w height:h device:deviceID usingTexture:bUseTexture];
	
	isInited = (grabber != nil);
	
	[pool release];	
}


void ofxQTKitVideoGrabber::update(){ 
	grabFrame(); 
}

void ofxQTKitVideoGrabber::grabFrame()
{
	if(confirmInit()){
		[grabber update];
	}
}

bool ofxQTKitVideoGrabber::isFrameNew()
{
	return isInited && [grabber isFrameNew];
}

void ofxQTKitVideoGrabber::listDevices()
{
	[QTKitVideoGrabber listDevices];	
}

void ofxQTKitVideoGrabber::close()
{	
	[grabber stop];
	[grabber release];
	isInited = false;	
}

unsigned char* ofxQTKitVideoGrabber::getPixels()
{
	if(confirmInit()){
		return [grabber pixels];
	}
	return NULL;
}

void ofxQTKitVideoGrabber::setUseTexture(bool _bUseTexture)
{
	if(_bUseTexture != bUseTexture){
		if(isInited){
			grabber.useTexture = _bUseTexture;
		}
		bUseTexture = _bUseTexture;
	}
}

ofTexture&	ofxQTKitVideoGrabber::getTextureReference()
{
	if(!bUseTexture){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- Requesting texture while use texture is false");
	}
	if(confirmInit() && bUseTexture){
		return *[grabber texture];
	}
}

void ofxQTKitVideoGrabber::setVerbose(bool bTalkToMe)
{
	if(confirmInit()){
		grabber.verbose = bTalkToMe;
	}
}

void ofxQTKitVideoGrabber::videoSettings()
{
	if(confirmInit()){
		NSLog(@"loading video settings");
		[grabber videoSettings];
	}
}

void ofxQTKitVideoGrabber::draw(float x, float y, float w, float h)
{
	if(confirmInit()){
		[grabber texture]->draw(x, y, w, h);
	}
}

void ofxQTKitVideoGrabber::draw(float x, float y)
{
	if(confirmInit()){
		[grabber texture]->draw(x, y);
	}
}

int ofxQTKitVideoGrabber::getDeviceID()
{
	if(confirmInit()){
		return grabber.deviceID;
	}
	return -1;
}

float ofxQTKitVideoGrabber::getHeight()
{
	if(confirmInit()){
		return float(grabber.height);
	}
	return 0;
}

float ofxQTKitVideoGrabber::getWidth()
{
	if(confirmInit()){
		return float(grabber.width);
	}
	return 0;
	
}
		  
bool ofxQTKitVideoGrabber::confirmInit()
{
	if(!isInited){
		ofLog(OF_LOG_ERROR, "ofxQTKitVideoGrabber -- ERROR -- Calling method on non intialized video grabber");
	}
	return isInited;
}

