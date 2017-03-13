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
    void setup(int _row, int _table,int _teamNum, string _address, string _secondAdress, float v, ofxBox2d *world){
        ID = _row;
        table = _table;
        address = _address;
        secondAdress = _secondAdress;
        
        teamNumber = _teamNum;
        
        value = v;
        beginningValue = value;
        
        box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //virtual void setPhysics(float density, float bounce, float friction);
        box2Dcircle.get()->setPhysics(3.0, 0.0, 400.0);
        box2Dcircle.get()->setup(world->getWorld(), ofRandom(200,500), ofRandom(200,500), 1);
        box2Dcircle.get()->alive = false;
        
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
                ofRotateZ(ofRadToDeg(r));
            }
            if( !isDead() || !sizeMatters ){
                float rad = sizeMatters ? box2Dcircle->getRadius() : 60 ;
                // float alpha = isColliding() ? 200:255;
                //body
                ofSetColor(colors[0]);
                ofDrawCircle(0,0,rad);
                
                //head
                ofSetColor(colors[1]);
                float eye = -rad + rad*0.45/2;
                ofDrawEllipse(0,eye, rad*0.85,rad*0.45);
                
                //eyes
                if(!isColliding()){
                    ofSetColor(0);
                    ofDrawCircle(rad*0.3 ,eye, 6);
                    ofDrawCircle(-rad*0.3,eye ,6);
                }
                //stomach
                ofSetColor(colors[2]);
                ofDrawEllipse(0,rad*0.35,rad*1.5,rad*1.4);
            }
            else {
            //    ofDrawRectangle(0,0,40,40);
                deadImg.draw(-15,-15,30,30);
            }
            if(transform)ofPopMatrix();
        }
    }
    
    void set(float _x, float _y, float _r){
        x=_x;
        y=_y;
        r=_r;
    }
    
    void update(int attraction ){
        if(isPlaying && !isDead()){
            if(!prev_on && on){
                prev_on=true;
                filterLowPass.clear(ofVec2f(getPos()));
                //box2Dcircle->setPosition(getPosRaw());
            }
            
            if(on){
                //ofPoint o = getPosRaw();
                //if(abs(getBiquadPos().x-o.x) < 3 && abs(getBiquadPos().y-o.y) < 3){
                box2Dcircle->setVelocity(0,0);
                box2Dcircle->addAttractionPoint(getPosRaw(), attraction );
                //}
                filterLowPass.update(getPos());
                
                if(value <= 0.0){
                    on = false;
                }
                
                //if(getPos().x<10||getPos().x>1970)box2Dcircle->setPosition(, <#float y#>)
            }
            else {
                prev_on = false;
            }
        }
        updateRadius();
    }
    
    void updateWithGravity(float jump ){
        if(isPlaying && !isDead()){
            if(on){
                ofPoint o = getPosRaw();
                if(on && prev_on){
                    dx = abs(px-o.x);
                    dy = abs(py-o.y);
                }
                else{
                    dx=0.0;
                    dy=0.0;
                }
            
                if(dx>0||dy>0)box2Dcircle->addForce(ofVec2f( -(px-x)*10. ,-CLAMP( py-y,0,10000 )*10. ),jump);
                else box2Dcircle->addForce(ofVec2f(dx,0), 10. );
                
                filterLowPass.update(getPos());
                
                if(value <= 0.0){
                    on = false;
                }
                if(!prev_on){
                    prev_on=true;
                }
            }
            else {
                prev_on = false;
            }
        }
        
        updateRadius();
    }
    
    void updateRadius(){
        if(on){
            if(value > 0)radius = (size_lim - (size_break/value)) / (1 + (size_break/value));
            else radius = 0;
            //if(ID == 0 && teamNumber == 0 && table == 0)cout << ofToString(value) +" : "+ofToString(radius)  << endl;
            //ofMap(CLAMP(value,0,100),0,100,10,100);
            if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(box2Dcircle->getRadius()+1);
            else box2Dcircle->setRadius(radius);
            box2Dcircle->alive = true;
        }
        else {
            box2Dcircle->setRadius(0);
            box2Dcircle->alive = false;
        }
    }
    
    void drain(float f){
        if(value > 0){
            if(on && prev_on){
                dx = abs(px-x);
                dy = abs(py-y);
            }
            else{
                dx=0.0;
                dy=0.0;
            }
            
            px=x;
            py=y;
            
            if(dx+dy<50 && on)
                value-=(dx+dy)*f;
        }
        if(value<0)value = 0;
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
    
    ofPoint getGridPos(){
        return ofPoint(table * 140 + 140 , ID * 140 + 140);
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
    
  //  commonObjects *co;
    
private:
    float r;
    double x;
    double y;
    ofImage deadImg;
};

#endif /* Button_h */
