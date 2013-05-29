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
	
	//ofSetFrameRate(60);
    bUseGaussian = true ; 
    setupGui() ;
    
    particleFbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA ) ;
    particleFbo.begin() ;
        ofClear( 0 , 0, 0 , 1 ) ; 
    particleFbo.end() ;
    
    particleSprite.loadImage( "assets/particle.jpg" ) ;
    particleSprite.resize( 8 , 8 ) ;
    particleSprite.setAnchorPercent( 0.5 , 0.5 ) ;
    
    landscape.setup() ;
    
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
    
        ofxCvGrayscaleImage smallImage = grayImage ;
        smallImage.resize( grayImage.getWidth() / 4.0f , grayImage.getHeight() / 4.0f ) ;
        flow.calcOpticalFlow( smallImage ) ;        
    }
    
    landscape.update() ; 
    /*
        for ( p = particles.begin() ; p != particles.end() ; p++ )
    {
        (*p).update() ;
        if ( (*p).frameLife < 0 )
            p = particles.erase( p ) ;
    }*/
    
    //if ( ofGetFrameNum() % 300 == 0 )
    //    particles.clear() ;
    
    if ( particles.size() > 0 )
    {
        vector<AvatarParticle*>::iterator p = particles.begin() ;
        while ( p != particles.end() )
        {
            (*p)->update() ;
            (*p)->p.z += zGravity ; 
            if ( (*p)->frameLife < 0 )
            {
                delete ( *p ) ;
                p = particles.erase( p ) ; 
            }
            else
            {
                p++ ;
            }
        }
    }
    /*
     gui->addSlider( "FLOW SCALING", 0.1f, 1.0f  ,  flowScaling ) ;
     gui->addSlider( "NUM PARTICLES" , 1 , 5000 , maxParticles ) ;
     gui->addSlider( "MIN PARTICLEn FORCE" , 0.01 , 5.0f , minParticleForce )
     ;*/
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground( 0 , 0 ,0 ) ; 
    ofSetColor( 255 ) ;
    
    cam.begin() ;
        ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
    
    ofPushMatrix() ;
    
    particleFbo.begin() ;
    ofSetColor( 0 , 0 , 0, particleFboFade ) ;
    ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ;
    
        ofVec2f offset = ofVec2f( (ofGetWidth()- kinect.getWidth() )/2  , ( ofGetHeight() - kinect.getHeight()  ) /2  ) ;
        ofTranslate( offset ) ;
    
        //ofTranslate( 275 , 10 ) ;
        //ofScale( 0.75 , 0.75 ) ;
        //grayImage.draw( 0 , 0 ) ;
        //kinect.draw( 0 , kinect.getHeight() ) ;
        vector<vector<cv::Point> > contours = contourFinder.getContours() ;
        
    
        
        

        for ( int c = 0 ; c < contours.size() ; c++ )
        {
  //          ofPolyline polyline ;
            ofPath path ;
            
            for ( int i = 0 ; i < contours[c].size() ;i++ )
            {
               
                cv::Point p = contours[c][i] ;
                if ( i == 0 )
                    path.moveTo( p.x , p.y ) ;
                else
                    path.lineTo( p.x , p.y ) ;
//                polyline.addVertex( p.x , p.y ) ;
            }
            
            path.close() ; 
            ofColor fill = ofColor::fromHsb( ofGetFrameNum() % 255 , 255 , 255 )  ;
            ofSetLineWidth( 4 ) ;
            path.setFillColor( fill ) ; 
            path.setStrokeColor(ofColor::white ) ;
            path.setStrokeWidth( 4.0f ) ;
            path.setFilled( true ) ;
            path.draw( ) ;
            /*
            for ( int s = 0 ; s < 5 ; s++ )
            {
                path.scale( 1.1 , 1.1 ) ;
                ofPolyline polyline = path.getOutline()[0] ;
                ofRectangle bounds = polyline.getBoundingBox() ;
                cout << "bounds :" << bounds.x << " , " << bounds.y << " , " << bounds.width << " , " << bounds.height << endl;
                
                bounds.width += -bounds.x ;
                bounds.height += -bounds.y ;
                ofTranslate( bounds.getWidth() / -2 , polyline.getBoundingBox().getHeight() / -2) ;
            
                path.setFilled( false ) ;
                path.draw( ) ;
            }*/
         //   polyline.setClosed(true) ;
          
            ofFill() ; 
            //polyline.draw() ;
            ofSetLineWidth( 1 ) ;
        }
        //contourFinder.draw() ;
        customFlowDraw( ) ;
    
        ofTranslate( offset * -1 ) ;
       
    
        ofSetColor( ofColor::fromHsb( (ofGetFrameNum()* 3) % 255 , 255 , 255 )  ) ;
        ofTranslate( offset ) ;
    
        ofEnableBlendMode(OF_BLENDMODE_ADD) ;
        vector<AvatarParticle*>::iterator p = particles.begin() ;
        while ( p != particles.end() )
        {
            //(*p)->draw( ) ;
            particleSprite.draw( (*p)->p ) ;
            p++ ;
        }
        ofEnableAlphaBlending() ;
        particleFbo.end() ;
    
        ofSetColor( 255 ) ;
        ofPushMatrix() ;
            ofTranslate(0 , particleFbo.getHeight() ) ;
            ofScale( 1 , -1 ) ;
            particleFbo.draw( 0 , 0 ) ;
        ofPopMatrix() ;

    ofPopMatrix() ;

    landscape.draw( ) ; 
    cam.end() ; 
    
    ofPopMatrix() ;
    
    stringstream ss ;
    ss << " PARTICLES : "  << particles.size() << " / " << maxParticles << endl ; 
    
    //ofDrawBitmapStringHighlight( ss.str() , 350 , 25 ) ;
    //particles.size()
    
    

}

