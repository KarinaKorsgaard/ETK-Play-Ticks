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
        int indx = 0;
        for(int i = 0; i<buttons->size();i++){

            if(buttons->at(i).isPlaying){
                ofPoint p = buttons->at(i).getBiquadPos();
                indx++;
                
                float dist = p.distance(midt);
                if(dist > bigRadius || dist < smallRadius){
                    isInside=false;
                    isInPlace[i] = false;
                }else{
                    isInPlace[i] = true;
                }
            }
        }
        if(indx < 5)isInside = false;
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
        float holesize =2 * smallRadius * 1.193 - 25;
        hole.draw(midt.x - holesize/2, midt.y - holesize/2 ,holesize,holesize);
        
        for(int i=0; i<buttons->size(); i++) {
            

            if(isInPlace[i]){
                // halo
                ofDrawCircle(buttons->at(i).getBiquadPos(),30);
            }
            buttons->at(i).draw();
            
            
            
        }
        if(co->debug){
            
            ofNoFill();
            ofSetColor(255,0,0);
            ofDrawCircle(midt, bigRadius);
            ofDrawCircle(midt, smallRadius);
            ofFill();
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
        numPlayers = MAX(numPlayers ,16);
        numPlayers+=6;
        float l = numPlayers * radius * 2;
        float d = l / PI;
  
        d/=2;
        
        bigRadius = d == 0 ? 286 : d;
        
        bigRadius += radius; /// albue rum
        smallRadius = MAX(bigRadius - 20, 0);
        isInPlace.resize(36);
        
        cout << smallRadius<< endl;
    };
    
    void reset(){
        isInPlace.clear();
        hole.clear();
    };


    
    
};
