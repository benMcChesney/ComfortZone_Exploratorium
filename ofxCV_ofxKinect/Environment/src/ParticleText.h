//
//  ParticleText.h
//  Environment
//
//  Created by Ben McChesney on 6/25/13.
//
//

#ifndef __Environment__ParticleText__
#define __Environment__ParticleText__

#include "ofMain.h"
#include <iostream>

class ParticleText
{
    public :
        ParticleText() { }
        ~ParticleText() { }
    
        void setup ( string _text , string fontPath ) ;
        void update ( ) ;
        void draw ( ) ;

        string text ;
        ofPoint pos ;   //position
        vector<ofPoint> attractVerts ;

        void createAttractVerts( int numVerts )  ;
        ofTrueTypeFont font ;

        ofFbo textFbo;

        void drawDebug( ) ;


};
#endif /* defined(__Environment__ParticleText__) */
