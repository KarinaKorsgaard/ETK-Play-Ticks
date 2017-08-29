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
    int gameOverPoint = 5;
    bool ballTimerSet = true;
    float ballTimer = 0;
    
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

            if(ballx < 100){
                co->tennisPoint[teamNumber] ++;
                
                ofxOscMessage m;
                m.setAddress("/tennisPoint"+ofToString(teamNumber*-1+2));
                co->oscOut.sendMessage(m);
                
                ball->setPosition(1920, 1080/2);
                ball->setVelocity(0, 0);
                ballTimer = ofGetElapsedTimef();
                ballTimerSet = false;
            }
            if(!ballTimerSet && ofGetElapsedTimef() - ballTimer > 3 ){
                int xvel = teamNumber == 1 ? 3 : -3;
                ball->setVelocity(xvel, 2);
                ballTimerSet=true;
            }
            
            
            int front = teamNumber == 0 ? -1 : 1;
            pad->setPosition( tennisCourt.x + teamNumber * tennisCourt.width, buttons->at(winButton).getBiquadPos().y);
            
            buttons->at(winButton).box2Dcircle->setPosition(
                                               buttons->at(winButton).getRawData().x*tennisCourt.width + tennisCourt.x,
                                               buttons->at(winButton).getRawData().y * tennisCourt.height + tennisCourt.y);
            buttons->at(winButton).filterLowPass.update(buttons->at(winButton).getPos());
            buttons->at(winButton).setDirection(buttons->at(winButton).filterLowPass.value(), buttons->at(winButton).lastPos);
            buttons->at(winButton).lastPos = buttons->at(winButton).filterLowPass.value();
            //update(co->attraction, false , tennisCourt);
        

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
        
        if(co->debug){
            pad->draw();
            buttons->at(winButton).drawDebug();
        }
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
            buttons->at(winButton).isWinner = true;
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
        //pad->setPosition(buttons->at(winButton).getBiquadPos().x + 50 * front , buttons->at(winButton).getBiquadPos().y);
        pad->setPosition( tennisCourt.x + teamNumber * tennisCourt.width, buttons->at(winButton).getBiquadPos().y);
        pad.get()->setData(new RacketData());
        RacketData * sd = (RacketData*)pad.get()->getData();
        sd->teamID = teamNumber;

        if(teamNumber == 1){
            ofxSVG svg;
            svg.load("svg/09_Fight.svg");
            solidPolys = getPolyline(svg);
            cout << "FIGHT POLYS"<<solidPolys.size()<<endl;
        }
        
    }
    
    void reset(){
        ballTimerSet = true;
        ballTimer = 0;

        
        pad->destroy();
        solidPolys.clear();
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }

    }    
};
