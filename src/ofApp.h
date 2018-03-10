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
#include "ofxAutoReloadedShader.h"

class Ball{
public:
    ofVec2f pos = ofVec2f(1090,1080/2);
    ofVec2f vel;
    float radius = 40;
    commonObjects * co;
    
    void update(ofRectangle rect1, ofVec2f v1, ofRectangle rect2, ofVec2f v2){
        pos += vel;
        
        if(pos.x + radius>1920*2){
            pos = ofVec2f(1920,1080/2);
            vel*=-1;
            co->tennisPoint[0]++;
        }
        if(pos.x - radius<0){
            pos = ofVec2f(1920,1080/2);
            vel*=-1;
            co->tennisPoint[1]++;
        }
        
        if(pos.y + radius>1080)vel.y*=-1;
        if(pos.y - radius<0)vel.y*=-1;
        
        intersect(rect1, v1);
        intersect(rect2, v2);

    }
    void draw(ofImage img){
        img.draw(pos, radius*2, radius*2);
        
    }
    void intersect(ofRectangle rect, ofVec2f velicity_pad){
        // Find the closest point to the circle within the rectangle
        
                float closestX = CLAMP(pos.x, rect.x, rect.x+rect.width);
                float closestY = CLAMP(pos.y, rect.y, rect.y + rect.height);
        
                // Calculate the distance between the circle's center and this closest point
                float distanceX = pos.x - closestX;
                float distanceY = pos.y - closestY;
        
                // If the distance is less than the circle's radius, an intersection occurs
                float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
                bool intersecting = distanceSquared < (radius * radius);
        
        if(intersecting){
           
            vel.x*=-1;
            
            float distanceToCenter = vel.y - rect.getCenter().y;
            vel.y += distanceToCenter*0.001*velicity_pad.length();
            vel.x += std::copysign(distanceToCenter*0.0001,vel.x)*velicity_pad.length();
        }

        
//        float closestX = CLAMP(_pos.x, rect.x, rect.x+rect.width);
//        float closestY = CLAMP(_pos.y, rect.y, rect.y + rect.height);
//
//        // Calculate the distance between the circle's center and this closest point
//        float distanceX = _pos.x - closestX;
//        float distanceY = _pos.y - closestY;
//
//        // If the distance is less than the circle's radius, an intersection occurs
//        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
//        bool intersecting = distanceSquared < (rad * rad);
//
//        if(intersecting){
//
//            ofVec2f dist = ofVec2f(pos.x - closestX, pos.y - closestY);
//
//            if (vel.dot(dist) < 0) { //if circle is moving toward the rect
//                float tangent_vel = dist.normalize().dot(vel);
//                vel = vel - (tangent_vel*2);
//                //cout << "something?" << endl;
//            }
//
//            float penetrationDepth = radius - dist.length();
//            ofVec2f penetrationVector = dist.getNormalized()*(penetrationDepth);
//            pos = pos-(penetrationVector);
//        }
    }
private:
    
};

class ofApp : public ofBaseApp{

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
    int removeTicksCounter=0;
    ofFbo fboTeams[2];
    
    ofxSyphonServer syphon;
    commonObjects co;
    commonFunctions cc;
    
    ofxAutoReloadedShader trailShader;
    
    ofxBox2d box2d;
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);

    ofSoundPlayer  sound[N_SOUNDS];
    
    ofxPanel gui;
    ofxPanel guiScenes;

    ofParameterGroup physics,market,gameMechs,gravity,charades,logic,design;
    ofParameter<bool>reverseX,reverseY,alertDialog;
    ofParameterGroup scenes;
    
    vector< ofParameter<bool> >b_scenes;
    vector<bool>p_b_scenes;
    
    vector<bool>receivingTables;
    vector<string>alive;
    vector<bool>tableHasBeenAlive;

    int teamSize=-1;
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
    string unemployedMessage,unemployedMessage1,unemployedMessage2;
    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    
    
    ofImage fightBallImg;
    bool firstServe;
    
    ofParameter<bool>restartApp;
    ofParameter<bool>resetApp;
    void reset();
    

    ofImage scoreImgWon;
    ofImage scoreImgLost;
    ofImage scoreImgTie;
    vector<vector<string>>scoreMessages;
    
   // shared_ptr<ofxBox2dCircle>fightBall;
    Ball fightBall;
};


