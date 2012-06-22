#include "JCHaarFinder.h"
#include "ofImage.h"
#include "ofRectangle.h"
#include "ofxXmlSettings.h"



JCHaarFinder::JCHaarFinder()
{
    //ctor
    iImage = new ofImage();

    defaultFeature = "haarcascade_frontalface_default.xml";
    loadFeatures(defaultFeature);
    
}

// Loads features from a Haar cascade XML file in the OpenCV format.
void JCHaarFinder::loadFeatures(char* fileName) {
    ofxXmlSettings featureXML;
    featureXML.loadFile(fileName);

    ofLog(OF_LOG_NOTICE, "Loading cascade...");

    featureXML.pushTag("opencv_storage");
    featureXML.pushTag("haarcascade_frontalface_default"); // TODO: find a way to dynamically go to the child of opencv_storage

    string size = featureXML.getValue("size", "");
    int sizeFound = size.find(" ");
    if (sizeFound != string::npos) {
        casc.width  = atoi(size.substr( 0,  sizeFound ).c_str());
        casc.height = atoi(size.substr( sizeFound + 1 ).c_str());

        ofLog(OF_LOG_NOTICE, "Width:  "+ofToString(casc.width));
        ofLog(OF_LOG_NOTICE, "Height: "+ofToString(casc.height));
    } else {
        ofLog(OF_LOG_ERROR, "Could not find size of Haar cascade. Is this a well-formatted XML file?");
    }

    featureXML.pushTag("stages");
    int numberOfStages = featureXML.getNumTags("_");
    ofLog(OF_LOG_NOTICE, "Loading "+ofToString(numberOfStages)+" stages.");

    for (int curStage = 0; curStage<numberOfStages; curStage++) {
        stage curStageStruct;
        featureXML.pushTag("_", curStage); // pushing to next state
        featureXML.pushTag("trees");

        int numberOfTrees = featureXML.getNumTags("_");
        for (int curTree = 0; curTree<numberOfTrees; curTree++) {
            feature curFeatureStruct;

            featureXML.pushTag("_", curTree);

            // we don't support trees with more nodes than the root node, so we push ourselves right into that without checking for other nodes
            featureXML.pushTag("_");

            featureXML.pushTag("feature");
            featureXML.pushTag("rects");
            
            int numberOfRectangles = featureXML.getNumTags("_");
            for (int curRectangle = 0; curRectangle < numberOfRectangles; curRectangle++) {
                featureRect rectangleStruct;
                int x, y, width, height;
                ofRectangle rectangle;
                
                string rectangleString = featureXML.getValue("_", "", curRectangle);
                
                sscanf(rectangleString.c_str(), "%d %d %d %d %d", &x, &y, &width, &height, &rectangleStruct.weight);
                
                // populate the ofRectangle
                rectangle.set(x, y, width, height);
                rectangleStruct.rectangle = rectangle;
                curFeatureStruct.rectangles.push_back(rectangleStruct);
            }
            
            featureXML.popTag(); // pop rectangles
            // we ignore tilted.
            featureXML.popTag(); // pop feature
            
            curFeatureStruct.threshold = featureXML.getValue("threshold", 0.0);
            curFeatureStruct.leftVal   = featureXML.getValue("left_val", 0.0);
            curFeatureStruct.rightVal  = featureXML.getValue("right_val", 0.0);
            
            featureXML.popTag(); // pop root node
            featureXML.popTag(); // pop current tree

            // add the feature to the stage
            curStageStruct.features.push_back(curFeatureStruct);
        }

        featureXML.popTag(); // pop trees
        
        curStageStruct.threshold = featureXML.getValue("stage_threshold", 0.0);
        
        featureXML.popTag(); // pop current stage

        // add the stage to the cascade
        casc.stages.push_back(curStageStruct);
    }

    featureXML.popTag(); // pop stages

    featureXML.popTag(); // pop "haarcascade_frontalface_default"
    featureXML.popTag(); // pop opencv_storage
}

void JCHaarFinder::draw() {
    // Used while debugging; draws over the processed image.
}

