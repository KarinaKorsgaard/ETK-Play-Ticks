#include "ofMain.h"
#include "Scene.h"



class Area : public Scene{
    
public:
    vector<Button>*buttons;
    //Blob blob;
    ofImage donut;
    ofPolyline big,small;
    
    Area(){};
    ~Area(){};
    
    

    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        svg.load("svg/01_win_area.svg");
        donut.load("img/01_dounut.png");
        vector<ofPolyline> p = getPolyline(svg);
        
    
        float a;
        float o;
        int indx=0;
        int aa, oo;
        for(int i = 0; i<p.size();i++){
            float w = p[i].getBoundingBox().width;
            if(w<1900){
                if(indx == 0){a = w;aa = i;}
                else{ o = w;oo=i;}
                indx++;
            }
        }
        
        if(a>o){
            big = p[aa];
            small = p[oo];
        }
        else{
            big = p[oo];
            small = p[aa];
        }
    };
    
    bool isDone(bool b = false){
        bool isInside=true;
        bool allAreDead = true;
        for(int i = 0; i<buttons->size();i++){

//            if(!buttons->at(i).on && buttons->at(i).isPlaying){
//                isInside=false;
//                break;
//            }
            if(buttons->at(i).isPlaying){
                ofPoint p = buttons->at(i).getBiquadPos();
                
                if(!big.inside(p) || small.inside(p)){
                    isInside=false;
                    break;
                }
            }
            if(!buttons->at(i).isDead()){
                allAreDead =false;
            }
            
        }
        if(allAreDead)isInside = false;
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
      //  donut.draw(0,0);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                
                ofSetColor(255,0,0);
                big.draw();
                small.draw();
            }
        }
    };
    
    void begin(ofxBox2d * world = nullptr){};
    void reset(){};

    commonObjects * co;
    ofxSVG svg;
    ofPolyline poly;
    
    
};
