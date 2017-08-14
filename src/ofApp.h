#pragma once

#define N_SOUNDS 5
#define VERSION "1.0"

#include "ofMain.h"
#include "ofxOsc.h"
#include "common.h"
#include "Team.h"

#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxSyphonServer.h"
#include "ButtonData.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

    
    void exit();
    void updateOsc();
    void handleSceneChange();
    string getAdress(int firstOrSecond, int _table, int _button);
    int p_sceneNumber;
    
    ofxOscReceiver receiver;
    int PORT;
    int NUM_TABLES;
    int BUTTONS_PR_TABLE;
    
    Team teams[2];
    ofFbo fbo;
    ofxSyphonServer syphon;
    commonObjects co;
    commonFunctions cc;
    
    ofxBox2d box2d;
//    void contactStart(ofxBox2dContactArgs &e);
//    void contactEnd(ofxBox2dContactArgs &e);
    
    ofSoundPlayer  sound[N_SOUNDS];
    
    ofxPanel gui;
    ofxPanel guiScenes;

    ofParameterGroup physics,market,gameMechs,gravity,charades,logic;
    ofParameter<bool>reverseX,reverseY,alertDialog;
    ofParameterGroup scenes;
    
    vector< ofParameter<bool> >b_scenes;
    vector<bool>p_b_scenes;
    
    vector<bool>receivingTables;
    vector<string>alive;

    int teamSize=-1;
    int alertCounter;
    
    ofxTrueTypeFontUC font_x_small;
    ofxTrueTypeFontUC font_small;
    ofxTrueTypeFontUC font_medium;
    ofxTrueTypeFontUC font_large;
    
    
    bool toggleRefill = false;
    float easeRefill1 = 0.;
    float easeRefill2 = 0.;
    
    void refill(int team, float timef);

    ofVideoPlayer groundVideo;
    bool isGroundDone;
    void updateGroundGame();
    ofPixels groundPixels;

    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    
    shared_ptr<ofxBox2dCircle>fightBall;
   // vector<Button>giantButtonMap;

};
