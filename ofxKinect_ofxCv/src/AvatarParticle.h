//
//  AvatarParticle.h
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 5/23/13.
//
//

#ifndef __ofxKinect_ofxCv__AvatarParticle__
#define __ofxKinect_ofxCv__AvatarParticle__

#include "ofMain.h"
#include "ofxParticle2D.h"

class AvatarParticle 
{
    public :
    AvatarParticle()
    {
        frameLife = -1 ;
        inertiaX = 0 ;
        inertiaY = 0 ; 
    }
    ~AvatarParticle( ) { }
    
    void setup ( float _x , float _y , int _frameLife , float _inertiaX , float _intertiaY ) ;
    void update ( ) ;
    void draw( ) ;
        
    int frameLife ;
    float inertiaX , inertiaY ;
    ofPoint p ; 
        
};

#endif /* defined(__ofxKinect_ofxCv__AvatarParticle__) */
