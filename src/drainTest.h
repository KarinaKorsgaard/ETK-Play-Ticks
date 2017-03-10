#include "ofMain.h"
#include "common.h"

class DrainTest : public commonFunctions{
    
public:
    vector<Button>*buttons;
    vector<float> orginalValue;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        head = transformToCollumn(getLine("text/drainTest.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/drainTest.txt",1),800, co->font_small);
        
        
    };
    bool isDone(){
        return true;
    };
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction);
            
            if(buttons->at(i).isColliding() && !buttons->at(i).isDead()){
                buttons->at(i).value -= co->spyDrainer;
            }
        }
        
    }
    
    void draw(){
        
        ofSetColor(255);
        //svg.draw();
        
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
        orginalValue.clear();
        for(int i = 0;i<buttons->size();i++){
            orginalValue.push_back(buttons->at(i).value);
        }
    }
    
    void reset(){
        for(int i = 0;i<buttons->size();i++){
            buttons->at(i).value = orginalValue[i];
        }
    };
    
    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    
    vector<string> head;
    vector<string> bread;
    
};
