#pragma once

#define N_SOUNDS 5

#include "ofMain.h"
#include "ofxOsc.h"
#include "common.h"
#include "Team.h"

#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxSyphonServer.h"
#include "ButtonData.h"
#include "pingPong.h"
#include "push_game.h"
#include "pushing.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    //void exit();
    void exit();
    int p_sceneNumber;
    ofxOscReceiver receiver;
    
    string getAdress(int firstOrSecond, int _table, int _button);
    
    Team teams[2];
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);

    
    ofFbo fbo;
    commonObjects co;
    commonFunctions cc;
    
    ofSoundPlayer  sound[N_SOUNDS];
    
    ofxPanel gui;
    ofxPanel guiScenes;
    vector<bool>receivingTables;
    ofParameterGroup physics;
    ofParameterGroup market;
    ofParameterGroup spyGame;
    
    ofParameterGroup gameMechs;
    ofParameterGroup gravity;
    ofParameter<bool>reverseX,reverseY;
    ofParameterGroup scenes;
    vector< ofParameter<bool> >b_scenes;
    vector<bool>p_b_scenes;
//    ofParameter<float>time_energy;
//    ofParameter<float>finalePushDrain;
    ofParameterGroup push;
    ofParameterGroup idle;

    vector<string>alive;
    void handleSceneChange();
    void drawScores();
    
    int PORT;
    int NUM_TABLES;
    int BUTTONS_PR_TABLE;
    
    int teamSize=-1;
    
    ofxTrueTypeFontUC font_x_small;
    ofxTrueTypeFontUC font_small;
    ofxTrueTypeFontUC font_medium;
    ofxTrueTypeFontUC font_large;
    
    ofxSyphonServer syphon;
    bool toggleRefill = false;
    float easeRefill1 = 0.;
    float easeRefill2 = 0.;
    
    void refill(int team, float timef);
    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    ofxBox2d world;
    PingPong pingPong;
    PushGame pushGame;
    Pushing push2;
   // vector<Button>giantButtonMap;

};
