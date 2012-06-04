#pragma once

#include "ofMain.h"
#include <ofVideoGrabber.h>
#include <ofTrueTypeFont.h>
#include "ofxUI.h"

class faceApp : public ofBaseApp{
    private:
        ofVideoGrabber videoGrabber;
        ofTrueTypeFont uiFont;
        ofTrueTypeFont uiHeadingFont;
        ofxUICanvas *gui;
        ofxUILabel *fpsLabel;

        bool showOverlays = true;


	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();
		void guiEvent(ofxUIEventArgs &e);
};
