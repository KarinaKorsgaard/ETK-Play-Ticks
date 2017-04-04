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
        box2Dcircle.get()->setPhysics(3., 0.0, 40.0);
        box2Dcircle.get()->setup(world->getWorld(), ofRandom(200,500), ofRandom(200,500), 1);
        box2Dcircle.get()->alive = false;
        box2Dcircle.get()->setRadius(beginningRad);
  //      box2Dcircle.get()->setMassFromShape = false;
        
        lastPos = ofVec2f(ofRandom(200,500),ofRandom(200,500));
        
//        ofColor c;
//        c.set(teamNumber*255,ofRandom(0,255),255-teamNumber*255);
//        colors.push_back(c);
//        
//        c.set(teamNumber*255,ofRandom(0,255),255-teamNumber*255);
//        colors.push_back(c);
//        
//        c.set(teamNumber*255,ofRandom(220,255),255-teamNumber*255);
//        colors.push_back(c);
        
        fc = 0.05f;
        filterLowPass.setFc(fc);
        
      //  deadImg.load("characters/dead.png");
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
    
    void draw(bool transform = true, bool sizeMatters = false, bool allon = false){
       // if(on || allon){
        if(isPlaying ){
            float rad = sizeMatters ? box2Dcircle->getRadius() : beginningRad ;
            if(transform){
                ofPushMatrix();
                ofTranslate(getBiquadPos());
               // if(!isDead())
                ofRotateZ(r);
            }
            
            float scale = rad / 25.0f;
            
//            if(on && !isDead()){
//                float eye = -rad + rad*0.45/2.;
//                ofSetColor(0);
//                ofDrawCircle(3 *scale,eye ,5);
//                ofDrawCircle(-3*scale,eye ,5);
//            }
            
// body
            ofSetColor(255);
            int charImg = isDead()?1:0;
            
            img->at(charImg).draw(-rad,-rad,rad*2,rad*2);
            ofSetColor(color);
            symbol->draw(-6*scale,13*scale,12*scale,12*scale);
            ofNoFill();
            ofDrawRectangle(-4*scale, -15*scale, 8*scale, 25*scale);
            ofFill();
            
            float rectSize=0;
            if(value>0)rectSize=ofMap((size_lim - (size_break/value)) / (1 + (size_break/value)),0,size_lim,0,25*scale );
            ofDrawRectangle(-4*scale,-15*scale+(25*scale-rectSize),8*scale,rectSize);
            
           // }
            ofSetColor(255);
            if(!isDead()){
                
                armSwapper+=(dx+dy)*10.;
                if(armSwapper>0.8)armSwapper=0.;
                int aInd = armSwapper>.4 ? 0:1;
                legs[aInd].draw(-(rad+10),-(rad+10),rad*2+20,rad*2+20);
            }
            
            if(transform)ofPopMatrix();
        }
//         ofSetColor(200);
//        ofDrawCircle( getPos(), 30 );
//         ofDrawCircle( getBiquadPos(), 30 );
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
    
    void update(float attraction , ofRectangle rect = ofRectangle(0,0,1920,1080)){
        box2Dcircle->setVelocity(0,0);
        if(isPlaying && !isDead()){
            
            if(on){
                
                ofVec2f p;

                p.x = rect.width*x + rect.x;
                p.y = rect.height*y + rect.y;
                
               // else p = getPosRaw();
                //ofVec2f p = doble ? ofVec2f(x*1920 + 1920*teamNumber , y*1080) : getPosRaw();
                
//                if(!prev_on){
//                    box2Dcircle->setPosition(lastPos);
//                    box2Dcircle->setVelocity(0,0);
//                    prev_on=true;
//                }
                
                
                distToOrg = abs(p.x - getPos().x) + abs(p.y - getPos().y);
                distToOrg /= (1980+1080);
                
                lastPos = getPos();
                
                setDirection(rect);

                //if(!freezeUpdate){
                
                    box2Dcircle->addAttractionPoint( p, attraction*distToOrg  );
            }
                
           // }
          //  if(!on) {
            //    prev_on = false;
          //  }
            
        }
        filterLowPass.update(getPos());
        //if(updateRad)
            updateRadius();
        
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
            if(on ){
//                if(!prev_on){
//                    box2Dcircle->setPosition(lastPos);
//                    box2Dcircle->setVelocity(0,0);
//                    prev_on=true;
//                }
                
              //  lastPos = getPos();
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
//            if(!on) {
//                prev_on = false;
//            }
            
            
        }
        filterLowPass.update(getPos());
        updateRadius();
//
//        // radius.
//        if(value > 0 && on ){
//            radius = beginningRad;
//            
//         //   if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(box2Dcircle->getRadius()+1);
//         //   else box2Dcircle->setRadius(radius);
//            box2Dcircle->alive = true;
//        }
//        if (value <= 0 || !on || isDead()) {
//         //   box2Dcircle->setRadius(0);
//            box2Dcircle->alive = false;
//        }
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
        if(on && !isDead() && isPlaying){
            radius = beginningRad;  //(size_lim - (size_break/value)) / (1 + (size_break/value));
//            if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(box2Dcircle->getRadius()+1);
        //    if(box2Dcircle->getRadius() < radius)box2Dcircle->setRadius(radius);
            box2Dcircle->alive = true;
        }
        else{
         //   if(!isPush)box2Dcircle->setRadius(0);
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
    void reviveDeadTicks(float thres, float amount){
        if(isDead()){
            deadTimer+=ofGetLastFrameTime();
            if(deadTimer>thres)setValue(amount);
        }else deadTimer=0.;
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
    ofImage *symbol;
    ofColor color;
    
private:
    ofImage legs[2];
    float armSwapper;
    
    float r, r_raw;
    double x;
    double y;
    double value;
    
    float deadTimer;
};

#endif /* Button_h */
