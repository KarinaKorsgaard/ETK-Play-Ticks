#include "ofMain.h"
#include "common.h"
#include "ofxBiquadFilter.h"

class EasyMaze : public commonFunctions{
    
public:
    ofPolyline maze;
    int end,start;
    bool go = false;

    ofxBiquadFilter2f filter;
    ofPoint average;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/03_EasyMaze.svg");
        path.load("img/03_EasyMaze.png");
        polys = getPolyline(svg);
        maze = polys[0];
        
        start = 300;
        end = 1920-50;
        
        filter.setFc(0.05f);
        
    };
    
    bool isDone(){
        bool isInside=false;
        bool allAreDead=true;
        if(go && filter.value().x > end) isInside = true;
        
        if(isInside){
            for(int i = 0; i<buttons->size();i++){
                
                if(!buttons->at(i).on && buttons->at(i).isPlaying){
                    isInside=false;
                    break;
                }
                
                if(!buttons->at(i).isDead()){
                    allAreDead =false;
                }
            }
        }
        
        if(allAreDead)isInside=false;
        return isInside;
    };
    
    
    void update(){
        float x = 0.0;
        float y = 0.0;
        float indx = 0.0;
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction ); // the is on is checked in button!
                
                if(buttons->at(i).isPlaying){ // its on, not dead. (if it is not playing, it can not be on)
                    x+=buttons->at(i).getBiquadPos().x;
                    y+=buttons->at(i).getBiquadPos().y;
                    indx++;
                }
            }
            
            if(indx>0)average = ofPoint(x/indx , y/indx);
            filter.update(average);
        }
        if(go)
            if(!maze.inside(filter.value()))go = false;
        if(filter.value().x < start)go=true;
    };
    
    void draw(){
        
        ofSetColor(255);
        path.draw(0,0);
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
        
        
        ofSetColor(255,100,100);
        ofPushMatrix();
        ofTranslate(filter.value());
        
        ofColor c;
        if(go)c.set(120,240,180);
        else c.set(255,120,120);
        
        int rad = average->getRadius();
        co->avergaTick.draw(-rad,-rad,rad*2,rad*2);
        
        ofPopMatrix();
        
        
    };
    
    
    void begin(){
        go = false;
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying)
                buttons->at(i).setPosition(200, 1040);
        }
    };
    
    void reset(){
       
    };
    
    commonObjects * co;
    ofxSVG svg;
    ofImage path;
    vector<ofPolyline> polys;
    
    vector<Button>*buttons;


    
};
