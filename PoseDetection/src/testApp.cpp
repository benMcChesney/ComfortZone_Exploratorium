#include "testApp.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	//kinect.setRegistration(true);
    
	//kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
    
    colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height );
	grayThreshNear.allocate(kinect.width, kinect.height );
	grayThreshFar.allocate(kinect.width, kinect.height );
	

    nearThreshold = 230;
	farThreshold = 70;
	
   
    
    PixelHitTestHub::Instance()->setup( ofGetWidth() , ofGetHeight() , 0x000000 , 6 , 4 ) ;
    userFillHex = PixelHitTestHub::Instance()->getColorfulUniqueHex() ; 
    
    ofSetVerticalSync( true ) ;
    ofSetFrameRate( 60 );
    
    contourFinder.setFindHoles(true) ;
    
    cam.enableMouseInput();
    
    //This is the main performance point, raising the last two paramters will significanlty boost performance
    //but they will be slightly less accurate per frame increment

    userPose = ofPtr<UserPose>( new UserPose() ) ; 
    userRenderOffset = ofVec2f( (ofGetWidth()- kinect.getWidth() )/2  , ( ofGetHeight() - kinect.getHeight()  ) / 2 ) ;
    
     setupGui() ;
    randomizePoseKeyPoints() ;
    ofAddListener( userPose->poseTimer.TIMER_COMPLETE , this , &testApp::poseTimerCompleteHandler ) ;
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofSetWindowTitle( "FPS: " + ofToString( ofGetFrameRate() ) ) ;
    kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew())
    {
        // load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height );
		
        //use ofxCV...
        grayThreshNear = grayImage ; 
        grayThreshFar = grayThreshNear ;
        //threshold( grayThreshNear , farThreshold );
        //threshold( grayThreshFar , nearThreshold );
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        //bitwise_and( grayThreshNear, grayThreshFar , grayImage ) ;
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
        grayImage.mirror( false , true ) ;
        //grayImage.update();
        contourFinder.findContours( grayImage ) ;
        
        userPose->bCheckPose( userFillHex , userRenderOffset ) ;
       
    }
    
    
    userPose->update() ;
 
}


//--------------------------------------------------------------
void testApp::draw(){
        
    ofSetColor( 255 ) ;
    
    // copy enable part of gl state
   // glPushAttrib(GL_ENABLE_BIT);
    
    // setup gl state
   // glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    
    //kinect.draw( 300 , 15 ) ;
    //cam.begin(  ) ;
    //cam.begin() ;
        //ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
    
    ofPushMatrix() ;
       
    
    //ofTranslate( offset ) ;
    
    vector<vector<cv::Point> > contours = contourFinder.getContours() ;
    
    
    //ofScale( 1 , -1 ) ;
    ofColor fill = ofColor::fromHsb( ofGetFrameNum() % 255 , 255 , 255 , 80.0f + 135.0f * userPose->poseTimer.getNormalizedProgress() )  ;
    
    vector<ofPtr<ofPath> > paths ;
    ofPushMatrix() ;
        
        for ( int c = 0 ; c < contours.size() ; c++ )
        {
            ofPath path ;
            
            for ( int i = 0 ; i < contours[c].size() ;i+=6 )
            {
               
                cv::Point p = contours[c][i] ;
                if ( i == 0 )
                    path.moveTo( p.x + userRenderOffset.x , p.y + userRenderOffset.y ) ;
                else
                    path.lineTo( p.x + userRenderOffset.x, p.y + userRenderOffset.y ) ;
            }
            
            path.close() ;
            /*
            ofColor fill = ofColor::fromHsb( ofGetFrameNum() % 255 , 255 , 255 )  ;
            ofSetLineWidth( 4 ) ;
            path.setFillColor( fill ) ; 
            path.setStrokeColor(ofColor::white ) ;
            path.setStrokeWidth( 4.0f ) ;
            path.setFilled( true ) ;
            */
            

            vector<ofPolyline> poly = path.getOutline() ;
            ofPolyline line = poly[0].getSmoothed( smoothingAmount ) ;
                
            paths.push_back ( ofPtr<ofPath>( new ofPath() ) ) ;
            for ( int i = 0 ; i < line.size() ; i++ )
            {
                paths[c]->curveTo( line[i] ) ;
            }
            paths[c]->close() ;
            
            ofSetLineWidth( 4 ) ;
            //paths[c]->setFillColor( fill ) ;
            
            paths[c]->setFilled( true ) ;
            ofFill() ; 
            //polyline.draw() ;
            ofSetLineWidth( 1 ) ;
        }
    
    
    
    if ( PixelHitTestHub::Instance()->beginFbo() == true )
    {
        for( int p = 0 ; p < paths.size() ; p++ )
        {
            paths[ p ]->setFillHexColor( userFillHex ) ;
            paths[ p ]->draw() ;
        }
        PixelHitTestHub::Instance()->endFbo() ;         //stop drawing the map
    }
   
    for( int p = 0 ; p < paths.size() ; p++ )
    {
        paths[ p ]->setFillColor( fill ) ;
        paths[ p ]->setStrokeColor(ofColor::white ) ;
        paths[ p ]->setStrokeWidth( 4.0f ) ;
        paths[ p ]->draw() ;
    }
    
    ofPopMatrix() ;
    ofPopMatrix() ; 
    
    ofPopMatrix() ;
    
    // copy enable part of gl state
    glPopAttrib() ; //
    
    // setup gl state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    paths.clear() ;
    
    if ( bDebug )
    {
        PixelHitTestHub::Instance()->debugDraw() ;
    }
    
    userPose->draw() ;

    stringstream ss ;
    ss << "% timer complete : " << userPose->poseTimer.getNormalizedProgress() << endl ;
    ofDrawBitmapStringHighlight( ss.str() , 315 , 200 ) ; 
}

