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
        
        ofxSVG svg2;
        svg2.load("svg/s05_winningArea.svg");
        winningArea = getPolyline(svg2)[0].getBoundingBox();
        
        svg.load("svg/s05.svg");
        polys = getPolyline(svg);
        theWinner=-1;
        
        head = transformToCollumn(getLine("text/05.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/05.txt",1),800, co->font_small);
        
        
    };
    
    bool reached(){
        bool isInside=false;
        bool allAreDead = true;
        for(int i = 0; i<buttons->size();i++){
            Button *b =&buttons->at(i);
            if(!b->isPlaying || !b->on)continue; // if b is not playing or not on, dont check.
            if(winningArea.inside(b->getBiquadPos())){
                isInside = true;
                theWinner = i;
                break;
            }
            if(b->isDead())allAreDead=false;
        }
        if(allAreDead)isInside = false;
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
                
                buttons->at(i).updateWithGravity(co->jump, co->x_jump);
            }
        }
    }
    
    void draw(){
        
        ofSetColor(255);
        svg.draw();
        if(co->debug){
            int red = reached() ? 0:255;
            ofSetColor(red,0,255-red);
            ofDrawRectangle(winningArea);
        }
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetColor(255,255,0);
               // ofDrawCircle(buttons->at(i).getPos().x, buttons->at(i).lowestY, 30);
                if(theWinner!=-1)ofDrawCircle(buttons->at(theWinner).getBiquadPos(),20);
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
        theWinner=-1;
        for(int i = 0; i<buttons->size();i++){
            if(!buttons->at(i).isPlaying)continue;
            buttons->at(i).box2Dcircle->setPosition(ofRandom(60,400), 1040);
         //   buttons->at(i).lowestY = 1080;
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
