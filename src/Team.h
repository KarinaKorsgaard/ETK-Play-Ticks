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
#include "07_AverageMaze.h"
#include "08_Fences.h"
#include "10_Factories.h"
#include "11_Cherades.h"
#include "12_Ground.h"

#include "Scene.h"

class Team{
    
public:
    
    ofxBox2d * box2d;
    double time;
    float distance;
    float deadTime = 0;
    bool isDone;
    bool logDone = false;
    bool playAnimation = false;
    vector<Button>buttons;
    int teamId;
    
    commonObjects *co;
    int p_sceneNum=0;
    ofVideoPlayer celebration;
    
    Area area;
    //representative game
    Maze maze;
    AverageMaze averageMaze;
    Fences fences;
    Factories factories;
    Idle idle;
    Idle charades07;
    Trail trail;
    Design design;
    Ground ground;

    std::map<string, Scene *>scenes;
    
    Team(){};
    
    ~Team(){

    };
    
    void setup(int team, commonObjects *c){
        teamId = team;
        co = c;
        celebration.load("celebration.mov");
    }
    void setupScenes(){
        
        design.setup(co,&buttons,"colors.png",6);
        area.setup(co,&buttons);
        maze.setup(co,&buttons);
        trail.setup(co,&buttons);
        averageMaze.setup(co,&buttons);
        fences.setup(co,&buttons);
        factories.setup(co,&buttons,teamId);
        charades07.setup(co,&buttons);
        idle.setup(co,&buttons);
        ground.setup(co,&buttons);
        
        scenes["Area"]=(&area);
        scenes["Design"]=(&design);
        scenes["Maze"]=(&maze);
        scenes["Trail"]=(&trail);
        scenes["AverageMaze"]=(&averageMaze);
        scenes["Fences"]=(&fences);
        scenes["Factories"]=(&factories);
        scenes["Charades"]=(&charades07);
        scenes["Idle"]=(&idle);
        scenes["GroundGame"]=(&ground);
        
        map<string, Scene *>::iterator it;
        for (it = scenes.begin(); it != scenes.end(); it++)
        {
            it->second->teamNumber = teamId;
            cout <<"MAPMAP " +ofToString( it->first )<< endl;
        }
           
    }
    
