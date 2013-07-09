//
//  PoseGestureMovie.cpp
//  poseMakerTool
//
//  Created by Ben McChesney on 6/29/13.
//
//

#include "PoseGestureMovie.h"

void PoseGestureMovie::setup ( string _moviePath  )
{
    moviePath = _moviePath ;
    selectedDebugPose = 0 ;
    timelineBounds = ofRectangle( 0 , ofGetHeight() - 100 , ofGetWidth() , 100 ) ;
  
}

void PoseGestureMovie::initEmptyPose()
{
    //int fakePoses  = ofRandom ( 2 , 9 ) ;
    //for ( int i = 0 ; i < 4 ; i++ )
    //{
        poses.push_back( ( ofPtr<KeyPointPose> ) new KeyPointPose() ) ;
        poses[ ( poses.size() -1 ) ]->setup() ;
        poses[ ( poses.size() -1 ) ]->uniqueInputHex = PixelHitTestHub::Instance()->getColorfulUniqueHex() ;
        //poses[i]->createDebugPoints() ;
    //}
    
    vector<ofPoint> v ;
    activePosePts.push_back( v ) ;
    

}

void PoseGestureMovie::update( )
{
    movie.update() ;
    
    if ( movie.getIsMovieDone() )
    {
        offset = ofPoint ( ofRandom( ofGetWidth() - movie.getWidth() ) , ofRandom( ofGetHeight() - movie.getHeight() ) )  ;
    //    cout << "Movie done playing ! " << endl ;
        movie.setSpeed( ofRandom( 0.8f , 1.20f ) ) ; 
        movie.firstFrame() ;
        movie.play() ; 
    }
       
    for ( int i = 0 ; i < poses.size() ; i++ )
    {
        activePosePts[ i ].clear() ;
        poses[i]->poseTimer.update();
    }

    
    if ( !movie.isFrameNew() ) return ;
    
//    activePosePts.clear() ;
       
    float moviePosition = movie.getPosition() ;
    for ( int i = 0 ; i < poses.size() ; i++ )
    {
        if ( poses[i]->getIsActive( moviePosition ) == true )
        {
            
            for ( int p = 0 ; p < poses[i]->pts.size() ; p++ )
            {
                activePosePts[ i ].push_back( poses[i]->pts[p] ) ;
            }
        }
    }
}

void PoseGestureMovie::draw( )
{
    ofPushMatrix() ;
    ofTranslate( offset ) ;
    
    ofSetColor( 255 ) ;
    ofEnableAlphaBlending() ;
    
    ofPushMatrix() ;
        ofTranslate( 0 , 0 ) ;
    ofPushStyle() ;
        ofEnableBlendMode( OF_BLENDMODE_MULTIPLY ) ;
        movie.draw( 0 , 0 );
    ofPopStyle() ;
    ofEnableAlphaBlending() ;
      //  ofSetColor( ofColor::fromHsb( sin( ofGetElapsedTimef() ) * 128.0f + 128.0f , 255 , 255 ) ) ;
    
    for ( int p = 0 ; p < poses.size() ; p++ )
    {
        poses[ p ]->drawDebugMap() ;
    }
    /*
    for ( int p = 0 ; p < activePosePts.size() ; p++ )
        {
            for ( int i = 0 ; i < activePosePts[ i ].size() ; i++ )
            {
                ofCircle( activePosePts[ p ][ i ] , poses[p]->keyPointRadius ) ;
            }
        }*/
    ofPopMatrix() ;
    
    ofPopMatrix() ; 
    drawTimeline( ) ; 
}

void PoseGestureMovie::drawInputMap( )
{
    ofPushMatrix() ;
        ofTranslate( offset ) ;
        for ( int p = 0 ; p < poses.size() ; p++ )
        {
            poses[ p ]->drawInputMap() ;
        }

    ofPopMatrix()  ;
}


void PoseGestureMovie::keyPressed( int key )
{
    int adjKey = key - 49 ;
    if ( adjKey  >= 0 && adjKey < poses.size() )
    {
        selectedDebugPose = adjKey ;
        cout << " new debug pose is : " << selectedDebugPose << endl ; 
    }
   
    
}

void PoseGestureMovie::debugDraw( )
{
    
    
    stringstream ss ;
    
    for ( int pose = 0 ; pose < poses.size() ; pose++ )
    {
        ss << " @ pose " << pose << " ( in : " << poses[ pose ]->_in << " , out : " << poses[ pose ]->_out << " ) # pts :" << poses[ pose ]->pts.size() <<  " % : " << poses[pose]->poseTimer.getNormalizedProgress() << endl ;
    }
    
    ofDrawBitmapStringHighlight( ss.str() , 350 , 75 ) ;
}

