#include "ofMain.h"
#include "common.h"
#include "ofxBox2d.h"


class PingPong : public commonFunctions{
    
public:
    vector<Button>*team1;
    vector<Button>*team2;
    
    shared_ptr<ofxBox2dRect> pad1;
    shared_ptr<ofxBox2dRect> pad2;
    shared_ptr<ofxBox2dCircle> ball;
    
    shared_ptr<ofxBox2dRect> wall;
    
    int score1;
    int score2;
    
    ofxBox2d world;
    
    void setup(commonObjects*_co, vector<Button>*b1 , vector<Button>*b2 ){
        team1 = b1;
        team2 = b2;
        
        co = _co;
        
        world.init();
        world.setGravity(0, 1);
        world.createBounds(ofRectangle(0,-100,1920*2, 1080 + 800));
        
        //box2d.enableEvents();
 
    };
    
    bool isDone(){
        return true;
    };
    
    void update(){
        
        float x = ball->getPosition().x;
        float y = ball->getPosition().y;
        if(y > 1080 + 600){
            if(x>1920){
                score1++;
                ball->setPosition( ofVec2f( 1920 , 200 ));
                ball->setVelocity( ofVec2f( ofRandom(-45,45) , -10 ));
            }
            else if(x<1920){
                score2++;
                ball->setPosition( ofVec2f( 1920 , 200 ));
                ball->setVelocity( ofVec2f( ofRandom(-45,45) , -10 ));
            }
        }
        
        
        if(abs(ball->getVelocity().x) < 15 )ball->setVelocity( ofVec2f( ball->getVelocity().x*1.6 , ball->getVelocity().y  ));
        

        float pos1 = 0;
        float indx1=0;
        float pos2 = 0;
        float indx2 = 0;
        
        for(int i=0; i<team1->size(); i++) {
            if(team1->at(i).on && team1->at(i).isPlaying){
                team1->at(i).update(co->attraction);
                pos1+=team1->at(i).getBiquadPos().x;
                indx1++;
            }
        }
        for(int i=0; i<team2->size(); i++) {
            if(team2->at(i).on && team2->at(i).isPlaying){
                team2->at(i).update(co->attraction);
                pos2+=team2->at(i).getBiquadPos().x;
                indx2++;
            //if(i==0)cout<< team1->at(i).getBiquadPos().y << endl;
            }
        }
       // pad1->setPosition(pad1->getPosition().x , pos1/indx1);
       // pad2->setPosition(pad2->getPosition().x , pos2/indx2);
        
        if(indx1>0)pad1->setPosition(pos1/indx1 , wall->getPosition().y);
        if(indx2>0)pad2->setPosition(pos2/indx2 + 1920, wall->getPosition().y);
        
        pad1->setRotation(-80);
        pad2->setRotation(80);
        
        world.update();
    }
    
    void draw(){
        ofSetColor(255);
        for(int i=0; i<team1->size(); i++) {
            team1->at(i).draw();
            if(co->debug){
                team1->at(i).drawDebug();
            }
        }
        ofPushMatrix();
        ofTranslate(1920,0);
        for(int i=0; i<team2->size(); i++) {
            team2->at(i).draw();
            if(co->debug){
                team2->at(i).drawDebug();
            }
        }
        ofPopMatrix();
        ofSetColor(10,10,60);
        pad1->draw();
        pad2->draw();
        ball->draw();
        wall->draw();
        int w = co->font_medium->getStringBoundingBox("Team 1: "+ofToString(10), 0, 0).width;
        co->font_medium->drawString("Team 1: "+ofToString(score1),50,80);
        co->font_medium->drawString("Team 2: "+ofToString(score2),1920*2-(w+50),80);
    };
    
    void begin(){
        
        ball = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //virtual void setPhysics(float density, float bounce, float friction);
        ball.get()->setPhysics(.1, .5, 8.0);
        ball.get()->setup(world.getWorld(), 1920, 1080/2, 70);
        ball.get()->alive = false;
        
        pad1= shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
        pad2= shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
        wall= shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
        
        pad1.get()->setPhysics(0.0, 0.53, 0.1);
        pad1.get()->setup(world.getWorld(), 100, 1080/2, 40, 300);
        pad1.get()->alive = false;
        
        pad2.get()->setPhysics(0.0, 0.53, 0.1);
        pad2.get()->setup(world.getWorld(), 1920*2 - 100, 1080/2, 40, 300);
        pad2.get()->alive = false;
        
        wall.get()->setPhysics(0.0, 0.53, 0.1);
        wall.get()->setup(world.getWorld(), 1920 , 800 , 80 , 1080-500 );
        wall.get()->alive = false;
        
        
        ball.get()->alive = true;
        ball->setVelocity( ofVec2f( ofRandom(-45,45) , 0 ));
        pad1.get()->alive = true;
        pad2.get()->alive = true;
        wall.get()->alive = true;
        
        score1=0;
        score2=0;
        
    };
    void reset(){
        ball.get()->destroy();
        pad1.get()->destroy();
        pad2.get()->destroy();
        wall.get()->destroy();
    };
    
    commonObjects * co;

    
};
