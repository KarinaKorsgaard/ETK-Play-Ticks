#include "ofMain.h"
#include "common.h"


class Scene03 : public commonFunctions{
    
public:

    shared_ptr<ofxBox2dCircle> average;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/s03.svg");
        polys = getPolyline(svg);
        
        winningArea = ofRectangle(0,1080/2-161/2,267,161);
        
        head = transformToCollumn(getLine("text/03.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/03.txt",1),800, co->font_small);
  
    };
    bool isDone(){
        bool isInside=true;
        
        isInside = winningArea.inside(average->getPosition());
       
        if(isInside){ // all playing and alive buttons must be on for the average to count!
            co->log("average is home, but all buttons are not on!");
            for(int i = 0; i<buttons->size();i++){
              //  if(!buttons->at(i).isPlaying || buttons->at(i).isDead())continue;
                
                if(!buttons->at(i).on && buttons->at(i).isPlaying){
                    isInside=false;
                    break;
                }
            }
        }
        
        
        return isInside;
    };
    
    
    void update(){
        float x = 0.0;
        float y = 0.0;
        float indx = 0.0;
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction); // the is on is checked in button! 
            
            if(!buttons->at(i).isDead() && buttons->at(i).on){ // its on, not dead. (if it is not playing, it can not be on)
                x+=buttons->at(i).getBiquadPos().x;
                y+=buttons->at(i).getBiquadPos().y;
                indx++;
            }
        }
        
        average->setVelocity(0,0);
        if(indx>0)average->addAttractionPoint(ofPoint(x/indx , y/indx),co->attraction);
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
        ofSetColor(255,100);
        ofDrawRectangle(winningArea);
        
//        ofSetColor(ofColor::royalBlue);
//        for(int i = 0; i< head.size();i++)
//            co->font_medium->drawString(head[i], 50, 100+i*co->font_medium->getLineHeight());
//        
//        for(int i = 0; i< bread.size();i++)
//            co->font_small->drawString(bread[i], 50, 180+i*co->font_small->getLineHeight());
       
        
        ofSetColor(255,0,0);
        ofPushMatrix();
        ofTranslate(average->getPosition());
        //ofRotateZ(average->getRotation());
        //average->draw();
        int rad = average->getRadius();
        ofSetColor(100);
        ofDrawCircle(0,0,rad);
        
        //head
        ofSetColor(180);
        float eye = -rad + rad*0.45/2;
        ofDrawEllipse(0,eye, rad*0.85,rad*0.45);
        
        //eyes
        if(ofRandom(1000)>0.1){
            ofSetColor(0);
            ofDrawCircle(rad*0.3 ,eye, 10);
            ofDrawCircle(-rad*0.3,eye ,10);
        }
        //stomach
        ofSetColor(255);
        ofDrawEllipse(0,rad*0.35,rad*1.5,rad*1.4);
        ofPopMatrix();
        
        

    };

    
    void begin(){
        for(int i = 0; i<buttons->size();i++){
            buttons->at(i).box2Dcircle->setPosition(1900, 1040);
        }
    };
    void reset(){}
    
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
