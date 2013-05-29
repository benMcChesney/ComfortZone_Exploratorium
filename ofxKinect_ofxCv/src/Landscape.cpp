//
//  Landscape.cpp
//  ofxKinect_ofxCv
//
//  Created by Ben McChesney on 5/26/13.
//
//

#include "Landscape.h"

void Landscape::setup( )
{
    setupUI( ); 
}

void Landscape::update( )
{
    startColor = ofColor::fromHsb( (ofGetFrameNum() + colorRange  )%255 , 255 , 255 ) ;
    endColor = ofColor::fromHsb( (ofGetFrameNum() )%255 , 255 , 255 ) ;
}

void Landscape::draw( )
{
    ofPushStyle() ;
    ofPushMatrix() ;
    
   
        ofTranslate( 0 , ofGetHeight() / 2 ) ;
        ofTranslate( offset.x , offset.y , offset.z ) ;
    
        ofVec2f prevPoint = ofVec2f( 0 , 0 ) ;
    
        float segmentsSpacing = ( ofGetWidth() / ( float ) segments ) ;
    
        float landscapeFactor = 1.0f / (float) (segments/2.0f) ;
        float midPoint = segments / 2 ;
    
        ofVec2f p1 = ofVec2f ( 0  , 0 ) ;
        ofVec2f p2 = ofVec2f ( ofGetWidth() , 0 ) ;
        ofVec2f p3 = ofVec2f ( ofGetWidth() , ofGetHeight( )) ;
        ofVec2f p4 = ofVec2f ( 0 , ofGetHeight() ) ;
    
        ofEnableAlphaBlending() ;
        for ( int c = 0 ; c < copies ; c++ )
        {
            float lastZOffset = 0.0f  ;
            float lastHeightFactor = 0.0f ;
            float lastNormalZ = 0.0f ; 
            for ( int r = 0 ; r < segments ; r++ )
            {
                ofColor _lerp = startColor ;
                _lerp.lerp( endColor,  (float)c / copies ) ;
                
                
                float diff = midPoint - r ;
                diff = midPoint - abs(diff) ;
                if ( diff < 0 )
                    diff *= -1 ;
                float heightFactor = diff  * landscapeFactor ; // * landscapeFactor ;
                ofPushMatrix() ;
                    
                float zOffset = zSpacing * c ;
                float normalZ = (float) c / (float) copies ;
                
                _lerp.a = ( 1.0f - normalZ ) * 255.0f ;
                ofSetColor( _lerp ) ;
                ofTranslate( 0 , 0, zOffset ) ;
                    
                    ofVec2f point = ofVec2f ( (float)r * segmentsSpacing , ( ofNoise( ofGetElapsedTimef() * landscapeTimeFactor + r ) + 0.1 )  ) ;
                     // ofNoise( ofGetElapsedTimef() + r ) * ( landscapeHeight ) ;
                    if ( r != 0 )
                    {
                        
                        
                        float _diff = midPoint - (r-1) ;
                        _diff = midPoint - abs(_diff) ;
                        if ( _diff < 0 )
                            _diff *= -1 ;
                        float _heightFactor = _diff  * landscapeFactor ; // * landscapeFactor ;
                        
                        ofVec2f pointAhead = point ;
                        point.y *= ( heightFactor ) * landscapeHeight * (normalZ)  ;
                        pointAhead.y *= ( _heightFactor )  * landscapeHeight * (lastNormalZ)  ;
                        ofPushMatrix() ;
                        ofLine( prevPoint , point ) ;
                        ofLine( point.x , point.y , 0 , pointAhead.x , pointAhead.y , -zSpacing ) ;
                        
                        ofPopMatrix() ;
                    }
                    
                lastNormalZ = normalZ ; 
                    
                ofPopMatrix() ; 
                
                
                
                
                prevPoint = point ;
                lastHeightFactor = heightFactor ;
                lastZOffset = zOffset ; 
            }
        }
        
    
    ofPopMatrix() ;
    ofPopStyle() ; 
}

void Landscape::setupUI( )
{
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 200 ;
    float dim = 25 ;
    
    gui = new ofxUIScrollableCanvas(0, 0, length+xInit, ofGetHeight());
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer() ;
    
    gui->addSlider("OFFSET X" , -2000.0 , 2000.0 , offset.x ) ;
    gui->addSlider("OFFSET Y" , -2000.0 , 2000.0 , offset.y ) ;
    gui->addSlider("OFFSET Z" , -2000.0 , 2000.0 , offset.z ) ;
    
    gui->addSlider("SEGMENTS" , 2 , 165 , segments ) ;
    gui->addSlider("COPIES" , 2 , 100 , copies ) ;
    gui->addSlider("Z SPACING" , -800, 0 , zSpacing ) ;
    gui->addSlider("COLOR OFFSET" , 0 , 255 , colorRange ) ; 
    gui->addSlider("HEIGHT NOISE OFFSET" , 0 , 500.00f , heightNoiseOffset ) ;
    gui->addSlider("LANDSCAPE HEIGHT" , 0 , 500.0f , landscapeHeight ) ;
    gui->addSlider("LANDSCAPE TIME FACTOR" , 0.0f , 1.5f , landscapeTimeFactor ) ;
    ofAddListener(gui->newGUIEvent,this,&Landscape::guiEvent);
    gui->loadSettings( "GUI/landscape_settings.xml" ) ;
    gui->disable() ; 
}

void Landscape::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    ofxUISlider * slider = ((ofxUISlider * )e.widget) ;
    if ( name == "OFFSET X" )   offset.x = slider->getScaledValue() ;
    if ( name == "OFFSET Y" )   offset.y = slider->getScaledValue() ;
    if ( name == "OFFSET Z" )   offset.z = slider->getScaledValue() ;
    if ( name == "SEGMENTS" )   segments = slider->getScaledValue() ;
    if ( name == "COPIES" )     copies = slider->getScaledValue() ;
    if ( name == "Z SPACING" )  zSpacing = slider->getScaledValue() ;
    if ( name == "LANDSCAPE HEIGHT"  )  landscapeHeight = slider->getScaledValue() ;
    if ( name == "HEIGHT NOISE OFFSET" )  heightNoiseOffset = slider->getScaledValue() ;
    if ( name == "LANDSCAPE TIME FACTOR" )  landscapeTimeFactor = slider->getScaledValue() ;
    if ( name == "COLOR OFFSET" ) colorRange = slider->getScaledValue() ; 
    /*
     gui->addSlider("COLOR OFFSET" , 0 , 255 , colorRange ) ;
     gui->addSlider("LANDSCAPE TIME FACTPR" , 0.0f , 1.5f , landscapeTimeFactor ) ;
     gui->addSlider("HEIGHT NOISE OFFSET" , 0 , 50.0f , heightNoiseOffset ) ;
     gui->addSlider("LANDSCAPE HEIGHT" , 0 , 50.0f , heightNoiseOffset ) ;
     */
    gui->saveSettings("GUI/landscape_settings.xml") ; 


}
