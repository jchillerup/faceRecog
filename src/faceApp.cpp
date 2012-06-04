#include "faceApp.h"

//--------------------------------------------------------------
void faceApp::setup(){
    ofBackground(200,0,0);

    uiFont.loadFont("FreeMono.ttf", 12, true, true);
    uiHeadingFont.loadFont("FreeMono.ttf", 18, true, true);

    videoGrabber.initGrabber(640, 480);

    // set up the "cheating" detector
    haarFinder.setup("haarcascade_frontalface_default.xml");

    grabbedImage = new ofImage();

    gui = new ofxUICanvas(724,0,300,768);
    gui->setFont("FreeMono.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 12);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);

    fpsLabel = new ofxUILabel("what", OFX_UI_FONT_SMALL);

    gui->addWidgetDown(new ofxUILabel("tuneables", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUIToggle(20, 20, true, "show overlays"));

    gui->addWidgetDown(new ofxUILabel("values", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(fpsLabel);

    ofAddListener(gui->newGUIEvent, this, &faceApp::guiEvent);

}

//--------------------------------------------------------------
void faceApp::update(){

    videoGrabber.grabFrame();

    if (videoGrabber.isFrameNew() && showOverlays) {
        grabbedImage->setFromPixels(videoGrabber.getPixels(), 640, 480, OF_IMAGE_COLOR);
        haarFinder.findHaarObjects(*grabbedImage);
    }
}

//--------------------------------------------------------------
void faceApp::draw(){

    ofPushMatrix();
    ofTranslate(5, 35, 0);
    videoGrabber.draw(0,0);
    if (showOverlays) {
        ofNoFill();
        for(int i = 0; i < haarFinder.blobs.size(); i++) {
            ofRect( haarFinder.blobs[i].boundingRect );
        }
    }
    ofPopMatrix();

    uiHeadingFont.drawString("facial recognition by jchillerup", 5,25);

    fpsLabel->setLabel("fps: "+ ofToString(ofGetFrameRate(), 2));


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
