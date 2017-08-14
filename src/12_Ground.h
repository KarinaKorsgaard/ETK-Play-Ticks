#include "ofMain.h"
#include "Scene.h"

class Ground : public Scene{
    
public:

    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
    };
    
    
    bool isDone(bool b = false){

    };
    
    
    void update(){

    }
    
    
    void draw(){
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }
    }
    
    
    void begin(ofxBox2d * world = nullptr){

    }
    
    
    void reset(){
    }
};
