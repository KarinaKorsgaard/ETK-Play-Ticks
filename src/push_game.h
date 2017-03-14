#include "ofMain.h"
#include "common.h"


class PushGame : public commonFunctions{
    
public:
    
    
    
    void setup(commonObjects*_co, vector<Button>*b, int team){
        buttons = b;
        co = _co;
        if(team == 0)svg.load("svg/s07_0.svg");
        if(team == 1)svg.load("svg/s07_1.svg");
        polys = getPolyline(svg);
        
        for(int i = 0; i<6 ; i++){
            ofRectangle r;
            int x = team == 0 ? 960:0 ;
            r.set( x ,  29 + i*(208-29) , 960 , 133);
            rects.push_back(r);
        }
    
    };
    bool isDone(){
        bool isInside=true;
        bool allAreDead=true;

        return isInside;
    };
    
    
    void update(){

        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction ); // the is on is checked in button!
        }
    }
    
    void draw(){
        
        ofSetColor(255);
        svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
        for(int i = 0; i<6;i++)ofDrawRectangle(rects[i]);
        
//        ofSetColor(255,0,0);
//        ofSetLineWidth(30);
//        for(int i = 0; i<polys.size();i++)polys[i].draw();
        
    };
    
    
    void begin(ofxBox2d * world, int team){
        
        for(int i=0; i<buttons->size(); i++) {
            int x = team == 0 ? ofRandom(40,960) : ofRandom(960,1980 - 40);
            buttons->at(i).box2Dcircle->setPosition(ofVec2f(x,ofRandom(80,1000)));
            
        }

    };
    
    void reset(){

    }
    
    vector<ofRectangle> rects;
    
    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    vector<ofPoint>resources;
    vector<Button>*buttons;
    
    vector<string> head;
    vector<string> bread;
    
    ofVec2f averagePos = ofVec2f(1920/2.f,1080.f);
    ofxBiquadFilter2f filterLowPass;
    float fc;
    
    ofRectangle winningArea;
    
};
