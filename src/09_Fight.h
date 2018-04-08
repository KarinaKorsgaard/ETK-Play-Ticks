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
    
    ofRectangle pad_normal;

    ofImage padImg;
    
    int point ;
    int gameOverPoint = 5;
    bool ballTimerSet = true;
    float ballTimer = 0;
    
    ofRectangle tennisCourt;
    ofVec2f velocity_pad;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        pad_normal = ofRectangle(100,100,34,363);
    }
    
    bool isDone(bool b = false){
        int points = MAX(co->tennisPoint[0],co->tennisPoint[1]);
        return points >= gameOverPoint;
    }
    
    void update(){

        
        if(!isDone() && co->startScene){

            
            int front = teamNumber == 0 ? -1 : 1;
            velocity_pad = ofVec2f(pad_normal.getCenter());
            pad_normal.setFromCenter( ofPoint(tennisCourt.x + teamNumber * tennisCourt.width, buttons->at(winButton).getBiquadPos().y), pad_normal.width, pad_normal.height);
            velocity_pad-= ofVec2f(pad_normal.getCenter());
        
            buttons->at(winButton).update(co->attraction*1000);

        }

    }
    
    void draw(){
        ofSetColor(255);
        

        buttons->at(winButton).draw();
        
        ofPushMatrix();

        int x = pad_normal.x;
        int y = pad_normal.y;
        int w = pad_normal.width;
        int h = pad_normal.height;
        
        ofTranslate(x,y);
        //ofRotateZ(teamNumber * 180);

        padImg.draw(0,0,w,h);
        ofPopMatrix();

        if(co->debug){
            ofDrawRectangle(pad_normal);
            buttons->at(winButton).drawDebug();
        }
    }
    
    void begin(ofxBox2d * world = nullptr){
        
        co->tennisPoint[teamNumber] = 0;
       // cout << "set button" << endl;
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
            buttons->at(winButton).isWinner = true;
            cout << "winbutton " << firstOn << endl;
        }
        
        padImg.load("img/specialAssets/08_FightPad.png");

        if(teamNumber == 0)tennisCourt = ofRectangle( 880 , 120 , 2080 / 2 , 840 );
        if(teamNumber == 1)tennisCourt = ofRectangle( 1920 , 120 , 2080 / 2 , 840 );
//
        buttons->at(winButton).setPosition(tennisCourt.getCenter());
        int front = teamNumber == 0 ? 1 : -1;

        
    }
    
    void reset(){
        ballTimerSet = true;
        ballTimer = 0;

        
       // pad->destroy();
       // solidPolys.clear();
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }

    }    
};
