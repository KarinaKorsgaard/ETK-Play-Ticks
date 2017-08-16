#include "ofMain.h"
#include "Scene.h"



class Fight : public Scene{
    
public:
    
    ofPolyline winArea;
    
    Fight(){};
    ~Fight(){};
    
    ofPolyline poly;
    
    int winButton;
    shared_ptr<ofxBox2dRect>pad;
    shared_ptr<ofxBox2dCircle>ball;
    
    int point ;
    int gameOverPoint = 20;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
   
    }
    
    bool isDone(bool b = false){
        int t = teamNumber * -1 +1;
        return co->tennisPoint[t] > gameOverPoint;
    }
    
    void update(){
        
        
        if(!isDone()){
            // translate ball pos
            int ballx = ball->getPosition().x;
            ballx = teamNumber == 0 ? ballx : ofMap(ballx , 1920, 1920*2 , 1920, 0);

            if(ballx < ball->getRadius() * 2){
                co->tennisPoint[teamNumber] ++;
                
                ball->setPosition(1920, 1080/2);
                int xvel = teamNumber == 1 ? -20 : 20;
                ball->setVelocity(xvel, 20);
            }
            
            buttons->at(winButton).update(co->attraction);
        }
        pad->setVelocity(0,0);
        pad->addAttractionPoint(buttons->at(winButton).getBiquadPos(),100);
        pad->setRotation(buttons->at(winButton).getRawData().z + PI/2);
        
    }
    
    void draw(){
        ofSetColor(255);
        buttons->at(winButton).draw();
        pad->draw();
        
        //lifebar
        ofNoFill();
        ofDrawRectangle(20 + 1920*teamNumber , 1080-40, 1880 , 20);
        ofFill();
        int left = gameOverPoint - co->tennisPoint[teamNumber];
        
        ofDrawRectangle(20 + 1920*teamNumber , 1080-40 , left * ( 1880  / gameOverPoint ) , 20);
        
        if(co->debug)
            buttons->at(winButton).drawDebug();
    }
    
    void begin(ofxBox2d * world = nullptr){
        
        co->tennisPoint[teamNumber] = 0;
        cout << "set button" << endl;
        int firstOn = -1;
        bool isSet = false;
        for(int i=0; i<buttons->size(); i++) {
            if(buttons->at(i).isPlaying && firstOn == -1)
                firstOn = i;
            if(buttons->at(i).isWinner){
                winButton = i;
                isSet = true;
            }
        }
        if(!isSet){
            firstOn = firstOn == -1 ? 0 : firstOn;
            winButton = firstOn;
            cout << "winbutton " << firstOn << endl;
        }
        
        pad = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
        pad->setPhysics(1., 10., 1.);
        pad->setup(world->getWorld(), 1920, 1080/2, 30, 150);
    }
    
    void reset(){
        pad->destroy();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }
        
    }    
};
