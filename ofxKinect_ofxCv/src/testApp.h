#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxUI.h"

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
        
        ofImage grayImage; // grayscale depth image
        ofImage grayThreshNear; // the near thresholded image
        ofImage grayThreshFar; // the far thresholded image
    
        ofxKinect kinect;
        int nearThreshold;
        int farThreshold;
        
        int angle;
    
        ofxCv::ContourFinder contourFinder;
    
        //ofxUI
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);
        void setupGui() ;
    
    
        //Optical Flow Stuff
        //ofxCv::FlowFarneback farneback;
        ofxCv::FlowPyrLK flow;
        float qualityLevel , minDistance ;
        int winSize , maxLevels , maxFeatures ;
 
        float motionScale ; 
    
  

};
