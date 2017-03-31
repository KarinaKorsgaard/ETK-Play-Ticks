//
//  Team.h
//  _ETK_March_2017
//
//  Created by Karina Jensen on 06/03/17.
//
//

#ifndef Team_h
#define Team_h
#include "ofxBox2d.h"
#include "common.h"

#include "button.h"
#include "s01.h"
#include "s02.h"
#include "s02alt.h"
#include "s03.h"
#include "s03_alt.h"
#include "s04.h"
#include "s05.h"
#include "s06.h"
#include "designCharacter.h"
#include "drainTest.h"
#include "easyMaze.h"
#include "idle.h"


class Team{
    
public:
    
    ofxBox2d box2d;
    double time;
    float distance;
    
    bool isDone;
    bool logDone = false;
    bool playAnimation = false;
    vector<Button>buttons;
    int teamId;
    
    commonObjects *co;
    int p_sceneNum=0;
    ofVideoPlayer celebration;
    
    Scene01 area00; // area
    Scene02 getUp01; // getup
    Scene02alt getUp_alt; // getup
    Scene03 maze02; // maze . not done
    Scene03alt maze02_alt;
    Scene04 spy04; // spy
    Scene05 gravity03; // gravity
    Scene06 market05; // market
    Idle idle;
    Idle charades07;
    EasyMaze easyMaze;
    DesignACharacter sDesign;
    DrainTest drainTest;
    
    void setup(int team, commonObjects *c){
        teamId = team;
        box2d.init();
        box2d.setGravity(0, 0);
        box2d.createBounds(ofRectangle(0,0,1920,1080));
        box2d.enableEvents();
        co = c;
        celebration.load("celebration.mov");
    }
    void setupScenes(){
        sDesign.setup(co,&buttons,"colors.png",6);
        drainTest.setup(co,&buttons);
        area00.setup(co,&buttons);
        getUp01.setup(co,&buttons);
        getUp_alt.setup(co,&buttons);
        maze02.setup(co,&buttons);
        maze02_alt.setup(co,&buttons);
        spy04.setup(co,&buttons);
        gravity03.setup(co,&buttons);
        market05.setup(co,&buttons);
        charades07.setup(co,&buttons);
        idle.setup(co,&buttons);
        easyMaze.setup(co,&buttons);
    }
    
