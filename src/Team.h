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

#include "01_Design.h"
#include "02_Area.h"
#include "03_Trail.h"
#include "04_Maze.h"
#include "05_RepresentativeGame.h"
#include "06_LogicGame.h"
#include "07_AverageMaze.h"
#include "08_Fences.h"
#include "09_Fight.h"
#include "10_Factories.h"
#include "10a_ReDesign.h"
#include "11_Cherades.h"
#include "12_Ground.h"
#include "13_Trial.h"

#include "Scene.h"





class Team{
    
public:
    
    ofxBox2d * box2d;
    double time;
    bool isDone;
    bool playAnimation;
    bool addedToWinnerlist = false;
    vector<Button>buttons;
    int teamId;
    int fenceAttraction;
    
    commonObjects *co;
    int p_sceneNum=0;
    
    
    Design design;
    ReDesign reDesign;
    Area area;
    Trail trail;
    Maze maze;
    Representative representative;
    Logic logic;
    AverageMaze averageMaze;
    Fences fences;
    Fight fight;
    Factories factories;
    Idle idle;
    Trial trial;
    Idle charades07;
    
    //bool setSecondsToTakeOff = true;
    Ground ground;
    
    std::map<string, Scene *>scenes;
    
    Team(){};
    ~Team(){};
    
    void setup(int team, commonObjects *c){
        teamId = team;
        co = c;
    }
    
    void setupScenes(){
        scenes["Area"]=(&area);
        scenes["Design"]=(&design);
        scenes["Maze"]=(&maze);
        scenes["Representative"]=(&representative);
        scenes["LogicGame"]=(&logic);
        scenes["Trail"]=(&trail);
        scenes["AverageMaze"]=(&averageMaze);
        scenes["Fences"]=(&fences);
        scenes["Fight"]=(&fight);
        scenes["Factories"]=(&factories);
        scenes["ReDesign"]=(&reDesign);
        scenes["Charades"]=(&charades07);
        scenes["Idle"]=(&idle);
        scenes["GroundGame"]=(&ground);
        scenes["Trial"]=(&trial);
        
        map<string, Scene *>::iterator it;
        for (it = scenes.begin(); it != scenes.end(); it++)
        {
            it->second->teamNumber = teamId;
        }
        
        design.setup(co,&buttons,"img/colors"+ofToString(teamId)+".png",6);
        reDesign.setup(co,&buttons,"img/colors"+ofToString(teamId)+".png",6);
        area.setup(co,&buttons);
        representative.setup(co,&buttons);
        logic.setup(co,&buttons);
        maze.setup(co,&buttons);
        trail.setup(co,&buttons);
        averageMaze.setup(co,&buttons);
        fences.setup(co,&buttons);
        fight.setup(co,&buttons);
        factories.setup(co,&buttons);
        charades07.setup(co,&buttons);
        idle.setup(co,&buttons, false, ofRectangle(0,0,1920*2,1080));
        ground.setup(co,&buttons);
        trial.setup(co,&buttons);
        
        scenes["Design"]->begin(box2d);
        scenes["Design"]->update();
        scenes["Design"]->reset();
        
//        scenes["ReDesign"]->begin(box2d);
//        scenes["ReDesign"]->update();
//        scenes["ReDesign"]->reset();
        
        scenes[co->sMap[co->sceneNumber]]->reset();
        scenes[co->sMap[co->sceneNumber]]->begin(box2d);
        
        
    }
    
