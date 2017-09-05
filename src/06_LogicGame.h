#include "ofMain.h"
#include "Scene.h"


class circle{
public:
    
   // Button * b;
   // ofPath p;
    float targetRotation;
    bool isTarget = false;
 
    float p_rotation;
    float cur_rotation;
    float startRot;
    ofImage ring;
    
    void setup(float start, float target){
 
        targetRotation = target;
        cur_rotation = start;
        p_rotation = start;
        startRot = start;
        
    }
    void setTarget(float t){
        targetRotation = t;
    }
    
    float update(float precision, float attraction, float z){
        //if (b != nullptr)b->update(2000);
        isTarget = abs(cur_rotation-targetRotation) < precision;
        //float
       
        float dr = ofRadToDeg(z) - p_rotation;

        if(abs(dr) < 355)cur_rotation += dr ;
        if(cur_rotation > 360.f)cur_rotation = 0.f;
        if(cur_rotation < 0.f)cur_rotation = 360.f;
        
        p_rotation = ofRadToDeg(z);
        
        return dr;
    }
    
    void draw(ofVec2f midt){
        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(cur_rotation);
        int s = ring.getWidth();
        ring.draw(-s/2,-s/2,s,s);
        //p.draw();
        ofPopMatrix();
        
        //if (b != nullptr) b->draw();
    }
    void drawTargets(ofVec2f midt){

        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(targetRotation);
        int s = ring.getWidth();
        ring.draw(-s/2,-s/2,s,s);
        ofPopMatrix();

    }
};

class Logic : public Scene{
    
public:

    Logic(){};
    ~Logic(){};

    vector<circle> circles;
    vector<int>winnerButtons;
    float collectedRotation = 0.f;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        for(int i = 0; i < 6 ; i++){
            circle c = *new circle;
            c.setup(ofRandom(360), co->targetCircleRot[i]);
            circles.push_back(c);
            circles.back().ring.load("img/specialAssets/rings/ring-0"+ofToString(i+1)+".png");
        }

    }
    
    bool isDone(bool b = false){
        
        bool isInside = true;
        for(int i = 0; i<circles.size();i++){
            if(!circles[i].isTarget)isInside = false;
        }
        return isInside;
    }
    
    void update(){

        float temp = collectedRotation;
        collectedRotation = 0.f;
        
        for(int i = 0; i<winnerButtons.size();i++){
          
            if(!isDone())
                collectedRotation+=circles[i].update(co->logicPrecision, co->attraction, buttons->at(winnerButtons[i]).getRawData().z);
            if(co->showLogicTargets)
                circles[i].setTarget(co->targetCircleRot[i]);
        }
        if(collectedRotation!=temp){
            ofxOscMessage m;
            m.setAddress("/collectedRotation"+ofToString(teamNumber+1));
            m.addFloatArg(collectedRotation);
            co->oscOut.sendMessage(m);
        }
    }
    
    void draw(){
        ofSetColor(255);
        for(int i = 0; i<circles.size();i++){
            ofVec2f m = ofVec2f(1920/2 + teamNumber*1920 , 1080/2);
            circles[i].draw(m);
            if(co->showLogicTargets)circles[i].drawTargets(m);
        }
        for(int i=0; i<winnerButtons.size(); i++) {
            ofPushMatrix();

            if(i>2)ofTranslate(1920 + teamNumber *1920 - 340, 324 + (i-3) *216 );
            else ofTranslate( teamNumber *1920 + 340, 324 + i *216 );
            ofRotateZ(ofRadToDeg(buttons->at(winnerButtons[i]).getRawData().z));
            buttons->at(winnerButtons[i]).draw(false, 2.2);
            if(co->debug)
                co->font_small->drawString(ofToString(
                                                      buttons->at(winnerButtons[i]).table) +" "
                                                      +ofToString(buttons->at(winnerButtons[i]).ID),0,0);
            
            ofPopMatrix();
        }
    }
    
    void begin(ofxBox2d * world = nullptr){


        vector<int> firstOn;
        int count = 0;
        
        for(int b=0; b<buttons->size(); b++) {

            //cout << firstOn.back() << endl;
            
            if (buttons->at(b).isWinner && count < 6){
                //circles[count].b = &buttons->at(b);
                winnerButtons.push_back(b);
                cout << "winbutton: "<< count <<" is " << b << "on team "<< teamNumber+1<< endl;
                count++;
            }
            else if (buttons->at(b).isPlaying)
                firstOn.push_back(b);
        }
        
        if(firstOn.size() < 6){
            for(int i = 0; i<36;i++){
                bool exists = false;
                for(int u = 0; u<firstOn.size();u++){
                    if( i == firstOn[u])
                        exists = true;
                }
                if (!exists)
                    firstOn.push_back(i);
            }
        }

        int i = 0;
        while (winnerButtons.size() < MIN(firstOn.size(), 6)){
            int setTo = firstOn[i];
            i++;
            buttons->at(firstOn[i]).isWinner = true;
            winnerButtons.push_back(setTo);
            cout << "winbutton "<< i <<" was not found and is set to " << setTo << "on team "<< teamNumber+1<< endl;
        }
        
        for(int i = 0; i < 6 ; i++){
            circles[i].isTarget = false;
            
            float random = ofRandom(360);
            float pRot = i > 0 ? circles[i-1].p_rotation : 0;
            circles[i].cur_rotation = pRot + ofRandom(60,160);
            
            circles[i].p_rotation = ofRadToDeg(buttons->at(winnerButtons[i]).getRawData().z);
            circles[i].startRot = circles[i].cur_rotation;
            
            cout << random<< endl;
            
        }
    }
    
    void reset(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }
        
        for(int i = 0; i < 6 ; i++){
            circles[i].isTarget = false;            
        }
    
    }
    
};