    void update(){
        
        int s = co->sceneNumber;
        
        if(p_sceneNum != s){
            logDone = false;
            playAnimation = false;
            destroyMaze();
            
            //for(int i = 0 ; i<buttons.size();i++)buttons[i].freezeUpdate=false;
            
            if(s == co->sMap["Area"])area00.begin();
            else if(p_sceneNum == co->sMap["Area"])area00.reset();
            
            if(s == co->sMap["GetThrough"]){ // get up
                getUp01.begin();
                createScene(getUp01.polys);
            }else if(p_sceneNum == co->sMap["GetThrough"])getUp01.reset();
            
            
            if(s == co->sMap["GetThrough_alt"]){ // get up
                createScene(getUp_alt.polys);
                getUp_alt.begin();
            }else if(p_sceneNum == co->sMap["GetThrough_alt"])getUp_alt.reset();
            
            
            if(s == co->sMap["EasyMaze"])easyMaze.begin();
            else if(p_sceneNum == co->sMap["EasyMaze"])easyMaze.reset();
            
            if(s == co->sMap["Maze"]){ // maze
                createScene(maze02.polys);
                maze02.begin(&box2d);
            }else if(p_sceneNum == co->sMap["Maze"])maze02.reset();
            
            if(s == co->sMap["Maze_alt"]){ // maze
                createScene(maze02_alt.polys);
                maze02_alt.begin(&box2d);
            }else if(p_sceneNum == co->sMap["Maze_alt"])maze02_alt.reset();
            
            
            if(s == co->sMap["Gravity"]){
                createScene(gravity03.polys);
                gravity03.begin();
                box2d.setGravity(0,co->gravity);
            }
            if(p_sceneNum == co->sMap["Gravity"]){
                if(gravity03.theWinner!=-1)
                    buttons[gravity03.theWinner].addValue(co->gravityReward);
                gravity03.reset();
                box2d.setGravity(0,0);
            }
            
            if(s == co->sMap["SpyGame"])spy04.begin();
            else if(s == co->sMap["SpyGame"])spy04.reset();
            
            if(s == co->sMap["Market"]){ // !
                market05.begin();
                createScene(market05.polys);
                cout << "market has "+ofToString(market05.polys.size())+ "solids"<< endl;
            }else if(p_sceneNum == co->sMap["Market"])market05.reset();
            
            if(s == co->sMap["Charades"])charades07.begin();
            else if(p_sceneNum == co->sMap["Charades"])charades07.reset();
            
            if(s == co->sMap["DrainTest"])drainTest.begin();
            else if(p_sceneNum == co->sMap["DrainTest"])drainTest.reset();
            
            p_sceneNum=s;
        }
        if(s==co->sMap["Area"])area00.update();
        if(s==co->sMap["GetThrough"])getUp01.update();
        if(s==co->sMap["GetThrough_alt"])getUp_alt.update();
        
        if(s==co->sMap["EasyMaze"])easyMaze.update();
        
        if(s==co->sMap["Maze"])maze02.update();
        if(s==co->sMap["Maze_alt"])maze02_alt.update();
        if(s==co->sMap["Gravity"])gravity03.update();
        if(s==co->sMap["SpyGame"] && spy04.spyId !=-1)spy04.update();
        if(s==co->sMap["Market"])market05.update();
        
        // push game!
        //if(teamId == 1 && s==6)market05.update();
        if(s==co->sMap["Charades"])charades07.update(false);
        if(s==co->sMap["DrainTest"])drainTest.update();
        // ping pong
        if(s==co->sMap["Design"])sDesign.update();
        if(s==co->sMap["Idle"])idle.update(true);
        
        //if(teamId == 0 && s==7)finale.update();
        //if(s==8)pingPong.update();
        
        
        
        box2d.update();
        
        
        //is done:
        
        if(s==co->sMap["Area"])isDone= area00.isDone();
        if(s==co->sMap["GetThrough"])isDone= getUp01.isDone();
        if(s==co->sMap["GetThrough_alt"])isDone= getUp_alt.isDone();
        if(s==co->sMap["EasyMaze"])isDone= easyMaze.isDone();
        if(s==co->sMap["Maze"])isDone= maze02.isDone();
        if(s==co->sMap["Maze_alt"])isDone= maze02_alt.isDone();
        if(s==co->sMap["Gravity"])isDone= gravity03.isDone();
        if(s==co->sMap["SpyGame"]&& spy04.spyId !=-1)isDone= spy04.isDone();
        if(s==co->sMap["Market"]){
            bool b = teamId == 0? co->marketDone1:co->marketDone2;
            isDone = market05.isDone(b);
            if(isDone){
                if(teamId == 0 )co->marketDone1=true;
                if(teamId == 1 )co->marketDone2=true;
                divideTimeToButtons();
            }
        }
        // push game 06
        if(s==co->sMap["Charades"]){
            bool b = teamId == 0 ? co->idleA : co->idleB;
            isDone=charades07.isDone(b);
        }
        
        if(s==co->sMap["DrainTest"])isDone = drainTest.isDone();
        // 09 ping pong
        if(s==co->sMap["Design"])isDone= sDesign.isDone();
        if(s==co->sMap["Idle"])isDone= true; // idle
        
        
        if(isDone && !logDone){
            logDone = true;
            co->log("team "+ofToString(teamId)+" finished at "+ofGetTimestampString(co->timeStamp+":%S"));
        }
        
        if(!isDone){
            drainTime();
            drainIndividuals();
            playAnimation=false;
        }
        
        if(isDone){
            if(!playAnimation){
                vector<int>win={
                    co->sMap["Area"],
                    co->sMap["GetThrough"],
                    co->sMap["GetThrough_alt"],
                    co->sMap["Maze"],
                    co->sMap["Maze_alt"],
                    co->sMap["EasyMaze"],
                    co->sMap["Gravity"],
                    co->sMap["SpyGame"],
                    co->sMap["Market"],
                    co->sMap["Charades"]
                };
                for(int i = 0 ; i<win.size();i++){
                    if(s == win[i]){
                        if(s == 4 && spy04.areWeDone()){
                            playAnimation = false;
                        }else{
                            celebration.play();
                            playAnimation = true;
                            for(int i = 0; i<buttons.size();i++){
                                Button * b = &buttons[i];
                                
                                b->setArmSwap(0);
                                b->setRotation(0);
                                // b->setRotation(b->getRotation()+5);
                                
                            }
                        }
                        
                    }
                }
            }
            if(playAnimation)celebration.update();
        }
        
        
        if(co->debug){
            setFcFilter();
        }
        
        
    }
    
