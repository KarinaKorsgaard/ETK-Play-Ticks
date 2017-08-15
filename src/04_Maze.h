#include "ofMain.h"
#include "Scene.h"

class Maze : public Scene{
    
public:
    

    float upThres;
    bool done = false;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;

    };
    
    bool isDone(bool b = false){
        bool isInside=true;
        bool allAreDead = true;
        
        for(int i = 0; i<buttons->size();i++){
            Button *b = &buttons->at(i);
            if(!buttons->at(i).isDead()){
                allAreDead =false;
            }
            if(b->isPlaying /*&& !b->isDead()*/){ // if b is not playing or is dead dont account.
                
                ofPoint p =b->getBiquadPos();
                if(p.y<upThres){
                    isInside=false;
                }
            }
            
        }
    //    if(allAreDead)isInside=false;
        return isInside;
    };
    
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
    }
    
    
    void draw(){
        
        ofSetColor(255);
      //  svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetLineWidth(4);
                ofDrawLine(0,solidPolys[0].getBoundingBox().y,1920,solidPolys[0].getBoundingBox().y);
                
                ofSetColor(255);
                ofSetLineWidth(40);
                for(int i = 0 ; i<solidPolys.size() ; i++)solidPolys[i].draw();
            }
        }
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                float addX = teamNumber == 0 ? 0 : 1920;
                
                if(buttons->at(i).getPosRaw().x>0)buttons->at(i).setPosition(buttons->at(i).getPosRaw().x + addX, 50);
                else buttons->at(i).setPosition(ofRandom(100,1920-100)+ addX, 50);
            }
        }
        done = false;
        
        ofxSVG svg;
        svg.load("svg/04_Maze.svg");
        solidPolys = getPolyline(svg);
        
        float max = 0;
        for(int i = 0; i<solidPolys.size();i++){
            float x = solidPolys[i].getBoundingBox().y+solidPolys[i].getBoundingBox().height;
            if(x<1060 && x>max )max = x;
        }
        upThres = max;
        
        
    };
    void reset(){
        solidPolys.clear();
    };
    

};