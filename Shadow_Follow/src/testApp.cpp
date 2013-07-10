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
	
    
    
    ofSetVerticalSync( true ) ;
    ofSetFrameRate( 60 );
    
    contourFinder.setFindHoles(true) ;
    
    cam.enableMouseInput();
    
    for ( int i = 0 ; i < 1000 ; i++ )
    {
        createParticle() ;
    }
    
    sprite.loadImage( "sprite.png" ) ;
    sprite.resize( 22 , 22 ) ;
    sprite.setAnchorPercent( 0.5 , 0.5 ) ;

    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());
    
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);
    
    setupGui() ;
    particleTargetTimer.setup( 500 ) ;
    ofAddListener( particleTargetTimer.TIMER_COMPLETE , this , &testApp::timerCompleteHandler ) ; 
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofSetWindowTitle( "FPS: " + ofToString( ofGetFrameRate() ) ) ;
    kinect.update();
	particleTargetTimer.update() ;
    
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

    vector< ofPtr<ofxParticle3D> >::iterator p = particles.begin() ;
    
    float time = ofGetElapsedTimef() ;
    
  //  ofVec2f newForce = ofVec2f( ofSignedNoise( time ) * gravityVariance.x  , ofSignedNoise( time + 2.0f ) * gravityVariance.y ) ;
    ofVec2f centroid = ofVec2f ( 0 , 0 ) ;
    if ( contourFinder.getContours().size() > 0 )
        centroid = ofVec2f ( contourFinder.getCentroid(0).x , contourFinder.getCentroid(0).y ) ; 
    //ofVec2f( ofSignedNoise( time ) * 0.3 , ofSignedNoise( cos( time ) * 0.35 ) * -0.1 , -0.2f ) ;
    ofVec3f target = ofVec3f( ofGetWidth()/2 , ofGetHeight()/2 ) ;
    int count = 0 ; //0.0f ;
    
    float xAmount = 100 ;
    float yAmount = 75 ;

    
     vector<ofPoint> targets ;
    float _r = sin ( ofGetElapsedTimef() ) * centroidRadius + centroidRadius/2 ;
    if ( contourFinder.getContours().size() > 0 )
    {
        //int skipNum = contourFinder.getContours()[0].size() / numParticleTargets ;
       
     //   cout << "skipNum : " << skipNum << endl ;
        //targets.push_back( centroid ) ;
        //int maxIndex = (int)numParticleTargets - 1 ;
        //for( int i = 0 ; i < (int)numParticleTargets ; i+=skipNum )
        //{
        //    if ( i < maxIndex )
        //        targets.push_back( ofPoint( contourFinder.getContours()[0][i].x , contourFinder.getContours()[0][i].y )) ;
        //}
        
        float anglePer = TWO_PI / ( float ) numParticleTargets ;
        for ( int i = 0 ; i < numParticleTargets ; i++ )
        {
            ofVec3f t = centroid ;
            t.x += sin ( (float) i * anglePer ) * _r ;
            t.y += cos ( (float) i * anglePer ) * _r * scaleY ;
            targets.push_back( t ) ; 
        }
    
    }
    targets.push_back( centroid ) ;
    
    cout << "targets.size() : " << targets.size() << endl;
  //  ofVec2f randomTarget = ofVec2f( centroid.x += ofRandom ( -50 , 50 ) , centroid.y += ofRandom ( -50 , 50 ) );
    while ( p != particles.end() )
    {
        //(*p)->addForce( ofSignedNoise( time ) , ofSignedNoise(count + time ) , 0 ) ;
        
        // if ( bFlockToggle )
             //(*p)->seek( target.x , target.y , target.z ) ;
        float d = (*p)->distance( centroid) ;
        
       // ofVec3f tar ;
        
        int indexGroup = count % targets.size() ;
        
        for ( int t = 0 ; t < targets.size() ; t++ )
        {
            if ( indexGroup == t )
            {
                (*p)->seek( targets[t].x , targets[t].y , 0 ) ;
            }
        }
        /*
        if ( count % 4 == 0 )
            tar = target1 ;
        if ( count % 4 == 1 )
            tar = target2 ;
        if ( count % 4 == 2 )
            tar = target3 ;
        if ( count % 4 == 3 )
            tar = target4 ;
        */
        
       // else
       //     (*p)->addForce( 0.1 , 0.1 ) ; // centroid.x , centroid.y , 0 ) ;
          //  (*p)->flee(centroid.x , centroid.y , 0 ) ;
        //fVec3f& a_target, bool a_slowdown, float a_scale,
        //(*p)->steer( ofVec3f( 0 , 0, 0 ) , false , 5.0f ) ;
        (*p)->addDamping( damping ) ;
        (*p)->x += (*p)->acc.x ;
        (*p)->y += (*p)->acc.y ;
        (*p)->life++ ;
        (*p)->rotation += ofSignedNoise( rotationLifeMultiplier ) ;
        //(*p)->steer( (*p), false ) ;
        time += 0.25f ;
        
        if ( (*p)->y < -45.0f )
        {
            p->reset() ;
            p = particles.erase( p ) ;
        }
        else
            p++ ;
        
        count++;
    }
    
   // light.setPosition( lightPosition ) ;
    //cam.dolly(  ) ;
    //cam.setPosition( 0 , 0 , -800 ) ;
    float _time = ofGetElapsedTimef() ; // * timeFactor ;
    cam.setPosition(  0 , sin ( _time * 0.89 ) * wanderFactor.y , 700  ) ;
    cam.truck( sin ( _time ) * wanderFactor.x ) ;
    //cam.pan(  sin ( ofGetElapsedTimef() * 0.89 ) * wanderFactor.y ) ;
    
    cam.dolly( cos ( _time * 0.96f )  * wanderFactor.z ) ;
    
    cam.lookAt( ofVec3f() ) ; //, ofVec3f( 0 , 1 , 0 ) ) ;
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground( 0 , 0 , 0 ) ; 
    ofSetColor( 255 ) ;
    //kinect.drawDepth( 300 , 0 , 320 , 240 ) ;
   // grayImage.draw( 600 , 0 , 320 , 240 ) ;
    
    post.begin( cam ) ;
        
    ofEnableAlphaBlending() ;
    
    ofPushMatrix() ;
    
    ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
    
    vector< ofPtr<ofxParticle3D> >::iterator p = particles.begin() ;
    
    float time = ofGetElapsedTimef() ;
    int count = 0 ;
    while ( p != particles.end() )
    {
        ofPushMatrix() ;
        ofTranslate( (*p)->x , (*p)->y , (*p)->z ) ; //->acc ) ;
        ofSetColor( (*p)->color ) ;
        float _scale = cos ( time + (float)((*p)->life)/100.0f  ) * 0.12f  + 0.9f;
        ofRotateZ( (*p)->rotation ) ;
        ofScale( _scale , _scale ) ;
        sprite.draw( 0, 0 ) ;
        ofPopMatrix() ;
        
        count++ ;
        p++;
    }
    
    ofPopMatrix() ;
    
    post.end() ;

    glDisable(GL_DEPTH_TEST ) ; 
    cam.begin(  ) ;
        ofPushMatrix() ;

        ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
    
               
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
         
            

            vector<ofPolyline> poly = path.getOutline() ;
            ofPolyline line = poly[0].getSmoothed( smoothingAmount ) ;
                
            ofPath filledPath ;
            for ( int i = 0 ; i < line.size() ; i++ )
            {
                filledPath.curveTo( line[i] ) ; 
            }
            filledPath.close() ;
            ofColor fill = ofColor::fromHsb( 255 , 255 , 255 , shadowFillAlpha )  ;
            ofSetLineWidth( 4 ) ;
            filledPath.setFillColor( fill ) ;
            filledPath.setStrokeColor(ofColor::white ) ;
            filledPath.setStrokeWidth( 0.0f ) ;
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
   // return ;
    
   
    
    
    // copy enable part of gl state
    glPopAttrib() ; //
    
    // setup gl state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    
}