    void update(){
        
        int s = co->sceneNumber;
        if(p_sceneNum != s){
            
            reset();
            
            scenes[co->sMap[s]]->begin(box2d);
            
            if(scenes[co->sMap[s]]->solidPolys.size()>0)
                createScene(scenes[co->sMap[s]]->solidPolys);
            
            //
            //            if(co->sMap[s] != "Idle" &&
            //               co->sMap[s] != "GroundGame" &&
            //               co->sMap[s] != "Fight"
            //
            //               ){
            //                createWall();
            //            }
            
            scenes[co->sMap[p_sceneNum]]->reset();
            
            if(teamId == 0){
                co->background.load("img/backgrounds/"+co->sMap[s]+".png");
                
                string file1 = "videos/celebrations/"+co->sMap[s]+"Winner"+".mov";
                string file2 = "videos/celebrations/"+co->sMap[s]+"Looser"+".mov";
                if (!ofFile::doesFileExist(file1)){
                    file1 = "videos/celebrationDefaultWinner.mov";
                }
                if (!ofFile::doesFileExist(file2)){
                    file2 = "videos/celebrationDefaultLooser.mov";
                }
                co->celebration[0].load(file1);
                co->celebration[1].load(file2);
            }
            
            co->teamIsDone.clear();
            addedToWinnerlist = false;
            p_sceneNum = s;
        }
        
        scenes[co->sMap[s]]->update();
        if(co->moveThemOut){
            scenes[co->sMap[s]]->moveOutOfSolids(scenes[co->sMap[s]]->solidPolys);
            if(teamId == 1)co->moveThemOut = false;
        }
        
        //--------------------------------------------------------------
        bool forceDone = false;
        
        
        if(co->sMap[s] == "Factories")
            forceDone = teamId == 0 ? co->marketDone1 : co->marketDone2;
        if(co->sMap[s] == "Design")
            forceDone = teamId == 0 ? co->designDone1 : co->designDone2;
        if(co->sMap[s] == "Charades")
            forceDone = teamId == 0 ? co->idleA : co->idleB;
        //--------------------------------------------------------------
        isDone = scenes[co->sMap[s]]->isDone(forceDone);
        
        
        if(!isDone){
            drainTime();
            drainIndividuals();
            playAnimation=false;
        }
        
        else if (isDone){
            
            if(!addedToWinnerlist){
                addedToWinnerlist=true;
                co->teamIsDone.push_back(teamId);
            }
            if (!co->celebration[co->teamIsDone[teamId]].isPlaying()){
                co->celebration[co->teamIsDone[teamId]].play();
                
                playAnimation = true;
                
                //dance
                for(int i = 0; i<buttons.size();i++){
                    Button * b = &buttons[i];
                    b->setArmSwap(0);
                    b->setRotation(0);
                }
            }
            
            else {
                co->celebration[co->teamIsDone[teamId]].update();
                
                if(!wonSent){
                    ofxOscMessage m;
                    m.setAddress("/won"+ofToString(teamId+1));
                    co->oscOut.sendMessage(m);
                    wonSent=true;
                }
            }
        }
        
        
        if(co->debug){
            setFcFilter();
        }
        
        if(co->sendAverageData){
            sendAverageData();
        }
    }
    
    void draw(){
        ofFill();
        int s = co->sceneNumber;
        scenes[co->sMap[s]]->draw();
        
        
        if(playAnimation){
            ofSetColor(255);
            co->celebration[co->teamIsDone[teamId]].draw( (teamId*1920) + ( 1920/2 - co->celebration[co->teamIsDone[teamId]].getWidth()/2 ),
                                                         1080/2 - co->celebration[co->teamIsDone[teamId]].getHeight()/2 );
            
            for(int i = 0; i<buttons.size();i++){
                buttons[i].dy = 0.01;
            }
        }
        
        if(co->debug){
            for(int i = 0; i<scenes[co->sMap[s]]->solidPolys.size();i++)
                scenes[co->sMap[s]]->solidPolys[i].draw();
        }
        
        drawResult();
        
    }
    
    
    
    
    
    void drawResult(){
        
        float t = time;
        int s = co->sceneNumber;
        if(co->sMap[s]!="Design"){
            
            string timeString = timeToString(t);
            int x = 50 + (teamId * 1920);
            if(co->sMap[s]=="Fight"){
                timeString = ofToString(co->tennisPoint[teamId]+1);
                if (teamId == 1){
                    int w = co->font_medium->getStringBoundingBox(timeString,0,0).width + 50;
                    x = 1920*2 - w;
                }
            }
            
            ofSetColor(ofColor::royalBlue);
            co->font_medium->drawString( timeString , x , 120);
        }
        
        
    }
    
    
    void drainTime(){
        if(co->startTime){
            if(co->sMap[co->sceneNumber]!="Design" && co->sMap[co->sceneNumber]!="Idle" && co->sMap[co->sceneNumber]!="Fight")
                time += ofGetLastFrameTime();
        }
    }
    
    void drainIndividuals(){
        for(int i=0; i<buttons.size(); i++) {
            if(teamId==0)buttons[i].drain(co->drainCoefficient1);
            else buttons[i].drain(co->drainCoefficient2);
        }
    }
    
