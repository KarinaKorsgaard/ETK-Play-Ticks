#include "ofMain.h"
#include "common.h"

class Scene04 : public commonFunctions{
    
public:
    vector<Button>*buttons;
    bool haveLogged;
    int spyId; // received spy Id! 

    
    void setup(commonObjects*_co, vector<Button>*b ){
        buttons = b;
        co = _co;
    
        spyId = -1;
        
        head = transformToCollumn(getLine("text/04_spy.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/04_spy.txt",1),800, co->font_small);
        
        
    };
    bool isDone(){
        
        if(buttons->at(spyId).value==0.0 || areWeDone())return true;
        else return false;
    };
    
    bool areWeDone(){
        float val = 0.;
        int team ;
        for(int i = 0; i<buttons->size();i++){
            if(i!=spyId)val += buttons->at(i).value;
        }
        if(val == 0.0 && !haveLogged){
            haveLogged = true;
            string s = "spy from team "+ ofToString( buttons->at(spyId).teamNumber )+" beat the other team at"+ofGetTimestampString(co->timeStamp);
            co->log(s);
            buttons->at(spyId).value*=2;
        }
        return val==0.0;
    }
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
            
                if(buttons->at(i).isColliding() && !buttons->at(i).isDead()){
                    if(!isDone())buttons->at(i).value-=co->spyDrainer;
                }
                
                buttons->at(i).update(co->attraction);
            }
        }
        if(spyId!=-1){
            if(!buttons->at(spyId).on){
                buttons->at(spyId).on = true;
                buttons->at(spyId).box2Dcircle->setPosition(1920/2,1080/2);
                // buttons->at(spyId).value -=1;
            }
        }
        
    }
    
    void draw(){

        if(co->debug){
            int blue = buttons->at(spyId).teamNumber==0 ? 255:0;
            
            int alpha = buttons->at(spyId).on ? 255:100;
            ofSetColor(255-blue,0, blue, alpha);
            ofDrawCircle(buttons->at(spyId).getBiquadPos(),60);
        }
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
//        ofSetColor(ofColor::royalBlue);
//        for(int i = 0; i< head.size();i++)
//            co->font_medium->drawString(head[i], 50, 100+i*co->font_medium->getLineHeight());
//        
//        for(int i = 0; i< bread.size();i++)
//            co->font_small->drawString(bread[i], 50, 180+i*co->font_small->getLineHeight());
    };
    
    void begin(){
        haveLogged = false;
    };
    void reset(){

    };
    
    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    
    vector<string> head;
    vector<string> bread;
    
};
