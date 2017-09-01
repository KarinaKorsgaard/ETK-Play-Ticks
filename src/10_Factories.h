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
    int factoryWidth;
    int slots;
    bool bucketIsFull;
    int amountOfDots = 0;
    vector<Button>buttons;
    int numSymbolsPresent, numColorsPresent;
    
    
    
    void setup(ofPolyline p, int _slots){
        poly = p;
        slots = _slots;
  
        center = poly.getCentroid2D();
        factoryWidth = poly.getBoundingBox().width;
        top = poly.getBoundingBox().y;
        
        bucketIsFull = false;
    }
    
    bool isIn(ofPoint p){
        return poly.inside(p);
    }
    
    void setColandSymbol(int _numSymbolsPresent, int _numColorsPresent){
        numSymbolsPresent=_numSymbolsPresent;
        numColorsPresent=_numColorsPresent;
    }
    
    void setEfficiency(){

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

    void drawDots(float factoryRotation){
        int spacing = 10;
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(center);
        ofRotateZ(factoryRotation);
        ofSetLineWidth(5);
        ofFill();
        ofSetColor(255,0,0);
        for(int i = 0; i< slots * 2 ;i++){
            ofPushMatrix();
            if(i+1>amountOfDots)
                ofNoFill();
            int y = (i/slots)*30;
            
            if(i%2 == 0) ofRotateZ( spacing * i/2);
            else ofRotateZ(360 - spacing * (i-1)/2 - 100);
            ofDrawCircle(factoryWidth / 2 + 10, 0, 10);
            ofPopMatrix();
            
        }
        ofPopStyle();
        ofPopMatrix();
    }
    
};


class Factories : public Scene{
    
public:
    vector<Basket>baskets;

    vector<ofPolyline> polysOutline;

    vector<bool>empolyedTicks;
    
    bool done = false;
    ofImage overlay;
    int looserId;
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
            for(int i = 0; i<empolyedTicks.size();i++)
                empolyedTicks[i] = false;
            
            updateBaskets();
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).update(co->attraction);
            }
        }
         //   cout << looserId << "is looser team "<< teamNumber << " from factories"<< endl;
    }
    
    void updateBaskets(){
        for(int i = 0; i< baskets.size();i++){
            baskets[i].buttons.clear();
            
            for(int b = 0; b<buttons->size(); b++) {
                
                if( buttons->at(b).isPlaying ){
                    if(baskets[i].isIn(buttons->at(b).getBiquadPos())){
                        baskets[i].buttons.push_back(buttons->at(b));
                        
                        empolyedTicks[b] = true;

                    }
                }
            }
            baskets[i].setEfficiency();
            
            ofxOscMessage m;
            m.setAddress("/Factory"+ofToString(i+1)+"Team"+ofToString(teamNumber+1));
            m.addIntArg(baskets[i].amountOfDots);
            co->oscOut.sendMessage(m);
            

        }
        for(int i = 0; i<empolyedTicks.size();i++){
            if (!empolyedTicks[i] && buttons->at(i).isPlaying ){
                co->isUnemployed[teamNumber] = ofPoint( buttons->at(i).table , buttons->at(i).ID );
                buttons->at(i).isLooser = true;
                looserId = i;
            }else{
                buttons->at(i).isLooser = false;
            }
        }
    }


    void draw(){

       
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }

        ofSetColor(255,0,0);
        for(int i = 0; i<baskets.size();i++){
            ofFill();
            baskets[i].drawDots(co->factoryRotation[i]);

        }
        ofSetColor(255);
        overlay.draw(teamNumber * 1920, 0,1920, 1080);
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
        ofxSVG svg;
        svg.load("svg/10_FactorySolids.svg");
        solidPolys = getPolyline(svg);
        
        ofxSVG svg2;
        svg2.load("svg/10_FactoryAreas.svg");
        polysOutline = getPolyline(svg2);
        
        overlay.load("img/specialAssets/09_FactoriesOverlay.png");
        
        int X = co->numPresentButtons[teamNumber];
        int F = max(co->lookUp[X][0],co->lookUp[X][1]);
        int FsTotal = X-1;
        cout << FsTotal << endl;
        int FsMax = min(co->lookUp[X][0],co->lookUp[X][1]);
        
        int count = 0;
        for(int i = 0;i<F;i++){
            int slots = 0;
            
            if(count < FsTotal-FsMax){
                slots = FsMax;
                count += slots;
                cout << "COUNT: "<<count<<endl;
            }
            else{
                slots = FsTotal - count;
                count += slots;
                cout << "REST OF THE SLOTS: "<<count<<endl;
            }
            if(count > FsTotal)slots -= count-FsTotal;
            Basket basket;
            baskets.push_back(basket);
            baskets.back().setup(polysOutline[i], slots);
        
            cout << "BASKET: "<<i<<" slots_ "<<slots<<endl;
        }
        
        co->marketDone1=false;
        co->marketDone2=false;
        
        empolyedTicks.resize(36);
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                float addX = teamNumber == 0 ? 0 : 1920;
                buttons->at(i).isLooser = false;
                float p = ofRandom(-100,100);
                buttons->at(i).setPosition(1920/2 + addX + p, 1080/2 + p);
                
            }
        }
    };
    
    void reset(){
        empolyedTicks.clear();
     
        polysOutline.clear();
        baskets.clear();
        solidPolys.clear();
        overlay.clear();
    };
    

    
};
