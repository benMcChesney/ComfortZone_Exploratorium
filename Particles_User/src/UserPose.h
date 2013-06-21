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

class UserPose
{
    public :
    UserPose( )
    {
        bActivated = false ; 
    }
    ~UserPose( ) { }
    
    
    bool bCheckPose() ;
    void addKeyPoint( ofPoint p ) ; 
    void draw ( ) ;
    
    vector<ofPoint> keyPoints ;
    bool bActivated ; 
    
};
#endif /* defined(__ofxKinect_ofxCv__UserPose__) */
