#include "ofMain.h"
#include "common.h"

class Scene02 : public commonFunctions{
    
public:
    
    vector<Button>*buttons;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/s02.svg");
        polys = getPolyline(svg);
        
        cout << polys.size()<<endl;
        head = transformToCollumn(getLine("text/02.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/02.txt",1),800, co->font_small);
        
        
    };
    bool isDone(){
        bool isInside=true;
        
        for(int i = 0; i<buttons->size();i++){
            Button *b = &buttons->at(i);
            if(!b->isPlaying||b->isDead())continue; // if b is not playing or is dead dont account.
            
            if(!b->on || b->getBiquadPos().y + b->radius > polys[0].getBoundingBox().y){
                isInside=false; // if 1 b is not on or is under the line, is false and break loop
                break;
            }
        }
        return isInside;
    };
    
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction);
        }
    }

    
    void draw(){
        
        ofSetColor(255);
        svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetLineWidth(4);
                ofDrawLine(0,polys[0].getBoundingBox().y,1920,polys[0].getBoundingBox().y);
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
        for(int i = 0; i<buttons->size();i++){
            if(!buttons->at(i).isPlaying)continue;
            buttons->at(i).box2Dcircle->setPosition(buttons->at(i).getPosRaw().x, 1040);
        }
        done = false;
    };
    void reset();
    
    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    bool done = false;
    vector<string> head;
    vector<string> bread;
    
};
