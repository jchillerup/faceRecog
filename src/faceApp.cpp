#include "faceApp.h"
#include "OpenCvHaarFinder.h"

//--------------------------------------------------------------
void faceApp::setup(){


    ofBackground(200,0,0);

    brightness = 0;
    contrast = 0;

    uiFont.loadFont("FreeMono.ttf", 12, true, true);
    uiHeadingFont.loadFont("FreeMono.ttf", 18, true, true);

    videoGrabber.initGrabber(320, 240);

    // set up the detector
    haarFinder = new JCHaarFinder();

    gui = new ofxUICanvas(724,0,300,768);
    gui->setFont("FreeMono.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 12);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);

    fpsLabel = new ofxUILabel("what", OFX_UI_FONT_SMALL);

    gui->addWidgetDown(new ofxUILabel("tuneables", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUIToggle(20, 20, false, "use webcam"));
    gui->addWidgetDown(new ofxUISlider(250, 24, -1, 1, 0, "brightness adjustment"));
    gui->addWidgetDown(new ofxUISlider(250, 24, -1, 1, 0, "contrast adjustment"));
    gui->addWidgetDown(new ofxUISlider(250, 24, 0.5, 1.5, 1, "scale adjustment"));
    gui->addWidgetDown(new ofxUISpacer(0, 25));
    gui->addWidgetDown(new ofxUILabel("values", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(fpsLabel);

    ofAddListener(gui->newGUIEvent, this, &faceApp::guiEvent);

    ofSetWindowTitle("face detection by jens christian hillerup");
    
    probeImage = new ofImage();
    
    
    ofDirectory dir("images/");
    dir.listDir();
 	imageCount = dir.numFiles();
    images = new ofImage[imageCount];
 	
    for(int i = 0; i < imageCount; i++){
        images[i].loadImage(dir.getPath(i));
    }
    
    assert(imageCount > 0);
    probeImage = &images[0];
    
    frameNew = true;
    useWebcam = false;
    curImage = 0;
    
    scale = 0.0;
}

//--------------------------------------------------------------
void faceApp::update(){  
    
    videoGrabber.grabFrame();
    
    frameNew = useWebcam?videoGrabber.isFrameNew():frameNew;
    
    if (frameNew) {
        if (useWebcam) {
            //free(probeImage);
            probeImage = new ofImage();
            probeImage->setFromPixels(videoGrabber.getPixels(), videoGrabber.getWidth(), videoGrabber.getHeight(), OF_IMAGE_COLOR);
        } else {
            probeImage = new ofImage();
            probeImage->setFromPixels(images[curImage].getPixels(), images[curImage].getWidth(), images[curImage].getHeight(), OF_IMAGE_COLOR);
        }
    
        doHaarStuff();
    }
    frameNew = false;
}

//--------------------------------------------------------------
void faceApp::draw(){
    ofPushMatrix();
    ofTranslate(5, 35, 0);
    videoGrabber.draw(0,0);
    ofTranslate(0, videoGrabber.getHeight()+5, 0);
        
    if (haarInput.isAllocated()) {
        haarInput.draw(0, 0);
    }   
    
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
    int total_pixels = probeImage->getWidth() * probeImage->getHeight();

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
        
        value = constrain((value - 0.5) * (tan ((contrast + 1) * PI/4) ) + 0.5, 0, 1);
        // end copy

        // Map value back to [0; 255]
        value *= 255;
        
        gray_pixels[i] = (int) value;
        
    }

    haarInput.setFromPixels(gray_pixels, probeImage->getWidth(), probeImage->getHeight(), OF_IMAGE_GRAYSCALE);
    
    haarInput.resize(probeImage->getWidth()*scale, probeImage->getHeight()*scale);
    
    haarFinder->getRectsFromImage(&haarInput);
}

inline float faceApp::constrain(float amount, float min, float max) {
    return (amount>max)?max:((amount<min)?min:amount);
}

//--------------------------------------------------------------
void faceApp::keyPressed(int key){    
    if (key == OF_KEY_RIGHT) {
        curImage = (curImage +1) % imageCount;
        frameNew = true;
    } else if (key == OF_KEY_LEFT) {
        curImage = (imageCount + curImage - 1) % imageCount;
        frameNew = true;
    }
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
    if(e.widget->getName() == "use webcam")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        useWebcam = toggle->getValue();
        frameNew = true;
        
        curImage = 0;
    } else if (e.widget->getName() == "brightness adjustment" || e.widget->getName() == "contrast adjustment" || 
               e.widget->getName() == "scale adjustment") {

        float * container;
        ofxUISlider *slider = (ofxUISlider *) e.widget;

        if (e.widget->getName() == "brightness adjustment") {
            container = &brightness;
        } else if (e.widget->getName() == "contrast adjustment"){
            container = &contrast;
        } else if (e.widget->getName() == "scale adjustment") {
            container = &scale;
        }

        *container = slider->getScaledValue();
        frameNew=true;
    }
}
