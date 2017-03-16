#include "ofMain.h"
#include "common.h"

class DesignACharacter : public commonFunctions{
    
public:
    vector<Button>*buttons;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
  
    };
    bool isDone(){
        // do not drain here...
        return true;
    };
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            ofVec3f data = buttons->at(i).getRawData();
            buttons->at(i).colors[0].setHsb(ofMap(data.x,0,1,0,255),200,255);
            buttons->at(i).colors[1].setHsb(ofMap(data.y,0,1,0,255),200,255);
            buttons->at(i).colors[2].setHsb(ofMap(data.z,0,2*PI,0,255),200,255);
            
        }
    }
    
    void draw(){
        
        for(int i = 0 ; i<buttons->size();i++) {
            
            Button b = buttons->at(i);
            if(b.isPlaying){
                ofPushMatrix();
                ofTranslate(b.getGridPos(   b.table - (b.table%2) * b.teamNumber , b.ID )   );
                ofTranslate(800,0);
                b.draw(false, false, true);
                ofPopMatrix();
            }
        }

    };
    
    void begin(){
        
    };
    void reset(){
        
    };
    
    commonObjects * co;

    
};