void testApp::randomizePoseKeyPoints( )
{
    userPose->keyPoints.clear() ;
    
    
    for ( int i = 0 ; i < numKeyPoints ; i++ )
    {
        userPose->addKeyPoint( ofPoint( ofRandom( userRenderOffset.x + keyPointEdgePadding , userRenderOffset.x + kinect.getWidth() - keyPointEdgePadding ) ,
                                        ofRandom( userRenderOffset.y + keyPointEdgePadding , userRenderOffset.y + kinect.getHeight() - keyPointEdgePadding ) ) ) ;
    }
    
}


void testApp::setupGui()
{
    //ofxUICanvas *gui;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 200 ;
    float dim = 25 ;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    //gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    //gui->addSpacer() ;
    gui->addLabel("DEPTH RANGE" ) ;
    gui->addRangeSlider( "KINECT PIXEL RANGE", 0 , 255, farThreshold , nearThreshold ) ;
    gui->addSpacer( ) ;
    int totalPixels = kinect.getWidth() * kinect.getHeight() ;
    gui->addLabel( "CV BLOB SIZE " ) ;
    gui->addRangeSlider( "CV BLOB SIZE" , totalPixels * .01 , totalPixels * .75f , minBlobSize , maxBlobSize ) ; 
    
    gui->addLabel( "KINECT MOTOR ANGLE " ) ;
    gui->addSlider( "KINECT MOTOR ANGLE" , -30 , 30 ,  angle ) ;

    gui->addLabel( "SMOOTHING " ) ;
    gui->addSlider( "SMOOTHING" , 0.1f , 12.0f ,  &smoothingAmount ) ;

    gui->addLabel( "ENABLE DEBUG MODE " ) ;
    gui->addToggle( "ENABLE DEBUG MODE" , &bDebug ) ;
    
    gui->addLabel( "NUM OF KEYPOINTS " ) ;
    gui->addSlider( "NUM OF KEYPOINTS" , 1.0f , 15.0f , &numKeyPoints ) ;
    
    gui->addLabel( "RANDOMIZE KEYPOINTS " ) ;
    gui->addToggle( "RANDOMIZE KEYPOINTS" , false ) ;
    
    gui->addLabel( "EDGE PADDING " ) ;
    gui->addSlider( "KEY POINTS EDGE PADDING" , 1.0f , 45.0f , &keyPointEdgePadding ) ;
    
    gui->addLabel( "POSE HOLD TIME MILLIS " ) ;
    gui->addSlider( "POSE HOLD TIME MILLIS" , 500.0f , 6000.0f , &userPose->poseTimerDelayMillis ) ;

    //keyPointEdgePadding
    //smoothingAmount
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings( "GUI/settings.xml" ) ;
}

void testApp::hideAllGuis( )
{
    gui->disable() ;
}

void testApp::poseTimerCompleteHandler( int &args )
{
    cout << "POSE TIMER COMPELTE ! <" << ofGetElapsedTimef() << endl ;
    randomizePoseKeyPoints() ;
    userPose->poseTimer.stop() ;
    ofBackground( ofColor::fromHsb( ofRandom( 0 , 255 ), ofRandom( 215 , 255 ) , ofRandom( 220, 255 ) ) ) ;
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
        
    if(name == "CV BLOB SIZE" )
	{
		ofxUIRangeSlider *slider = ((ofxUIRangeSlider *) e.widget);
		minBlobSize = slider->getScaledValueLow();
        maxBlobSize = slider->getScaledValueHigh() ;
        contourFinder.setMinArea( minBlobSize ) ;
        contourFinder.setMaxArea( maxBlobSize ) ; 
        // cout << "value: " << slider->getScaledValue() << endl;
	}
    
    
    if(name == "RANDOMIZE KEYPOINTS" )
	{
        ofxUIToggle * toggle = (ofxUIToggle *) e.widget ;
        if ( toggle->getValue() == true )
        {
            randomizePoseKeyPoints() ;
            toggle->setValue( false ) ; 
        }
    }
//   gui->addToggle( "RANDOMIZE KEYPOINTS " , false ) ;
    gui->saveSettings( "GUI/settings.xml" ) ;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //unsigned idx = key - '0';
    //if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
    
    switch ( key )
    {
        case 'q':
        case 'Q':
            hideAllGuis() ;
            break ;
            
        case 'w':
        case 'W':
            hideAllGuis() ; 
            gui->enable() ;
            break ;
            
        case 'e':
        case 'E':
            hideAllGuis() ;
            break ;
            
        case 'f':
        case 'F':
            ofToggleFullscreen() ;
            break; 
    }
    
    PixelHitTestHub::Instance()->keyPressed( key ) ; 
    
}

    



//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    int hex = PixelHitTestHub::Instance()->getHexAt( ofVec2f( x , y ) ) ;
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