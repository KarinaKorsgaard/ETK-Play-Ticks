#include "ofMain.h"
#include "common.h"

class Scene02alt : public commonFunctions{
    
public:
    
    vector<Button>*buttons;
    //ofRectangle small, big;
    float upThres;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        //borders.load("img/02_borders_alt.png");
        
        svg.load("svg/02_borders_alt.svg");
        polys = getPolyline(svg);
        
        float max = 0;
        for(int i = 0; i<polys.size();i++){
            float x = polys[i].getBoundingBox().y+polys[i].getBoundingBox().height;
            if(x<1060 && x>max )max = x;
        }
        upThres = max;
        cout << upThres<< endl;
        cout << "maze debugging " << endl;
        
    
    };
    bool isDone(){
        bool isInside=true;
        bool allAreDead = true;
        
        for(int i = 0; i<buttons->size();i++){
            Button *b = &buttons->at(i);
            if(!buttons->at(i).isDead()){
                allAreDead =false;
            }
            if(b->isPlaying /*&& !b->isDead()*/){ // if b is not playing or is dead dont account.
                
                ofPoint p =b->getBiquadPos();
//                if(!b->on){
//                    isInside=false;
//                    //break;
//                }
//                if( small.inside(p) ){
//                    isInside=false;
//                    //break;
//                }
//                if(!big.inside(p)){
//                    isInside=false;
//                    //break;
//                }
                if(p.y<upThres){
                    isInside=false;
                }
            }
            
        }
    //    if(allAreDead)isInside=false;
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
      //  svg.draw();
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetLineWidth(4);
                ofDrawLine(0,polys[0].getBoundingBox().y,1920,polys[0].getBoundingBox().y);
                
                //ofDrawRectangle(big);
               // ofDrawRectangle(small);
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
                if(buttons->at(i).getPosRaw().x>0)buttons->at(i).setPosition(buttons->at(i).getPosRaw().x, 50);
                else buttons->at(i).setPosition(ofRandom(100,1920-100), 50);
            }
        }
        done = false;
        

        
//        ofxSVG svg2;
//        svg2.load("svg/02_win_rects_alt.svg");
//        vector<ofPolyline>p = getPolyline(svg2);
//        
//        big = p[0].getBoundingBox();
//        ofRectangle o = p[1].getBoundingBox();
//        
//        if(a.width>o.width){
//            big = a;
//            small = o;
//        }
//        else{
//            big = o;
//            small = a;
//        }
        
    };
    void reset(){
        //polys.clear();
    };
    //ofImage borders;
    commonObjects * co;
    ofxSVG svg;
    vector<ofPolyline> polys;
    
    bool done = false;
};
