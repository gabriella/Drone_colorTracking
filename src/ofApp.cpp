#include "ofApp.h"
using namespace cv;
using namespace ofxCv;


extern "C" {
#include "macGlutfix.h"
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    s = 0.01;
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //Clear all keys
    memset(keys, 0, sizeof(*keys));
    
    doPause = false;
    debug = false;
    drone.connect();
    
    isTracking = false;
    isScanning = false;
    
    myArea = 0;
    num = 0;

    
    // setup command history lengths for debugging and dumping onscreen (OPTIONAL)
    //drone.controller.commandHistory.setMaxLength(30);
    //drone.dataReceiver.commandHistory.setMaxLength(30);}
    
    /*
     //-----------------------------
     //TCP stuff
     //have we typed?
     typed = false;
     
     //our typing position
     pos = 0;
     
     //send and receive strings
     msgTx = "";
     msgRx = "";
     
     //are we connected to the server - if this fails we
     //will check every few seconds to see if the server exists
     weConnected = tcpClient.setup("192.168.1.1", 5555);
     //optionally set the delimiter to something else.  The delimter in the client and the server have to be the same
     tcpClient.setMessageDelimiter("\n");
     
     connectTime = 0;
     deltaTime = 0;
     
     tcpClient.setVerbose(true);
     */
    
    
    
    
    //ScreenGrab--------------------------------
    captureWidth = ofGetWidth();
    captureHeight = ofGetHeight();
    image.allocate(captureWidth, captureHeight, OF_IMAGE_COLOR);
    //pixels.allocate(captureWidth, captureHeight, OF_IMAGE_COLOR);
    
    //cv
	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(200);
	trackingColorMode = TRACK_COLOR_RGB;
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    //DRONE STUFF-----------------------------------------
    
    
    
    if(doPause) return;
    
    {
        float s = 0.02;
        
        if(keys[OF_KEY_UP]) drone.controller.pitchAmount -= s;
        else if(keys[OF_KEY_DOWN]) drone.controller.pitchAmount += s;
        
        if(keys['a']) drone.controller.rollAmount -= s;
        else if(keys['d']) drone.controller.rollAmount += s;
        
        if(keys['w']) drone.controller.liftSpeed += s;
        else if(keys['s']) drone.controller.liftSpeed -= s;
        
        if(keys[OF_KEY_LEFT]) drone.controller.spinSpeed -= s;
        else if(keys[OF_KEY_RIGHT]) drone.controller.spinSpeed += s;
        
    }
    
    // update the drone (process and send queued commands to drone, receive commands from drone and update state
    
    if(drone.state.isFlying() && drone.state.getAltitude() > 730)
    {
        cout << "scanning!" << endl;
        isScanning = true;
       
    }

  
    if(isScanning){
        scanning();
    }
    
    if(isTracking){
    
    }
    
    drone.update();
    
    /*
     
     
     //TCP STUFF--------------------------------------------
     
     bool dataRecd = false;
     unsigned char buffer[32];
     int recd = 32;
     int totalReceived = 0;
     int messageSize = 0;
     while(recd > 0) {
     
     if(recd > messageSize  ){
     tcpClient.receiveRawBytes( (char*) &buffer[totalReceived], messageSize);
     recd -= messageSize;
     totalReceived += messageSize;
     }
     else{
     tcpClient.receiveRawBytes( (char*) &buffer[totalReceived], recd);
     totalReceived += recd;
     recd = 0;
     dataRecd = true;
     }
     }
     
     if(dataRecd){
     
     colorImg.setFromPixels( &buffer[0], 50, 52); //,OF_IMAGE_COLOR);
     }
     */
    
    /*
     //we are connected - lets send our text and check what we get back
     if(weConnected){
     if(tcpClient.send(msgTx)){
     
     //if data has been sent lets update our text
     string str = tcpClient.receive();
     if( str.length() > 0 ){
     msgRx = str;
     ofBuffer databuffer = str;
     
     }
     }else if(!tcpClient.isConnected()){
     weConnected = false;
     }
     }else{
     //if we are not connected lets try and reconnect every 5 seconds
     deltaTime = ofGetElapsedTimeMillis() - connectTime;
     
     if( deltaTime > 5000 ){
     weConnected = tcpClient.setup("192.161.1.1", 5555);
     connectTime = ofGetElapsedTimeMillis();
     }
     
     }
     
     */
    
    
    //ScreenGrabStuff-----------------------------------------
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
        finder.findHaarObjects(image.getPixelsRef());
     
        
        
        threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
		contourFinder.setThreshold(threshold);
		contourFinder.findContours(image);
        
        //cv::Rect myBlob = getBoundingRect();
        
       // if (!tracking){
        cv::Point2f blob = getCenterRect();
        //tracking(myBlob);
        trackingCentroid(blob);
        
        
    }
    else {return;}
    //cout << imageBelowWindow()[0] << endl;
     
     
    
    
    
}