//
/*
 
 class Tile{
 
 private:
 int tileConfig[4];
 
 public:
 int get(int i){
 return tileConfig[i];
 }
 void set(int i, int a){
 tileConfig[i] = a;
 }
 
 Tile * top;
 Tile * right;
 Tile * bottom;
 Tile * left;
 
 bool isConnected = false;
 bool hasbeentested = false;
 
 
 int x, y;
 void connect( Tile * t, Tile * r, Tile * b, Tile * l){
 top = t;
 right = r;
 bottom = b;
 left = l;
 
 
 if(x == 0)left = nullptr;
 else if(x == GRID-1)right = nullptr;
 
 if(y == 0)top = nullptr;
 else if(y == GRID-1)bottom = nullptr;
 }
 
 void setup(int _x, int _y){
 
 x = _x;
 y = _y;
 
 cout << "tile"<<x<<y<<" : ";
 for(int i = 0; i<4; i++){
 tileConfig[i]=int(floor(ofRandom(2)));
 cout << tileConfig[i];
 }
 cout << " " <<endl;
 }
 
 void turn(bool clockwise){
 if(!clockwise){
 int temp = tileConfig[3];
 for(int i = 0; i<3; i++)
 tileConfig[3-i]=tileConfig[3-i-1];
 tileConfig[0] = temp;
 
 }
 else {
 int temp = tileConfig[0];
 for(int i = 0; i < 3; i++)
 tileConfig[i]=tileConfig[i+1];
 tileConfig[3] = temp;
 }
 }
 
 void update(){
 
 if(!hasbeentested){
 hasbeentested = true;
 
 if ( top != nullptr && get(0) == 1 ){
 if(top->get(2) == 1){
 top->isConnected = true;
 top->update();
 }
 }
 
 if ( right != nullptr && get(1) == 1 ){
 if(right->get(3) == 1){
 right->isConnected = true;
 right->update();
 }
 }
 
 if ( bottom != nullptr && get(2) == 1 ){
 
 if(bottom->get(0) == 1){
 bottom->isConnected = true;
 bottom->update();
 }
 }
 
 if ( left != nullptr && get(3) == 1 ){
 if(left->get(1) == 1){
 left->isConnected = true;
 left->update();
 }
 }
 
 
 
 
 }
 }
 
 
 
 void draw(){
 ofPushStyle();
 ofPushMatrix();
 
 ofTranslate(200*x + 200, 200*y + 200);
 
 ofColor c = isConnected ? ofColor::blue : ofColor::red;
 ofSetColor(c);
 
 ofSetLineWidth(20);
 for(int i = 0; i<4; i++){
 
 if(tileConfig[i] == 1){
 cout <<"tile : "<< x << y << " : "<<i<<endl;
 if(i==0)ofDrawLine(0, 0 , 0, -50);
 if(i==1)ofDrawLine(0, 0 , 50, 0);
 if(i==2)ofDrawLine(0, 0 , 0, 50);
 if(i==3)ofDrawLine(0, 0 , -50, 0);
 //                0 = 0,-1
 //                1 = 1,0
 //                2 = 0,1
 //                3 = -1,0
 //ofDrawLine(0, 0 , 0, -50);
 }
 //ofRotateZ(90);
 }
 ofNoFill();
 
 ofSetLineWidth(1);
 ofDrawRectangle(-50,-50,100,100);
 
 ofPopMatrix();
 ofPopStyle();
 }
 
 
 
 };*/