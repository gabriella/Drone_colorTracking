#pragma once


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxARDrone.h"
#include "ofxCv.h"


class ofApp : public ofBaseApp{
    
public:
    
    
    
    //drone variables
    ofxARDrone::Drone drone;
    bool doPause;
    bool keys[65535];
    bool isTracking;
    
    //TCP variables
    ofxTCPClient tcpClient;
    string msgTx, msgRx;
    ofTrueTypeFont  mono;
    ofTrueTypeFont  monosm;
    float counter;
    int connectTime;
    int deltaTime;
    bool weConnected;
    int size;
    int pos;
    bool typed;
    
    
    //ScreenGrab Stuff
    int captureWidth, captureHeight;
    
    ofImage image;
    ofxCvHaarFinder finder;
    
   // ofxCV stuff
    ofxCv::ContourFinder contourFinder;
	float threshold;
	ofxCv::TrackingColorMode trackingColorMode;
	ofColor targetColor;
    
    
    //OpenCV Stuff
    
    ofxCvColorImage colorImg;
    ofImage img;
    ofPixels pixels;
    
    float myArea;
    ofPoint myCentroid;
    
    bool debug;
   
    bool isScanning;
    int num;
    float s;
    
    void checkContours();
    void scanning();
    cv::Point2f getCenterRect();
    
   // void tracking(cv::Rect myBlob);
    void trackingCentroid(cv::Point2f blob);
    
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
};
