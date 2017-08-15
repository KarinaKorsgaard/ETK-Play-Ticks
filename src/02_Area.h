#include "ofMain.h"
#include "Scene.h"



class Area : public Scene{
    
public:

    int bigRadius,smallRadius;
    
    Area(){};
    ~Area(){};
    

    ofxSVG svg;
    ofPolyline poly;


    ofVec2f midt;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        midt = ofVec2f(1920 / 2 + (1920 * teamNumber) , 1080 / 2);
    };
    
    bool isDone(bool b = false){
        bool isInside=true;

        for(int i = 0; i<buttons->size();i++){

            if(buttons->at(i).isPlaying){
                ofPoint p = buttons->at(i).getBiquadPos();
                
                
                ofVec2f a = p - midt;
                float dist = a.dot(a);
                              
                if(dist > bigRadius || dist < smallRadius){
                    isInside=false;
                    break;
                }
//                if(!big.inside(p) || small.inside(p)){
//                    isInside=false;
//                    break;
//                }
            }
//            if(!buttons->at(i).isDead()){
//                allAreDead =false;
//            }
            
        }
      //  if(allAreDead)isInside = false;
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
      //  donut.draw(0,0);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofNoFill();
                ofSetColor(255,0,0);
                ofDrawCircle(midt, bigRadius);
                ofDrawCircle(midt, smallRadius);
                ofFill();
            }
        }
    };
    
    void begin(ofxBox2d * world = nullptr){
        
        int numPlayers = 0;
        int radius = 1;
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                radius = buttons->at(i).radius;
                numPlayers ++;
            }
        }
        
        float l = numPlayers * radius * 2;
        float d = l / PI;
  
        d/=2;
        
        bigRadius = d == 0 ? 286 : d;
        
        bigRadius += radius; /// albue rum
        smallRadius = bigRadius - 20;
    };
    
    void reset(){
    
    };


    
    
};