void PoseGestureMovie::drawTimeline( ) 
{
    ofPushStyle() ;
    ofPushMatrix() ;
        ofEnableAlphaBlending() ; 
        ofSetColor( 255 , 128 ) ;
        ofRect(timelineBounds) ;
    
        ofPushMatrix() ; 
            ofTranslate( timelineBounds.x , timelineBounds.y ) ;
            
            int totalRows = 0 ;
            for ( int pose = 0 ; pose < poses.size() ; pose++ )
            {
                totalRows += poses[pose]->pts.size() ;
                //cout << " # poses[" << pose << "]->size() " << poses[pose]->pts.size() << endl ;
            }
            
            float rowHeight = timelineBounds.getHeight() / totalRows ;
           // cout << "totalRows : " << totalRows << " rowHeight : " << rowHeight << endl ;
        
            for ( int row = 0 ; row < totalRows ; row++ )
            {
                int whichPose = -1 ;
                int count = 0 ;
                for ( int p = 0 ; p < poses.size() ; p++ )
                {
                    for ( int pt = 0 ; pt < poses[ p ]->pts.size() ; pt++ )
                    {
                        if ( row == count )
                        {
                            whichPose = p ;
                            break ;
                        }
                        
                        count++ ; 
                    }
                }
                
                float startX = poses[ whichPose ]->_in * timelineBounds.getWidth() ;
                float endX = poses[ whichPose ]->_out * timelineBounds.getWidth() ; 
                ofPushMatrix() ;
                    float ratio = (float) whichPose / ( float ) poses.size() ;
                    float colorHue = ratio * 255.0 ;
                    ofEnableAlphaBlending() ;
                    ofColor col ;
                    col = ofColor::fromHsb( colorHue , 255.0f , 255.0f ) ;
                    ofSetColor( col ) ; //colorHue , 0 , 0 ) ; //ofColor::fromHsb( colorHue , 255,0 , 255.0f ) ) ;
                    ofRect( startX , row * rowHeight , endX , rowHeight ) ;
                ofPopMatrix() ;
            }
        ofPopMatrix() ; 
    
        
        int markerSize = 5 ;
        float width = timelineBounds.getWidth() - markerSize ;
        
        ofSetColor( 0 , 255 ) ;
        ofRect( width * movie.getPosition() , timelineBounds.y , markerSize , timelineBounds.getHeight() ) ;

    
    ofPopMatrix() ;
    ofPopStyle() ;
}

void PoseGestureMovie::checkPoseAgainstInputMap( PixelHitTestHub * pixelHub , int hitHex )
{
    if ( poses.size() > 0 && activePosePts.size() > 0 )
    {
        //cout << "MORE ACTIVE POINTS THAN 0 : " << activePosePts.size() << endl ;
        for ( int p = 0 ; p < poses.size() ; p++ )
        {
            //poses[ p]->resetPtsStatus() ;
            bool bAllHit = true ;
            bool bPoseActive =  poses[p]->getIsActive( movie.getPosition() ) ;
            for ( int i = 0 ; i < activePosePts[p].size() ; i++ )
            {
                //if ( poses[p]->getIsActive( poses[p]->pts[i] ) )
                //{
                    int iHex = pixelHub->getHexAt( poses[p]->pts[ i ] + offset ) ;
                    if ( iHex != hitHex )
                    {
                        bAllHit = false ;
                        poses[ p ] ->ptsStatus[ i ]  = false ;
                        //break ;
                    }
                    else
                    {
                        if ( bPoseActive )
                        poses[ p ] ->ptsStatus[ i ]  = true ;
                    }
                //}
            }
            
            if ( bAllHit == true && bPoseActive == true )
            {
                if ( poses[ p ]->poseTimer.bIsRunning == false )
                    poses[ p ]->poseTimer.start(false,false) ;
            }
            else
            {
                poses[ p ]->poseTimer.stop() ;
            }
        }
    }
}

void PoseGestureMovie::setPoseTime ( float poseTimeDelayInMillis ) 
{
    for ( int p = 0 ; p < poses.size() ; p++ )
    {
        poses[p]->poseTimer.setTimerDelayInMillis( poseTimeDelayInMillis ) ; 
    }
}

void PoseGestureMovie::inputTimeline( float x  , float y )
{
    if (  ( x > timelineBounds.x && x < ( timelineBounds.x + timelineBounds.getWidth() ) )
       && ( y > timelineBounds.y && y < ( timelineBounds.y + timelineBounds.getHeight() ) ) ) 
    {
        ofPoint adjusted = ofPoint ( x - timelineBounds.x , y = timelineBounds.y ) ;
        
        //cout << "within bounds ! : " << endl ;
        int totalRows = 0 ;
        for ( int pose = 0 ; pose < poses.size() ; pose++ )
        {
            totalRows += poses[pose]->pts.size() ;

        }
        
        float rowHeight = timelineBounds.getHeight() / totalRows ;
        

        int whichPose = -1 ;
        for ( int row = 0 ; row < totalRows ; row++ )
        {

            int count = 0 ;
            for ( int p = 0 ; p < poses.size() ; p++ )
            {
                for ( int pt = 0 ; pt < poses[ p ]->pts.size() ; pt++ )
                {
                    if ( adjusted.y > ( count * rowHeight ) )
                    {
                        whichPose = p ;
                        break ; 
                    }
                    
                    
                    count++ ;
                }
            }
        }
        
        
        float normalizedX =adjusted.x / (float ) timelineBounds.getWidth() ;
        
        if ( abs( normalizedX - poses[whichPose]->_in ) < abs( normalizedX - poses[whichPose]->_out ) )
        {
            //closer to w
            poses[ whichPose ]->_in = normalizedX ;
        }
        else
        {
            //closer to end
            poses[ whichPose ]->_out = normalizedX ;
        }

    }
    
}

