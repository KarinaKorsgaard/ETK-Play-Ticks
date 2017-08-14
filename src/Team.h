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
    
    Design design;
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
    Idle charades07;
    
    bool setSecondsToTakeOff = true;
    Ground ground;

    std::map<string, Scene *>scenes;
    
    Team(){};
    ~Team(){};
    
    void setup(int team, commonObjects *c){
        teamId = team;
        co = c;
        celebration.load("celebration.mov");
    }
    void setupScenes(){
        

        
        scenes["Area"]=(&area);
        scenes["Design"]=(&design);
        scenes["Maze"]=(&maze);
        scenes["Representative"]=(&representative);
        scenes["Logic"]=(&logic);
        scenes["Trail"]=(&trail);
        scenes["AverageMaze"]=(&averageMaze);
        scenes["Fences"]=(&fences);
        scenes["Fight"]=(&fight);
        scenes["Factories"]=(&factories);
        scenes["Charades"]=(&charades07);
        scenes["Idle"]=(&idle);
        scenes["GroundGame"]=(&ground);
        
        map<string, Scene *>::iterator it;
        for (it = scenes.begin(); it != scenes.end(); it++)
        {
            it->second->teamNumber = teamId;
        }
        
        design.setup(co,&buttons,"colors.png",6);
        area.setup(co,&buttons);
        representative.setup(co,&buttons);
        logic.setup(co,&buttons);
        maze.setup(co,&buttons);
        trail.setup(co,&buttons);
        averageMaze.setup(co,&buttons);
        fences.setup(co,&buttons);
        fight.setup(co,&buttons);
        factories.setup(co,&buttons,teamId);
        charades07.setup(co,&buttons);
        idle.setup(co,&buttons);
        ground.setup(co,&buttons);
        

           
    }
    
    void update(){
        
        int s = co->sceneNumber;
        
        if(p_sceneNum != s){
            reset();
            

            if(scenes[co->sMap[s]]->solidPolys.size()>0){
                createScene(scenes[co->sMap[s]]->solidPolys);
            }
            scenes[co->sMap[s]]->begin(box2d);
            scenes[co->sMap[p_sceneNum]]->reset();
            

            //--------------------------------------------------------------
            // special rules:
            if(co->sMap[p_sceneNum]=="Factories"){
                secondsToTakeOff = 0;
                setSecondsToTakeOff = true;
            }
            
            if(co->sMap[p_sceneNum]=="Fences")
                box2d->setGravity(0,0);
            //--------------------------------------------------------------
            p_sceneNum = s;
        }
        
        scenes[co->sMap[s]]->update();
        if(co->moveThemOut){
            scenes[co->sMap[s]]->moveOutOfSolids(scenes[co->sMap[s]]->solidPolys);
            co->moveThemOut = false;
        }
        
        //--------------------------------------------------------------
        bool forceDone = false;
        
        if(co->sMap[s] == "Factories")
            forceDone = teamId == 0 ? co->marketDone1 : co->marketDone2;
        if(co->sMap[s] == "Charades")
            forceDone = teamId == 0 ? co->idleA : co->idleA;

        isDone = scenes[co->sMap[s]]->isDone(forceDone);
        
        //--------------------------------------------------------------
        // special rules
        if(co->sMap[s]=="Factories"){
            if(isDone && setSecondsToTakeOff){
                setSecondsToTakeOff = false;
                Factories * f = static_cast<Factories *>(scenes[co->sMap[s]]);
                secondsToTakeOff = f->basketEfficency ;
                cout << secondsToTakeOff << endl;
            }
        }
    
        //--------------------------------------------------------------
        
        
      
        
        if(!isDone){
            drainTime();
            drainIndividuals();
            playAnimation=false;
        }
        
        else if (isDone){
            if(co->sMap[s] == "Factories")
                takeOffSeconds();
            
            if (!celebration.isPlaying()){
                celebration.play();
                
                playAnimation = true;
                
                //dance
                for(int i = 0; i<buttons.size();i++){
                    Button * b = &buttons[i];
                    b->setArmSwap(0);
                    b->setRotation(0);
                }
            }
            
            else {
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
            sendAverageData();
        }
    }
    
    void draw(){
        ofFill();
        int s = co->sceneNumber;
        
        scenes[co->sMap[s]]->draw();
        

        if(playAnimation){
            ofSetColor(255);
            celebration.draw( (teamId*1920) + ( 1920/2 - celebration.getWidth()/2 ),
                             1080/2 - celebration.getHeight()/2 );
            
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
        ofSetColor(ofColor::royalBlue);
        string t = timeToString(time);
        co->font_medium->drawString( t , 50 + (teamId * 1920), 120);
        ofRectangle r = co->font_medium->getStringBoundingBox("00:00",0,0);
    }

    
    void drainTime(){
        time += ofGetLastFrameTime();
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
    

    
    void divideTimeToButtons(){
        if(time>0){
            for(int i = 0; i<buttons.size();i++){
                if(buttons[i].isDead())continue;
                buttons[i].addValue(-ofGetLastFrameTime()*co->divideTimeTime);
                time-=ofGetLastFrameTime();
            }
        }
    }
    
    void takeOffSeconds(){
        
        if(secondsToTakeOff>0){
            secondsToTakeOff-=ofGetLastFrameTime() * 5.;
            time-=ofGetLastFrameTime() * 5.;
        }
    };
    
    
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
                if(polys[i].getVertices().size()>3 && (r.width<1900 && r.height < 1070)){
                    
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


    
    void destroyMaze(){
      
        for(int i = 0; i<polyShapes.size();i++)
            polyShapes[i]->destroy();
        
        polyShapes.clear();
        
    }
    void reset(){
        celebration.stop();
        logDone = false;
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