    void setFcFilter(){
        for(int i=0; i<buttons.size(); i++) {
            buttons[i].filterLowPass.setFc(co->fc);
        }
    }
    
    void reviveTicks(float thres, float amount){
        for(int i=0; i<buttons.size(); i++) {
            if(buttons[i].isPlaying)
                buttons[i].reviveDeadTicks(thres, amount);
        }
    }
    
    float getDistVal(){
        float r=0.f;
        for(int i = 0 ; i<buttons.size();i++){
            if(buttons[i].isPlaying){
                r+=buttons[i].getValue();
            }
        }
        return r;
    }
    
    
    
    //    void divideTimeToButtons(){
    //        if(time>0){
    //            for(int i = 0; i<buttons.size();i++){
    //                if(buttons[i].isDead())continue;
    //                buttons[i].addValue(-ofGetLastFrameTime()*co->divideTimeTime);
    //                time-=ofGetLastFrameTime();
    //            }
    //        }
    //    }
    
    //    void takeOffSeconds(){
    //
    //        if(secondsToTakeOff>0){
    //            secondsToTakeOff-=ofGetLastFrameTime() * 5.;
    //            time-=ofGetLastFrameTime() * 5.;
    //        }
    //    };
    
    
private:
    
    float secondsToTakeOff;
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
    
    
    void createScene(vector<ofPolyline>polys, float d = 0.,float f = 0.,float b = 0.){
        
        if(polyShapes.size()==0){
            for(int i = 0 ;i <polys.size();i++){
                ofRectangle r = polys[i].getBoundingBox();
                if(polys[i].getVertices().size()>3 && (r.width<1900 || r.height < 1070)){
                    
                    shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    
                    vector<ofPoint>verts;
                    for(auto p : polys[i].getVertices())
                        verts.push_back(ofPoint(p.x, p.y));
                    
                    poly->addVertices(verts);
                    poly->setPhysics(d,f,b);
                    poly->triangulatePoly();
                    
                    poly->create(box2d->getWorld());
                    polyShapes.push_back(poly);
                }
            }
        }
        cout << ofToString(polyShapes.size())+ " polyshapes size in " + co->sMap[co->sceneNumber]<< endl;
    }
    
    void createWall(){
        if(teamId == 0){
            shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
            
            vector<ofPoint>verts;
            
            verts.push_back(ofPoint(1920-1,0));
            verts.push_back(ofPoint(1920+1,0));
            verts.push_back(ofPoint(1920+1,1080));
            verts.push_back(ofPoint(1920-1,1080));
            verts.push_back(ofPoint(1920-1,0));
            
            poly->addVertices(verts);
            poly->setPhysics(0,0,0);
            poly->triangulatePoly();
            
            poly->create(box2d->getWorld());
            polyShapes.push_back(poly);
            
        }
        
    }
    
    
    
    void destroyMaze(){
        
        for(int i = 0; i<polyShapes.size();i++)
            polyShapes[i]->destroy();
        
        polyShapes.clear();
        
    }
    void reset(){
        co->celebration[teamId].stop();
        
        playAnimation = false;
        wonSent = false;
        destroyMaze();
    }
    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    
    void sendAverageData(){
        ofVec3f d = getAverageData();
        if(p_averageData!=d){
            p_averageData=d;
            
            ofxOscMessage m;
            m.setAddress("/averageX"+ofToString(teamId+1));
            m.addFloatArg(d.x);
            co->oscOut.sendMessage(m);
            
            m.clear();
            m.setAddress("/averageY"+ofToString(teamId+1));
            m.addFloatArg(d.y);
            co->oscOut.sendMessage(m);
            
            m.clear();
            m.setAddress("/averageSpeed"+ofToString(teamId+1));
            m.addFloatArg(d.z);
            co->oscOut.sendMessage(m);
        }
    }
    
    ofVec3f getAverageData(){
        ofVec3f res;
        res.set(0,0,0);
        
        float indx = 0.;
        for(int i = 0; i<buttons.size();i++){
            if(buttons[i].isPlaying){
                res.x += buttons[i].getRawData().x;
                res.y += buttons[i].getRawData().y;
                
                res.z += buttons[i].getSpeed()*100.;
                indx++;
            }
        }
        if(indx>0.){
            res.x/=indx;
            res.y/=indx;
        }
        return res;
    }
    
    ofVec3f p_averageData;
    bool wonSent;
};

#endif /* Team_h */
