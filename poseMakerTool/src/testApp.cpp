#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    poseMovie.setup( "single_pose_test.mov" ) ;
    poseMovie.initEmptyPose( ) ; 
}

//--------------------------------------------------------------
void testApp::update(){
    poseMovie.update() ;
}

//--------------------------------------------------------------
void testApp::draw(){
    poseMovie.draw( ) ;
    poseMovie.debugDraw() ; 
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch ( key )
    {
        case ' ':
            poseMovie.start() ; 
            break ;
            
        case '<':
        case ',':
            poseMovie.decrementFrame() ;
            break ;
            
        case '>':
        case '.':
            poseMovie.incrementFrame() ;
            break ;
            
        case 's':
        case 'S':
            cout << "saving out to XML : " << endl ;
            poseMovie.savePoseToXml() ;
            break ; 
            
    }
    
    poseMovie.keyPressed( key ) ; 
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
    
    //cout << " mousePressed:: button ( ) " << button << endl ;
    if ( button == 0 )
        poseMovie.addPoint( x , y ) ;
    if ( button == 2 )
        poseMovie.inputTimeline( x , y ) ; 
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