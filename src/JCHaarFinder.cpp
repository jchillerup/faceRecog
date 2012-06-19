#include "JCHaarFinder.h"
#include "ofImage.h"
#include "ofRectangle.h"
#include "ofxXmlSettings.h"



JCHaarFinder::JCHaarFinder()
{
    //ctor
    iImage = new ofImage();

    _iiArray = (int*)malloc(320 * 240 * sizeof(int));

    defaultFeature = "haarcascade_frontalface_default.xml";
    loadFeatures(defaultFeature);
}

// Loads features from a Haar cascade XML file in the OpenCV format.
void JCHaarFinder::loadFeatures(char* fileName) {
    ofxXmlSettings featureXML;
    featureXML.loadFile(fileName);
    //cascade casc;

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
        //ofLog(OF_LOG_NOTICE, "Stage #"+ofToString(curStage)+" has "+ofToString(numberOfTrees)+ " trees.");
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
                
                // ofLog(OF_LOG_NOTICE, ofToString(x)+" "+ofToString(y)+" "+ofToString(width)+" "+ofToString(height)+" "+ofToString(rectangleStruct.weight));
                
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
            
            // ofLog(OF_LOG_NOTICE, "threshold "+ofToString(curFeatureStruct.threshold, 4));
            // ofLog(OF_LOG_NOTICE, "left_val  "+ofToString(curFeatureStruct.leftVal, 4));
            // ofLog(OF_LOG_NOTICE, "right_val "+ofToString(curFeatureStruct.rightVal, 4));
            
            featureXML.popTag(); // pop root node
            featureXML.popTag(); // pop current tree

            // add the feature to the stage
            curStageStruct.features.push_back(curFeatureStruct);
        }

        featureXML.popTag(); // pop trees
        
        curStageStruct.threshold = featureXML.getValue("stage_threshold", 0.0);
        
        //ofLog(OF_LOG_NOTICE, "Stage threshold: "+ofToString(curStageStruct.threshold, 8));
        
        featureXML.popTag(); // pop current stage

        // add the stage to the cascade
        casc.stages.push_back(curStageStruct);
    }

    featureXML.popTag(); // pop stages

    featureXML.popTag(); // pop "haarcascade_frontalface_default"
    featureXML.popTag(); // pop opencv_storage
}

void JCHaarFinder::update() {
    /*
    iImage = new ofImage();
    iImage->clone(curImage);

    for (int i = 0; i < (iImage->getWidth()*iImage->getHeight()) ; i++) {
        int x = i / iImage->getWidth();
        int y = i % iImage->getWidth();

        iImage->getPixels()[i] = ii(x, y);
    }
    */

    //tmp_ii = ii(0, 0);
}

void JCHaarFinder::draw() {
    float scale = 1;
    
    ofPushStyle();
    
    ofScale(scale, scale, 0);
    
    ofSetColor(255,0,0);
    ofSetLineWidth(2);
    ofRect(0,0,24,24);
    ofPopStyle();
    
    stage curStage = casc.stages.at(0);
    feature curFeature = curStage.features.at(1);
    
    for (int i = 0; i<curFeature.rectangles.size(); i++) {
        featureRect rect = curFeature.rectangles.at(i);
    
        ofPushStyle();
        ofFill();
        if (rect.weight < 0) {
            ofSetColor(0,0,0);
        } else {
            ofSetColor(255,255,255);
        }
        ofRect( rect.rectangle );
        ofPopStyle();
    }

    
    
    
    //if (curImage != NULL) 
        //curImage->draw(0,0);
}

