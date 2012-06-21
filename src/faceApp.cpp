#include "faceApp.h"
#include "OpenCvHaarFinder.h"

//--------------------------------------------------------------
void faceApp::setup(){


    ofBackground(200,0,0);

    camWidth = 320;
    camHeight = 240;

    brightness = 0;
    contrast = 0;

    uiFont.loadFont("FreeMono.ttf", 12, true, true);
    uiHeadingFont.loadFont("FreeMono.ttf", 18, true, true);

    videoGrabber.initGrabber(camWidth, camHeight);

    // set up the detector
    haarFinder = new JCHaarFinder();

    gui = new ofxUICanvas(724,0,300,768);
    gui->setFont("FreeMono.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 12);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);

    fpsLabel = new ofxUILabel("what", OFX_UI_FONT_SMALL);

    gui->addWidgetDown(new ofxUILabel("tuneables", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUIToggle(20, 20, true, "show overlays"));
    gui->addWidgetDown(new ofxUISlider(250, 24, -1, 1, 0, "brightness adjustment"));
    gui->addWidgetDown(new ofxUISlider(250, 24, -1, 1, 0, "contrast adjustment"));
    gui->addWidgetDown(new ofxUISpacer(0, 25));
    gui->addWidgetDown(new ofxUILabel("values", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(fpsLabel);

    ofAddListener(gui->newGUIEvent, this, &faceApp::guiEvent);

    ofSetWindowTitle("face detection by jens christian hillerup");
    
    probeImage = new ofImage();
    /*
    probeImage = new ofImage("lena.jpg");
    camWidth = probeImage->getWidth();
    camHeight = probeImage->getHeight();
    */
    frameNew = false;
}

//--------------------------------------------------------------
void faceApp::update(){  
    
    
    videoGrabber.grabFrame();
    
    if (videoGrabber.isFrameNew()) {
        probeImage->setFromPixels(videoGrabber.getPixels(), videoGrabber.getWidth(), videoGrabber.getHeight(), OF_IMAGE_COLOR);
        frameNew = true;
    }
    
    
    if (frameNew) {
        doHaarStuff();
    }
}

//--------------------------------------------------------------
void faceApp::draw(){
    ofPushMatrix();
    ofTranslate(5, 35, 0);
    videoGrabber.draw(0,0);
    ofTranslate(0, camHeight+5, 0);
    
    if (grabbedImage.isAllocated()) {
        grabbedImage.draw(0, 0);
    }

    // Whatever the haar finder draws should be in the same translated matrix as the processed image.
    haarFinder->draw();
    
    if (showOverlays) {
        ofNoFill();
        for(int i = 0; i < haarFinder->blobs.size(); i++) {
            ofRect( haarFinder->blobs[i].boundingRect );
        }
    }

    ofPopMatrix(); // from processedImage to videoGrabber

    ofPopMatrix(); // to root

    uiHeadingFont.drawString("facial recognition by jchillerup", 5,25);

    fpsLabel->setLabel("fps: "+ ofToString(ofGetFrameRate(), 2));
}

void faceApp::doHaarStuff() {
    // get pixels as char array and make a grayscale buffer from them
    int total_pixels = camWidth*camHeight;

    //unsigned char* rgb_pixels   = videoGrabber.getPixels();
    unsigned char* rgb_pixels = probeImage->getPixels();
    unsigned char gray_pixels[total_pixels];

    for (int i = 0; i<total_pixels; i++) {
        // First, pick out a gray value from the RGB representation
        float value = 0.3*rgb_pixels[3*i+0] + 0.59*rgb_pixels[3*i+1] + 0.11*rgb_pixels[3*i+2];
        
        // Then, map it from [0; 255] -> [0; 1]
        value /= 255;

        // Then, adjust brightness and contrast. Code copied from Wikipedia which in turn copied it from GIMP.
        // https://en.wikipedia.org/wiki/Image_editing#Contrast_change_and_brightening
        if (brightness < 0.0)
            value = value * ( 1.0 + brightness);
        else
            value = value + ((1 - value) * brightness);
        
        //value = (value - 0.5) * (tan ((contrast + 1) * PI/4) ) + 0.5;
        // end copy

        // Map value back to [0; 255]
        value *= 255;
        
        gray_pixels[i] = (int) value;
    }

    grabbedImage.setFromPixels(gray_pixels, camWidth, camHeight, OF_IMAGE_GRAYSCALE);
    
    haarFinder->getRectsFromImage(&grabbedImage);
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
    } else if (e.widget->getName() == "brightness adjustment" || e.widget->getName() == "contrast adjustment") {
        float * container;
        ofxUISlider *slider = (ofxUISlider *) e.widget;

        if (e.widget->getName() == "brightness adjustment") {
            container = &brightness;
        } else {
            container = &contrast;
        }

        *container = slider->getValue();
    }
}
