//
//  Button.h
//  ETK_2017
//
//  Created by Karina Jensen on 10/01/17.
//
//

#ifndef Button_h
#define Button_h


#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxBox2d.h"
#include "ofxBiquadFilter.h"
//#include "common.h"
#include "ButtonData.h"

class Button{
public:
    void setup(int _row, int _table,int _teamNum, string _address, string _secondAdress, float val, float size, ofxBox2d *world){
        ID = _row;
        table = _table;
        address = _address;
        secondAdress = _secondAdress;
        
        teamNumber = _teamNum;
        
        value = val;
        beginningValue = val;
        beginningRad = size;
        
        
        box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //virtual void setPhysics(float density, float bounce, float friction);
        box2Dcircle.get()->setPhysics(3.0, 0.0, 40.0);
        box2Dcircle.get()->setup(world->getWorld(), ofRandom(200,500), ofRandom(200,500), 1);
        box2Dcircle.get()->alive = false;
        
        lastPos = ofVec2f(ofRandom(200,500),ofRandom(200,500));
        
        ofColor c;
        c.set(teamNumber*255,ofRandom(0,255),255-teamNumber*255);
        colors.push_back(c);
        
        c.set(teamNumber*255,ofRandom(0,255),255-teamNumber*255);
        colors.push_back(c);
        
        c.set(teamNumber*255,ofRandom(220,255),255-teamNumber*255);
        colors.push_back(c);
        
        fc = 0.05f;
        filterLowPass.setFc(fc);
        
        deadImg.load("characters/dead.png");
        legs[0].load("characters/arms/arm1.png");
        legs[1].load("characters/arms/arm2.png");
        
    }
    
    
    void drawDebug(){
        
        ofPushMatrix();
        ofTranslate(getPosRaw());
        if(on)ofSetColor(ofColor::royalBlue , 100);
        else ofSetColor(ofColor::orangeRed , 20);
        ofDrawCircle(0, 0, 25);
        ofPopMatrix();

        if(on)ofDrawLine(getPos(),getPosRaw());
        
    }
    
    void draw(bool transform = true, bool sizeMatters = true, bool allon = false){
        if(on || allon){
            if(transform){
                ofPushMatrix();
                ofTranslate(getBiquadPos());
                ofRotateZ(r);
            }
            if( !isDead() || !sizeMatters ){
                float rad = sizeMatters ? box2Dcircle->getRadius() : 60 ;
                // float alpha = isColliding() ? 200:255;
                //body
                ofSetColor(colors[0]);
                ofDrawCircle(0,0,rad);
                
                float alpha = ((255 - (112/value)) / (1 + (112/value)));
                //head                
                ofSetColor(colors[1],alpha);
                float eye = -rad + rad*0.45/2;
                ofDrawEllipse(0,eye, rad*0.85,rad*0.45);
                
                //eyes
                if(!freezeUpdate){
                    ofSetColor(0);
                    ofDrawCircle(rad*0.3 ,eye, 6);
                    ofDrawCircle(-rad*0.3,eye ,6);
                }
                //stomach
                
                
                ofSetColor(colors[2], alpha);
                ofDrawEllipse(0,rad*0.35,rad*1.5,rad*1.4);
                
                
                armSwapper+=(dx+dy)*10.;
                if(armSwapper>0.2)armSwapper=0.;
                
                int aInd = armSwapper>.1 ? 0:1;
                legs[aInd].draw(-(radius+10),-(radius+10),radius*2+20,radius*2+20);
                
            }
            else {
                deadImg.draw(-15,-15,30,30);
            }
            if(transform)ofPopMatrix();
        }
    }
    void setPosition(float x, float y){
        setPosition(ofVec2f(x,y));
    }
    
    void setPosition(ofVec2f v){
        box2Dcircle->setPosition(v);
        filterLowPass.clear(v);
        lastPos = v;
    }
    
    void set(float _x, float _y, float _r){
        x=_x;
        y=_y;
    }
    
    void update(float attraction , bool doble = false){
        if(isPlaying && !isDead()){
            if(on){
                ofVec2f p = doble ? ofVec2f(x*1920 + 1920*teamNumber , y*1080) : getPosRaw();
                
                if(!prev_on){
                    box2Dcircle->setPosition(lastPos);
                    box2Dcircle->setVelocity(0,0);
                    prev_on=true;
                }
                
                
                distToOrg = abs(p.x - getPos().x) + abs(p.y - getPos().y);
                distToOrg /= (1980+1080);
                
                lastPos = getPos();
                
                setDirection(doble);

                if(!freezeUpdate){
                    box2Dcircle->setVelocity(0,0);
                    box2Dcircle->addAttractionPoint( p, attraction*distToOrg  );
                }
                
            }
            if(!on) {
                prev_on = false;
            }
            filterLowPass.update(getPos());
        }
        updateRadius();
        
    }
    
