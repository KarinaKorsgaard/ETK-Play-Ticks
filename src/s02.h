#include "ofMain.h"
#include "common.h"

class Scene02 : public commonFunctions{
    
public:
    
    vector<Button>*buttons;
    ofRectangle small, big;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    };
    bool isDone(){
        bool isInside=true;
        int moreThanOne = 0;
        
        for(int i = 0; i<buttons->size();i++){
            Button *b = &buttons->at(i);
        
            if(b->isPlaying){ // if b is not playing or is dead dont account.
                moreThanOne++;
                ofPoint p =b->getBiquadPos();
                if( small.inside(p) ){
                    isInside=false;
                    //break;
                }
                if(!big.inside(p)){
                    isInside=false;
                    //break;
                }
            }

        }
        if(moreThanOne==0)isInside=false;
        return isInside;
    };
    
    
    void update(){
        if(!isDone()){
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
    }

    
    void draw(){
        
        ofSetColor(255);
      //  borders.draw(0,0);
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetLineWidth(4);
                ofDrawLine(0,polys[0].getBoundingBox().y,1920,polys[0].getBoundingBox().y);
                
                ofDrawRectangle(big);
                ofDrawRectangle(small);
                ofSetColor(255);
                ofSetLineWidth(40);
                for(int i = 0 ; i<polys.size() ; i++)polys[i].draw();
            }
        }
        
//        ofSetColor(ofColor::royalBlue);
//        for(int i = 0; i< head.size();i++)
//            co->font_medium->drawString(head[i], 50, 100+i*co->font_medium->getLineHeight());
//        
//        for(int i = 0; i< bread.size();i++)
//            co->font_small->drawString(bread[i], 50, 180+i*co->font_small->getLineHeight());
    };

    
    void begin(){
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                if(buttons->at(i).getPosRaw().x>0)buttons->at(i).setPosition(buttons->at(i).getPosRaw().x, 1040);
                else buttons->at(i).setPosition(ofRandom(100,1920-100), 1040);
            }
        }
        done = false;
        
        svg.load("svg/02_borders.svg");
        polys = getPolyline(svg);
        
        ofxSVG svg2;
        svg2.load("svg/02_win_rects.svg");
        vector<ofPolyline>p = getPolyline(svg2);
        
        ofRectangle a = p[0].getBoundingBox();
        ofRectangle o = p[1].getBoundingBox();
        
        if(a.width>o.width){
            big = a;
            small = o;
        }
        else{
            big = o;
            small = a;
        }
        
    };
    void reset(){
        polys.clear();
    };
  //  ofImage borders;
    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    bool done = false;
};