// Returns a vector of rectangles that pass the requirements for faces.
vector<ofxCvBlob> JCHaarFinder::getRectsFromImage(ofImage* inputImage) {
    curImage = inputImage;
    
    float scale = 1;
    float scaleMultiplier = 1.25;
    blobs.clear();
    
    generateIIArray();

    /*
     * Strategy: Loop through the image such that a window moves over the
     * picture, checking stages. Once the window has moved through the image
     * scale it a bit up, and re-run the algorithm. 
     *
     * The values to be for'ed over are nested as follows:
     *
     * the scale of the cascade window
     * window's position on y axis
     * window's position on x axis
     * stages of the cascade
     * features of the stage
     * rectangles of the feature
     * 
     * Since we don't support larger trees than the ones which only have a
     * root node, we can ignore traversing trees and just consider a list
     * of rectangles for any one feature.
     */
        
    // This implementation keeps enlarging the window until it is bigger than the picture.
    for (scale = 1; scale*casc.height <= curImage->getHeight() || scale*casc.width <= curImage->getWidth(); scale *= scaleMultiplier) {
        
        int window_area = scale*casc.height * scale*casc.width;
        
        // Keep moving the window down as long as its offset and its height are within the image. Likewise to the left
        // As an experiment, we're shifting the window in chunks equal to about a tenth of the current window height and width, respectively.
        for (int offsetY = 0; (offsetY+scale*casc.height) < curImage->getHeight(); offsetY += (int) ((scale*casc.height)/10)  ) {
            for (int offsetX = 0; (offsetX + scale*casc.width) < curImage->getWidth(); offsetX += (int) ((scale*casc.width)/10)) {
                bool passed = true;
                
                // *** LOOPING OVER STAGES
                for (int curStageIdx = 0; curStageIdx < casc.stages.size(); curStageIdx++) {
                    if (passed == false) break;
                    
                    stage* s = &casc.stages.at(curStageIdx);
                    float stage_sum = 0.0;
                    
                    // *** LOOPING OVER FEATURES
                    for (int featureIdx = 0; featureIdx < s->features.size(); featureIdx++) {
                        feature* f = & (s->features.at(featureIdx));
                        
                        float feature_sum = 0.0;
                        
                        // These coordinates are for the sliding window
                        int px1 = offsetX,
                            py1 = offsetY,
                            px2 = offsetX + scale*casc.width,
                            py2 = offsetY + scale*casc.height;
                        
                        float mean   = ((float) (ii(px2, py2)  + ii(px1, py1)  - ii(px1, py2)  - ii(px2, py1)))  / window_area;
                        float stddev = sqrt((ii2(px2, py2) + ii2(px1, py1) - ii2(px1, py2) - ii2(px2, py1)) / window_area - mean*mean);

                        // *** LOOPING OVER RECTANGLES
                        for (int rectangleIdx = 0; rectangleIdx < f->rectangles.size(); rectangleIdx++) {
                            featureRect* r = & ( f->rectangles.at(rectangleIdx));
                            
                            // These coordinates are for the rectangles inside the sliding window. The coordinates 
                            // are absolute; i.e. they share the same origin as the coordinates of the window.
                            int x1 = offsetX + (r->rectangle.x * scale),
                                y1 = offsetY + (r->rectangle.y * scale),
                                x2 = x1      +  r->rectangle.width * scale,
                                y2 = y1      +  r->rectangle.height * scale;
                            
                            int thisRect = (ii(x2, y2) + ii(x1, y1) - ii(x1, y2) - ii(x2, y1)) * r->weight;
                            
                            feature_sum += thisRect;
                        }
                        
                        // Determine in which direction the cascade should "fall". If the feature sum is less than
                        // its threshold, fall left, otherwise right.
                        // http://stackoverflow.com/questions/978742/what-do-the-left-and-right-values-mean-in-the-haar-cascade-xml-files
                        if (feature_sum/window_area < f->threshold*stddev) {
                            stage_sum += f->leftVal;
                        } else {
                            stage_sum += f->rightVal;
                        }                        
                    }
                    
                    // The stage is passed if its sum is above its threshold.
                    passed = (stage_sum > s->threshold);
                }
            
                // passed will be true iff all stages passed; if so, we detected a face.
                if (passed) {
                    ofLog(OF_LOG_NOTICE, "detected face (maybe)!");
                    blobs.push_back(makeBlob(offsetX, offsetY, scale*casc.width, scale*casc.height));
                }
            }
        }
    }
    
    return blobs;
}