//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    //DRONE DEBUG
    
    // draw debug strings
    string controllerString = "fps: " + ofToString(ofGetFrameRate()) + "\n";
    controllerString += "millisSinceLastSend: " + ofToString(drone.controller.getMillisSinceLastSend()) + "\n";
    controllerString += "\n";
    controllerString += "takeOff (t)\n";
    controllerString += "land (l)\n";
    controllerString += "calibrateHorizontal (c)\n";
    controllerString += "calibrateMagnetometer (m)\n";
    controllerString += "EMERGENCY (E)\n";
    controllerString += "\n";
    controllerString += "roll (a/d)        : " + ofToString(drone.controller.rollAmount) + "\n";
    controllerString += "pitch (up/down)   : " + ofToString(drone.controller.pitchAmount) + "\n";
    controllerString += "lift (w/s)        : " + ofToString(drone.controller.liftSpeed) + "\n";
    controllerString += "spin (left/right) : " + ofToString(drone.controller.spinSpeed) + "\n";
    controllerString += "\n";
    controllerString += "reset droneSimulator (r)\n";
    controllerString += "debug history (h)\n";
    controllerString += "fullscreen (f)\n";
    controllerString += "PAUSE (p)\n";
    
    ofxARDrone::State &state = drone.state;
    string stateString = "";
    stateString += "isFlying : " + ofToString(state.isFlying()) + "\n";
    stateString += "isTakingOff : " + ofToString(state.isTakingOff()) + ", " + ofToString(state.isTakingOffMillis()) + "\n";
    stateString += "isLanding : " + ofToString(state.isLanding()) + ", " + ofToString(state.isLandingMillis()) + "\n";
    stateString += "isCalibratingHorizontal : " + ofToString(state.isCalibratingHorizontal()) + ", " + ofToString(state.isCalibratingHorizontalMillis()) + "\n";
    stateString += "isCalibratingMagnetometer : " + ofToString(state.isCalibratingMagnetometer()) + ", " + ofToString(state.isCalibratingMagnetometerMillis()) + "\n";
    
    
    stateString += "\n\nisConnected: " + ofToString(state.isConnected()) + ", " + ofToString(state.isCalibratingMagnetometerMillis()) + "\n";
    stateString += "altitude: "+ ofToString(state.getAltitude())+"\n";
    stateString += "emergency state: "+ ofToString(state.inEmergencyMode())+"\n";
    stateString += "battery level: "+ ofToString(state.getBatteryPercentage())+"%\n";
    stateString += "vx: "+ ofToString(state.getVx())+" vy: "+ ofToString(state.getVy())+" vz: "+ ofToString(state.getVz())+"\n";
    
    /*
     //TCP String
     string TCPstring = "";
     TCPstring += "TCP Status\n";
     TCPstring += "Connected : " + ofToString(weConnected) + "\n";
     TCPstring += "Sending : " + msgTx + "\n";
     TCPstring += "From Server : " + msgRx + "\n";
     */
    
     
    
    
    //ScreenGrabStuff-------------------------------------------
    ofSetColor(255);
    
	image.draw(0, 0);
	
    cv::Point2f blob = getCenterRect();
    ofSetColor(255, 0, 0);
    ofFill();
    ofEllipse(blob.x, blob.y, 50, 50);
    
    if(debug == true){
     
        
        //Drone Stats
        ofSetColor(0, 0, 0, 200);
        ofFill();
        ofRect(8, 10, 220, 450);
        ofRect(ofGetWidth()-300, 10, 300, 250);
        
        
         ofSetColor(255);
        ofDrawBitmapString(controllerString, 10, 30);
        ofDrawBitmapString(stateString, ofGetWidth()-300, 30);
        // ofDrawBitmapString(TCPstring, 10, 400);
        
        ofDrawBitmapString(drone.controller.commandHistory.getAsString(), 10, 280);
        ofDrawBitmapString(drone.dataReceiver.commandHistory.getAsString("\n"), ofGetWidth()-300, 280);
        
        
        //Contour Finder
        contourFinder.draw();
        drawHighlightString(ofToString((int) ofGetFrameRate()) + " fps", 10, 300);
        drawHighlightString(ofToString((int) threshold) + " threshold", 10, 320);
        drawHighlightString(trackingColorMode == TRACK_COLOR_HSV ? "HSV tracking" : "RGB tracking", 10, 340);
        
        
        ofTranslate(10, 360);
        ofFill();
        ofSetColor(0);
        ofRect(-3, -3, 64+6, 64+6);
        ofSetColor(targetColor);
        ofRect(0, 0, 64, 64);
        ofNoFill();
         
        
        
    }
    for(int i = 0; i < contourFinder.getBoundingRects().size(); i++){
    ofSetColor(127);
        
        //ofCircle(  Point<float>(contourFinder.getCentroid(i)), 25);

    }
