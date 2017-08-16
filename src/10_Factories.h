#include "ofMain.h"
#include "Scene.h"
#include "ofxSvg.h"


//market

class Basket{
private:

    ofPolyline poly;
    int top;


public:
    
    int p_efficiency;
    ofPoint center;
    int slots;
    bool bucketIsFull;
    int amountOfDots = 0;
    vector<Button>buttons;
    int numSymbolsPresent, numColorsPresent;
    
    void setup(ofPolyline p, int _slots){
        poly = p;
        slots = _slots;
        
        center = poly.getCentroid2D();
        top = poly.getBoundingBox().y;
        

    }
    
    bool isIn(ofPoint p){
        return poly.inside(p);
    }
    
    void setColandSymbol(int _numSymbolsPresent, int _numColorsPresent){
        numSymbolsPresent=_numSymbolsPresent;
        numColorsPresent=_numColorsPresent;
    }
    
    void setEfficiency(){
        // spaces taken. if space is taken you minus, if not you add.
      
        //s[0].x = white, star
        //s[1].x = red, square
        //s[2].x = blue, triangle
        

        
        
        vector<int>colorsTaken;
        vector<int>symbolsTaken;
        amountOfDots = 0;
        
        if(buttons.size()<=slots){
            for(int i = 0; i<buttons.size();i++){
                
                int b_col  = buttons[i].colorInt;
                int b_sym = buttons[i].symbolInt;
                
                bool addDot = true;
                for(int i = 0; i<colorsTaken.size();i++)
                    if(b_col == colorsTaken[i]){
                        //amountOfDots--;
                        addDot = false;
                    }
                if(addDot){
                    amountOfDots++;
                    colorsTaken.push_back(b_col);
                }
                
                addDot = true;
                for(int i = 0; i<symbolsTaken.size();i++)
                    if(b_sym == symbolsTaken[i]){
                        //amountOfDots--;
                        addDot = false;
                    }
                if(addDot){
                    amountOfDots++;
                    symbolsTaken.push_back(b_sym);
                }
            }
        }
        
        bucketIsFull = amountOfDots == slots * 2;
    }

    void drawDots( ){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(center.x-(30.f*float(slots*2))/2.f,top - 70);

        ofFill();
        for(int i = 0; i< slots * 2 ;i++){
            if(i+1>amountOfDots)
                ofNoFill();
            int y = (i/slots)*30;
            
            ofDrawCircle(i*30 - (int((i/slots))*slots*30) ,y,10);
            
        }
        ofPopStyle();
        ofPopMatrix();
    }
    
};


class Factories : public Scene{
    
public:
    vector<Basket>baskets;

    vector<ofPolyline> polysOutline;
    vector<ofPolyline> solidPolysFull;
    
    bool done = false;
    
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    };
    
    bool isDone(bool b = false){
        done = true;
        for(int i = 0; i<baskets.size();i++)
            if(!baskets[i].bucketIsFull)done = false;
        if(b)done = b;
        if(baskets.size() == 0)done = false;
        return done;

    };
    
    void update(){
        if(!done) {
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
                if(!buttons->at(b).on || !buttons->at(b).isPlaying || buttons->at(b).isDead())
                    continue;
                
                if(buttons->at(b).on && !done){
                    if(baskets[i].isIn(buttons->at(b).getBiquadPos())){
                        baskets[i].buttons.push_back(buttons->at(b));
                    }else{
                        co->isUnemployed[teamNumber] = ofPoint( buttons->at(b).table , buttons->at(b).ID );
                    }
                }
            }
            
            baskets[i].setEfficiency();

        }
    }


    void draw(){

        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }

        
        for(int i = 0; i<baskets.size();i++){
            ofFill();
            baskets[i].drawDots();

        }
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
        ofxSVG svg;
        svg.load("svg/10_FactorySolids.svg");
        solidPolysFull = getPolyline(svg);
        
        ofxSVG svg2;
        svg2.load("svg/10_FactoryAreas.svg");
        polysOutline = getPolyline(svg2);
        
        
        int X = co->numPresentButtons[teamNumber];
        int F = max(co->lookUp[X][0],co->lookUp[X][1]);
        int FsTotal = X-1;
        int FsMax = min(co->lookUp[X][0],co->lookUp[X][1]);
        
        int count = 0;
        for(int i = 0;i<F;i++){
            int slots = 0;
            
            if(count < FsTotal-FsMax){
                slots = FsMax;
                count += slots;
            }
            else
                slots = FsTotal - count;
            
            Basket basket;
            baskets.push_back(basket);
            baskets.back().setup(polysOutline[i], slots);
            
            cout << "BASKET: "<<i<<" slots_ "<<slots<<endl;
        }
        
        co->marketDone1=false;
        co->marketDone2=false;
        
        solidPolys = solidPolysFull;
        solidPolys.resize(F);

        
    };
    
    void reset(){
        solidPolysFull.clear();
        polysOutline.clear();
        baskets.clear();
    };
    

    
};
