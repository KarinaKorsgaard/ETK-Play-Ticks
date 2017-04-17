#include "ofMain.h"
#include "common.h"
#include "ofxBiquadFilter.h"

class Scene03 : public commonFunctions{
    
public:

    shared_ptr<ofxBox2dCircle> average;
    ofxBiquadFilter2f filter;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/03_maze.svg");
        polys = getPolyline(svg);
        
        ofxSVG svg2;
        svg2.load("svg/03_win_area.svg");
        winningArea = getPolyline(svg2)[0].getBoundingBox();
        
        filter.setFc(0.05f);

    };
    bool isDone(){
        bool isInside=true;
        bool allAreDead=true;
        isInside = winningArea.inside(average->getPosition());
       
//        if(isInside){
//            for(int i = 0; i<buttons->size();i++){
//                
//                if(buttons->at(i).isPlaying){
//                    isInside=false;
//                    break;
//                }
//                
//                if(!buttons->at(i).isDead()){
//                    allAreDead =false;
//                }
//            }
//        }
        
       // if(allAreDead)isInside=false;
        return isInside;
    };
    
    
    void update(){
        float x = 0.0;
        float y = 0.0;
        float indx = 0.0;
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction ); // the is on is checked in button!
                
                if(buttons->at(i).isPlaying){ // its on, not dead. (if it is not playing, it can not be on)
                    x+=buttons->at(i).getBiquadPos().x;
                    y+=buttons->at(i).getBiquadPos().y;
                    indx++;
                }
            }
            
            average->setVelocity(0,0);
            if(indx>0)average->addAttractionPoint(ofPoint(x/indx , y/indx),co->attraction);
            filter.update(average->getPosition());
        }
    }

    void draw(){
        
        ofSetColor(255);
        svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }

        
     
        ofPushMatrix();
        ofTranslate(filter.value());
        
//        //ofRotateZ(average->getRotation());
//        //average->draw();
        ofSetColor(255);
        int rad = average->getRadius();
        co->avergaTick.draw(-rad,-rad,rad*2,rad*2);
//        
//        //body
//        ofSetColor(100);
//        ofDrawCircle(0,0,rad);
//        
//        //head
//        ofSetColor(180);
//        float eye = -rad + rad*0.45/2;
//        ofDrawEllipse(0,eye, rad*0.85,rad*0.45);
//        
//        //eyes
//        if(ofRandom(1000)>0.1){
//            ofSetColor(0);
//            ofDrawCircle(rad*0.3 ,eye, 7);
//            ofDrawCircle(-rad*0.3,eye ,7);
//        }
//        //stomach
//        ofSetColor(255);
//        ofDrawEllipse(0,rad*0.35,rad*1.5,rad*1.4);
        ofPopMatrix();
        
        

    };

    
    void begin(ofxBox2d * world){
        average = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //virtual void setPhysics(float density, float bounce, float friction);
        average.get()->setPhysics(3.0, 0.0, 400.0);
        average.get()->setup(world->getWorld(), 1920-120, 1080-120, 10);
        average->setRadius(45.f);
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                if(buttons->at(i).getPosRaw().x>0)buttons->at(i).setPosition( 1920-40 , buttons->at(i).getPosRaw().y);
                else buttons->at(i).setPosition(1920-40,ofRandom(100,1080-100));
            }
        }
    };
    void reset(){
        average->destroy();
    }
    
    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    vector<Button>*buttons;
    

    ofVec2f averagePos = ofVec2f(1920/2.f,1080.f);
    ofxBiquadFilter2f filterLowPass;
    float fc;
    
    ofRectangle winningArea;
    
};
