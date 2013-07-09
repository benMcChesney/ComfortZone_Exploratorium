//
//  KeyPointPose.cpp
//  poseMakerTool
//
//  Created by Ben McChesney on 6/29/13.
//
//

#include "KeyPointPose.h"

void KeyPointPose::setup ()
{
    _in = ofRandom( 0.0 , 0.49 ) ;
    _out =ofRandom( 0.5 , 1.0 ) ;
    
    keyPointRadius = 15 ;
    poseTimer.setup ( 500 ) ; 
}

void KeyPointPose::createDebugPoints( )
{
    _in = ofRandom( 0.0 , 0.49 ) ;
    _out =ofRandom( 0.5 , 1.0 ) ;
                   
    int numPoints = ofRandom( 2 , 6 ) ;
   for ( int i = 0 ; i < numPoints ; i++ )
   {
       pts.push_back( ofPoint( ofRandom ( 50 , 750 ) , ofRandom ( 50 , 550 ) ) ) ;
   }
}

bool KeyPointPose::getIsActive( float nPosition )
{
    if ( nPosition >= _in && nPosition <= _out )
    {
        return true ;
    }
    else
    {
        return false ; 
    }
}

void KeyPointPose::addPoint ( ofPoint p )
{
    pts.push_back( p ) ;
    ptsStatus.push_back( false ) ; 
}

void KeyPointPose::resetPtsStatus()
{
    for ( int i = 0 ; i < ptsStatus.size() ; i++ )
    {
        ptsStatus[ i ] = false ; 
    }
}

void KeyPointPose::drawDebugMap( )
{
    
    if ( pts.size() > 0 )
    {
        //ofSetHexColor( uniqueInputHex ) ;
        for ( int i = 0 ; i < pts.size() ; i++ )
        {
            if ( ptsStatus[i] == true )
            {
                ofSetColor( 0 , 255 , 0 ) ;
                ofCircle ( pts[ i ] ,keyPointRadius ) ;
            }
            else
            {
                ofSetColor( 255 , 0 , 0 , 200 ) ;
                ofCircle ( pts[ i ] , keyPointRadius* 0.75f ) ;
            }
            
        }
    }
    
}

void KeyPointPose::drawInputMap( )
{
    
    if ( pts.size() > 0 )
    {
        ofSetHexColor( uniqueInputHex ) ;
        for ( int i = 0 ; i < pts.size() ; i++ )
        {
            ofCircle ( pts[ i ] ,keyPointRadius ) ; 
        }
    }
    
}