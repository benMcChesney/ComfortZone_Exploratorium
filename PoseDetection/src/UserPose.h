//
//  UserPose.h
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 6/2/13.
//
//

#ifndef __ofxKinect_ofxCv__UserPose__
#define __ofxKinect_ofxCv__UserPose__

#include "ofMain.h"
#include <iostream>
#include "ofxPixelHitTest.h"
#include "ofxSimpleTimer.h"

class UserPose
{
    public :
    UserPose( )
    {
        bActivated = false ;
        poseTimer.setup( poseTimerDelayMillis ) ;
    }
    ~UserPose( ) { }
    
    struct KeyPoint
    {
        ofPoint p ;
        bool bActive = false ;
    };
    
    bool bCheckPose( int userFillHex , ofPoint offset ) ;
    void addKeyPoint( ofPoint p ) ;
    
    void update(  ) ;
    void draw ( ) ;
    
    vector<KeyPoint> keyPoints ;
    bool bActivated ;
    
    ofxSimpleTimer poseTimer ;
    float poseTimerDelayMillis ;
};
#endif /* defined(__ofxKinect_ofxCv__UserPose__) */
