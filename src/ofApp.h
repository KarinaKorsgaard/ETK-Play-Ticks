#pragma once

#define N_SOUNDS 5
#define VERSION "1.0"

#include "Team.h"
#include "common.h"
#include "ofMain.h"
#include "ofxOsc.h"

#include "ButtonData.h"
#include "ofxAutoReloadedShader.h"
#include "ofxGui.h"
#include "ofxSyphonServer.h"
#include "ofxXmlSettings.h"

class Ball {
  public:
    ofVec2f pos = ofVec2f(1090, 1080 / 2);
    ofVec2f vel;
    float radius = 40;
    commonObjects *co;
    bool goal = false;
    double goalTimer = 0.0;
    
    void update(ofRectangle rect1, ofVec2f v1, ofRectangle rect2, ofVec2f v2) {
        if(!goal) {
            pos += vel* (ofGetLastFrameTime() * 65);
            cout<< ofGetLastFrameTime() * 65<< endl;
            if (pos.y + radius > 1080) {
                vel.y *= -1*co->ballDampening;
                // vel.x = 0.;
            }
            if (pos.y - radius < 0) {
                vel.y *= -1*co->ballDampening;
                // vel.x = 0.;
            }
            
            intersect(rect1, v1);
            intersect(rect2, v2);
        }

        if (pos.x + radius > 1920 * 2) {
            pos = ofVec2f(1920, 1080 / 2);
            goal = true;
            co->tennisPoint[0]++;
        }
        if (pos.x - radius < 0) {
            pos = ofVec2f(1920, 1080 / 2);
            goal = true;
            co->tennisPoint[1]++;
        }

        if(goal) {
            goalTimer+=ofGetLastFrameTime();
            //cout << goalTimer << endl;
            if(goalTimer > 2.) {
                vel *= -1;
                vel.y*=0.5;
                goal = false;
                goalTimer = 0.0;
            }
        }
        
        if(vel.length()>co->ballSpeed)vel*=0.99;
    }
    void draw(ofImage img) {
        img.draw(pos - ofVec2f(radius, radius), radius * 2, radius * 2);
    }
    void intersect(ofRectangle rect, ofVec2f velicity_pad) {
        // Find the closest point to the circle within the rectangle

        float closestX = CLAMP(pos.x, rect.x, rect.x + rect.width);
        float closestY = CLAMP(pos.y, rect.y, rect.y + rect.height);

        // Calculate the distance between the circle's center and this closest
        // point
        float distanceX = pos.x - closestX;
        float distanceY = pos.y - closestY;

        // If the distance is less than the circle's radius, an intersection
        // occurs
        float distanceSquared =
            (distanceX * distanceX) + (distanceY * distanceY);
        bool intersecting = distanceSquared < (radius * radius);

        if (intersecting) {
            float old = vel.x;
            ofxOscMessage m;
            int winner = std::copysign(1, vel.x); // -1,1;
            winner = MIN(winner + 2, 2);          // 1,3 - max 2
            m.setAddress("/racket" + ofToString(winner));
            m.addFloatArg(1.f);
            co->oscOut.sendMessage(m);

            vel.x *= -1;

            float distanceToCenter = vel.y - rect.getCenter().y;
            vel.y += distanceToCenter * 0.001 * velicity_pad.length();

            vel.x += std::copysign(distanceToCenter * 0.0001, vel.x) *
                     velicity_pad.length();
        }
    }

  private:
};

class ofApp : public ofBaseApp {

  public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);

    void restart();
    void loadFromRestart();
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
    int removeTicksCounter = 0;
    ofFbo fboTeams[2];

    ofxSyphonServer syphon;
    commonObjects co;
    commonFunctions cc;

    ofxAutoReloadedShader trailShader;
    ofxAutoReloadedShader looserShader;

    ofxBox2d box2d;
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);

    ofSoundPlayer sound[N_SOUNDS];

    ofxPanel gui;
    ofxPanel guiScenes;

    ofParameterGroup physics, market, gameMechs, gravity, charades, logic,
        design;
    ofParameter<bool> reverseX, reverseY, alertDialog;
    ofParameterGroup scenes;

    vector<ofParameter<bool>> b_scenes;
    vector<bool> p_b_scenes;

    vector<bool> receivingTables;
    vector<string> alive;
    vector<bool> tableHasBeenAlive;

    int teamSize = -1;
    int alertCounter[12];

    ofxTrueTypeFontUC font_small;
    ofxTrueTypeFontUC font_medium;

    ofRectangle timePlacement[2];

    bool toggleRefill = false;
    float easeRefill1 = 0.;
    float easeRefill2 = 0.;

    void refill(int team, float timef);

    ofVideoPlayer groundVideo;
    bool isGroundDone;
    void updateGroundGame();
    ofPixels groundPixels;
    string unemployedMessage, unemployedMessage1, unemployedMessage2;

    float ease(float t, float b, float c, float d) {
        c -= b;
        t /= d;
        return c * t * t + b;
    }

    ofImage fightBallImg;
    bool firstServe;

    ofParameter<bool> restartApp;
    ofParameter<bool> resetApp;
    void reset();

    ofImage scoreImgWon;
    ofImage scoreImgLost;
    ofImage scoreImgTie;
    vector<vector<string>> scoreMessages;

    // shared_ptr<ofxBox2dCircle>fightBall;
    Ball fightBall;
};
