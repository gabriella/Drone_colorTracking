#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

extern "C" {
#include "macGlutfix.h"
}


void ofApp::setup() {
	//cam.initGrabber(640, 480);
    captureWidth = ofGetWidth();
    captureHeight = ofGetHeight();
    image.allocate(captureWidth, captureHeight, OF_IMAGE_COLOR);


	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(200);
	trackingColorMode = TRACK_COLOR_RGB;
}

void ofApp::update() {
	//cam.update();
	//if(cam.isFrameNew()) {
		
	//}
    
    
    captureWidth = ofGetWidth();
    captureHeight = ofGetHeight();
    
	unsigned char * data = pixelsBelowWindow(ofGetWindowPositionX(), ofGetWindowPositionY(), captureWidth, captureHeight);
	
	// now, let's get the R and B data swapped, so that it's all OK:
	for (int i = 0; i < captureWidth * captureHeight; i++){
		
		unsigned char r = data[i*4]; // mem A
		
		data[i*4]   = data[i*4+1];
		data[i*4+1] = data[i*4+2];
		data[i*4+2] = data[i*4+3];
		data[i*4+3] = r;
	}
	
	
	if (data!= NULL) {
		//tex.loadData(data, captureWidth, captureHeight, GL_RGBA);
		//tex.readToPixels(pixels);
		//image = pixels;
		image.setFromPixels(data, captureWidth, captureHeight, OF_IMAGE_COLOR_ALPHA, true);
		image.setImageType(OF_IMAGE_COLOR);
		image.update();
		//finder.findHaarObjects(image.getPixelsRef());
        
        threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
		contourFinder.setThreshold(threshold);
		contourFinder.findContours(image);
		
	}

}

void ofApp::draw() {
	ofSetColor(255);
	image.draw(0, 0);
	contourFinder.draw();
	drawHighlightString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
	drawHighlightString(ofToString((int) threshold) + " threshold", 10, 40);
	drawHighlightString(trackingColorMode == TRACK_COLOR_HSV ? "HSV tracking" : "RGB tracking", 10, 60);
	
	ofTranslate(8, 75);
	ofFill();
	ofSetColor(0);
	ofRect(-3, -3, 64+6, 64+6);
	ofSetColor(targetColor);
	ofRect(0, 0, 64, 64);
}

void ofApp::mousePressed(int x, int y, int button) {
	targetColor = image.getPixelsRef().getColor(x, y);
	contourFinder.setTargetColor(targetColor, trackingColorMode);
}

void ofApp::keyPressed(int key) {
	if(key == 'h') {
		trackingColorMode = TRACK_COLOR_HSV;
	}
	if(key == 'r') {
		trackingColorMode = TRACK_COLOR_RGB;
	}
	contourFinder.setTargetColor(targetColor, trackingColorMode);
}