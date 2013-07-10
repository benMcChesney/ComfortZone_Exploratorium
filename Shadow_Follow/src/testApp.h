#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "ofxParticle3D.h" 
#include "ofxPostProcessing.h"
#include "ofxSimpleTimer.h"

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
     
        ofFbo blobFbo ; 
        ofxKinect kinect;
        int nearThreshold;
        int farThreshold;
        
        int angle;
    
        ofxCv::ContourFinder contourFinder;
    
        //ofxUI
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);
        void setupGui() ;
        void hideAllGuis( ) ; 
    
    
        float smoothingAmount ; 
        float minBlobSize , maxBlobSize ;
        float shadowFillAlpha ;
    
        ofEasyCam cam ;
    
        void createParticle( ) ;
        vector< ofPtr<ofxParticle3D> > particles ;
        float maxParticles ;
        float damping ;
    
        ofxPostProcessing post ;
    
        ofImage sprite ;
        float rotationLifeMultiplier ;
        float rotation ;
    
        ofPoint wanderFactor ;
        float timeFactor ;
    
        ofPoint drawOffset ;
    
        float maxSteerAmount ;
        float maxSpeedAmount ;
    
        float minDistance ;
    
        ofxSimpleTimer particleTargetTimer ;
        float numParticleTargets ;
        void timerCompleteHandler( int &args ) ;
    
        float centroidRadius ;
        float scaleY ; 
};
