//
//  ParticleText.cpp
//  Environment
//
//  Created by Ben McChesney on 6/25/13.
//
//

#include "ParticleText.h"

/*
 
 string text ;
 ofPoint pos ;   //position
 vector<ofPoint> attractVerts ;
 */
void ParticleText::setup ( string _text , string _fontPath )
{
    text = _text ;
    //ofTrueTypeFont::loadFont(string _filename, int _fontSize, bool _bAntiAliased, bool _bFullCharacterSet, bool _makeContours, float _simplifyAmt, int 
    font.loadFont( _fontPath , 28 , true , false , true  ) ;
    
    textFbo.allocate( 0 , 0 ) ; 

}

void ParticleText::update ( )
{
    
}

void ParticleText::draw ( )
{
   
    ofRectangle fontBounds = font.getStringBoundingBox( text , 0, 0 ) ;
    //ofSetColor( 65 ) ;
    //ofRect( fontBounds ) ;
    
    ofPushMatrix() ;
        ofTranslate( -fontBounds.getWidth() / 2 , -fontBounds.getHeight() / 2 ) ;
        ofEnableAlphaBlending() ; 
        ofSetColor( 255 , 80 ) ;
        font.drawString( text , pos.x , pos.y ) ;
    
     ofSetColor( 255 ) ;
      // textFbo.draw( 0 , 0 ) ;

        glDisable(GL_DEPTH_TEST ) ;
        if ( attractVerts.size() > 0 )
        {
            //ofTranslate( 0 , 0 , 10 ) ;
            ofSetColor( 255 , 125 ) ;
            for ( int v = 0 ; v < attractVerts.size() ; v++ )
            {
                ofCircle( attractVerts[v] , 2 ) ;
            }
        }
    ofPopMatrix() ;
}

void ParticleText::drawDebug( )
{
    ofRectangle fontBounds = font.getStringBoundingBox( text , 0, 0 ) ;
    textFbo.allocate( fontBounds.getWidth() , fontBounds.getHeight() ) ;
    textFbo.begin( ) ;
    //
    ofClear( 0 , 0 , 0 , 1 ) ;
    ofSetColor( 255 ) ; // 255 ) ;
    int size = 1000 ;
    
    //ofTranslate( -fontBounds.getWidth() / 2 , -fontBounds.getHeight() / 2 ) ;
    ofRect( fontBounds ) ;
    ofRect( -size/2 , -size/2 , size , size ) ;
    ofEnableAlphaBlending() ;
    ofSetColor( 255 ) ;
    font.drawString( text , 0 , 0 ) ;
    
    textFbo.end() ;
 
}

void ParticleText::createAttractVerts( int numVerts )
{
    
    /*
    drawDebug() ; 
    ofPixels pix ;
    pix.allocate( textFbo.getWidth() , textFbo.getHeight() , 3 ) ;
    textFbo.readToPixels( pix ) ;
    
    attractVerts.clear() ;
    
    int w = pix.getWidth() ;
    cout << " w : " << pix.getWidth() << " , y : " << pix.getHeight() << endl ; 
    for ( int y= 0 ; y < pix.getHeight() ; y++ )
    {
        for ( int x = 0 ; x < w ; x++ )
        {
            ofColor color = pix.getColor( x , y ) ;
            if ( color.getBrightness() > 20 )
            {
                attractVerts.push_back( ofVec3f( x , y , 0 ) ) ; //ofRandom( -2 , 2 ) ) ;
            }
        }
    }*/
    
    
    int numLetters = 0 ;
    
    vector<ofPoint> allPoints ;
    string space = " " ;
    
    ofVec3f offset ;
    for ( int c = 0 ; c < text.size() ; c++ )
    {
        string _thisChar = ofToString( text[c] ) ;
        cout << "_thisChar: '" << _thisChar << "' vs [c]: '" <<text[c] << "'" << endl ;
        
        
        if ( _thisChar.compare( space ) != 0 )
        {
            numLetters++ ;
            cout << "total # letters : " << numLetters << endl ;
           
            vector<ofTTFCharacter> chars = font.getStringAsPoints( _thisChar ) ;
            //cout << "chars.size() " << chars.size() ;
            
            int numOutlines = chars[0].getOutline().size() ;
            for ( int o = 0 ; o < numOutlines ; o++ )
            {
                vector<ofPoint> pts = chars[0].getOutline()[o].getVertices() ;
                
                for ( int p = 0 ; p < pts.size() ; p++ )
                {
                    //cout << "adding pts " << p << endl ;
                    allPoints.push_back( pts[p] + offset ) ;
                }
            }
        }
        else
        {
//            string xString = "x";
//            ofRectangle fontRect = font.getStringBoundingBox( xString , 0 , 0 ) ;
            offset.x +=  26 ; //fontRect.getWidth() ;
        }
        
        ofRectangle fontRect = font.getStringBoundingBox(_thisChar , 0 , 0 ) ;
        offset.x += fontRect.getWidth() + 0.5 ;
    }
    
    cout << "numVerts : " << numVerts << " , vs allPoints.size() : " << allPoints.size() << endl ;
    int skipNum = numVerts / allPoints.size() ; 
    cout << "skipNum : " << skipNum << endl ; 
    attractVerts.clear() ;
    
    for ( int i = 0 ; i < numVerts ; i++ )
    {
        //cout << "adding point[ " << i * skipNum << " ] ( " << allPoints[ i * skipNum ] << " ) " << endl ;
        attractVerts.push_back( allPoints[i ]  ) ;
    }

    cout << "attractVerts.size() : " << attractVerts.size() << endl; 
    
    
}

