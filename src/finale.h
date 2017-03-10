#include "ofMain.h"
#include "common.h"

class Finale : public commonFunctions{
    
public:
    vector<Button>*buttons;

    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
     
        co = _co;
        
        head = transformToCollumn(getLine("text/finale.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/finale.txt",1),800, co->font_small);
    };
    
    bool isDone(){
        int alive=0;

        for(int i = 0; i<buttons->size();i++){
            if(!buttons->at(i).isDead())alive++;
        }
        return alive == 1;
    };
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).update(co->attraction);
        }
        
    }
    int getTeamOne(){
        int v = 0;
        for(int i=0; i<buttons->size() / 2; i++) {
            if(!buttons->at(i).isDead())v++;
        }
        return v;
    }
    int getTeamTwo(){
        int v = 0;
        for(int i=buttons->size() / 2; i<buttons->size(); i++) {
            if(!buttons->at(i).isDead())v++;
        }
        return v;
    }
    
    void draw(){
        
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            
            if(!buttons->at(i).isDead() && co->debug){
                ofPushMatrix();
                ofTranslate(buttons->at(i).getBiquadPos());
                string s = ofToString(buttons->at(i).value,0);
                ofSetColor(255);
                ofRectangle r = co->font_medium->getStringBoundingBox(s, 0, 0);
                co->font_medium->drawString(s,-r.width/2,0);
                
                ofPopMatrix();

                buttons->at(i).drawDebug();
            }
        }
    };
    
    void begin(){
    }
    
    void reset(){
    };
    
    commonObjects * co;
    
    vector<string> head;
    vector<string> bread;
    
};
