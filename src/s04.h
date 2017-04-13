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
        
        svg.load("svg/04_spy.svg");
        polys = getPolyline(svg);
        cout << "spy debugger:"<<endl;
        
        cout << polys.size()<< endl;
        cout << "---"<<endl;
        spyId = -1;
        
    };
    bool isDone(){
        
        if(buttons->at(spyId).getValue()==0.0 || areWeDone())return true;
        else return false;
    };
    
    bool areWeDone(){
        float val = 0.;
        int team ;
        for(int i = 0; i<buttons->size();i++){
            if(i!=spyId)val += buttons->at(i).getValue();
        }
        if(val == 0.0 && !haveLogged){
            haveLogged = true;
            string s = "spy from team "+ ofToString( buttons->at(spyId).teamNumber )+" beat the other team at"+ofGetTimestampString(co->timeStamp);
            co->log(s);
            buttons->at(spyId).multValue(2);
        }
        return val==0.0;
    }
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {

                // dont drain on collide
//                if(buttons->at(i).isColliding() && !buttons->at(i).isDead()){
//                    if(!isDone())buttons->at(i).addValue(-co->spyDrainer);
//                }
                
                if(i!=spyId)buttons->at(i).update(co->attraction);
                else buttons->at(i).update(co->spySpeed);
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
    };
    
    void begin(){
        haveLogged = false;
    };
    void reset(){

    };
    
    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    vector<ofPolyline> polys;

    
};
