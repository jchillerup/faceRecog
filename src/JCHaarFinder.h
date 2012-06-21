#ifndef JCHAARFINDER_H
#define JCHAARFINDER_H

#include "ofMain.h"
#include "ofxCvBlob.h"

// defining a feature struct to be used for features.
struct featureRect {
    ofRectangle rectangle;
    int weight;
};

struct feature {
    vector<featureRect> rectangles;
    float threshold;
    float leftVal;
    float rightVal;
};

struct stage {
    vector<feature> features;
    float threshold;
};

struct cascade {
    int width;
    int height;
    vector<stage> stages;
};

class JCHaarFinder
{
    public:
        JCHaarFinder();
        virtual ~JCHaarFinder();
        vector<ofxCvBlob> getRectsFromImage(ofImage* inputImage);
        vector<ofxCvBlob> blobs;
        void draw();
    protected:
    private:
        inline int ii(int x, int y);
        inline int ii2(int x, int y);
        inline int i(int x, int y);
        void generateIIArray();
        void loadFeatures(char* fileName);
        char* defaultFeature;
        int * _iiArray;
        int * _ii2Array;
        ofImage * curImage;
        ofImage * iImage;
        int tmp_ii;
        ofxCvBlob makeBlob(int x, int y, int w, int h);
        cascade casc;

};

#endif // JCHAARFINDER_H