void testApp::timerCompleteHandler( int &args )
{
        /*
         timerCompleteHandler
         */
    
}

void testApp::createParticle( )
{
    ofPtr<ofxParticle3D>  p = ofPtr<ofxParticle3D> (new ofxParticle3D() ) ;
    p->x = ofRandomWidth() ;
    p->y = ofRandom( -500 , 500 ) ; //spawnY + ofRandom( -spawnYVariance , spawnYVariance ) ;
    p->z = ofRandom( -50 , 50 ) ; //-spawnZVariance , spawnZVariance ) ;
    p->color = ofColor::cyan ;
    //fromHsb( ofRandom( 0 , 250 ) , ofRandom( 15 ) , 255 )  ;
    p->color.a =  ofRandom( 60 , 215 ) ;
    p->rotation = ofRandom( 0 , 360 ) ;
    particles.push_back( p ) ;
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
    
    gui->addLabel( "SHADOW FILL ALPHA " ) ;
    gui->addSlider( "SHADOW FILL ALPHA" , 0.0f , 255.0f , &shadowFillAlpha ) ;
    //shadowFillAlpha
    gui->addSlider("MAX PARTICLES" , 100.0f , 10000.0f , &maxParticles ) ; 
    gui->addSlider("DAMPING" , 0.0f , 1.0f , &damping ) ;
    
      gui->addSlider("ROTATION LIFE MULTIPLIER" , 0.0f , 0.65f , &rotationLifeMultiplier ) ;
    
    gui->addToggle("BLOOM" , false ) ;
    gui->addToggle("DOF" , false ) ;
    
    float maxWander = 500.0f ; 
    gui->addSlider("WANDER X", 0.0f, maxWander, &wanderFactor.x ) ;
    gui->addSlider("WANDER Y", 0.0f, maxWander , &wanderFactor.y ) ;
    gui->addSlider("WANDER Z", 0.0f, maxWander , &wanderFactor.z ) ;
    gui->addSlider( "TIME FACTOR" , 0.0f , 1.0f , &timeFactor ) ;

    
    gui->addSlider( "MAX STEER AMOUNT" , 0.0f , 1.5f , &maxSteerAmount ) ;
    gui->addSlider( "MAX SPEED AMOUNT" , 0.0f , 1.5f , &maxSpeedAmount ) ;

    gui->addSlider( "MIN DISTANCE" , 0.0f , 100.0f , &minDistance ) ; 

    gui->addSlider( "NUM PARTICLE TARGETS" , 0.0f , 100.0f , &numParticleTargets ) ;
    gui->addSlider( "CENTROID RADIUS" , 0.0f , 300.0f , &centroidRadius ) ;
    gui->addSlider( "Y SCALE" , 0.1 , 1.5f , &scaleY ) ; 
    //
    //
    /*
     
     ofxSimpleTimer particleTargetTimer ;
\
     
     gui->addWidgetDown(new ofxUILabel("SLIDER WIDGETS", OFX_UI_FONT_LARGE));
     gui->addSpacer(length-xInit, 1);
     gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("NORMAL SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addSlider("RED", 0.0, 255.0, red, length-xInit,dim);
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("MINIMAL SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, 0.0, 255.0, green, "GREEN", OFX_UI_FONT_MEDIUM));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("BILABEL SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUIBiLabelSlider(length-xInit, 0.0, 255.0, blue, "BLUE", "LESS BLUE", "MORE BLUE", OFX_UI_FONT_MEDIUM));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("VERTICAL SLIDERS", OFX_UI_FONT_MEDIUM));
     gui->addSlider("1", 0.0, 255.0, 100.0, dim,80);
     gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
     gui->addSlider("2", 0.0, 255.0, 150.0, dim, 80);
     gui->addSlider("3", 0.0, 255.0, 200.0, dim, 80);
     gui->addSlider("4", 0.0, 255.0, 250.0, dim, 80);
     gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("RANGE SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUIRangeSlider(length-xInit,dim, 0.0, 255.0, 50.0, 100.0, "RSLIDER"));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("ROTARY SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUIRotarySlider(dim*2.0, 0.0, 100.0, 25.0, "CSLIDER 2"));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("2D PAD", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUI2DPad(length-xInit,80, ofPoint((length-xInit)*.5,80*.5), "PAD"));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("CIRCLE SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUICircleSlider(dim*2, 0.0, 255.0, green, "GREEN", OFX_UI_FONT_MEDIUM));
     gui->addSpacer(length-xInit, 1);
     gui->addWidgetDown(new ofxUILabel("IMAGE SLIDER", OFX_UI_FONT_MEDIUM));
     gui->addWidgetDown(new ofxUIImageSlider(length-xInit, 42, 0.0, 255.0, red, "slider.png", "IMAGE SLIDER"));
     gui->setColorBack(ofColor(255,100));
    
     */
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
    
    if ( name == "BLOOM" )
    {
        bool bValue = ((ofxUIToggle *)e.widget)->getValue() ;
        post.getPasses()[0]->setEnabled( bValue ) ;
    }
    
    if ( name == "DOF" )
    {
        bool bValue = ((ofxUIToggle *)e.widget)->getValue() ;
        post.getPasses()[1]->setEnabled( bValue ) ;
    }
    
    if ( name == "MAX STEER AMOUNT" )
    {
        float value = ((ofxUISlider *) e.widget)->getScaledValue() ;
        for ( int i = 0 ; i < particles.size() ; i++ )
        {
            particles[i]->MAX_STEER = value ; 
        }
    }
    
    if ( name == "MAX SPEED AMOUNT" )
    {
        float value = ((ofxUISlider *) e.widget)->getScaledValue() ;
        for ( int i = 0 ; i < particles.size() ; i++ )
        {
            particles[i]->MAX_SPEED = value ;
        }
    }
/*
 gui->addSlider(  , 0.0f , 15.0f , & ) ;
 gui->addSlider( "" , 0.0f , 10.0f , &maxSpeedAmount ) ;

 */
    
   
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