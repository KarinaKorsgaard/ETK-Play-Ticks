#include "ofMain.h"
#include "Scene.h"



class Representative : public Scene{
    
public:

    int bigRadius,smallRadius;
    
    int bigRadiusWin,smallRadiusWin;
    
    Representative(){};
    ~Representative(){};
    
    ofVec2f midt;

    bool done = false;
    
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        midt = ofVec2f(1920 / 2 + (1920 * teamNumber) , 1080 / 2);

    };
    
    bool isDone(bool b = false){
        return done;
    }
    
    void updateIsDone(){
        
        bool isInside = true;
        int ousideCount = 0;
        vector<int> checkThisOne;
        vector<int> checkThisOneDist;
        
        for(int i = 0; i<buttons->size();i++){
            
            if(buttons->at(i).isPlaying){
                ofPoint p = buttons->at(i).getBiquadPos();
                
                ofVec2f a = p - midt;
                float dist = a.dot(a);
                
                if(dist > bigRadius || dist < smallRadius){
                    checkThisOne.push_back(i);
                    checkThisOneDist.push_back(dist);
                    ousideCount ++;
                }
                
                
                if(ousideCount > 6){
                    isInside = false;
                    done = false;
                    break;
                }
            }
        }
        
        done = checkThisOne.size() == 6;
        // << checkThisOne.size() << endl;
        if(done){
            for(int i = 0; i<checkThisOne.size();i++){
                if(isInside){
                    float dist = checkThisOneDist[i];
                    
                    if(dist < bigRadiusWin && dist > smallRadiusWin){
                        buttons->at(checkThisOne[i]).isWinner = true;
                    }else
                        done = false;
                }
            }
        }
    };
    
    void update(){
        updateIsDone();
        
        if(!done){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
    }
    
    void draw(){
        ofSetColor(255);

        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            
            if(co->debug){
                buttons->at(i).drawDebug();
                
                ofSetColor(255,0,0);
                ofNoFill();
                //winArea.draw();
                ofDrawCircle(midt, bigRadius);
                ofDrawCircle(midt, smallRadius);
                
                ofDrawCircle(midt, bigRadiusWin);
                ofDrawCircle(midt, smallRadiusWin);
                ofFill();
            }
        }
    };
    
    void begin(ofxBox2d * world = nullptr){
        int radius = 1;
        int numPlayers = 0;
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
            
            if(buttons->at(i).isPlaying){
                radius = buttons->at(i).beginningRad;
                numPlayers ++;
            }
        }
        
        float l = numPlayers * radius * 2;
        float d = l / PI;
        d/=2;
        cout << d << endl;
        bigRadius = d == 0 ? 286 : d;
        
        bigRadius += radius;
        smallRadius = bigRadius - 20;
    
        
        // big small win radius
        l = 6 * radius * 2;
        d = l / PI;
        d/=2;
        bigRadiusWin = d;
        bigRadiusWin += radius;
        smallRadiusWin = bigRadiusWin-20;
        
    };
    
    void reset(){
        done = false;
    };

    
    
};
