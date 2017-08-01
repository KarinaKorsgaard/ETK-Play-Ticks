#include "ofMain.h"
#include "Scene.h"

class Ground : public Scene{
    
public:
    vector<Button>*buttons;
//    commonObjects * co;
//    bool done = false;

    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
//        co = _co;
    };
    
    
    bool isDone(bool b = false){

    };
    
    
    void update(){
//        isDone();
//        
//        if(!done){
//            for(int i=0; i<buttons->size(); i++) {
//                buttons->at(i).update(co->attraction, false);
//            }
//        }
    }
    
    
    void draw(){
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }
    }
    
    
    void begin(ofxBox2d * world = nullptr){
      //  done = false;
    }
    
    
    void reset(){
    }
};
