#include "ofMain.h"
#include "common.h"
#include "ofxSvg.h"
//market

class Basket{
public:
    vector<Button>buttons;
    ofPolyline poly;
    float output=0;
    int efficiency;
    ofPoint center;
    
    vector<ofPoint>spaces;
    vector<ofPoint>spacesToDraw;
    
    void setup(ofPolyline p){
        poly=p;
        center=poly.getCentroid2D();
        for(int i = 0; i<6;i++)spaces.push_back(ofPoint(-1,-1));
    }
    bool isIn(ofPoint p){
        return poly.inside(p);
    }
    
    int coef(){
        // spaces taken. if space is taken you minus, if not you add.
      
        for(int i = 0; i<6;i++)spaces[i]=(ofPoint(-1,-1));
        spacesToDraw = spaces;
        for(int i = 0; i<buttons.size();i++){
            
            int b_id  = buttons[i].ID;
            int b_tab = buttons[i].table;
            
            if(spaces[b_id].x == -1){
                spaces[b_id].x = 1;
                spacesToDraw[b_id].x=1;
             //   result ++;
            }else{
                spacesToDraw[b_id].x=-1;
               // result --;
            }
            
            if(spaces[b_tab].y == -1){
                spaces[b_tab].y = 1;
                spacesToDraw[b_id].y=1;
              //  result ++;
            }else{
                spacesToDraw[b_id].y=-1;
              //  result --;
            }
        }
        efficiency=0;
        for(int i = 0; i<spacesToDraw.size();i++){
            if(spacesToDraw[i].x==1)efficiency++;
            if(spacesToDraw[i].y==1)efficiency++;
        }
        
       
        return efficiency;
    }
    void calculateOutput(){
        output = 0;
        float c = coef();
        for(int i =0; i<buttons.size();i++)output+=buttons[i].getValue()*c;
    }
    
    // output is lala. Not necessary I guess... ? - It is a vector of values added to the buttons in the basket?
    void drawDots(){
        ofPushMatrix();
        ofTranslate(center.x-30*spacesToDraw.size()/2,center.y);

        int indx=0;
        for(int i = 0; i<spacesToDraw.size();i++){
                int red = spacesToDraw[i].x==-1 ?  230:25;
                ofSetColor(red,255-red,150);
                ofDrawCircle(i*30,0,10);
            
                red = spacesToDraw[i].y==-1 ?  255:0;
                ofSetColor(red,255-red,150);
                ofDrawCircle(i*30,30,10);

        }
       // ofPopMatrix();
    }
    
};


class Scene06 : public commonFunctions{
    
public:
    
    vector<Button>*buttons;
    ofImage basket;
    ofImage basketTop;

    float timeGained;
    map <int,ofVec3f> buttonInfo;
    double countDown;
    double static_countDown;
    bool recDone = false;
    bool stopGivingPoints = false;
    vector<Basket>baskets;
    
    ofRectangle win;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/06_solid_objects.svg");
        polys = getPolyline(svg);

        
        ofxSVG svg2;
        svg2.load("svg/06_factory_areas.svg");
        polysOutline = getPolyline(svg2);
        for(int i = 0;i<polysOutline.size();i++){
            Basket basket;
            baskets.push_back(basket);
            baskets.back().setup(polysOutline[i]);
        }
        
        basket.load("img/06_factories.png");
        basketTop.load("img/06_factories_top_layer.png");
        
        ofxSVG svg3;
        svg3.load("svg/06_win_rect.svg");
        win = getPolyline(svg3)[0].getBoundingBox();
        
        winImg.load("img/06_win.png");
        
    };
    
    bool isDone(bool forceTrue = false){
        if(forceTrue) return true;
        else return countDown > 3.;
    };
    
    void update(){
        
        
        recDone = false;
        
        for(int i = 0 ; i<buttons->size();i++){
            if(win.inside(buttons->at(i).getBiquadPos())){
                recDone = true;
            }
        }
        
        if(recDone){
            countDown+=ofGetLastFrameTime();
        }else countDown = 0.;
        
        
        if(isDone()){
            givePoints();
        }else{
            updateBaskets();
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
        
        
        
    }
    void updateBaskets(){
        
        for(int i = 0; i< baskets.size();i++){
            baskets[i].buttons.clear();
            for(int b=0; b<buttons->size(); b++) {
                if(!buttons->at(b).on || !buttons->at(b).isPlaying || buttons->at(b).isDead())continue;
                
                if(buttons->at(b).on && !isDone()){
                    if(baskets[i].isIn(buttons->at(b).getBiquadPos())){
                        baskets[i].buttons.push_back(buttons->at(b));
                    }
                }
            }
            baskets[i].calculateOutput();
        }
    }

    
    void givePoints(){
        if(!stopGivingPoints){
            stopGivingPoints=true;
            for(int i = 0; i<baskets.size();i++){
                for(int j=0; j<buttons->size(); j++){ // ??
                    if(baskets[i].isIn(buttons->at(j).getBiquadPos())){
                        buttons->at(j).multValue(baskets[i].efficiency*co->marketReward);
                    }
                }
            }
        }
    }
    
    
    void draw(){
        

        //ofSetColor(255,200);
        //basketTop.draw(0,0);
        
        int red = CLAMP(int(ofMap(countDown,3,0,0,255)),50,200);
        ofSetColor(red,255-red,50);
        
        winImg.draw(win);
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                
                ofPushMatrix();
                ofSetColor(0);
                ofTranslate(buttons->at(i).getBiquadPos());
                co->font_medium->drawString(ofToString(buttons->at(i).ID)+","+ofToString(buttons->at(i).table),0,0);
                ofPopMatrix();
                
                ofSetColor(255);
                ofNoFill();
                ofSetLineWidth(10);
                for(int i = 0; i<polysOutline.size();i++)polysOutline[i].draw();
                
                ofFill();
                ofSetLineWidth(1);
                
            }
        }
        

        
        ofSetColor(255,200);
        basketTop.draw(0,0);
        
        for(int i = 0; i<baskets.size();i++){
            baskets[i].drawDots();

            ofPopMatrix();
            if(co->debug){
                ofPushMatrix();
                ofTranslate(baskets[i].center);
                ofSetColor(0);
                co->font_medium->drawString(ofToString(baskets[i].output,0)+","+ofToString(baskets[i].coef())+","+ofToString(baskets[i].buttons.size()) ,0,0);
                ofPopMatrix();
            }
        }
        
    };
    
    
    void begin(){
        stopGivingPoints = false;
        countDown = 0.;
    };
    
    void reset(){
        recDone = false;
        co->marketDone1=false;
        co->marketDone2=false;
    };
    
    commonObjects * co;
    ofxSVG svg;
    ofImage winImg;
    vector<ofPolyline> polys;
    vector<ofPolyline> polysOutline;
    
    bool done = false;

    
};
