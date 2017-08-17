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

    float dy_jump, dx_jump;
    bool isJumping;
    
    float distToOrg;
    
    ofVec2f vel;
    float p_drain;
    bool deadSoundCheck = false;
    
    ofVec2f lastPos;
    
    ofVec2f lastPosForPolys;
    
    vector<ofImage> *img;
    ofImage *symbol;
    vector<ofColor> color;
    int symbolInt,colorInt;
    
    ofImage * legs[4];
    
    bool isWinner = false;
    
    void setup(int _row, int _table,int _teamNum, string _address, string _secondAdress, float val, float size, ofxBox2d *world){
        ID = _row;
        table = _table;
        address = _address;
        secondAdress = _secondAdress;
        color.resize(2);
        teamNumber = _teamNum;
        isPlaying = false;
        value = val;
        beginningValue = val;
        radius = size;
        
        box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //virtual void setPhysics(float density, float bounce, float friction);
        box2Dcircle.get()->setPhysics(1., .0, 0.0);
        box2Dcircle.get()->setup(world->getWorld(), ofRandom(200,500), ofRandom(200,500), radius);
        box2Dcircle.get()->alive = false;
        box2Dcircle.get()->setRadius(radius);
  //      box2Dcircle.get()->setMassFromShape = false;
        
        lastPos = ofVec2f(ofRandom(200,500),ofRandom(200,500));

        fc = 0.05f;
        filterLowPass.setFc(fc);
        
        
        
           
    }
    
    
    void drawDebug( ofRectangle rect = ofRectangle(0,0, 1920,1080)){
        ofPushStyle();
        if( isPlaying){
            ofVec2f p;
            
            p.x = rect.width * x + rect.x;
            p.y = rect.height * y + rect.y;
            
            p.x += teamNumber * 1920;
            
            ofPushMatrix();
            ofTranslate(p);
            if(on)ofSetColor(ofColor::royalBlue , 100);
            else ofSetColor(ofColor::orangeRed , 20);
            ofDrawCircle(0, 0, 25);
            ofSetColor(255);
            ofDrawBitmapString(ofToString(table+1)+" :"+ofToString(ID+1), -10, 20);
            ofPopMatrix();
            
            if(on)ofDrawLine(getPos(),p);
        }
        ofPopStyle();
    }
    
    void draw(bool transform = true, float scale = 1.f){
        float radiusUsed = radius * scale;
        ofPushStyle();
        if(isPlaying ){
            
            if(transform){
                ofPushMatrix();
                ofTranslate(getBiquadPos());
                ofRotateZ(r);
            }
            
            
            ofSetColor(color[0]);
            
            if(!isDead()){
                armSwapper+=getSpeed()*4.;
                if(armSwapper>0.8)armSwapper=0.;
                
                int aInd = 0;
                if (armSwapper > 0.4)
                    aInd = int(floor (ofMap(armSwapper,0.4,0.8, 4,0)));
                else
                    aInd = int(floor (ofMap(armSwapper,0.,0.4, 0,4)));
                aInd = std::min(aInd,3);
                aInd = std::max(aInd,0);
                
                int w = 132 * scale;
                int h = 128 * scale;
                legs[aInd]->draw(-w/2,-h/2 , w,h);
            }
            int charImg = isDead()?1:0;
            img->at(charImg).draw(-radiusUsed,-radiusUsed,radiusUsed*2,radiusUsed*2);

            ofSetColor(color[1]);
            symbol->draw(-radiusUsed,-radiusUsed,radiusUsed*2,radiusUsed*2);
            ofFill();
            
            if(transform)ofPopMatrix();
        }
        ofPopStyle();
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
        x=CLAMP(_x,0,2);
        y=CLAMP(_y,0,1);
        r_raw=_r;
    }
    
    void update(float attraction ,bool doubleSize = true, ofRectangle rect = ofRectangle(0,0, 1920,1080)){
        box2Dcircle->setVelocity(0,0);
        
        if(isPlaying && !isDead()){
            
           // if(on){

                ofVec2f p;
                
                int multiX = doubleSize ? teamNumber * 1920 : 0;
                
                p.x = multiX + (rect.width*x)+ rect.x;
                p.y = rect.height*y + rect.y;
                
 
                ofRectangle r = ofRectangle(multiX + rect.x , rect.y , rect.width, rect.height);
            
                box2Dcircle->addAttractionPoint( p , attraction  );
                
                setDirection(filterLowPass.value(), lastPos);

                lastPos = filterLowPass.value();

        }
        filterLowPass.update(getPos());

    }

    
    void updateFences(float attraction, float gravity ,bool doubleSize = true, ofRectangle rect = ofRectangle(0,0, 1920,1080)){
        box2Dcircle->setVelocity(0,0);
        
        if(isPlaying && !isDead()){
            
            //if(on){
                
                ofVec2f p;
                
                int multiX = doubleSize ? teamNumber * 1920 : 0;
                p.x = multiX + (rect.width * x) + rect.x;
                p.y = MAX(getPos().y - gravity, 1080);
                
                ofRectangle r = ofRectangle(multiX + rect.x , rect.y , rect.width, rect.height);
                
                //distToOrg = abs(p.x - getPos().x) + abs(p.y - getPos().y);
                //distToOrg /= (rect.width+1080);
                
                box2Dcircle->addAttractionPoint( p , attraction  );
                //box2Dcircle->addForce(ofVec2f(0,1), gravity);
                
                setDirection(filterLowPass.value(), lastPos);
                //setDirection(filterLowPass.value(), getPos());
                lastPos = filterLowPass.value();
                
                
                moveBackToBoard(r);
                //if(!freezeUpdate){
                
                
          //  }
        }
        filterLowPass.update(getPos());
        
    }

    
    void moveBackToBoard(ofRectangle r){
        

        // needs to get ticks back on their own side.. 
        if(getPos().x<r.x)setPosition(r.x + radius, getPos().y);
        if(getPos().x>(r.width+r.x))setPosition((r.width+r.x) - radius, getPos().y);
        
        if(getPos().y<0)setPosition(getPos().x, radius);
        if(getPos().y>1080)setPosition(getPos().x, 1080-radius);
    }
    
    void getOutOfPolys(vector<ofPolyline>polys){
        if(isPlaying){
            if(lastPosForPolys == getPos()){
                cout << "... checking "<< ofGetElapsedTimef() <<endl;
                
                for(int i = 0; i<polys.size();i++){
                    
                    if(polys[i].inside(getPos())){
                        
                        ofRectangle r = polys[i].getBoundingBox();
                        ofVec2f newPos = getPos();
                        
                        float distLeft = newPos.x - r.x;
                        float distRight = newPos.x - (r.x+r.width);
                        float distTop = newPos.y - r.y;
                        float distBot = newPos.y - (r.y + r.height);
                        
                        cout << distLeft << " "<< distRight << " "<< distTop << " "<< distBot << " "<<endl;
                        
                        float xdist = MIN(abs(distLeft),abs(distRight));
                        float ydist = MIN(abs(distTop),abs(distBot));
                        cout << "x dist "<<xdist << " ydist "<<ydist<<endl;
                        bool changeX = xdist < ydist;
                        
                        cout << "old pos "<<newPos.x <<" "<<newPos.y <<endl;
                        if(changeX)newPos.x += xdist;
                        else newPos.y += ydist;
                        
                        
                        if(newPos.y > 1080)
                            newPos.y -= r.height;
                        if(newPos.x > 1920*2)
                            newPos.x -= r.width;
                        
                        if(newPos.y < 0)
                            newPos.y += r.height;
                        if(newPos.x < 0)
                            newPos.x += r.width;
                        
                        
                        setPosition(newPos);
                        cout << "new pos "<<getPos().x <<" "<<getPos().y <<endl;
                        
                        if(polys[i].inside(newPos))
                            cout << "still in poly!!"<<endl;
                        
                        cout << "changed pos " << endl;
                        
                        break;
                    }//if isinside
                }//forloop
                cout << "done checking "<< ofGetElapsedTimef() <<endl;
            }//lastpos ==
        }//if is playing
        
        lastPosForPolys = getPos();
    }
    
    void setDirection(ofVec2f v1, ofVec2f v2){
        ofVec2f v = v2 - v1;
        if( v.length() > 1. ){
            
            
            ofVec2f up = ofVec2f(0,1);
            r = up.angle(v);
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
        return ofPoint((teamNumber * 1920)+ (i * 140 + 140), t * 70 + 140 );
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
    void setValue(float v){
        if(isPlaying){
            value = v;
            if(value <0.)value = 0.;
        }
    }
    float getValue(){
        return value;
    }
    void addValue(float v){
        if(isPlaying){
            value+=v;
            if(value <0.)value = 0.;
        }
    }
    void multValue(float v){
        if(isPlaying){
            value*=v;
            if(value <0.)value = 0.;
        }
    }

private:
    
    float armSwapper;
    
    float r, r_raw;
    float x=0.;
    float y=0.;
    float value=0.;
    
    float deadTimer;
};

#endif /* Button_h */
