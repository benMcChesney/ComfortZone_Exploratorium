//
//  UserPose.cpp
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 6/2/13.
//
//

#include "UserPose.h"

bool UserPose::bCheckPose( int userFillHex , ofPoint offset )
{
    bool bFailed = false ; 
    for ( int i = 0 ; i < keyPoints.size() ; i++ )
    {
        if ( PixelHitTestHub::Instance()->getHexAt( keyPoints[i].p ) == userFillHex )
        {
            //sail along...
            keyPoints[i].bActive = true ;
        }
        else
        {
            cout << "one point is being written over ! " << endl ;
            bActivated = false ;
            keyPoints[i].bActive = false ; 
            bFailed = true ; 
        }
    }
    
    if ( bFailed != false )
    {
        if ( poseTimer.bIsRunning )
            poseTimer.stop() ;
        return false ;
    }
    else
    {
        bActivated = true ;
        if ( poseTimer.bIsRunning == false )
        {
            poseTimer.delayMillis = poseTimerDelayMillis ;
            poseTimer.start( false ) ; 
        }
        
        
        return true ;
    }
}

void UserPose::addKeyPoint( ofPoint _p )
{
    KeyPoint kp ;
    kp.p = _p ;
    kp.bActive = false ; 
    keyPoints.push_back( kp ) ;
}

void UserPose::update(  )
{
    poseTimer.update() ; 
}

void UserPose::draw ( )
{
    ofEnableAlphaBlending() ; 
    int alpha = 135 ; 
    
    
    for ( int i = 0 ; i < keyPoints.size() ; i++ )
    {
        if ( !keyPoints[i].bActive )
            ofSetColor( 255 , 0 , 0 , 135 ) ;
        else
            ofSetColor( 0 , 255 , 0 , 135 ) ;
        ofCircle( keyPoints[i].p , 20 ) ;
        
        ofPushStyle() ;

            ofCircle( keyPoints[i].p , 6 ) ;
            ofNoFill() ;
            ofSetColor( 0 ) ;
            ofEnableSmoothing() ; 
            ofSetLineWidth( 1 ) ;
            ofCircle( keyPoints[i].p , 6 ) ;
        ofPopStyle() ;
        
        
        float percentComplete = poseTimer.getNormalizedProgress() ;
        if ( i > 0 )
        {
            ofPushStyle() ;
                ofSetLineWidth( 3 ) ; 
                ofSetColor( 255 , percentComplete * 255.0f ) ;
                ofLine( keyPoints[i-1].p, keyPoints[i].p ) ;
            ofPopStyle() ;
        }
    }
    
    
    
}