// Calculates the array of values used as the integral image, lest values need calculation.
void JCHaarFinder::generateIIArray() {
    // if true, the gnerated II array will be printed to a file after calculation
    bool takeSnapshot = false;

    int width = curImage->getWidth(),
        height = curImage->getHeight();
    
    if (_iiArray == NULL) {
        free(_iiArray);
    }
    
    _iiArray = (int*) malloc(width * height * sizeof(int));
    _ii2Array = (int*) malloc(width * height * sizeof(int));

    for (int i = 0; i<width*height; i++) {
        _iiArray[i] = 0;
        _ii2Array[i] = 0;
    }

    for (int row=0; row < height; row++) {
        int rowsum = 0;
        int rowsum2 = 0;
        
        /* The strategy here is to keep a sum of the current row that is calculated.
         * The values of the fields in the rows are continuously added to the rowsum
         * variable, and the columns are taken care of by looking earlier in the array
         * as we're building it (we look in the previous row at the same column). This
         * eventually yields the integral image.
         * 
         * Also, we keep an integral image over all the squares of the pixel values. This
         * is used for the standard deviations in the normalization stage when the feature
         * sums are compared to their thresholds.
         */
        
        for(int col=0; col < width; col++) {
            rowsum += i(col, row);
            rowsum2 += i(col, row)*i(col, row);
            
            if (row > 0) {
                
                _iiArray[row*width + col] = rowsum + ii(col, row-1);
                _ii2Array[row*width + col] = rowsum2 + ii2(col, row-1);
            } else {
                _iiArray[row*width + col] = rowsum;
                _ii2Array[row*width + col] = rowsum2;
            }
        }
    }
    
    // Debug functionality that writes out the integral images in CSV structures.
    if (takeSnapshot) {
        ofFile iiFile;
        ofFile ii2File;
        iiFile.open(ofToDataPath("ii.csv"), ofFile::WriteOnly, false);
        ii2File.open(ofToDataPath("ii2.csv"), ofFile::WriteOnly, false);
        
        for (int row= 0; row<height; row++) {
            for (int col = 0; col<width; col++) {
                iiFile << ofToString(ii(col, row)) + ";";
                ii2File << ofToString(ii2(col, row)) + ";";
            }
            
            iiFile << endl;
            ii2File << endl;
        }
        
        iiFile.close();       
    }
}

ofxCvBlob JCHaarFinder::makeBlob(int x, int y, int w, int h) {
    ofxCvBlob blob = ofxCvBlob();
    blob.boundingRect = ofRectangle(x, y, w, h);

    return blob;
}

// Gets the pixel value at the given coordinate
inline int JCHaarFinder::i(int x, int y) {
    // sanity checks. The first one makes sure the function returns 0 if the image has not been initialized yet.
    if (curImage == NULL || x<0 || y<0 || x >= curImage->getWidth() || y >= curImage->getHeight()) {
        return 0;
    }

    return (int) curImage->getPixelsRef()[(int) (y*curImage->getWidth()+x)];
}

// Makes a look-up in the integral image table (shorthand instead of accessing directly)
inline int JCHaarFinder::ii(int x, int y) {
    if (x<0 || y<0) {return 0;}
    
    return _iiArray[(int)(y*curImage->getWidth()+x)];
}

inline int JCHaarFinder::ii2(int x, int y) {
    if (x<0 || y<0) {return 0;}
    
    return _ii2Array[(int)(y*curImage->getWidth()+x)];
}

// Destructor
JCHaarFinder::~JCHaarFinder() {
    //dtor
}
