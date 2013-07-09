//
//  PoseGestureMovie.h
//  poseMakerTool
//
//  Created by Ben McChesney on 6/29/13.
//
//

#ifndef __poseMakerTool__PoseGestureMovie__
#define __poseMakerTool__PoseGestureMovie__

#include <iostream>

#include "ofMain.h"
#include "KeyPointPose.h"
#include "ofxXmlSettings.h"
#include "ofxSimpleTimer.h"
#include "PixelHitTestHub.h"

class PoseGestureMovie
{
public:
    PoseGestureMovie() { }
    ~PoseGestureMovie() { }
    
    void setup ( string _moviePath );
    void update( ) ;
    void draw( ) ;
    void drawInputMap( ) ;
    
    void initEmptyPose( ) ;
    void calcInputMap( ) ; 
    
    string moviePath ; 
    ofVideoPlayer movie ;
    vector< ofPtr<KeyPointPose> > poses ;
    
    void start( ) ;

    ofPoint offset ; 
    vector< vector<ofPoint> > activePosePts ;

    void addPoint ( float x , float y ) ;
    void setEndOnPoint( float moviePosition , int x , int y ) ;
    int getClosestPointTo( float x , float y ) ;
    
    void checkPoseAgainstInputMap( PixelHitTestHub * pixelHub , int hitHex ) ;
    
    void decrementFrame ( ) ;
    void incrementFrame ( ) ;
    
    void debugDraw( ) ; 
    void savePoseToXml( ) ;
    void loadXml( string xmlPath ) ; 
    
    ofRectangle timelineBounds ;
    void drawTimeline( ) ;
    void inputTimeline( float x  , float y ) ;
    
    int selectedDebugPose ;
    void keyPressed( int key ) ;
    void setPoseTime ( float poseTimeDelayInMillis ) ;
    
    
    //ofxSimpleTimer poseTimer ;
    
    
    
    
};

#endif /* defined(__poseMakerTool__PoseGestureMovie__) */
