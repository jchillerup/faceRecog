#include "faceApp.h"
#include <ofVideoGrabber.h>
#include <ofTrueTypeFont.h>

//--------------------------------------------------------------
void faceApp::setup(){
    ofBackground(200,0,0);

    uiFont.loadFont("FreeMono.ttf", 12, true, true);
    uiHeadingFont.loadFont("FreeMono.ttf", 18, true, true);

    videoGrabber.listDevices();

    videoGrabber.initGrabber(640, 480);



    gui = new ofxUICanvas(724,0,300,768);
    gui->setFont("FreeMono.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 12);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);

    gui->addWidgetDown(new ofxUILabel("tuneables", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUIToggle(20, 20, true, "show overlays"));
    ofAddListener(gui->newGUIEvent, this, &faceApp::guiEvent);

}

//--------------------------------------------------------------
void faceApp::update(){
    videoGrabber.grabFrame();
}

//--------------------------------------------------------------
void faceApp::draw(){

    videoGrabber.draw(5,35);

    uiHeadingFont.drawString("facial recognition by jchillerup", 5,25);
    uiFont.drawString("fps: "+ ofToString(ofGetFrameRate(), 2), 600, 600);

    if (showOverlays) {
        ofNoFill();
        ofRect(10, 40, 40, 40);
    }

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

void faceApp::exit() {

}

void faceApp::guiEvent(ofxUIEventArgs &e) {
    if(e.widget->getName() == "show overlays")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        showOverlays = toggle->getValue();
    }
}
