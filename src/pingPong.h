#include "ofMain.h"
#include "common.h"



class PingPong : public commonFunctions{
    
public:
    vector<Button>*buttons;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
 
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
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
    };
    
    void begin(){};
    void reset(){};
    
    commonObjects * co;

    
};
