#include "faceApp.h"
#include <ofVideoGrabber.h>

//--------------------------------------------------------------
void faceApp::setup(){
    videoGrabber.setVerbose(true);
    videoGrabber.listDevices();

    videoGrabber.initGrabber(640, 480);

}

//--------------------------------------------------------------
void faceApp::update(){
    videoGrabber.grabFrame();
}

//--------------------------------------------------------------
void faceApp::draw(){
    videoGrabber.draw(0,0);
}

//--------------------------------------------------------------
void faceApp::keyPressed(int key){

}

//--------------------------------------------------------------
void faceApp::keyReleased(int key){

}

//--------------------------------------------------------------
void faceApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void faceApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void faceApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void faceApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void faceApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void faceApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void faceApp::dragEvent(ofDragInfo dragInfo){

}
