#include "testApp.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
    
    colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height , OF_IMAGE_GRAYSCALE );
	grayThreshNear.allocate(kinect.width, kinect.height , OF_IMAGE_GRAYSCALE );
	grayThreshFar.allocate(kinect.width, kinect.height , OF_IMAGE_GRAYSCALE );
	

    nearThreshold = 230;
	farThreshold = 70;
	
	//ofSetFrameRate(60);
    winSize = 6 ;
    qualityLevel = 0.02 ;
    maxLevels = 8 ;
    maxFeatures = 100 ;
    minDistance = 4 ; 
    setupGui() ;
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofSetWindowTitle( "FPS: " + ofToString( ofGetFrameRate() ) ) ;
    kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew())
    {
        // load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE );
		
        //use ofxCV...
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        threshold( grayThreshNear , nearThreshold );
        threshold( grayThreshFar , farThreshold );
        bitwise_and( grayThreshNear, grayThreshFar , grayImage ) ;
        grayImage.update(); 
        contourFinder.findContours( grayImage ) ; 
      
        
        flow.setWindowSize( winSize );
        flow.setQualityLevel( qualityLevel ) ;
        flow.setMaxFeatures( maxFeatures );
        flow.setMinDistance( minDistance ) ;
        flow.setMaxLevel( maxLevels ) ;
        flow.calcOpticalFlow( grayImage );

        
        

                
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor( 255 ) ;
    
    ofPushMatrix() ;
        ofTranslate( 275 , 10 ) ;
        ofScale( 0.75 , 0.75 ) ; 
        grayImage.draw( 0 , 0 ) ;
        kinect.draw( 0 , kinect.getHeight() ) ; 
        //contourFinder.draw() ;
        flow.draw( ) ;
        //flow.drawFlow( ofRect( 0  , 0, ofGetWidth() , ofGetHeight() ) ;
    
        vector<ofPoint> features = flow.getFeatures() ;
        vector<ofVec2f> motion = flow.getMotion() ;
    
    int maxIndex = features.size() ;
    if ( motion.size() < maxIndex )
        maxIndex = motion.size() ;
    
    ofEnableAlphaBlending() ;
    //cout << "motion.size() " << motion.size() << endl ;
     //   vector<ofPoint> flos =
        for ( int i = 0 ; i < maxIndex; i++ )
        {
            ofSetColor( 255 , 80 ) ;
            ofPushMatrix() ;
                ofTranslate( features[i] ) ;
                ofCircle( 0 , 0, 0 , 4 ) ;
                ofSetColor( 25 , 215 , 20  ) ;
                ofCircle( motion[i] * motionScale , 2 ) ;
            ofPopMatrix() ;
        }
    
        //cout << "# features : " << features.size() << endl ;
    ofPopMatrix() ;
}

void testApp::setupGui()
{
    //ofxUICanvas *gui;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 200 ;
    float dim = 25 ;
    
    gui = new ofxUIScrollableCanvas(0, 0, length+xInit, ofGetHeight());
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer() ; 
    gui->addLabel("DEPTH RANGE" ) ; 
    gui->addRangeSlider( "KINECT PIXEL RANGE", 0 , 255, farThreshold , nearThreshold ) ;
    gui->addSpacer() ;
    gui->addSlider( "KINECT MOTOR ANGLE" , -30 , 30 ,  angle ) ; 
    gui->addLabel("OPTICAL FLOW PARAMS" ) ;
    gui->addSlider( "WINSIZE", 4, 64,  winSize ) ;
    gui->addSlider( "MAX FEATURES" , 1 , 1000 , maxFeatures ) ;
    gui->addSlider( "QUALITY LEVEL" , 0.0 , 1.0 , qualityLevel ) ;
    gui->addSlider( "MIN DISTANCE" , 0.01 , 16.0f , minDistance ) ;
    gui->addSlider( "MOTION SCALE" , 0.1 , 10000.0f , motionScale ) ;
      
    
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings( "GUI/settings.xml" ) ;
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
	
	if(name == "KINECT PIXEL RANGE" )
	{
		ofxUIRangeSlider *slider = ((ofxUIRangeSlider *) e.widget);
		farThreshold = slider->getScaledValueLow();
        nearThreshold = slider->getScaledValueHigh() ;
       // cout << "value: " << slider->getScaledValue() << endl;
	}
    
    // int angle;
    if ( name == "KINECT MOTOR ANGLE" )
    {
        angle = ((ofxUISlider *) e.widget)->getScaledValue() ;
        kinect.setCameraTiltAngle( angle ) ; 
    }
    
//    if ( name == "PYRAMID SCALE" ) pyramidScale = ((ofxUISlider *) e.widget)->getScaledValue() ;
//    if ( name == "LEVELS" ) levels = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if ( name == "WINSIZE" ) winSize = ((ofxUISlider *) e.widget)->getScaledValue() ;
    
    if ( name == "MAX LEVELS" ) maxLevels = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if ( name == "MAX FEATURES" ) maxFeatures = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if ( name == "QUALITY LEVEL" ) qualityLevel = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if ( name == "MIN DISTANCE" ) minDistance = ((ofxUISlider *) e.widget)->getScaledValue() ;
    if ( name == "MOTION SCALE" ) motionScale = ((ofxUISlider *) e.widget)->getScaledValue() ;
    
    gui->saveSettings( "GUI/settings.xml" ) ;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}