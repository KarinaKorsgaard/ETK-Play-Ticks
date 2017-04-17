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
        if(spyId >= 0 && spyId < buttons->size()-1 ){
            if(buttons->at(spyId).getValue()==0.0 || areWeDone())return true;
        }
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
           // buttons->at(spyId).multValue(2);
        }
        return val==0.0;
    }
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                ofRectangle r = ofRectangle(30,30,1920-60,1080-60);
                if(!r.inside(buttons->at(i).getBiquadPos()))buttons->at(i).setValue(0);
                
                if(i!=spyId)buttons->at(i).update(co->attraction, r);
                else buttons->at(i).update(co->spySpeed,r);
            }
        }
    }

    
    void draw(){
        
        

        if(co->debug){
            ofSetColor(0);
            ofDrawRectangle(0, 0, 30+25, 1080);
            ofDrawRectangle(0, 0, 1920, 30+25);
            ofDrawRectangle(1920-(30+25), 0, 30+25, 1080);
            ofDrawRectangle(0, 1080-(30+25), 1920, 30+25);
            if(spyId != -1){
                int blue = buttons->at(spyId).teamNumber==0 ? 255:0;
                int alpha = buttons->at(spyId).on ? 255:100;
                ofSetColor(255-blue,0, blue, alpha);
                ofDrawCircle(buttons->at(spyId).getBiquadPos(),60);
            }
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
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                buttons->at(i).setPosition(ofRandom(100,1920-100), ofRandom(100,1080-100));
            }
        }
    };
    void reset(){

    };
    
    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    vector<ofPolyline> polys;

    
};
