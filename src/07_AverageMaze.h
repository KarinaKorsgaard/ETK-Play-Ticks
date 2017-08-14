#include "ofMain.h"
#include "Scene.h"
#include "ofxBiquadFilter.h"

class AverageMaze : public Scene{
    
public:
    float upThres;
    shared_ptr<ofxBox2dCircle> average;
    ofxBiquadFilter2f filter;
    

    ofxSVG svg;
    
    
    
    ofVec2f averagePos = ofVec2f(1920/2.f,1080.f);
    ofxBiquadFilter2f filterLowPass;
    float fc;
    
    ofRectangle winningArea;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/03_maze_alt.svg");
        solidPolys = getPolyline(svg);
        
        float min = 1080;
        for(int i = 0; i<solidPolys.size();i++){
            float x = solidPolys[i].getBoundingBox().y;
            if(x>10 && x<min )min = x;
        }
        
        upThres = min;
        filter.setFc(0.05f);
        
    };
    
    bool isDone(bool b = false){
        bool isInside=true;
        bool allAreDead=true;
        isInside = average->getPosition().y<upThres;
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
      //  svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
        
        
        
        ofPushMatrix();
        ofTranslate(filter.value());
        ofSetColor(255);
        float rad = average->getRadius() * 2.;
        co->avergaTick.draw(-rad,-rad,rad*2,rad*2);
        ofPopMatrix();
        
        
        
    };
    
 
    
    void begin(ofxBox2d * world = nullptr){
        
        float addX = teamNumber == 0 ? 0 : 1920;
        
        average = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        //density, bounce, friction
        average.get()->setPhysics(3.0, 0.0, 40.0);
        average.get()->setup(world->getWorld(), 1920-120 + addX, 1080-120, 25);
       // average->setRadius(45.f);
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                
                
                if(buttons->at(i).getPosRaw().y>0)buttons->at(i).setPosition(
                                                                             buttons->at(i).getPosRaw().x + addX,
                                                                             1080 - 100);
                else buttons->at(i).setPosition(
                                                ofRandom(100,1920-100) +
                                                addX,1080 - 100);
            }
        }
    };
    
    void reset(){
        average->destroy();
    }
    

    
};
