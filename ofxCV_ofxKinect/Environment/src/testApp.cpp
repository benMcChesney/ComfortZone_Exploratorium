#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //vector<ofxParticle2D*> particles ;
    for ( int i = 0 ; i < 1000 ; i++ )
    {
        createParticle() ;
    }
    ofSetVerticalSync( true ) ;
    
    
    
    ofAddListener( spawnTimer.TIMER_COMPLETE, this , &testApp::spawnTimerCompleteHandler ) ;
    
    sprite.loadImage( "sprite.png" ) ;
    sprite.resize( 16 , 16 ) ; 
    sprite.setAnchorPercent( 0.5 , 0.5 ) ;
    
    ofSetCoordHandedness(OF_RIGHT_HANDED);
    
    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());

    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);

    // Setup light
	light.setPosition(1000, 1000, 2000);
    
    trailsFbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA_FLOAT32_APPLE ) ;
    trailsFbo.begin() ;
    ofClear(  0, 0 , 0 , 1 ) ;
    trailsFbo.end() ;
    
    particleText.setup( "CAN YOU FIGURE OUT THE RULES?" , "fonts/Museo.otf" ) ; 
    cam.reset() ;

    setupUI() ;
}

//--------------------------------------------------------------
void testApp::update(){
    
    spawnTimer.update() ;
    particleText.update() ;
    
    vector< ofPtr<ofxParticle3D> >::iterator p = particles.begin() ;
    
    float time = ofGetElapsedTimef() ;

    ofVec2f newForce = ofVec2f( ofSignedNoise( time ) * gravityVariance.x  , ofSignedNoise( time + 2.0f ) * gravityVariance.y ) ;
    
    //ofVec2f( ofSignedNoise( time ) * 0.3 , ofSignedNoise( cos( time ) * 0.35 ) * -0.1 , -0.2f ) ;
    ofVec3f target = ofVec3f( ofGetWidth()/2 , ofGetHeight()/2 ) ;
    float count = 0.0f ; 
    while ( p != particles.end() )
    {
        (*p)->addForce( ofSignedNoise( time ) * gravityVariance.x + gravity.x ,ofSignedNoise(count + time ) * gravityVariance.y + gravity.y , 0 ) ;
        
       // if ( bFlockToggle )
       //     (*p)->seek( target.x , target.y , target.z ) ;
        
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
        
        count += 1.0f ; 
    }
    
    if ( particles.size() < maxParticles )
    {
        spawnTimer.start( false , false ) ; 
    }
    
    light.setPosition( lightPosition ) ; 
    //cam.dolly(  ) ;
    //cam.setPosition( 0 , 0 , -800 ) ;
    float _time = ofGetElapsedTimef() * timeFactor ;
    cam.setPosition(  0 , sin ( _time * 0.89 ) * wanderFactor.y , 700  ) ; 
    cam.truck( sin ( _time ) * wanderFactor.x ) ;
    //cam.pan(  sin ( ofGetElapsedTimef() * 0.89 ) * wanderFactor.y ) ;
    
    cam.dolly( cos ( _time * 0.96f )  * wanderFactor.z ) ;

    cam.lookAt( ofVec3f() ) ; //, ofVec3f( 0 , 1 , 0 ) ) ;
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground( 0 , 0 , 0 ) ;
    ofEnableAlphaBlending() ;
    
    post.begin( cam ) ;
    
    

    //cam.begin() ;
    // copy enable part of gl state
    glPushAttrib(GL_ENABLE_BIT);
    
    // setup gl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    
        ofEnableAlphaBlending() ;
    
    //trailsFbo.begin() ;
    
    //ofClear(  0, 0 , 0 , 0.01 ) ;

   // ofSetColor( 0 , 0 , 0 , trailsFadeAmount ) ;
   // ofRect( 0 , 0, ofGetWidth() , ofGetHeight() ) ;
    
    
    light.enable();
    
    ofPushMatrix() ;
        
        ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
        //ofEnableBlendMode( OF_BLENDMODE_ADD ) ;
        //ofScale( 1 , -1 , 1 ) ;
       
    
        vector< ofPtr<ofxParticle3D> >::iterator p = particles.begin() ;
        
        float time = ofGetElapsedTimef() ;
        int count = 0 ; 
        while ( p != particles.end() )
        {
            ofPushMatrix() ;
                ofTranslate( (*p)->x , (*p)->y , (*p)->z ) ; //->acc ) ;
                ofSetColor( (*p)->color ) ;
                float _scale = cos ( time + (float)((*p)->life)/100.0f  ) * 0.5f  + 1.0f;
                ofRotateZ( (*p)->rotation ) ;
                ofScale( _scale , _scale ) ;
                sprite.draw( 0, 0 ) ;
            ofPopMatrix() ;
                
            count++ ;
            p++; 
        }
    
    
       
         //trailsFbo.end() ;
    
    ofPopMatrix() ; 
    
    
    post.end() ;
    
    
    //glDisable(GL_DEPTH_TEST ) ;
    cam.begin() ;
        ofPushMatrix() ;
            ofScale( 1 , -1 , 1 ) ;
            //ofTranslate( -ofGetWidth()/2 , ofGetHeight()/-2 ) ;
            particleText.draw( ) ;
        ofPopMatrix() ;
    cam.end() ;
    
    
    
    // set gl state back to original
    glPopAttrib();
    light.disable() ; 
    

   // trailsFbo.draw( 0 , 0 ) ;
}


