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
#include "finale.h"
#include "idle.h"
//#include "pingPong.h"

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
    
    Scene01 s01; // area
    Scene02 s02; // getup
    Scene03 s03; // maze . not done
    Scene04 s04; // spy
    Scene05 s05; // gravity
    Scene06 s06; // market
    Idle idle;
  //  PingPong pingPong;
    Finale finale;
    DesignACharacter sDesign;
    DrainTest drainTest;
    
    void setup(int team, commonObjects *c){
        teamId = team;
        box2d.init();
        box2d.setGravity(0, 0);
        box2d.createBounds(ofRectangle(0,0,1920,1080));
        box2d.setFPS(60.0);
        box2d.enableEvents();
        //box2d.registerGrabbing();
        co = c;
        celebration.load("celebration.mov");
    }
    void setupScenes(){
        sDesign.setup(co,&buttons);
        drainTest.setup(co,&buttons);
        s01.setup(co,&buttons);
        s02.setup(co,&buttons);
        s03.setup(co,&buttons);
        s04.setup(co,&buttons);
        s05.setup(co,&buttons);
        s06.setup(co,&buttons);
        idle.setup(co,&buttons);
        //pingPong.setup(co,&buttons);
        if(teamId == 0)finale.setup(co,&buttons);
    }
    
    void update(){
        if(p_sceneNum != co->sceneNumber){
            logDone = false;
            playAnimation = false;
            destroyMaze();
          //  box2d.disableEvents();

            
            if(co->sceneNumber == 0){ // area
                s01.begin();
            }
            if(co->sceneNumber == 1){ // get up
                createScene(s02.polys);
                s02.begin();
            }
            if(co->sceneNumber == 2){ // maze
                createScene(s03.polys);
                
                s03.average = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
                //virtual void setPhysics(float density, float bounce, float friction);
                s03.average.get()->setPhysics(3.0, 0.0, 400.0);
                s03.average.get()->setup(box2d.getWorld(), 1920-120, 1080-120, 60);
              
                
            
                s03.begin();
            }if(p_sceneNum == 2){ // reset if it was just a test!
                s03.average->destroy();
            }
            
            if(co->sceneNumber == 3){ // record values needed.
                drainTest.begin();
            }
            if(p_sceneNum == 3){ // reset if it was just a test!
                drainTest.reset();
            }
            
            
            if(co->sceneNumber == 4){ // spyGame SWAP!!! 
              //  createScene(s03.polys);
                s04.begin();
            }
            
            if(co->sceneNumber == 5){ // GRAVITY!
                createScene(s05.polys);
                s05.begin();
                box2d.setGravity(0,co->gravity);
            }
            if(p_sceneNum == 5){ // GRAVITY!
                if(s05.theWinner!=-1)buttons[s05.theWinner].value+=50;
                s05.reset();
                box2d.setGravity(0,0);
            }
            
            
            if(co->sceneNumber == 6){ // !
                createScene(s06.polys);
                s06.begin();
               // box2d.setGravity(0,co->gravity);
            }
            if(p_sceneNum == 6){ // !
                s06.reset();
                //box2d.setGravity(0,0);
            }
            

            
            
            p_sceneNum=co->sceneNumber;
        }
        
        if(co->sceneNumber==0)s01.update();
        if(co->sceneNumber==1)s02.update();
        if(co->sceneNumber==2)s03.update();
        if(co->sceneNumber==3)drainTest.update();
        if(co->sceneNumber==4 && s04.spyId !=-1)s04.update();
        if(co->sceneNumber==5)s05.update();
        if(co->sceneNumber==6)s06.update();
        //if(teamId == 1 && co->sceneNumber==6)s06.update();
        if(co->sceneNumber==10)idle.update();
        
        
        if(teamId == 0 && co->sceneNumber==7)finale.update();
       //if(co->sceneNumber==8)pingPong.update();
        
        if(co->sceneNumber==9)sDesign.update();
        
        box2d.update();

        
        //is done:
        
        if(co->sceneNumber==0)isDone= s01.isDone();
        if(co->sceneNumber==1)isDone= s02.isDone();
        if(co->sceneNumber==2)isDone= s03.isDone();
        if(co->sceneNumber==3)isDone= drainTest.isDone();
        if(co->sceneNumber==4&& s04.spyId !=-1)isDone= s04.isDone();
        if(co->sceneNumber==5)isDone= s05.isDone();
        if(co->sceneNumber==8)isDone=true;
        if(co->sceneNumber==10)isDone=true;
        if(co->sceneNumber==6){
            bool b = teamId == 0 ? co->marketDone1:co->marketDone2;
            isDone = s06.isDone(b);
            
            if(isDone){
                if(teamId == 0 )co->marketDone1=true;
                if(teamId == 1 )co->marketDone2=true;
                divideTimeToButtons();
            }
        }
        
        if(teamId == 0 && co->sceneNumber==7)isDone= finale.isDone();
        if(co->sceneNumber==9) isDone= sDesign.isDone();
        
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
                vector<int>win={0,1,2,4,5};
                for(int i = 0 ; i<win.size();i++){
                    if(co->sceneNumber == win[i]){
                        if(co->sceneNumber == 4 && s04.areWeDone()){
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
                
                buttons[i].drawb2d();
            }
            
            for(int i = 0; i<polyShapes.size();i++)polyShapes[i]->draw();
        }
        
        if(co->sceneNumber==0)s01.draw();
        if(co->sceneNumber==1)s02.draw();
        if(co->sceneNumber==2)s03.draw();
        if(co->sceneNumber==3)drainTest.draw();
        if(co->sceneNumber==4&& s04.spyId !=-1)s04.draw();
        if(co->sceneNumber==5)s05.draw();
        if(co->sceneNumber==6)s06.draw();
        if(co->sceneNumber==7 && teamId == 0)finale.draw();
        //if(co->sceneNumber==8)pingPong.draw();
        if(co->sceneNumber==9) sDesign.draw();
        if(co->sceneNumber==10) idle.draw();
        
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
           // if(!(co->sceneNumber==4 && i == s04.spyId))
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
                if(!(co->sceneNumber == 4 && i == s04.spyId ))
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
                if(polys[i].getVertices().size()>3 && (r.width<1920 && r.height < 1080)){
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
