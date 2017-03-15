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
#include "s03.h"
#include "s04.h"
#include "s05.h"
#include "s06.h"
#include "designCharacter.h"
#include "drainTest.h"

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
    Scene03 maze02; // maze . not done
    Scene04 spy04; // spy
    Scene05 gravity03; // gravity
    Scene06 market05; // market
    Idle idle;
    Idle charades07;

    DesignACharacter sDesign;
    DrainTest drainTest;
    
    void setup(int team, commonObjects *c){
        teamId = team;
        box2d.init();
        box2d.setGravity(0, 0);
        box2d.createBounds(ofRectangle(0,0,1920,1080));
        box2d.enableEvents();
        //box2d.registerGrabbing();
        co = c;
        celebration.load("celebration.mov");
    }
    void setupScenes(){
        sDesign.setup(co,&buttons);
        drainTest.setup(co,&buttons);
        area00.setup(co,&buttons);
        getUp01.setup(co,&buttons);
        maze02.setup(co,&buttons);
        spy04.setup(co,&buttons);
        gravity03.setup(co,&buttons);
        market05.setup(co,&buttons);
        charades07.setup(co,&buttons);
        idle.setup(co,&buttons);

    }
    
    void update(){
        if(p_sceneNum != co->sceneNumber){
            logDone = false;
            playAnimation = false;
            destroyMaze();

            for(int i = 0 ; i<buttons.size();i++)buttons[i].freezeUpdate=false;
            
            if(co->sceneNumber == 0)area00.begin();
            else if(p_sceneNum == 0)area00.reset();
            
            if(co->sceneNumber == 1){ // get up
                getUp01.begin();
                createScene(getUp01.polys);
            }else if(p_sceneNum == 1)getUp01.reset();
            
            if(co->sceneNumber == 2){ // maze
                createScene(maze02.polys);
                maze02.begin(&box2d);
            }else if(p_sceneNum == 2)maze02.reset();
            
            if(co->sceneNumber == 3){
                createScene(gravity03.polys);
                gravity03.begin();
                box2d.setGravity(0,co->gravity);
            }
            if(p_sceneNum == 3){
                if(gravity03.theWinner!=-1)
                    buttons[gravity03.theWinner].value+=50;
                gravity03.reset();
                box2d.setGravity(0,0);
            }
            
            if(co->sceneNumber == 4)spy04.begin();
            else if(co->sceneNumber == 4)spy04.reset();
            
            if(co->sceneNumber == 5){ // !
                createScene(market05.polys);
                market05.begin();
            }else if(p_sceneNum == 5)market05.reset();
            
            if(co->sceneNumber == 7)charades07.begin();
            else if(p_sceneNum == 7)charades07.reset();
            
            if(co->sceneNumber == 8)drainTest.begin();
            else if(p_sceneNum == 8)drainTest.reset();
            
            p_sceneNum=co->sceneNumber;
        }
        
        if(co->sceneNumber==0)area00.update();
        if(co->sceneNumber==1)getUp01.update();
        if(co->sceneNumber==2)maze02.update();
        if(co->sceneNumber==3)gravity03.update();
        if(co->sceneNumber==4 && spy04.spyId !=-1)spy04.update();
        if(co->sceneNumber==5)market05.update();
        // push game! 
        //if(teamId == 1 && co->sceneNumber==6)market05.update();
        if(co->sceneNumber==7)charades07.update(false);
        if(co->sceneNumber==8)drainTest.update();
        // ping pong
        if(co->sceneNumber==10)sDesign.update();
        if(co->sceneNumber==11)idle.update(true);
        
        //if(teamId == 0 && co->sceneNumber==7)finale.update();
       //if(co->sceneNumber==8)pingPong.update();
        
        
        
        box2d.update();

        
        //is done:
        
        if(co->sceneNumber==0)isDone= area00.isDone();
        if(co->sceneNumber==1)isDone= getUp01.isDone();
        if(co->sceneNumber==2)isDone= maze02.isDone();
        if(co->sceneNumber==3)isDone= gravity03.isDone();
        if(co->sceneNumber==4&& spy04.spyId !=-1)isDone= spy04.isDone();
        if(co->sceneNumber==5){
            bool b = teamId == 0? co->marketDone1:co->marketDone2;
            isDone = market05.isDone(b);
            if(isDone){
                if(teamId == 0 )co->marketDone1=true;
                if(teamId == 1 )co->marketDone2=true;
                divideTimeToButtons();
            }
        }
        // push game 06
        if(co->sceneNumber==7){
            bool b = teamId == 0 ? co->idleA : co->idleB;
            isDone=charades07.isDone(b);
        }
        
        if(co->sceneNumber==8)isDone = drainTest.isDone();
        // 09 ping pong
        if(co->sceneNumber==10)isDone= sDesign.isDone();
        if(co->sceneNumber==11)isDone= true; // idle

        
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
                vector<int>win={0,1,2,3,4,5,7};
                for(int i = 0 ; i<win.size();i++){
                    if(co->sceneNumber == win[i]){
                        if(co->sceneNumber == 4 && spy04.areWeDone()){
                            playAnimation = false;
                        }else{
                            celebration.play();
                            playAnimation = true;
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
        
        if(co->sceneNumber==0)area00.draw();
        if(co->sceneNumber==1)getUp01.draw();
        if(co->sceneNumber==2)maze02.draw();
        if(co->sceneNumber==3)gravity03.draw();
        if(co->sceneNumber==4&& spy04.spyId !=-1)spy04.draw();
        if(co->sceneNumber==5)market05.draw();
        // 06 push game
        if(co->sceneNumber==7)charades07.draw();
        if(co->sceneNumber==8)drainTest.draw();
        //09 pingpong
        if(co->sceneNumber==10) sDesign.draw();
        if(co->sceneNumber==11) idle.draw();
    
        drawResult();
        
        if(playAnimation)celebration.draw( 1920/2 - celebration.getWidth()/2,1080/2 - celebration.getHeight()/2 );
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
                    r+=buttons[i].value;
            }
        return r;
    }
    
    void recordValues(){
        for(int i = 0 ; i<buttons.size();i++)
            buttons[i].beforeRefillValue = buttons[i].value;
    }
    
    void divideTimeToButtons(){
        if(time>0){
            for(int i = 0; i<buttons.size();i++){
                if(buttons[i].isDead())continue;
                buttons[i].value-=ofGetLastFrameTime()*2.;
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
