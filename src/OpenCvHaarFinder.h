#ifndef OPENCVHAARFINDER_H
#define OPENCVHAARFINDER_H

#include "ofMain.h"
#include "ofxCvHaarFinder.h"

class OpenCvHaarFinder
{
    public:
        OpenCvHaarFinder();
        virtual ~OpenCvHaarFinder();
        vector<ofxCvBlob> getRectsFromImage(ofImage* inputImage);
        vector<ofxCvBlob> blobs;
    protected:
    private:
        ofxCvHaarFinder _haarFinder;

};

#endif // OPENCVHAARFINDER_H