void testApp::setupUI()
{
    dimX = 300 ;
    gui = new ofxUICanvas ( dimX , ofGetHeight() ) ;
    gui->addFPS() ;
    gui->addSlider("GRAVITY Y", 0.0 , -3.0f , &gravity.y ) ;
    gui->addSlider("GRAVITY VARIANCE Y", -2.0 , 2.0f , &gravityVariance.y ) ;
    gui->addSlider("GRAVITY VARIANCE X", -0.5 , 0.5f , &gravityVariance.x ) ;
    gui->addSlider("SPAWN Y", 0.0f , ofGetHeight() * 1.2f , &spawnY );
    gui->addSlider("SPAWN Y VARIANCE" , 0.0f , 400.0f , &spawnYVariance ) ;
    gui->addSlider("SPAWN Z VARIANCE" , 0.0 , 600.0f , &spawnZVariance ) ;
    gui->addSlider("NUM PER SPAWN WAVE" , 0.0f , 15.0f , &numPerSpawnWave ) ; 
    gui->addSlider("MAX PARTICLES" , 100.0f , 10000.0f , &maxParticles ) ; 
    gui->addSlider("DAMPING" , 0.0f , 1.0f , &damping ) ;
    
    gui->addSlider("SPAWN DELAY MILLIS" , 0.0f , 5000.0f , spawnDelay ) ;
    gui->addSlider("ROTATION LIFE MULTIPLIER" , 0.0f , 0.65f , &rotationLifeMultiplier ) ;
    gui->addSlider("TRAILS FADE AMOUNT" , 0.0f , 255.0f , &trailsFadeAmount ) ;
    
    gui->addSlider("LIGHT X", -2000.0f , 2000.0f , &lightPosition.x ) ;
    gui->addSlider("LIGHT Y", -2000.0f , 2000.0f , &lightPosition.y ) ;
    gui->addSlider("LIGHT Z", -2000.0f , 2000.0f , &lightPosition.z ) ;
    
    gui->addToggle("BLOOM" , false ) ;
    gui->addToggle("DOF" , false ) ;
    
    gui->addToggle("FLOCK TO TEST" , false ) ;
    
    float maxWander = 500.0f ; 
    gui->addSlider("WANDER X", 0.0f, maxWander, &wanderFactor.x ) ;
    gui->addSlider("WANDER Y", 0.0f, maxWander , &wanderFactor.y ) ;
    gui->addSlider("WANDER Z", 0.0f, maxWander , &wanderFactor.z ) ;
    gui->addSlider( "TIME FACTOR" , 0.0f , 1.0f , &timeFactor ) ;
    /*
     post.createPass<BloomPass>()->setEnabled(false);
     post.createPass<DofPass>()->setEnabled(false);*/
    ofAddListener( gui->newGUIEvent , this , &testApp::guiEvent ) ;
    gui->loadSettings( "GUI/settings.xml" ) ;
    
    /*
     unsigned idx = key - '0';
     if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());

     */
}

void testApp::guiEvent( ofxUIEventArgs &args )
{
    string wName = args.widget->getName() ;
    
    if ( wName == "SPAWN DELAY MILLIS" )
    {
        spawnDelay = ((ofxUISlider *)args.widget )->getScaledValue() ;
        spawnTimer.setup( spawnDelay ) ; 
    }
    
    if ( wName == "BLOOM" )
    {
        bool bValue = ((ofxUIToggle *)args.widget)->getValue() ;
        post.getPasses()[0]->setEnabled( bValue ) ;
    }
    
    if ( wName == "DOF" )
    {
        bool bValue = ((ofxUIToggle *)args.widget)->getValue() ;
        post.getPasses()[1]->setEnabled( bValue ) ;
    }
  
    if ( wName == "FLOCK TO TEST" )
    {
        bool bValue = ((ofxUIToggle *)args.widget)->getValue() ;
        bFlockToggle = bValue ;
        if ( bFlockToggle )
        {
            //particleText.createAttractVerts( (int)maxParticles ) ;
            //createFlockFbo ( &particleText.textFbo ) ;
        }
    }
     
    gui->saveSettings("GUI/settings.xml" ) ;
}

void testApp::createFlockFbo ( ofFbo * fbo )
{
    ofRectangle fontBounds = particleText.font.getStringBoundingBox( particleText.text , 0 , 0 ) ;
    fbo->allocate( fontBounds.getWidth() , fontBounds.getHeight() , GL_RGB ) ;
    fbo->begin() ;
    ofSetColor( 0 ) ;
    ofRect( 0 , 0, fbo->getWidth() , fbo->getHeight() ) ;
    ofSetColor( 255 ) ;
    particleText.font.drawString( particleText.text , 0 , 0 ) ; 
    fbo->end() ;
}

void testApp::spawnTimerCompleteHandler( int &args )
{
    for ( int i = 0 ; i < numPerSpawnWave ; i++ )
        createParticle() ;
}

void testApp::createParticle( )
{
    ofPtr<ofxParticle3D>  p = ofPtr<ofxParticle3D> (new ofxParticle3D() ) ;
    p->x = ofRandomWidth() ;
    p->y = spawnY + ofRandom( -spawnYVariance , spawnYVariance ) ;
    p->z = ofRandom( -spawnZVariance , spawnZVariance ) ; 
    p->color = ofColor::cyan ;
    //fromHsb( ofRandom( 0 , 250 ) , ofRandom( 15 ) , 255 )  ;
    p->color.a =  ofRandom( 60 , 215 ) ;
    p->rotation = ofRandom( 0 , 360 ) ;
    particles.push_back( p ) ;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch ( key )
    {
        case 'g':
        case 'G':
            gui->toggleVisible() ;
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