void testApp::customFlowDraw( )
{
    
    ofPushMatrix() ;
        //ofScale( 4.0f , 4.0f , 1.0f ) ;
    
        int w = flow.getWidth() ;
        int h = flow.getHeight() ;
    
                
    
        ofVec2f scale( (float)(windowSize) / (float) w , (float)(windowSize) / (float) h ) ;
        ofVec2f offset = ofVec2f( 0 ,0 ) ; // 0.75f   * kinect.getWidth() , 0.75 * kinect.getHeight() ) ;
    
        ofFill( ) ;

        //cout << "scale : " << scale << endl ;
        int stepSize = 4 ;
        for(int y = 0; y < w ; y += stepSize) {
            for(int x = 0; x <  h; x += stepSize) {
                offset = ofVec2f( x * 4.0f , y * 4.0f ) ;
                ofVec2f cur = ofVec2f(x, y) + offset ; // * scale ;+ offset;
                ofVec2f diff = ( flow.getFlowPosition(x, y) + offset) - cur ;
                if ( particles.size() < maxParticles && diff.length() > minParticleForce )
                {
                    //Create a particle !
                    AvatarParticle * p = new AvatarParticle() ;
                    p->setup( cur.x , cur.y , ofRandom( particleLife.x , particleLife.y ) , diff.x , diff.y ) ;
                    particles.push_back( p ) ; 
                }
            }
        }
    
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
    int totalPixels = kinect.getWidth() * kinect.getHeight() ; 
    gui->addRangeSlider( "CV BLOB SIZE" , totalPixels * .01 , totalPixels * .75f , minBlobSize , maxBlobSize ) ; 
    
    gui->addSlider( "KINECT MOTOR ANGLE" , -30 , 30 ,  angle ) ;
    gui->addSpacer() ;
    gui->addSlider( "FLOW SCALING", 0.1f, 1.0f  ,  flowScaling ) ;
    gui->addLabel("OPTICAL FLOW PARAMS" ) ;
    gui->addSlider( "WINDOW SIZE", 1, 64 ,  windowSize ) ;
    gui->addSlider( "NUM LEVELS" , 1 , 16 , numLevels ) ;
    gui->addSlider( "PYRAMID SCALE" , 0.01 , 0.80 , pyramidScale ) ;
    gui->addSlider( "POLY N" , 0 , 20 , polyN ) ;
    gui->addSlider( "POLY SIGMA" , 0.01 , 10.0 , polySigma ) ;
    gui->addSlider( "NUM ITERATIONS" , 1 , 5.0 , numIterations ) ;
    gui->addToggle( "USE GAUSSIAN" , bUseGaussian ) ;
    gui->addSpacer() ;
    gui->addSlider( "NUM PARTICLES" , 1 , 5000 , maxParticles ) ;
    gui->addSlider( "MIN PARTICLE FORCE" , 0.01 , 15.0f , minParticleForce ) ;
    gui->addSlider( "PARTICLE FBO FADE" ,  0.0f , 255.0f , particleFboFade ) ;
    //gui->add2DPad("GRAVITY", ofPoint(-10,10), ofPoint(-10,10), particleGravity ) ;
    gui->addSlider( "Z GRAVITY" , -5.0f , 5.0f , zGravity ) ; 
    //particleGravity
    gui->addRangeSlider( "PARTICLE LIFE", 10 , 450, particleLife.x , particleLife.y ) ;

    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings( "GUI/settings.xml" ) ;
}

