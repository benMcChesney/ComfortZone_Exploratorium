//
//  UserPose.cpp
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 6/2/13.
//
//

#include "UserPose.h"

bool UserPose::bCheckPose()
{
    for ( int i = 0 ; i < keyPoints.size() ; i++ )
    {
        if ( PixelHitTestHub::Instance()->getHexAt( keyPoints[i] ) != PixelHitTestHub::Instance()->backgroundHex )
        {
            
        }
        else
        {
            cout << "one point is being written over ! " << endl ;
            bActivated = false ;
            return false ; 
        }
    }
    
    bActivated = true ; 
    return true ;
}

void UserPose::addKeyPoint( ofPoint p )
{
    keyPoints.push_back( p ) ;
}

void UserPose::draw ( )
{
    ofSetColor( 255 , 0 , 0 ) ;
    if ( !bActivated )
        ofSetColor( 255 , 0 , 0 ) ; 
    for ( int i = 0 ; i < keyPoints.size() ; i++ )
    {
        
        ofCircle( keyPoints[i] , 5 ) ;
    }
}