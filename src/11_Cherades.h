#include "ofMain.h"
#include "Scene.h"

class Idle : public Scene{
    
public:
    
    bool done = false;
    bool isDouble = true;
    ofRectangle rect;
    
    void setup(commonObjects*_co, vector<Button>*b, bool _isDouble = true, ofRectangle _r =ofRectangle(0,0, 1920,1080)){
        buttons = b;
        co = _co;
        rect = _r;
        isDouble = _isDouble;
    };
    

    bool isDone(bool b = false){
        done = b;
        return b;
    };
    
 
    void update(){
        if(!done){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction, isDouble, rect);
            }
        }
    }
    
    
    void draw(){
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
    }
    
    
    void begin(ofxBox2d * world = nullptr){
        done = false;
    }
    
    
    void reset(){
    }
};
