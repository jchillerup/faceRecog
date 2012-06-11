#include "OpenCvHaarFinder.h"

OpenCvHaarFinder::OpenCvHaarFinder()
{
    _haarFinder.setup("haarcascade_frontalface_default.xml");
}

OpenCvHaarFinder::~OpenCvHaarFinder()
{
    //dtor
}

vector<ofxCvBlob> OpenCvHaarFinder::getRectsFromImage(ofImage* inputImage) {
    _haarFinder.findHaarObjects(*inputImage);

    blobs = _haarFinder.blobs;

    return blobs;
}