    void update(){
        
        int s = co->sceneNumber;
        
        if(p_sceneNum != s){
            
            logDone = false;
            playAnimation = false;
            wonSent = false;
            destroyMaze();
            
            
            if(scenes[co->sMap[s]]->solidPolys.size()>0)
                createScene(scenes[co->sMap[s]]->solidPolys);
            
            scenes[co->sMap[s]]->begin(box2d);
            scenes[co->sMap[p_sceneNum]]->reset();
            
            if(co->sMap[p_sceneNum]=="Fences")box2d->setGravity(0,0);
            
            p_sceneNum = s;
        }
        
        scenes[co->sMap[s]]->update();
        isDone = scenes[co->sMap[s]]->isDone();
        
        
//            if(s == co->sMap["Area"])area.begin();
//            else if(p_sceneNum == co->sMap["Area"])area=();
//            
//            
//            
//            if(s == co->sMap["Maze"]){ // get up
//                createScene(maze.polys);
//                maze.begin();
//            }else if(p_sceneNum == co->sMap["Maze"])maze=();
//            
//            
//            if(s == co->sMap["Trail"])trail.begin();
//            else if(p_sceneNum == co->sMap["Trail"])trail=();
//            
//         
//            if(s == co->sMap["AverageMaze"]){ // maze
//                createScene(averageMaze.polys);
//                averageMaze.begin(&box2d);
//            }else if(p_sceneNum == co->sMap["AverageMaze"])averageMaze=();
//            
//            
//            if(s == co->sMap["Fences"]){
//                createScene(fences.polys);
//                fences.begin();
//                box2d.setGravity(0,co->gravity);
//            }
//            if(p_sceneNum == co->sMap["Fences"]){
//                if(fences.theWinner!=-1)
//                    buttons[fences.theWinner].addValue(co->gravityReward);
//                fences=();
//                box2d.setGravity(0,0);
//            }
//            
//            if(s == co->sMap["Factories"]){ // !
//                factories.begin();
//                createScene(factories.polys);
//                cout << "Factories has "+ofToString(factories.polys.size())+ "solids"<< endl;
//            }else if(p_sceneNum == co->sMap["Factories"])factories=();
//            
//            if(s == co->sMap["Charades"])charades07.begin();
//            else if(p_sceneNum == co->sMap["Charades"])charades07=();
//            
//
//            p_sceneNum=s;
//        }
//        if(s==co->sMap["Area"])area.update();
//        if(s==co->sMap["Maze"])maze.update();
//        
//        if(s==co->sMap["Trail"]){
//            trail.update();
//            if(trail.sendOsc){
//                trail.sendOsc=false;
//                ofxOscMessage m;
//                m.setAddress("/Trail"+ofToString(teamId+1));
//                m.addIntArg(trail.oscInt);
//                co->oscOut.sendMessage(m);
//            }
//        }
//        
//        
//        if(s==co->sMap["AverageMaze"])averageMaze.update();
//        if(s==co->sMap["Fences"])fences.update();
//        
//        if(s==co->sMap["Factories"])factories.update();
//        
//        // push game!
//        //if(teamId == 1 && s==6)factories.update();
//        if(s==co->sMap["Charades"])charades07.update(false);
//        if(s==co->sMap["Design"])design.update();
//        if(s==co->sMap["Idle"])idle.update(true);
        
        //if(teamId == 0 && s==7)finale.update();
        //if(s==8)pingPong.update();
        
        
        
       // box2d.update();
        
        
//        //is done:
//        
//        if(s==co->sMap["Area"])isDone= area.isDone();
//        if(s==co->sMap["Maze"])isDone= maze.isDone();
//        if(s==co->sMap["Trail"])isDone= trail.isDone();
//        if(s==co->sMap["AverageMaze"])isDone= averageMaze.isDone();
//        if(s==co->sMap["Fences"])isDone= fences.isDone();
//        
//        if(s==co->sMap["Factories"]){
//            bool b = teamId == 0? co->marketDone1:co->marketDone2;
//            isDone = factories.isDone(b);
//            if(isDone){
//                if(teamId == 0 )co->marketDone1=true;
//                if(teamId == 1 )co->marketDone2=true;
//                divideTimeToButtons();
//            }
//        }
//        // push game 06
//        if(s==co->sMap["Charades"]){
//            bool b = teamId == 0 ? co->idleA : co->idleB;
//            isDone=charades07.isDone(b);
//        }
//
//        // 09 ping pong
//        if(s==co->sMap["Design"])isDone=design.isDone();
//        if(s==co->sMap["Idle"])isDone= true; // idle
//        
//        
//        if(isDone && !logDone){
//            logDone = true;
//            co->log("team "+ofToString(teamId)+" finished at "+ofGetTimestampString(co->timeStamp+":%S"));
//        }
        
        if(!isDone){
            bool drain = true;
            
            if(drain){
                drainTime();
                drainIndividuals();
                playAnimation=false;
            }
        }
        
        if(isDone){
            if(!playAnimation){
            
                playAnimation = true;
                celebration.play();
                
                for(int i = 0; i<buttons.size();i++){
                    Button * b = &buttons[i];
                    
                    b->setArmSwap(0);
                    b->setRotation(0);
                }

                
//                vector<int>win={
//                    co->sMap["Design"],
//                    co->sMap["Area"],
//                    co->sMap["Maze"],
//                    co->sMap["AverageMaze"],
//                    co->sMap["Trail"],
//                    co->sMap["Fences"],
//                    co->sMap["Factories"],
//                    co->sMap["Charades"]
//                };
//                for(int i = 0 ; i<win.size();i++){
//                    if(s == win[i]){
//                 //                    }
//                }
            }
            if(playAnimation){
                celebration.update();
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
    }
    
    void draw(){
        ofFill();
        int s = co->sceneNumber;
        
        scenes[co->sMap[s]]->draw();
        
        if(co->debug){
//            for(int i = 0; i< polyShapes.size();i++)polyShapes[i].draw();
//            ofSetColor(255,200);
//            for(int i=0; i<buttons.size(); i++) {
//                
//                if(buttons[i].isColliding())
//                    ofSetColor(255,0,0);
//                else ofSetColor(0,0,0);
//                
//                buttons[i].drawb2d();
//            }
//            
            for(int i = 0; i<scenes[co->sMap[s]]->solidPolys.size();i++)
                scenes[co->sMap[s]]->solidPolys[i].draw();
        }
        
//        if(co->sceneNumber==co->sMap["Area"])area.draw();
//        else if(s==co->sMap["Maze"])maze.draw();
//        else if(s==co->sMap["Trail"])trail.draw();
//        else if(s==co->sMap["AverageMaze"])averageMaze.draw();
//        else if(s==co->sMap["Fences"])fences.draw();
//
//        else if(s==co->sMap["Factories"])factories.draw();
//        // 06 push game
//        else if(s==co->sMap["Charades"])charades07.draw();
//        //09 pingpong
//        else if(s==co->sMap["Design"]) design.draw();
//        else if(s==co->sMap["Idle"]) idle.draw();
//        
        
        
        if(playAnimation){
            ofSetColor(255);
            celebration.draw( (teamId*1920) + ( 1920/2 - celebration.getWidth()/2 ),
                             1080/2 - celebration.getHeight()/2 );
            
            for(int i = 0; i<buttons.size();i++){
                buttons[i].dy = 0.01;
            }
        }
        
        drawResult();
//        if(allAreDead())deadTime+=ofGetLastFrameTime();
//        else deadTime = 0.;
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
    
    
    void drawResult(){
        ofSetColor(ofColor::royalBlue);
        string t = timeToString(time);
        co->font_medium->drawString( t , 50 + (teamId * 1920), 120);
        ofRectangle r = co->font_medium->getStringBoundingBox("00:00",0,0);

    }
    
    bool allAreDead(){
        if(anyButtons)return getDistVal()==0.0;
        else return false;
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
    
    void reviveTicks(float thres, float amount){
        for(int i=0; i<buttons.size(); i++) {
            if(buttons[i].isPlaying)
                buttons[i].reviveDeadTicks(thres, amount);
        }
    }
    
    float getDistVal(){
        float r=0.f;
        for(int i = 0 ; i<buttons.size();i++)
            if(buttons[i].isPlaying){
                anyButtons=true;
                r+=buttons[i].getValue();
            }
        return r;
    }
    

    
    void divideTimeToButtons(){
        if(time>0){
            for(int i = 0; i<buttons.size();i++){
                if(buttons[i].isDead())continue;
                buttons[i].addValue(-ofGetLastFrameTime()*co->divideTimeTime);
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
    
    bool anyButtons = false;
    
    void createScene(vector<ofPolyline>polys, float d = 0.,float f = 0.,float b = 0.){
    
        if(polyShapes.size()==0){
            
            for(int i = 0 ;i <polys.size();i++){
                ofRectangle r = polys[i].getBoundingBox();
                if(polys[i].getVertices().size()>3 && (r.width<1900 && r.height < 1070)){
                    
                    shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    
                    vector<ofPoint>verts;
                    for(auto p : polys[i].getVertices())
                        verts.push_back(ofPoint(p.x + (1920 * teamId), p.y));
                    
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


    
    void destroyMaze(){
        if(polyShapes.size()>0){
            for(int i = 0; i<polyShapes.size();i++)
                polyShapes[i]->destroy();
            
            polyShapes.clear();
        }
        //if(rectShapes.size()>0)rectShapes.clear();
    }
    
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    ofVec3f p_averageData;
    vector<ofVec3f>spyRects;
    bool wonSent;
};

#endif /* Team_h */
