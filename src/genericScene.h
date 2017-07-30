//
//  genericScene.h
//  _ETK_March_2017
//
//  Created by Karina Jensen on 30/07/2017.
//
//

#ifndef genericScene_h
#define genericScene_h

#include "ofMain.h"
#include "common.h"
#include "ofxBiquadFilter.h"

class GenericScene : public commonFunctions{
    
public:

    commonObjects * co;
  
    vector<ofPolyline> svgPolys;
    
    
    vector<Button>*buttons;
    
    shared_ptr<ofxBox2dCircle> average;
    ofVec2f averagePos = ofVec2f(1920/2.f,1080.f);
    
    ofxBiquadFilter2f filter;
    ofxBiquadFilter2f filterLowPass;
    float fc;
    
    ofRectangle winningArea;
    ofRectangle startArea;

    bool isAverage;
    bool isDoneStatic;
    

    void setup(commonObjects*_co, vector<Button>*b, bool _isAverage , string svgMaze = "", string startAreaSvg = "", string endAreaSvg = ""){
        buttons = b;
        co = _co;
        
        isAverage = _isAverage;
        

        ofxSVG svg;
        if(ofFile::doesFileExist(svgMaze)){
            svg.load(svgMaze);
            svgPolys = getPolyline(svg);
        }
      
        
        vector<ofPolyline> tempPolys;
        if(ofFile::doesFileExist(startAreaSvg)){
            svg.load(startAreaSvg);
            tempPolys = getPolyline(svg);
            startArea = tempPolys[0].getBoundingBox();
            tempPolys.clear();
        }
        
        if(ofFile::doesFileExist(endAreaSvg)){
            svg.load(endAreaSvg);
            tempPolys = getPolyline(svg);
            winningArea = tempPolys[0].getBoundingBox();
            tempPolys.clear();
        }
        
        
        filter.setFc(0.05f);
        
    }
    
    void isDone(){
        bool isInside=true;
        
        // all playing must always be on
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying && !buttons->at(i).on){
                isInside=false;
                break;
            }
        }
        
        
        if (isAverage && isAverageReadyToWin){
            isInside = winningArea.inside(average->getPosition());
        }
        else {
            for(int i = 0; i<buttons->size();i++){
                if(buttons->at(i).isPlaying){
                    isInside = winningArea.inside(buttons->at(i).getBiquadPos());
                    break;
                }
            }
        }
        isDoneStatic = isInside;
    }
    
    
    void update(){
        isDone();
        if(!isDoneStatic){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction ); // the is on is checked in button!
            }
        }
        
        if(isAverage){
            
            float x = 0.0;
            float y = 0.0;
            float indx = 0.0;
            
            for(int i=0; i<buttons->size(); i++) {
                if(buttons->at(i).isPlaying){ // its on, not dead. (if it is not playing, it can not be on)
                    x+=buttons->at(i).getBiquadPos().x;
                    y+=buttons->at(i).getBiquadPos().y;
                    indx++;
                }
            }
            
            average->setVelocity(0,0);
            
            if (indx>0){
                average->addAttractionPoint(ofPoint(x/indx , y/indx),co->attraction);
            }
            
            filter.update(average->getPosition());
        }
    }

    void draw(){
        ofSetColor(255);

        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
        
        if(isAverage){
            ofPushMatrix();
            ofTranslate(filter.value());
            ofSetColor(255);
            float rad = average->getRadius() * 2.;
            co->avergaTick.draw(-rad,-rad,rad*2,rad*2);
            ofPopMatrix();
        }
    }
    
    void begin(ofxBox2d * world){
        if(isAverage){
            average = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
            average.get()->setPhysics(3.0, 0.0, 40.0);
            average.get()->setup(world->getWorld(), 1920-120, 1080-120, 25);
        }
        
        for(int i = 0; i<buttons->size();i++){
            if(startArea != null){
                float x = startArea.x + ofRandom(startArea.width);
                float y = startArea.y + ofRandom(startArea.height);
                
                if(buttons->at(i).isPlaying){
                    buttons->at(i).setPosition(x,y);
                }
            }
        }
    }
    
    void reset(){
        if(isAverage)
            average->destroy();
        isDoneStatic = false;
        isAverageReadyToWin = false;
        
    }
    
    
    
};


#endif /* genericScene_h */