    void draw(){
        ofFill();
        int s = co->sceneNumber;
        if(co->debug){
            ofSetColor(255,200);
            for(int i=0; i<buttons.size(); i++) {
                
                if(buttons[i].isColliding())
                    ofSetColor(255,0,0);
                else ofSetColor(0,0,0);
                
                buttons[i].drawb2d();
            }
            
            for(int i = 0; i<polyShapes.size();i++)polyShapes[i]->draw();
        }
        
        if(co->sceneNumber==co->sMap["Area"])area00.draw();
        else if(s==co->sMap["GetThrough"])getUp01.draw();
        else if(s==co->sMap["GetThrough_alt"])getUp_alt.draw();
        else if(s==co->sMap["EasyMaze"])easyMaze.draw();
        else if(s==co->sMap["Maze"])maze02.draw();
        else if(s==co->sMap["Maze_alt"])maze02_alt.draw();
        else if(s==co->sMap["Gravity"])gravity03.draw();
        else if(s==co->sMap["SpyGame"]&& spy04.spyId !=-1)spy04.draw();
        else if(s==co->sMap["Market"])market05.draw();
        // 06 push game
        else if(s==co->sMap["Charades"])charades07.draw();
        else if(s==co->sMap["DrainTest"])drainTest.draw();
        //09 pingpong
        else if(s==co->sMap["Design"]) sDesign.draw();
        else if(s==co->sMap["Idle"]) idle.draw();
        
        if(s!=co->sMap["Design"] ||
           s!=co->sMap["PingPong"] ||
           s!=co->sMap["PushGame"] ||
           s!=co->sMap["Idle"] ) drawResult();
        
        if(playAnimation){
            celebration.draw( 1920/2 - celebration.getWidth()/2,1080/2 - celebration.getHeight()/2 );
            for(int i = 0; i<buttons.size();i++){
                Button * b = &buttons[i];
                b->dy = 0.01;
             
               // b->setRotation(b->getRotation()+5);
                
            }
        }
        
    }
    
    
    
    void drawResult(){
        ofSetColor(ofColor::royalBlue);
        string t = timeToString(time);
        co->font_medium->drawString( t ,50, 120);
        ofRectangle r = co->font_medium->getStringBoundingBox("00:00",0,0);
        ofDrawLine(r.width + 50 + 50, 40, r.width + 50 + 50, 40+20+co->font_medium->getLineHeight() );
        co->font_medium->drawString( ofToString(getDistVal(),0) ,r.width + 50 + 100,120);
        
        // cout << time << endl;
    }
    
    
    
    void drainTime(){
        time += ofGetLastFrameTime();
    }
    
    void drainIndividuals(){
        for(int i=0; i<buttons.size(); i++) {
            // if(!(co->sceneNumber==4 && i == spy04.spyId))
            if(teamId==0)buttons[i].drain(co->drainCoefficient1);
            else buttons[i].drain(co->drainCoefficient2);
        }
    }
    
    void setFcFilter(){
        for(int i=0; i<buttons.size(); i++) {
            buttons[i].filterLowPass.setFc(co->fc);
        }
    }
    
    float getDistVal(){
        float r=0.f;
        for(int i = 0 ; i<buttons.size();i++)
            if(buttons[i].isPlaying){
                if(!(co->sceneNumber == 4 && i == spy04.spyId ))
                    r+=buttons[i].getValue();
            }
        return r;
    }
    
    void recordValues(){
        for(int i = 0 ; i<buttons.size();i++)
            buttons[i].beforeRefillValue = buttons[i].getValue();
    }
    
    void divideTimeToButtons(){
        if(time>0){
            for(int i = 0; i<buttons.size();i++){
                if(buttons[i].isDead())continue;
                buttons[i].addValue(-ofGetLastFrameTime()*2.);
                time-=ofGetLastFrameTime();
            }
        }
    }
    
    
private:
    vector <shared_ptr<ofxBox2dPolygon> > polyShapes;
    string timeToString(double time){
        time = floorf(time);
        if(time<0)time = 0;
        
        string timeString = "";
        int min = time / 60 ;
        int sec = time - min*60;
        
        timeString = ofToString(min,2,'0')+":";
        timeString.append(ofToString(sec,2,'0'));
        
        return timeString;
    }
    
    void createScene(vector<ofPolyline>polys){
        
        if(polyShapes.size()==0){
            
            for(int i = 0 ;i <polys.size();i++){
                ofRectangle r = polys[i].getBoundingBox();
                if(polys[i].getVertices().size()>3 && (r.width<1900 && r.height < 1070)){
                    shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    
                    poly->addVertices(polys[i].getVertices());
                    poly->setPhysics(0.0, 0.0, 0.0);
                    poly->triangulatePoly();
                    
                    poly->create(box2d.getWorld());
                    polyShapes.push_back(poly);
                }
            }
            
        }
    }
    void destroyMaze(){
        if(polyShapes.size()>0){
            polyShapes.clear();
        }
    }
    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    
};

#endif /* Team_h */
