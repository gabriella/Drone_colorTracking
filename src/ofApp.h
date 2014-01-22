#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxARDrone.h"


class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);
	void keyPressed(int key);
	
    

	ofVideoGrabber cam;
	ofxCv::ContourFinder contourFinder;
	float threshold;
	ofxCv::TrackingColorMode trackingColorMode;
	ofColor targetColor;
    
    //screengrab
    int captureWidth;
    int captureHeight;
    
    ofImage image;
    
    //drone variables
    ofxARDrone::Drone drone;//if this is initialized after ofxcv is then things seem ok. Why? if not then it crashes [note: look up the error]
        bool doPause;
        bool keys[65535];
    
       //debug mode
        bool debug;
    

};