    void freezeControl(float thres){
        if(freezeUpdate){
            freezeTimer+=ofGetLastFrameTime();
            if(freezeTimer>thres)freezeUpdate=false;
        }
        if(updatefinaleValue){
            finaleValueTimer+=ofGetLastFrameTime();
            if(finaleValueTimer>thres)updatefinaleValue=false;
        }else{
            finaleValue = value;
        }
        
    }
    
    void updateWithGravity(float jump , float x_jump){
        if(isPlaying && !isDead()){
            if(on && box2Dcircle->getRadius()==beginningRad){
                if(!prev_on){
                    box2Dcircle->setPosition(lastPos);
                    box2Dcircle->setVelocity(0,0);
                    prev_on=true;
                }
                
                lastPos = getPos();
                setDirection();
                
                float dif_y = dy_jump - getPosRaw().y;
                dy_jump = getPosRaw().y;
                
                float dif_x = getBiquadPos().x - getPosRaw().x;
                
                if( abs(box2Dcircle->getVelocity().y) < 0.001)isJumping = false;
                else isJumping = true;

                if(!isJumping && dif_y>0)box2Dcircle->addForce(ofVec2f(0,-1), jump);
                if(abs(dif_x)>0){
                    box2Dcircle->addForce(ofVec2f(-dif_x*x_jump,0),0.1);
                }
                
            }
            if(!on) {
                prev_on = false;
            }
            filterLowPass.update(getPos());
            
        }

        // radius.
        if(value > 0 && on ){
            radius = beginningRad;
            
            if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(box2Dcircle->getRadius()+1);
            else box2Dcircle->setRadius(radius);
            box2Dcircle->alive = true;
        }
        if (value <= 0 || !on || isDead()) {
            box2Dcircle->setRadius(0);
            box2Dcircle->alive = false;
        }
    }
    
    void setDirection(bool doble = false){
        ofVec2f p = doble ? ofVec2f(x*1920 + 1920*teamNumber , y*1080) : getPosRaw();
        ofVec2f up;
        vel.x = -(p.x - filterLowPass.value().x);
        vel.y = p.y - filterLowPass.value().y;
        up.set(0,-1);
        r = vel.angle(up);
    }
    
    void updateRadius(){
        if(on && value > 0){
            radius = beginningRad;  //(size_lim - (size_break/value)) / (1 + (size_break/value));
            if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(box2Dcircle->getRadius()+1);
            else box2Dcircle->setRadius(radius);
            box2Dcircle->alive = true;
        }
        if (value <= 0 || !on || isDead()) {
            box2Dcircle->setRadius(0);
            box2Dcircle->alive = false;
        }
    }
    
    void drain(float f){
        if(value > 0){
            if(on){
                dx = abs(px-x);
                dy = abs(py-y);
            }
            else{
                dx=0.0;
                dy=0.0;
            }
            px=x;
            py=y;
        
            if(on){
                value-=(dx+dy)*f;
            }
        }
        if(value<0)value = 0;
    }
    
    void printInfo(){
        cout << "table: "+ofToString(table) +" ";
        cout << "ID: "+ofToString(ID) + " ";
        cout << "teamnumber: "+ofToString(teamNumber) + " ";
        cout << "value: "+ofToString(value) + " ";
        cout << " " << endl;
    }
    
    bool isColliding(){
        ButtonData * data = (ButtonData*)box2Dcircle.get()->getData();
        if(data){
            if(data->bHit && on && !isDead()) return true;
            else return false;
        }else return false;
    }
    bool isDead(){
        return value==0.0;
    }
    
    ofPoint getGridPos(int t, int i){
        return ofPoint(t * 140 + 140 , i * 140 + 140);
    }
    
    ofVec2f getPosRaw(){
        return ofVec2f(x*1920.0f,y*1080.0f);
    }
    ofVec2f getPos(){
        return box2Dcircle->getPosition();
    }
    ofVec2f getBiquadPos(){
        return filterLowPass.value();
    }
    ofVec3f getRawData(){
        return ofVec3f(x,y,r);
    }

    void drawb2d(){
        box2Dcircle->draw();
    }
    
    ofxBiquadFilter2f filterLowPass;
    float fc;
    

    
    float px,py,dx,dy;
    
    
    double value;
    float beforeRefillValue;
    float beginningValue;
    float radius;
    
    int row;
    int table;
    int ID;
    
    int teamNumber;
    
    bool isPlaying = false;
    
    vector<ofColor>colors;
    
    string address;
    string secondAdress;
    
    shared_ptr<ofxBox2dCircle> box2Dcircle;
    
    bool on = false;
    bool prev_on = false;
    
    float size_break;
    float size_lim;
    
    // gravity shit :(
    float dy_jump, dx_jump;
    bool isJumping;
    
    // finale crap!! :(
    bool freezeUpdate = false;
    float freezeTimer = 0.;
    float finaleValue;
    bool updatefinaleValue = false;
    float finaleValueTimer = 0.;
    
    float distToOrg;

    
    ofVec2f vel;
    float p_drain;
    ofImage legs[2];
    float armSwapper;
    ofVec2f lastPos;
    float beginningRad;
    
private:
    float r;
    double x;
    double y;
    ofImage deadImg;

    
};

#endif /* Button_h */
