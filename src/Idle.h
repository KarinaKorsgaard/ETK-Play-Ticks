#include "ofMain.h"
#include "common.h"

class Idle : public commonFunctions{
    
public:
    vector<Button>*buttons;
    Blob blob;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
//        head = transformToCollumn(getLine("text/01.txt",0),800, co->font_medium);
//        bread = transformToCollumn(getLine("text/01.txt",1),800, co->font_small);
        
    };
    
    bool isDone(){
        return true;
    };
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction);
        }
    }
    
    void draw(){
        ofSetColor(255);
        //   svg.draw();
        blob.draw();
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
    
    void begin(){};
    void reset(){};
    
    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    
    vector<string> head;
    vector<string> bread;
    
};
