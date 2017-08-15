#include "ofMain.h"
#include "Scene.h"



class Area : public Scene{
    
public:

    int bigRadius,smallRadius;
    
    Area(){};
    ~Area(){};
    
    ofImage hole;

    vector<bool>isInPlace;

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
                    isInPlace[i] = false;
                }else{
                    isInPlace[i] = true;
                }
            }
        }
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
        
        hole.draw(midt,smallRadius,smallRadius);
        
        for(int i=0; i<buttons->size(); i++) {
            
            ofPushMatrix();
            ofTranslate(buttons->at(i).getBiquadPos());
            ofRotateZ(buttons->at(i).getRotation());
            
            if(isInPlace[i]){
                // halo
                ofDrawCircle(0,0,30,30);
            }
            buttons->at(i).draw(false);
            
            ofPopMatrix();
            
            if(co->debug){
                
                ofNoFill();
                ofSetColor(255,0,0);
                ofDrawCircle(midt, bigRadius);
                ofDrawCircle(midt, smallRadius);
                ofFill();
            }
        }
        
        
    };
    
    void begin(ofxBox2d * world = nullptr){
        
        hole.load("img/specialAssets/02_AreaHole.png");
        
        int numPlayers = 0;
        int radius = 1;
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                radius = buttons->at(i).radius;
                numPlayers ++;
            }
        }
        numPlayers = CLAMP(numPlayers + 5, 0, 36);
        float l = numPlayers * radius * 2;
        float d = l / PI;
  
        d/=2;
        
        bigRadius = d == 0 ? 286 : d;
        
        bigRadius += radius; /// albue rum
        smallRadius = bigRadius - 20;
        isInPlace.resize(36);
    };
    
    void reset(){
        isInPlace.clear();
        hole.clear();
    };


    
    
};
