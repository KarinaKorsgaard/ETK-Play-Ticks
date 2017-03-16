#include "ofMain.h"
#include "common.h"

//class Blob{
//public:
//    ofPolyline area;
//    ofVec2f pos;
//    ofVec2f speed;
//    float radius = 600.;
//    int wave_height=radius*0.1; // height of a sine wave
//    int waves_amount= 16; // amount of sine aves per circle
//    int circle_resolution = 1600;
//    // so we just multiplying the angle on amount of waves we want to have
//    
//    
//    void setup(){
//        for(int i = 0; i< 36; i++){
//            float x=radius*cos(i);
//            float y=radius*sin(i);
//            area.addVertex(ofVec2f(x,y));
//            area.close();
//        }
//        pos = ofVec2f(1920/2,1080/2);
//        speed = ofVec2f(5,5);
//    }
//    void move(){
//        pos+=speed;
//        if(pos.x>1920-100)speed.x*=-1;
//        if(pos.x<100)speed.x*=-1;
//        
//        if(pos.y>1080-100)speed.y*=-1;
//        if(pos.y<100)speed.y*=-1;
//    }
//    void updatePoly(){
//        ofPolyline copy = area;
//        area.clear();
//        wave_height= radius * ofMap(sin(ofGetElapsedTimef()),0,1,0.02,0.1);
//        float speed_increment= ofGetElapsedTimef();
//        int anim_shape=16;
//       // int sine_radius=radius*0.1;
//        
//        for(int i=0; i<circle_resolution; i++){
//            
//            float angle=TWO_PI/circle_resolution*i;
//            float raidus_addon=wave_height*sin((angle+speed_increment)*anim_shape);
//            
//            float x=cos(angle)*(radius+raidus_addon);
//            float y=sin(angle)*(radius+raidus_addon);
//            x/=2;
//            y/=2;
//            x = CLAMP(x+pos.x,0,1920);
//            y = CLAMP(y+pos.y,0,1080);
//            area.addVertex(ofPoint(x,y));
//        }
//        
//        area.close();
//        
//        area = area.getSmoothed(100);
//    }
//    void draw(){
//
//        ofPushStyle();
//        ofSetColor(0,0,60);
//        ofSetLineWidth(20);
//        area.draw();
//        ofPopStyle();
//    }
//};


class Scene01 : public commonFunctions{
    
public:
    vector<Button>*buttons;
    //Blob blob;
    ofImage donut;
    ofPolyline big,small;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/01_win_area.svg");
        donut.load("img/01_dounut.png");
        vector<ofPolyline> p = getPolyline(svg);
        //poly = getPolyline(svg);
        
        float a = p[0].getBoundingBox().width;
        float o = p[1].getBoundingBox().width;
        
        if(a>o){
            big = p[0];
            small = p[1];
        }
        else{
            big = p[1];
            small = p[0];
        }
    };
    
    bool isDone(){
        bool isInside=true;
        bool allAreDead = true;
        for(int i = 0; i<buttons->size();i++){

            if(!buttons->at(i).on && buttons->at(i).isPlaying){
                isInside=false;
                break;
            }
            if(buttons->at(i).on){
                ofPoint p = buttons->at(i).getBiquadPos();
                if(!big.inside(p) || small.inside(p)){
                    isInside=false;
                    break;
                }
            }
            if(!buttons->at(i).isDead()){
                allAreDead =false;
            }
            
        }
        if(allAreDead)isInside = false;
        return isInside;
    };
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
    }

    void draw(){
        ofSetColor(255);
        dounut.draw();
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
    };
    
    void begin(){};
    void reset(){};

    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    
    
};
