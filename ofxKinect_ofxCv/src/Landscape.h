//
//  Landscape.h
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 5/26/13.
//
//

#ifndef __ofxKinect_ofxCv__Landscape__
#define __ofxKinect_ofxCv__Landscape__

#include "ofMain.h"
#include "ofxUI.h"

class Landscape {
    
    
public:
    Landscape() { }
    ~Landscape( ) { }
    
    
    void setup( ) ;
    void update( ) ;
    void draw( ) ;
    void setupUI( ) ; 
    
    
    //ofxUI hooks
    ofxUIScrollableCanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    
    ofVec3f offset ;
    int segments, copies ; 
    float zSpacing ;
    float heightNoiseOffset ;
    float landscapeHeight ;
    float landscapeTimeFactor ;
    
    ofColor startColor ;
    ofColor endColor ;
    int colorRange ;
    
    
};

#endif /* defined(__ofxKinect_ofxCv__Landscape__) */
