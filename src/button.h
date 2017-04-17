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
        color.resize(2);
        teamNumber = _teamNum;
        isPlaying=false;
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

        fc = 0.05f;
        filterLowPass.setFc(fc);
           
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
                ofRotateZ(r);
            }
            
            float scale = rad / 25.0f;
            ofSetColor(255);
        
            if(!isDead()){
                armSwapper+=(dx+dy)*10.;
                if(armSwapper>0.8)armSwapper=0.;
                int aInd = armSwapper>.4 ? 0:1;
                int w = rad*3.8;
                legs[aInd]->draw(-w/2,-w/2 + rad*0.23 ,w,w);
            }
            int charImg = isDead()?1:0;
            img->at(charImg).draw(-rad,-rad,rad*2,rad*2);
            
            ofSetColor(color[0]);
            ofTranslate(0,rad*0.1);
            
            symbol->draw(-rad*0.65/2,-rad*0.65/2,rad*0.65,rad*0.65);
            
            float rectSize=0;
            if(value>0)rectSize=ofMap((size_lim - (size_break/value)) / (1 + (size_break/value)),0,size_lim,0,360 );
            float bugSize = 0.5;
            ofPath curve;
            curve.setStrokeWidth(4.5*scale);
            
            curve.setFilled(false);
            curve.setStrokeColor(color[0]);
            //curve.setArcResolution(1000);
            curve.moveTo(rad*bugSize,0);
            curve.arc(0, 0, rad*bugSize, rad*bugSize, 0, rectSize);
            curve.draw();
            
            ofNoFill();
            ofSetLineWidth(scale);
            ofDrawCircle(0, 0, rad*bugSize-(curve.getStrokeWidth()*0.5));
            ofDrawCircle(0, 0, rad*bugSize+(curve.getStrokeWidth()*0.5));
            //ofDrawRectangle(-4*scale, -15*scale, 8*scale, 35*scale);
            ofFill();
            
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
    
    void update(float attraction , ofRectangle rect = ofRectangle(0,0,1920,1080)){
        box2Dcircle->setVelocity(0,0);
        if(isPlaying && !isDead()){
            
            if(on){
                
                ofVec2f p;

                p.x = rect.width*x + rect.x;
                p.y = rect.height*y + rect.y;
                
                distToOrg = abs(p.x - getPos().x) + abs(p.y - getPos().y);
                distToOrg /= (1980+1080);
                
                lastPos = getPos();
                
                setDirection(rect);

                //if(!freezeUpdate){
                
                    box2Dcircle->addAttractionPoint( p, attraction*distToOrg  );
            }
        }
        filterLowPass.update(getPos());
        //if(updateRad)
            updateRadius();
        
    }

    void updateWithGravity(float jump , float x_jump, float thresY){
        if(isPlaying && !isDead()){
            if(on ){
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
        }
        filterLowPass.update(getPos());
        updateRadius();

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
            radius = beginningRad;
            box2Dcircle->alive = true;
        }
        else{
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
        if(isPlaying){
            value = v;
            if(value <0.)value = 0.;
        }
    }
    double getValue(){
        return value;
    }
    void addValue(double v){
        if(isPlaying){
            value+=v;
            if(value <0.)value = 0.;
        }
    }
    void multValue(double v){
        if(isPlaying){
            value*=v;
            if(value <0.)value = 0.;
        }
    }
    ofxBiquadFilter2f filterLowPass;
    float fc;
    

    
    float px = 0.;
    float py = 0.;
    float dx = 0.;
    float dy = 0.;
    
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
    
    float size_break=50.;
    float size_lim=50.;
    
    float dy_jump, dx_jump;
    bool isJumping;
    
    float distToOrg;

    ofVec2f vel;
    float p_drain;
    bool deadSoundCheck = false;
    
    ofVec2f lastPos;
    float beginningRad;
    vector<ofImage> *img;
    ofImage *symbol;
    vector<ofColor> color;
    int symbolInt,colorInt;
    
    ofImage * legs[2];
private:
    
    float armSwapper;
    
    float r, r_raw;
    double x=0.;
    double y=0.;
    double value=0.;
    
    float deadTimer;
};

#endif /* Button_h */
