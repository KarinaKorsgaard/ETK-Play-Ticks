#include "ofMain.h"
#include "common.h"

class Idle : public commonFunctions{
    
public:
    vector<Button>*buttons;
    //Blob blob;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
 
    };
    
    bool isDone(bool forceTrue){
        done = forceTrue;
        return forceTrue;
    };
    
    void update(bool updateAnyWay){
        if(!done || updateAnyWay){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
    }
    
    void draw(){
        ofSetColor(255);
        //   svg.draw();
        //blob.draw();
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
    };
    
    void begin(){
        done = false;
    };
    void reset(){};
    
    commonObjects * co;
    bool done = false;
    
};
