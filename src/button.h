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
    
    
    void drawDebug(ofRectangle rect = ofRectangle(0,0,1920,1080)){
        if(on && !isDead() && isPlaying){
            ofVec2f p;
            
            p.x = rect.width*x + rect.x;
            p.y = rect.height*y + rect.y;
            
            
            ofPushMatrix();
            ofTranslate(p);
            if(on)ofSetColor(ofColor::royalBlue , 100);
            else ofSetColor(ofColor::orangeRed , 20);
            ofDrawCircle(0, 0, 25);
            ofPopMatrix();
            
            if(on)ofDrawLine(getPos(),p);
        }
    }
    
    void draw(bool transform = true, bool sizeMatters = true, bool allon = false){
        if(on || allon){
            if(transform){
                ofPushMatrix();
                ofTranslate(getBiquadPos());
                ofRotateZ(r);
            }
            if( !isDead()){
                float rad = sizeMatters ? box2Dcircle->getRadius() : beginningRad ;
                // float alpha = isColliding() ? 200:255;
                //body
                float alpha=255;
                if(sizeMatters){
                    alpha =CLAMP( value*value*0.1 , 0, 255 );
//                    colors[0].setSaturation(alpha);
//                    colors[2].setSaturation(alpha);
                }
                
                //eyes
                if(!isColliding()){
                    float eye = -rad + rad*0.45/2.;
                    ofSetColor(0);
                    ofDrawCircle(rad*0.5 ,eye ,5);
                    ofDrawCircle(-rad*0.5,eye ,5);
                }
                
                for(int i = 0; i<3; i++){
                    ofSetColor(colors[i]);
                    img->at(i).draw(-rad,-rad,rad*2,rad*2);
                }
                
//                ofSetColor(colors[1]);
//                img[1]->draw(-img[1]->getWidth()/2,-img[1]->getHeight()/2);
//                ofSetColor(colors[2]);
//                img[2]->draw(-img[2]->getWidth()/2,-img[2]->getHeight()/2);
//                
//                
//                //head                
//                ofSetColor(colors[1]);
//                float eye = -rad + rad*0.45/2;
//                ofDrawEllipse(0,eye, rad*0.85,rad*0.45);
//                
//                
//                //stomach
//                
//                
//                ofSetColor(colors[2]);
//                ofDrawEllipse(0,rad*0.35,rad*1.5,rad*1.4);
//                
                
                armSwapper+=(dx+dy)*10.;
                if(armSwapper>0.8)armSwapper=0.;
                
                int aInd = armSwapper>.4 ? 0:1;
                legs[aInd].draw(-(rad+10),-(rad+10),rad*2+20,rad*2+20);
                
            }
            else {
                ofSetColor(colors[1]);
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
    float getSpeed(){
        return dx+dy;
    }
    
    void set(float _x, float _y, float _r){
        x=_x;
        y=_y;
        r_raw=_r;
    }
    
    void update(float attraction , ofRectangle rect = ofRectangle(0,0,1920,1080), bool updateRad = true){
        if(isPlaying && !isDead()){
            if(on){
                
                ofVec2f p;

                p.x = rect.width*x + rect.x;
                p.y = rect.height*y + rect.y;
                
               // else p = getPosRaw();
                //ofVec2f p = doble ? ofVec2f(x*1920 + 1920*teamNumber , y*1080) : getPosRaw();
                
                if(!prev_on){
                    box2Dcircle->setPosition(lastPos);
                    box2Dcircle->setVelocity(0,0);
                    prev_on=true;
                }
                
                
                distToOrg = abs(p.x - getPos().x) + abs(p.y - getPos().y);
                distToOrg /= (1980+1080);
                
                lastPos = getPos();
                
                setDirection(rect);

                //if(!freezeUpdate){
                    box2Dcircle->setVelocity(0,0);
                    box2Dcircle->addAttractionPoint( p, attraction*distToOrg  );
                //}
                
            }
            if(!on) {
                prev_on = false;
            }
            filterLowPass.update(getPos());
        }
        if(updateRad)updateRadius();
        
    }
//    
//    void freezeControl(float thres){
//        if(freezeUpdate){
//            freezeTimer+=ofGetLastFrameTime();
//            if(freezeTimer>thres)freezeUpdate=false;
//        }
//        if(updatefinaleValue){
//            finaleValueTimer+=ofGetLastFrameTime();
//            if(finaleValueTimer>thres)updatefinaleValue=false;
//        }else{
//            finaleValue = value;
//        }
//        
//    }
    
    void updateWithGravity(float jump , float x_jump, float thresY){
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
                
                float dif_x = getPosRaw().x - getPos().x ;
                
                if( abs(box2Dcircle->getVelocity().y) < thresY)isJumping = false;
                else isJumping = true;

                if(!isJumping && dif_y>0)box2Dcircle->addForce(ofVec2f(0,-1), jump);
                
                box2Dcircle->setVelocity(0,box2Dcircle->getVelocity().y);
                box2Dcircle->addForce(ofVec2f( dif_x*x_jump , 0) , abs(dif_x)*x_jump);
                
                
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
    
    void setDirection(ofRectangle rect = ofRectangle(0,0,1920,1080)){
        
        ofVec2f p;
        
        p.x = rect.width*x + rect.x;
        p.y = rect.height*y + rect.y;
        
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
                addValue(-(dx+dy)*f);
            }
        }

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
        return ofPoint(i * 140 + 140, t * 70 + 140 );
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
        return ofVec3f(x,y,r_raw);
    }
    void setRotation(float _r){
        r = _r;
    }
    float getRotation(){
        return r;
    }
    void setArmSwap(float a){
        armSwapper = a; 
    }

    void drawb2d(){
        box2Dcircle->draw();
    }
    void setValue(double v){
        value = v;
        if(value <0)value = 0;
    }
    double getValue(){
        return value;
    }
    void addValue(double v){
        value+=v;
        if(value <0)value = 0;
    }
    void multValue(double v){
        value*=v;
        if(value <0)value = 0;
    }
    ofxBiquadFilter2f filterLowPass;
    float fc;
    

    
    float px,py,dx,dy;
    
    
    
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
    
//    // finale crap!! :(
//    bool freezeUpdate = false;
//    float freezeTimer = 0.;
//    float finaleValue;
//    bool updatefinaleValue = false;
//    float finaleValueTimer = 0.;
    
    float distToOrg;

    ofVec2f vel;
    float p_drain;
    bool deadSoundCheck = false;
    
    ofVec2f lastPos;
    float beginningRad;
    vector<ofImage> *img;
    
private:
    ofImage legs[2];
    float armSwapper;
    
    float r, r_raw;
    double x;
    double y;
    ofImage deadImg;
    double value;
    
    
};

#endif /* Button_h */
