#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxUI.h"
#include "PixelHitTestHub.h"
#include "UserPose.h"
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
    
        ofEasyCam cam ;
    
        int userFillHex ;

        bool bDebug ;
    
    
        //vector< ofPtr<UserPose> > poses;
        ofPtr<UserPose> userPose ;
        void randomizePoseKeyPoints( ) ;
        float keyPointEdgePadding ; 
        float numKeyPoints ;
    
        
    
        ofPoint userRenderOffset ;
        void poseTimerCompleteHandler( int &args ) ;
};