//
//    //for each face "blob" we found, draw a rectangle around the face
//    //#2
//    for(int i = 0; i < finder.blobs.size(); i++) {
//        ofRect(finder.blobs[i].boundingRect);
//    }
    
    
}
//--------------------------------------------------------------

void ofApp::scanning(){
    
    
    
    
    drone.controller.spinSpeed += s;
    
    
    if( drone.controller.spinSpeed > 0.5 || drone.controller.spinSpeed < -0.5){
        cout << "REVERSE!" << endl;
        drone.controller.spinSpeed = 0;
        s = s*-1;
    }
    /*
    float time = ofGetElapsedTimeMillis();
    while(ofGetElapsedTimeMillis() - time < 2000){
    drone.controller.spinSpeed += 0.01;
    }
    cout << "Turn Left " << drone.controller.spinSpeed << endl;
    
    drone.controller.spinSpeed = 0;
    float time2 = ofGetElapsedTimeMillis();
    while(ofGetElapsedTimeMillis() - time2 < 1000){
        
    }
    
    float time3 = ofGetElapsedTimeMillis();
    while(ofGetElapsedTimeMillis() - time3 < 2000){
        drone.controller.spinSpeed -= 0.01;
    }
    drone.controller.spinSpeed = 0;
    float time4 = ofGetElapsedTimeMillis();
    while(ofGetElapsedTimeMillis() - time4 < 1000){
     
    }
     
     */
   
    

}


//----------------------------------------------------------------

cv::Point2f ofApp::getCenterRect(){
    
   
    myArea = 0;
    for(int i = 0; i < contourFinder.getBoundingRects().size(); i++){
        float area = contourFinder.getBoundingRects()[i].width*contourFinder.getBoundingRects()[i].height;
        cout << "Area #" << i << " = " << area << endl;
        if(area > myArea){
            myArea = area;
            num = i;
            
        }
        cout << "Which? " << num << endl;
        return contourFinder.getCenter(num);

}
}
//----------------------------------------------------------------

void ofApp::trackingCentroid(cv::Point2f blobCoordinates){
    cout<<blobCoordinates;
    
    
    
}
//--------------------------------------------------------------

void ofApp::checkContours(){
    
    
    for(int i = 0; i < contourFinder.getBoundingRects().size(); i++){
        
        //if(contourFinder.getContourArea()
        
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key) {
        case '1': drone.controller.exitBootstrap(); break;
        case '2': drone.controller.sendAck(); break;
        case '3': drone.dataReceiver.sendDummyPacket(); break;
        case '0': drone.controller.resetCommunicationWatchdog(); break;
            
        case 't': drone.controller.takeOff(!drone.state.isTakingOff(), 3000); break;
        case 'l': drone.controller.land(!drone.state.isLanding(), 3000); break;
        case 'c': drone.controller.calibrateHorizontal(!drone.state.isCalibratingHorizontal(), 3000); break;
        case 'm': drone.controller.calibrateMagnetometer(!drone.state.isCalibratingMagnetometer(), 3000); break;
        case 'p': doPause ^= true; break;
            
        case 'e': drone.controller.emergency(0); break;
        case 'E': drone.controller.emergency(1); break;
            
        case 'R': drone.resetSequenceNumber(); break;
            
        case 'h':
            drone.controller.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength() ? 0 : (ofGetHeight()-280)/14);
            drone.dataReceiver.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength());
            break;
            
        case 'f': ofToggleFullscreen(); break;
            
        case ' ': debug = !debug; break;
    }
    
    keys[key] = true;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    keys[key] = false;
    
    switch(key) {
        case OF_KEY_UP:
        case OF_KEY_DOWN:
            drone.controller.pitchAmount = 0;
            break;
            
        case OF_KEY_LEFT:
        case OF_KEY_RIGHT:
            drone.controller.spinSpeed = 0;
            break;
            
        case 'w':
        case 's':
            drone.controller.liftSpeed = 0;
            break;
            
        case 'a':
        case 'd':
            drone.controller.rollAmount = 0;
            break;
            
    }
    
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    mousePressed(x, y, button);
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    targetColor = image.getPixelsRef().getColor(x, y);
	contourFinder.setTargetColor(targetColor, trackingColorMode);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
    if(drone.controller.commandHistory.getMaxLength()) {
        drone.controller.commandHistory.setMaxLength((ofGetHeight()-280)/14);
        drone.dataReceiver.commandHistory.setMaxLength(drone.controller.commandHistory.getMaxLength());
    }
    
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}