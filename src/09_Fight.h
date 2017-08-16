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
    
    ofImage padImg;
    
    int point ;
    int gameOverPoint = 20;
    
    ofRectangle tennisCourt;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    }
    
    bool isDone(bool b = false){
        int t = teamNumber * -1 +1;
        return co->tennisPoint[t] >= gameOverPoint;
    }
    
    void update(){

        
        if(!isDone() && co->startScene){
            // translate ball pos
            int ballx = ball->getPosition().x;
            ballx = teamNumber == 0 ? ballx : ofMap(ballx , 1920, 1920*2 , 1920, 0);

            if(ballx < 880){
                co->tennisPoint[teamNumber] ++;
                
                ball->setPosition(1920, 1080/2);
                int xvel = teamNumber == 1 ? -5 : 5;
                ball->setVelocity(xvel, 5);
            }
            
            
            buttons->at(winButton).update(co->attraction, false , tennisCourt);
            
            int front = teamNumber == 0 ? 1 : -1;
            pad->setPosition(buttons->at(winButton).getBiquadPos().x + 50 * front , buttons->at(winButton).getBiquadPos().y);

        }
        

    }
    
    void draw(){
        ofSetColor(255);
        

        buttons->at(winButton).draw();

        ofPushMatrix();
        
            int x = pad->getPosition().x;
            int y = pad->getPosition().y;
            int w = pad->getWidth();
            int h = pad->getHeight();
        
        ofTranslate(x,y);
        ofRotateZ(teamNumber * 180);

        padImg.draw(-w/2,-h/2,w,h);
        ofPopMatrix();
        
        //lifebar
        ofNoFill();
        ofDrawRectangle(20 + 1920*teamNumber , 1080-40, 1880 , 20);
        ofFill();
        int left = gameOverPoint - co->tennisPoint[teamNumber];
        
        ofDrawRectangle(20 + 1920*teamNumber , 1080-40 ,MAX( left * ( 1880  / gameOverPoint ) , 0), 20);
        
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
        
        padImg.load("img/specialAssets/08_FightPad.png");
        
        pad = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
        pad->setPhysics(0.,0.,0.);
        pad->setup(world->getWorld(), 1920, 1080/2, padImg.getWidth(), padImg.getHeight());
        pad->setFixedRotation(true);
       
        
        if(teamNumber == 0)tennisCourt = ofRectangle( 880 , 120 , 2080 / 2 , 840 );
        if(teamNumber == 1)tennisCourt = ofRectangle( 1920 , 120 , 2080 / 2 , 840 );
        
        buttons->at(winButton).setPosition(tennisCourt.getCenter());
        int front = teamNumber == 0 ? 1 : -1;
        pad->setPosition(buttons->at(winButton).getBiquadPos().x + 50 * front , buttons->at(winButton).getBiquadPos().y);
        
    }
    
    void reset(){
        pad->destroy();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }

    }    
};