// Returns a vector of rectangles that pass the requirements for faces.
vector<ofxCvBlob> JCHaarFinder::getRectsFromImage(ofImage* inputImage) {
    curImage = inputImage;
    
    float scale = 1;
    float scaleMultiplier = 1.25;
    int   area = curImage->getWidth() * curImage->getHeight(), 
          right_count = 0, 
          left_count = 0;
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
    */
        
    // This implementation keeps enlarging the window until it is bigger than the picture.
    for (scale = 1; scale*casc.height <= curImage->getHeight() || scale*casc.width <= curImage->getWidth(); scale *= scaleMultiplier) {
        //ofLog(OF_LOG_NOTICE, "Search window height: " + ofToString(scale*casc.height));
        
        area = scale*casc.height * scale*casc.width;
        
        //ofLog(OF_LOG_NOTICE, "Search window area: "+ofToString(area, 2));
        
        // Keep moving the window down as long as its offset and its height are within the image. Likewise to the left
        // As an experiment, we're shifting the window in chunks equal to about a tenth of the current window height and width, respectively.
        for (int offsetY = 0; (offsetY+scale*casc.height) < curImage->getHeight(); offsetY += (int) ((scale*casc.height)/10)  ) {
            for (int offsetX = 0; (offsetX + scale*casc.width) < curImage->getWidth(); offsetX += (int) ((scale*casc.width)/10)) {
                bool passed = true;
                
                for (int curStageIdx = 0; curStageIdx < casc.stages.size(); curStageIdx++) {
                    if (passed == false) break;
                    
                    stage* s = &casc.stages.at(curStageIdx);
                    float stage_sum = 0.0;
                    
                    if (curStageIdx > 6) {
                        ofLog(OF_LOG_NOTICE, "Current stage: "+ofToString(curStageIdx));
                    }
                    
                    for (int featureIdx = 0; featureIdx < s->features.size(); featureIdx++) {
                        feature* f = & (s->features.at(featureIdx));
                        
                        float feature_sum = 0.0;
                        
                        for (int rectangleIdx = 0; rectangleIdx < f->rectangles.size(); rectangleIdx++) {
                            featureRect* r = & ( f->rectangles.at(rectangleIdx));
                            
                            int x1 = offsetX + r->rectangle.x*scale,
                                y1 = offsetY + r->rectangle.y*scale,
                                x2 = x1 + r->rectangle.width*scale,
                                y2 = y1 + r->rectangle.height*scale;
                            
                            feature_sum += (ii(x2, y2) - ii(x1, y2) - ii(x2, y1) + ii(x1, y1)) * r->weight;
                            /*
                            if (scale == 1 && offsetY == 0 && offsetX == 0 && curStageIdx == 0 & featureIdx == 0) {
                                ofLog(OF_LOG_NOTICE, "featureSum = "+ofToString(feature_sum, 5));
                            }
                            */
                            
                        }
                        
                        if (feature_sum/area < s->threshold) {
                            //ofLog(OF_LOG_NOTICE, "adding left value. value: " + ofToString(feature_sum/area, 10) + " thrshold: "+ofToString(s->threshold, 10));
                            stage_sum += f->leftVal;
                            left_count++;
                        } else {
                            //ofLog(OF_LOG_NOTICE, "adding right value. value: " + ofToString(feature_sum/area, 10) + " thrshold: "+ofToString(s->threshold, 10));
                            stage_sum += f->rightVal;
                            right_count++;
                        }
                        
                    }
                    
                    //ofLog(OF_LOG_NOTICE, "Stage sum: "+ofToString(stage_sum, 6)+", threshold: "+ofToString(s->threshold, 6));
                    
                    passed = (stage_sum > s->threshold);
                    
                    if (!passed && curStageIdx > 6) {
                        ofLog(OF_LOG_NOTICE, "Stage " + ofToString(curStageIdx) + "failed. Stage sum: "+ofToString(stage_sum)+", required: "+ofToString(s->threshold));
                    }
                }
            
                if (passed) {
                    ofLog(OF_LOG_NOTICE, "detected face (maybe)!");
                    blobs.push_back(makeBlob(offsetX, offsetY, scale*casc.width, scale*casc.height));
                }
            }
        
        }
        
        
    }
    
    // http://stackoverflow.com/questions/978742/what-do-the-left-and-right-values-mean-in-the-haar-cascade-xml-files

    // dblobs.push_back(makeBlob(50, 50, 50, 50));
    // blobs.push_back(makeBlob(75, 75, 50, 50));


    ofLog(OF_LOG_NOTICE, "Left count: "+ofToString(left_count) + ", right count: "+ofToString(right_count));

    return blobs;
}


// Calculates the array of values used as the !!! image, lest values need calculation.
void JCHaarFinder::generateIIArray() {
    int row = 0,
        col = 0,
        rowsum = 0,
        width = curImage->getWidth(),
        height = curImage->getHeight();


    // if true, the gnerated II array will be printed (slow)
    bool takeSnapshot = false;

    for (col=0; col < width; col++) {
        rowsum = 0;

        for(row=0; row < height; row++) {
            rowsum += i(row, col); // this is s(x,y)

            if (col > 0) {
                _iiArray[row*height+col] = _iiArray[row*height+(col - 1)] + rowsum;
            } else {
                _iiArray[row*height+col] = 0                              + rowsum;
            }

            if (takeSnapshot) {
                printf("%d,", _iiArray[row*height+col]);
            }
        }
        if (takeSnapshot) {
            printf("\n");
        }
    }

    takeSnapshot = false;
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
    return _iiArray[(int)(y*curImage->getHeight()+x)];
}

// Destructor
JCHaarFinder::~JCHaarFinder() {
    //dtor
}
