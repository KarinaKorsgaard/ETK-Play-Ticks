#include "ofMain.h"
#include "Scene.h"


class circle{
public:
    
    Button * b;
    ofPath p;
    float targetRotation;
    bool isTarget = false;
    int ringNumber;
    float p_rotation;
    float cur_rotation;
    float startRot;
    
    void setup(int _ringNumber, float start, float target){
        
        ringNumber = _ringNumber;
        targetRotation = target;
        cur_rotation = start;
        p_rotation = start;
        startRot = start;
        
        p.moveTo(ringNumber * 90 + 80,0);
        p.arc(ofPoint(0,0), ringNumber * 90 + 80, ringNumber * 90 + 80, 0, 350);
        p.setCircleResolution(500);
        p.setFilled(false);
        p.setStrokeWidth(5);
    }
    void setTarget(float t){
        targetRotation = t;
    }
    
    void update(float precision, float attraction){
        b->update(attraction);
        isTarget = abs(cur_rotation-targetRotation) < precision;
        float dr = ofRadToDeg(b->getRawData().z) - p_rotation;
        
        if(abs(dr) < 5)cur_rotation += dr*0.5f ;
        if(cur_rotation > 360)cur_rotation = 0;
        if(cur_rotation < 0)cur_rotation = 360;
        
        p_rotation = ofRadToDeg(b->getRawData().z);
    }
    
    void draw(ofVec2f midt){
        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(cur_rotation);
        p.draw();
        ofPopMatrix();
        b->draw();
    }
    void drawTargets(ofVec2f midt){
        p.setStrokeWidth(35);
        p.setColor(ofColor::blue);
        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(targetRotation);
        p.draw();
        ofPopMatrix();
        b->draw();
        p.setColor(ofColor::white);
        p.setStrokeWidth(5);
    }
};

class Logic : public Scene{
    
public:

    Logic(){};
    ~Logic(){};

    vector<circle> circles;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;

    }
    
    bool isDone(bool b = false){
        
        bool isInside = true;
        for(int i = 0; i<circles.size();i++){
            if(!circles[i].isTarget)isInside = false;
        }
        return isInside;
    }
    
    void update(){
        for(int i = 0; i<circles.size();i++){
            if(!isDone())
                circles[i].update(co->logicPrecision, co->attraction);
            if(co->showLogicTargets)
                circles[i].setTarget(co->targetCircleRot[i]);
        }
    }
    
    void draw(){
        ofSetColor(255);
        for(int i = 0; i<circles.size();i++){
            ofVec2f m = ofVec2f(1920/2 + teamNumber*1920 , 1080/2);
            circles[i].draw(m);
            if(co->showLogicTargets)circles[i].drawTargets(m);
        }
    }
    
    void begin(ofxBox2d * world = nullptr){
   
        for(int i = 0; i < 6 ; i++){
            circle c = *new circle;
            c.setup(i, 280, co->targetCircleRot[i]);
            circles.push_back(c);
        }
        
        vector<int> firstOn;
        int count = 0;
        
        for(int b=0; b<buttons->size(); b++) {
            if (buttons->at(b).isPlaying)
                firstOn.push_back(b);
            //cout << firstOn.back() << endl;
            
            if (buttons->at(b).isWinner && count < 6){
                circles[count].b = &buttons->at(b);
                cout << "winbutton: "<<count <<" is " << b << endl;
                count++;
            }
        }
        
        for(int i = 0; i<6 ; i++){
            if (circles[i].b == nullptr){
                int setTo = i;
                
                if(firstOn.size()>i)
                    setTo = firstOn[i];
                circles[i].b = &buttons->at(setTo);
                
                cout << "winbutton "<< i <<" was not found and is set to " << setTo << endl;
            }
           // rotations[i] = ofRandom(360);
        }
        

    }
    
    void reset(){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }
        for(int i = 0; i<circles.size();i++){
            circles[i].b = nullptr;
            
            circles[i].cur_rotation = circles[i].startRot;
            circles[i].p_rotation = circles[i].startRot;
            circles[i].isTarget = false;
        }
        
        circles.clear();
        
        
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