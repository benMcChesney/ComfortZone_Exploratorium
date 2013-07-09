//
//  KeyPointPose.h
//  poseMakerTool
//
//  Created by Ben McChesney on 6/29/13.
//
//

#ifndef __poseMakerTool__KeyPointPose__
#define __poseMakerTool__KeyPointPose__

#include <iostream>
#include "ofMain.h"
#include "ofxSimpleTimer.h"

class KeyPointPose
{
public:
    KeyPointPose( ) { }
    ~KeyPointPose( ) { }


    void setup () ;
    void createDebugPoints( ) ;
    bool getIsActive( float nPosition ) ;
    void addPoint ( float x , float y ) { addPoint ( ofPoint ( x , y ) ) ; }
    void addPoint ( ofPoint p ) ;
    void drawInputMap( ) ;
    void drawDebugMap( ) ;
    void resetPtsStatus ( ) ; 
    
    float _in , _out ;
    vector<ofPoint> pts ;
    vector<bool> ptsStatus ;
    int uniqueInputHex ;
    float keyPointRadius ;
    
    ofxSimpleTimer poseTimer ; 
};

#endif /* defined(__poseMakerTool__KeyPointPose__) */
