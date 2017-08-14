#include "ofMain.h"
#include "Scene.h"
#include "ofxSvg.h"


//market

class Basket{
private:
    int efficiency;
    ofPolyline poly;
    
    int top;
    vector<ofPoint>spaces;
    vector<ofPoint>spacesToDraw;
    

public:
    
    int p_efficiency;
    ofPoint center;
    vector<Button>buttons;
    int numSymbolsPresent, numColorsPresent;
    void setup(ofPolyline p, int teamNumber){
        poly = p;
        

        if(teamNumber == 1){
            for(int i = 0; i<poly.getVertices().size();i++)
                poly.getVertices()[i].x += 1920;
        }
        
        center = poly.getCentroid2D();
        top = poly.getBoundingBox().y;
        for(int i = 0; i<6;i++)
            spaces.push_back(ofPoint(-1,-1));
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
      
        for(int i = 0; i<6;i++)
            spaces[i]=(ofPoint(-1,-1));
        
        spacesToDraw = spaces;
        
        for(int i = 0; i<buttons.size();i++){
            
            int b_tab  = buttons[i].colorInt;
            int b_id = buttons[i].symbolInt;
            
            
            
            if(spaces[b_id].x == -1){
                spaces[b_id].x = 1;
                spacesToDraw[b_id].x=1;
             //   result ++;
            }else{
                spacesToDraw[b_id].x=-1;
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
        
        efficiency = 0;
        
        for(int i = 0; i<spacesToDraw.size();i++){
            if(spacesToDraw[i].x==1)efficiency++;
            if(spacesToDraw[i].y==1)efficiency++;
        }
        
    }
    float getEfficiency(){
        int maxE = numSymbolsPresent + numColorsPresent;
        float e = 12.f * (float(efficiency) / float(maxE));
        return  e;
    }

    
    // output is lala. Not necessary I guess... ? - It is a vector of values added to the buttons in the basket?
    void drawDots( ){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(center.x-(30.f*float(spacesToDraw.size()))/2.f,top - 70);

        int indx=0;
        int symbolCount = 0;
        int colors = 0;
        
        for(int i = 0; i<spacesToDraw.size();i++){
            
            ofSetColor(ofColor::royalBlue);
            ofSetLineWidth(5);
            
            if( colors < numColorsPresent ){
                bool fill = spacesToDraw[i].x==-1 ?  false:true;
                if(fill)ofFill();
                else ofNoFill();
                ofDrawCircle(i*30,0,10);
                
                colors++;
            }
            
            if( symbolCount < numSymbolsPresent ){
                bool fill = spacesToDraw[i].y==-1 ?  false:true;
                if(fill)ofFill();
                else ofNoFill();
                symbolCount++;
                
                ofDrawCircle(i*30,30,10);
            }

        }
        ofPopStyle();
        ofPopMatrix();
    }
    
};


class Factories : public Scene{
    
public:
    
    

    float timeGained;
    map <int,ofVec3f> buttonInfo;
    double countDown;
    double static_countDown;
    bool recDone = false;
    bool stopGivingPoints = false;
    vector<Basket>baskets;
    ofxSVG svg;
    ofImage winImg;
    vector<ofPolyline> polysOutline;
    
    bool done = false;
    int team;
    int basketEfficency = 0;
    
    ofRectangle win;
    
    bool doneResult;
    
    void setup(commonObjects*_co, vector<Button>*b, int _team){
        buttons = b;
        co = _co;
        svg.load("svg/10_FactorySolids.svg");
        solidPolys = getPolyline(svg);
        team = _team;
        
        ofxSVG svg2;
        svg2.load("svg/10_FactoryAreas.svg");
        polysOutline = getPolyline(svg2);
        
        cout << polysOutline.size() << " num baskets" << endl;
        for(int i = 0;i<polysOutline.size();i++){
            Basket basket;
            baskets.push_back(basket);
            baskets.back().setup(polysOutline[i], teamNumber);
        }
        
        ofxSVG svg3;
        svg3.load("svg/10_FactoryButton.svg");
        win = getPolyline(svg3)[0].getBoundingBox();
        win.x += 1920 * teamNumber;
        winImg.load("img/06_win.png");
        
    };
    
   
    
    bool isDone(bool b = false){
        if(b) {
            done = true;
            return true;
        }
        
        else{
            recDone = false;
            
            for(int i = 0 ; i<buttons->size();i++){
                if(buttons->at(i).on && !buttons->at(i).isDead()){
                    
                    ofVec2f a = ofVec2f(win.getCenter())-buttons->at(i).getBiquadPos();
                    float dist = a.dot(a);
                    
                    if( dist < win.width/2 ){
                        recDone = true;
                    }
                }
            }
            
            if(recDone){
                countDown+=ofGetLastFrameTime();
            }else countDown = 0.;
            
            done = countDown > 6.;
            return countDown > 6.;
        }
    };
    
    void update(){
        if (done) {
            givePoints();
        }
        else {
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
                
                if(buttons->at(b).on && !done){
                    if(baskets[i].isIn(buttons->at(b).getBiquadPos())){
                        baskets[i].buttons.push_back(buttons->at(b));
                    }
                }
            }
            
            baskets[i].setEfficiency();
            //baskets[i].calculateOutput();
            
            if(baskets[i].p_efficiency != baskets[i].getEfficiency()){
                ofxOscMessage m;
                m.setAddress("/factory"+ofToString(i)+"team"+ofToString(team));
                m.addIntArg(baskets[i].getEfficiency());
                co->oscOut.sendMessage(m);
                baskets[i].p_efficiency = baskets[i].getEfficiency();
            }
        }
    }

    
    void givePoints(){
        if(!stopGivingPoints){
            stopGivingPoints = true;
            basketEfficency = 0;
            
            for(int i = 0; i<baskets.size();i++){
                basketEfficency += baskets[i].getEfficiency();
                //cout << "take from team time"<< endl;
                
                for(int j=0; j<buttons->size(); j++){ // ??
                    if(baskets[i].isIn(buttons->at(j).getBiquadPos())){
                        buttons->at(j).multValue(baskets[i].getEfficiency()*co->marketReward);
                    }
                }
            }
            cout << basketEfficency << endl;
        }
    }
    
    
    void draw(){

        int red = CLAMP(int(ofMap(countDown,3,0,0,255)),50,200);
        
        ofSetColor(red,255-red,50); 
        winImg.draw(win);
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }

        
        for(int i = 0; i<baskets.size();i++){
            ofFill();
            baskets[i].drawDots();

            if(co->debug){
                ofPushMatrix();
                ofTranslate(baskets[i].center);
                ofSetColor(0);
                co->font_small->drawString(ofToString(baskets[i].getEfficiency())+","+ofToString(baskets[i].buttons.size()) ,0,0);
                ofPopMatrix();
            }
        }
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
        basketEfficency = 0;
        stopGivingPoints = false;
        countDown = 0.;
        recDone = false;
        co->marketDone1=false;
        co->marketDone2=false;
        for(int i = 0; i<baskets.size();i++)
            baskets[i].setColandSymbol(co->numSymbolsPresent[teamNumber],co->numColorsPresent[teamNumber]);
        
    };
    
    void reset(){

    };
    

    
};
