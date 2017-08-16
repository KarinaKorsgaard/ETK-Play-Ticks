#include "ofMain.h"
#include "Scene.h"
#include "ofxBiquadFilter.h"

class Trail : public Scene{
    
public:
    
    ofPolyline maze;
    int end,start;
    bool go = false;

    ofxBiquadFilter2f filter;
    ofPoint average;
    
    ofxSVG svg;
    ofImage path;

    ofPoint trailStart;
    
    bool sendOsc;
    int oscInt;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;

        
        start = 300 + teamNumber * 1920;
        end = 1920-100 + teamNumber * 1920;
        
        filter.setFc(0.05f);
        
        
        svg.load("svg/03_Trail.svg");
        maze = getPolyline(svg)[0];
        
        int y = 0;
        ofPoint p = ofPoint (start - 150 , y);
        int begin = 0;
        int endBegin = 0;
        while(!maze.inside(p)){
            y+=5;
            p = ofPoint (start - 150 , y);
            endBegin = y;
            
        }
        while(maze.inside(p)){
            y+=5;
            p = ofPoint (start - 150 , y);
            endBegin = y;
            
        }
        trailStart = ofPoint(start - 200, begin + (endBegin - begin)/2 );
        maze.clear();
        
    };
    
    bool isDone(bool b = false){
        bool isInside=false;
        if(go && filter.value().x > end) isInside = true;
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
        
        if(go){
            if(!maze.inside(filter.value())){
                go=false;
                sendOsc = true;
                oscInt = 0;
            }
        }
        
        if(filter.value().x < start && maze.inside(filter.value()))
        {
            if(!go){
                go=true;
                sendOsc = true;
                oscInt = 1;
            }
        }

        if(!go){
            if(co->trailRadius[teamNumber] > 0.0)
                co->trailRadius[teamNumber]*=0.99;
            else co->trailRadius[teamNumber] = 0.;
        }else {
            if(co->trailRadius[teamNumber] < co->maxTrailRadius)
                co->trailRadius[teamNumber]+=0.002;
            else co->trailRadius[teamNumber] = co->maxTrailRadius;
        }
    };
    
    void draw(){
        
       // ofSetColor(255, 200);

       // ofDrawCircle(trailStart,abs( (sin(ofGetElapsedTimef()))+10) * 5.  );
        
        ofSetColor(255);
        ofPushMatrix();
        ofTranslate(filter.value());
        float rad = co->avergaTick.getWidth()/2;
        co->avergaTick.draw(-rad,-rad,rad*2,rad*2);
        ofPopMatrix();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            
            if(co->debug){
                buttons->at(i).drawDebug();
                if(!go) ofSetColor(255,0,0);
                else ofSetColor(0,255,0);
                maze.draw();
                ofDrawLine(300 + 1920*teamNumber,0,300,1080);
            }
        }
        
        

        
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
        if(teamNumber == 0){
            co->avergaTick.load("img/specialAssets/03_Lantern.png");
            
        }
        go = false;
        
        svg.load("svg/03_Trail.svg");
        maze = getPolyline(svg)[0];
        

    };
    
    void reset(){
        if(teamNumber == 0)
            co->avergaTick.clear();
        
        maze.clear();
        
    };


    
};
