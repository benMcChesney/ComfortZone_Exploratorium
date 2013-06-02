#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "AvatarParticle.h"
#include "Landscape.h"
#include "ofxPostProcessing.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofxCvColorImage colorImg;
        
        ofxCvGrayscaleImage grayImage; // grayscale depth image
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
        ofxKinect kinect;
        int nearThreshold;
        int farThreshold;
        
        int angle;
    
        ofxCv::ContourFinder contourFinder;
    
        //ofxUI
        ofxUIScrollableCanvas *gui;
        void guiEvent(ofxUIEventArgs &e);
        void setupGui() ;
        void hideAllGuis( ) ; 
    
    
        //Optical Flow Stuff
        ofxCv::FlowFarneback flow;
        //ofxCv::FlowPyrLK flow;
        float polySigma , polyN , pyramidScale;
        int windowSize , numLevels , numIterations ;
        bool bUseGaussian ;
        float motionScale ;
    
        float minBlobSize , maxBlobSize ;
    
        void customFlowDraw ( ) ;
        float flowScaling ;
        float minParticleForce ;
        vector<AvatarParticle*> particles ;
        int maxParticles ;

        ofPoint particleGravity ;
        ofPoint particleLife ;

        ofFbo particleFbo ;
        float particleFboFade ;


        ofImage particleSprite ;
    
        Landscape landscape ;
    
        float zGravity ; 
        ofEasyCam cam ;
        //AnimatedBackground
        ofxPostProcessing post;
    
        ofVec3f renderOffset ; 


};
