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
	
    setupGui() ;
    
    ofSetVerticalSync( true ) ;
    ofSetFrameRate( 60 );
    
    contourFinder.setFindHoles(true) ;
    
    cam.enableMouseInput();
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
        
        //grayImage.update();
        contourFinder.findContours( grayImage ) ; 
    }

 
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor( 255 ) ;
    
    // copy enable part of gl state
    glPushAttrib(GL_ENABLE_BIT);
    
    // setup gl state
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    
    //kinect.draw( 300 , 15 ) ;
    cam.begin(  ) ; 
    //cam.begin() ;
        ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
    
    ofPushMatrix() ;
       
    ofVec2f offset = ofVec2f( (ofGetWidth()- kinect.getWidth() )/2  , ( ofGetHeight() - kinect.getHeight()  ) ) ;
    ofTranslate( offset ) ;
    
    vector<vector<cv::Point> > contours = contourFinder.getContours() ;
    
    ofScale( 1 , -1 ) ;


    ofPushMatrix() ;
        
        for ( int c = 0 ; c < contours.size() ; c++ )
        {
            ofPath path ;
            
            for ( int i = 0 ; i < contours[c].size() ;i+=6 )
            {
               
                cv::Point p = contours[c][i] ;
                if ( i == 0 )
                    path.moveTo( p.x , p.y ) ;
                else
                    path.lineTo( p.x , p.y ) ;
//                polyline.addVertex( p.x , p.y ) ;
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
                
            ofPath filledPath ;
            for ( int i = 0 ; i < line.size() ; i++ )
            {
                filledPath.curveTo( line[i] ) ; 
            }
            filledPath.close() ;
            ofColor fill = ofColor::fromHsb( ofGetFrameNum() % 255 , 255 , 255 )  ;
            ofSetLineWidth( 4 ) ;
            filledPath.setFillColor( fill ) ;
            filledPath.setStrokeColor(ofColor::white ) ;
            filledPath.setStrokeWidth( 4.0f ) ;
            filledPath.setFilled( true ) ;
            
            //path.draw( ) ;
            filledPath.draw( ) ; 
            ofFill() ; 
            //polyline.draw() ;
            ofSetLineWidth( 1 ) ;
        }
    ofPopMatrix() ;
    ofPopMatrix() ;

    
    cam.end() ;
    
    ofPopMatrix() ;
    
    //ofDrawBitmapStringHighlight( ss.str() , 350 , 25 ) ;
    //particles.size()
    
    
    // copy enable part of gl state
    glPopAttrib() ; //
    
    // setup gl state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
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
    
    int totalPixels = kinect.getWidth() * kinect.getHeight() ;
    //gui->addLabel( "CV BLOB SIZE " ) ;
    gui->addRangeSlider( "CV BLOB SIZE" , totalPixels * .01 , totalPixels * .75f , minBlobSize , maxBlobSize ) ; 
    
    gui->addLabel( "KINECT MOTOR ANGLE " ) ;
    gui->addSlider( "KINECT MOTOR ANGLE" , -30 , 30 ,  angle ) ;

    gui->addLabel( "SMOOTHING " ) ;
    gui->addSlider( "SMOOTHING" , 1.0f , 30.0f ,  &smoothingAmount ) ;

    //smoothingAmount
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings( "GUI/settings.xml" ) ;
}

void testApp::hideAllGuis( )
{
    gui->disable() ;
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