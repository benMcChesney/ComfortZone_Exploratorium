#pragma once

#include "ofMain.h"
#include "ofxParticle3D.h"
#include "ofxUI.h"
#include "ofxSimpleTimer.h" 
#include "ofxPostProcessing.h"
#include "ParticleText.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
        void createParticle( ) ; 
        vector< ofPtr<ofxParticle3D> > particles ;
		
        ofVec2f gravity ;
        ofVec2f gravityVariance ;
    
        float spawnY ;
        float spawnYVariance ;
        float spawnZVariance ;
        float numPerSpawnWave ;
            
        float dimX ;
        
        ofxUICanvas * gui ;
        float maxParticles ;
        float damping ; 
    
        void setupUI( ) ; 
        void guiEvent( ofxUIEventArgs &args ) ;
    
        float spawnDelay ; 
        ofxSimpleTimer spawnTimer ;
        void spawnTimerCompleteHandler( int &args ) ;
    
        ofImage sprite ; 
        float rotationLifeMultiplier ;
        float rotation ;
    
        ofEasyCam cam ;
    
        ofxPostProcessing post ;
        ofLight light ;
        ofVec3f lightPosition ;
    
        float trailsFadeAmount ;
        ofFbo trailsFbo ;
    
        ParticleText particleText ;
    
        bool bFlockToggle ;
        void createFlockFbo ( ofFbo * fbo ) ;
        ofPoint wanderFactor ;
float timeFactor ; 
    
};