void testApp::hideAllGuis( )
{
    gui->disable() ;
    landscape.gui->disable() ; 
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
    if ( name == "WINDOW SIZE" )
    {
        windowSize = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setWindowSize( windowSize ) ; 
    }
    
    if ( name == "NUM LEVELS" )
    {
        numLevels = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setNumLevels( numLevels ) ; 
    }
    
    if ( name == "PYRAMID SCALE" )
    {
        pyramidScale = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setPyramidScale( pyramidScale ) ;
    }
    
    if ( name == "POLY N" )
    {
        polyN = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setPolyN(polyN ) ;
    }
    
    if ( name == "POLY SIGMA" )
    {
        polySigma = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setPolySigma(polySigma) ; 
    }
    
    if ( name == "NUM ITERATIONS" )
    {
        numIterations = ((ofxUISlider *) e.widget)->getScaledValue() ;
        flow.setNumIterations(numIterations) ;
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
    
    if ( name == "NUM PARTICLES" )
    {
        maxParticles = ((ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "FLOW SCALING" )
    {
        flowScaling = ((ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "MIN PARTICLE FORCE" )
    {
        minParticleForce = ((ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "PARTICLE FBO FADE" )
    {
        particleFboFade = ((ofxUISlider *) e.widget)->getScaledValue() ;
    }
    
    if ( name == "Z GRAVITY" )
    {
        zGravity = ((ofxUISlider *) e.widget)->getScaledValue() ;
    }

    
    /*
     gui->addSlider( "Z GRAVITY" , -5.0f , 5.0f , zGravity ) ; 
     */

    if(name == "PARTICLE LIFE" )
	{
		ofxUIRangeSlider *slider = ((ofxUIRangeSlider *) e.widget);
		particleLife.x = slider->getScaledValueLow();
        particleLife.y = slider->getScaledValueHigh() ;
	}
     
    /*
     gui->addRangeSlider( "PARTICLE LIFE", 10 , 450,  , particleLife.y ) ;
     gui->addSlider( "FLOW SCALING", 0.1f, 1.0f  ,  flowScaling ) ;
          gui->addSlider( "NUM PARTICLES" , 1 , 5000 , maxParticles ) ;
     gui->addSlider( "MIN PARTICLEn FORCE" , 0.01 , 5.0f , minParticleForce ) 
     ;*/
    if ( name == "RESET FEATURES" )
    {
        bool bValue = ((ofxUIToggle *) e.widget)->getValue() ;
        if ( bValue == true )
        {
            ((ofxUIToggle *) e.widget)->toggleValue() ; 
        }
    }
    if ( name == "USE GAUSSIAN" )
    {
        bUseGaussian = ((ofxUIToggle *) e.widget)->getValue() ;
        flow.setUseGaussian( bUseGaussian ) ;
    }
    
    //gui->add2DPad("GRAVITY", ofPoint(-10,10), ofPoint(-10,10), particleGravity ) ;
    if ( name == "USE GAUSSIAN" )
    {
        ofxUI2DPad * pad = (ofxUI2DPad *) e.widget ;
        particleGravity = pad->getScaledValue() ;
        flow.setUseGaussian( bUseGaussian ) ;
    }

    gui->saveSettings( "GUI/settings.xml" ) ;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch ( key )
    {
        case '1':
            hideAllGuis() ;
            break ;
            
        case '2':
            hideAllGuis() ; 
            gui->enable() ;
            break ;
            
        case '3':
            hideAllGuis() ;
            landscape.gui->enable() ;
            break ;
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