void PoseGestureMovie::savePoseToXml()
{
    string xmlName = "pose_at_" + ofToString( ofGetSystemTimeMicros() ) + ".xml" ;
    ofxXmlSettings xml ;
    
    xml.setValue("moviePath", moviePath ) ;
    
    
    for ( int p = 0 ; p < poses.size() ; p++ )
    {
        
        int tagNum = xml.addTag("pose" ) ;
        xml.pushTag("pose" , tagNum ) ; 
            xml.setValue( "in" , poses[p]->_in ) ;
            xml.setValue( "out" , poses[p]->_out ) ;
            
            int numPts = poses[p]->pts.size() ;
            for ( int i = 0 ; i < numPts ; i++ )
            {
                int tagNum = xml.addTag("keyPoint") ;
                xml.pushTag("keyPoint" , tagNum );
                xml.setValue("x", poses[p]->pts[i].x ) ;
                xml.setValue("y", poses[p]->pts[i].y ) ;
                xml.popTag() ;
            }
        xml.popTag() ;
    }
    xml.saveFile( xmlName ) ; 
}

void PoseGestureMovie::loadXml( string xmlPath )
{
    //poseGestureMovie->loadXml( "single_pose_test.xml" ) ;
    ofxXmlSettings xml ;
    xml.loadFile( xmlPath ) ;
    
    string _moviePath = xml.getValue("moviePath", "noMovie" ) ;
    moviePath = _moviePath ;
    
    int numPoses = xml.getNumTags("pose") ; 
    
    for ( int p = 0 ; p < numPoses ; p++ )
    {
        initEmptyPose() ;
        
        int k = ( poses.size() -1 ) ; 

        xml.pushTag("pose" , p ) ; 
            float _in = xml.getValue( "in" , 0.0f ) ;
            float _out = xml.getValue( "out" , 1.0f ) ; 
            poses[ k ]->_in = _in ;
            poses[ k ]->_out = _out ;
        
            int numPts = xml.getNumTags( "keyPoint" ) ; //poses[p]->pts.size() ;
            for ( int i = 0 ; i < numPts ; i++ )
            {
                //int tagNum = xml.addTag("keyPoint") ;
                xml.pushTag("keyPoint" , i );
                    float _x = xml.getValue("x", 0.0f ) ;
                    float _y = xml.getValue("y", 0.0f ) ;
                    poses[ k ]->addPoint( _x , _y ) ; 
                xml.popTag() ;
            }
        
        xml.popTag() ; 
    }

}

void PoseGestureMovie::start( )
{
    bool bResult = movie.loadMovie( moviePath ) ;
    cout << " did the movie load ? " << bResult << endl ; 
    movie.play() ;
    movie.setLoopState( OF_LOOP_NONE ) ;
    for ( int p = 0 ; p < poses.size() ; p++ )
    {
        poses[ p ]->poseTimer.stop() ; 
    }
}

void PoseGestureMovie::decrementFrame ( )
{
    movie.setPaused( true ) ;
    int frame = movie.getCurrentFrame() ;
    frame-- ;
    if ( frame >= 0 )
    {
        movie.setFrame( frame ) ;
        movie.update() ;
    }
}

void PoseGestureMovie::incrementFrame ( )
{
    movie.setPaused( true ) ;
    int frame = movie.getCurrentFrame() ;
    frame++ ;
    if ( frame < movie.getTotalNumFrames() )
    {
        movie.setFrame( frame ) ;
        movie.update() ;
    }

}

void PoseGestureMovie::addPoint ( float x , float y )
{
    if ( x < movie.getWidth() && y < movie.getHeight() )
        poses[ selectedDebugPose ]->addPoint( x , y ) ;
    
}

void PoseGestureMovie::setEndOnPoint( float moviePosition , int x , int y )
{
    int nearIndex = getClosestPointTo( x , y ) ;
}

int PoseGestureMovie::getClosestPointTo( float x , float y )
{
    return -1 ; 

}

/*


ofVideoPlayer movie ;
vector<KeyPointPose> poses ;

 string moviePath ;
 ofVideoPlayer movie ;
 vector< ofPtr<KeyPointPose> > poses ;
 
 void start( ) ;
 
 vector<ofPoint> activePosePts ;
 
 void addPoint ( float x , float y ) ;
 
 
 void decrementFrame ( ) ;
 void incrementFrame ( ) ;
*/