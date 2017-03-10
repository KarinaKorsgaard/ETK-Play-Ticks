// gravity!!

#include "ofMain.h"
#include "common.h"

class Scene05 : public commonFunctions{
    
public:
    vector<Button>*buttons;
    ofRectangle winningArea;
    int theWinner;
    double isDoneCounter;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        winningArea = ofRectangle(1920/2,100,100,100);
//        svg.load("svg/s05.svg");
//        poly = getPolyline(svg)[0];
        
        svg.load("svg/s05.svg");
        polys = getPolyline(svg);
        theWinner=-1;
        
        head = transformToCollumn(getLine("text/05.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/05.txt",1),800, co->font_small);
        
        
    };
    
    bool reached(){
        bool isInside=false;
        for(int i = 0; i<buttons->size();i++){
            Button *b =&buttons->at(i);
            if(!b->isPlaying || !b->on)continue; // if b is not playing or not on, dont check.
            if(ofDist(winningArea.x,winningArea.y,b->getBiquadPos().x,b->getBiquadPos().y) < 100+b->radius){
                isInside = true;
                theWinner = i;
                break;
            }
        }
        return isInside;
    };
    
    bool isDone(){
        if(reached())isDoneCounter+=ofGetLastFrameTime();
        else isDoneCounter=0;
        
        return isDoneCounter>5.f;
    }
    
    void update(){

        if(!isDone()){ // freeze when has reached...
            for(int i=0; i<buttons->size(); i++) {
                
                buttons->at(i).updateWithGravity(co->attraction);
            }
        }
    }
    
    void draw(){
        
        ofSetColor(255);
        svg.draw();
        int red = reached() ? 0:255;
        ofSetColor(red,0,255-red);
        ofDrawRectangle(winningArea);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
            if(theWinner!=-1)ofDrawCircle(buttons->at(theWinner).getBiquadPos(),20);
        }
        
//        ofSetColor(ofColor::royalBlue);
//        for(int i = 0; i< head.size();i++)
//            co->font_medium->drawString(head[i], 50, 100+i*co->font_medium->getLineHeight());
//        
//        for(int i = 0; i< bread.size();i++)
//            co->font_small->drawString(bread[i], 50, 180+i*co->font_small->getLineHeight());
    };
    
    void begin(){
        theWinner=-1;
        for(int i = 0; i<buttons->size();i++){
            buttons->at(i).box2Dcircle->setPosition(buttons->at(i).getPosRaw().x, 1040);
        }
    };
    void reset(){
        isDoneCounter=0.;
    }

    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    vector<string> head;
    vector<string> bread;
    
};
