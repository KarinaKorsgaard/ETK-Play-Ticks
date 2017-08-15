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
    vector<ofPolyline> solidPolysFull;
    
    bool done = false;
    int team;
    int basketEfficency = 0;
    
    ofRectangle win;
    
    bool doneResult;
    
    void setup(commonObjects*_co, vector<Button>*b, int _team){
        buttons = b;
        co = _co;
        svg.load("svg/10_FactorySolids.svg");
        solidPolysFull = getPolyline(svg);
        team = _team;
        
        ofxSVG svg2;
        svg2.load("svg/10_FactoryAreas.svg");
        polysOutline = getPolyline(svg2);
        
        //cout << polysOutline.size() << " num baskets" << endl;
        
        /*
         X people present
         F amount factories
         Fs factory slots
         FnS factroy number n amount of slots
         
         F = max(lookup[X-1][0],lookup[X-1][1])
         FsTotal = X-1
         FsMax = min(lookup[X-1][0],lookup[X-1][1])
         
         count = 0;
         if(count < FSTotal)
         FnS1 = FsMax;
         cont += FnS1;
         else
         FnS1 = FSTotal - count;
         
         */

        
        
//        ofxSVG svg3;
//        svg3.load("svg/10_FactoryButton.svg");
//        win = getPolyline(svg3)[0].getBoundingBox();
//        win.x += 1920 * teamNumber;
//        winImg.load("img/06_win.png");
        
    };
    
   
    
    bool isDone(bool b = false){
        done = true;
        for(int i = 0; i<baskets.size();i++)
            if(!baskets[i].bucketIsFull)done = false;
        if(b)done = b;
        return done;
//        if(b) {
//            done = true;
//            return true;
//        }
//        
//        else{
//            recDone = false;
//            
//            for(int i = 0 ; i<buttons->size();i++){
//                if(buttons->at(i).on && !buttons->at(i).isDead()){
//                    
//                    ofVec2f a = ofVec2f(win.getCenter())-buttons->at(i).getBiquadPos();
//                    float dist = a.dot(a);
//                    
//                    if( dist < win.width/2 ){
//                        recDone = true;
//                    }
//                }
//            }
//            
//            if(recDone){
//                countDown+=ofGetLastFrameTime();
//            }else countDown = 0.;
//            
//            done = countDown > 6.;
//            return countDown > 6.;
//        }
    };
    
    void update(){
//        if (done) {
//            givePoints();
//        }
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
                if(!buttons->at(b).on || !buttons->at(b).isPlaying || buttons->at(b).isDead())continue;
                
                if(buttons->at(b).on && !done){
                    if(baskets[i].isIn(buttons->at(b).getBiquadPos())){
                        baskets[i].buttons.push_back(buttons->at(b));
                    }
                }
            }
            
            baskets[i].setEfficiency();
            //baskets[i].calculateOutput();
            
//            if(baskets[i].p_efficiency != baskets[i].getEfficiency()){
//                ofxOscMessage m;
//                m.setAddress("/factory"+ofToString(i)+"team"+ofToString(team));
//                m.addIntArg(baskets[i].getEfficiency());
//                co->oscOut.sendMessage(m);
//                baskets[i].p_efficiency = baskets[i].getEfficiency();
//            }
        }
    }

    
//    void givePoints(){
//        if(!stopGivingPoints){
//            stopGivingPoints = true;
//            basketEfficency = 0;
//            
//            for(int i = 0; i<baskets.size();i++){
//                basketEfficency += baskets[i].getEfficiency();
//                //cout << "take from team time"<< endl;
//                
//                for(int j=0; j<buttons->size(); j++){ // ??
//                    if(baskets[i].isIn(buttons->at(j).getBiquadPos())){
//                        buttons->at(j).multValue(baskets[i].getEfficiency()*co->marketReward);
//                    }
//                }
//            }
//            cout << basketEfficency << endl;
//        }
//    }
    
    
    void draw(){

        //int red = CLAMP(int(ofMap(countDown,3,0,0,255)),50,200);
        
        //ofSetColor(red,255-red,50);
        //winImg.draw(win);
        
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
        }

        
        for(int i = 0; i<baskets.size();i++){
            ofFill();
            baskets[i].drawDots();

//            if(co->debug){
//                ofPushMatrix();
//                ofTranslate(baskets[i].center);
//             //   ofSetColor(0);
//             //   co->font_small->drawString(ofToString(baskets[i].getEfficiency())+","+ofToString(baskets[i].buttons.size()) ,0,0);
//                ofPopMatrix();
//            }
        }
        
    };
    
    
    void begin(ofxBox2d * world = nullptr){
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
        //for(int i = 0; i<baskets.size();i++)
        //    baskets[i].setColandSymbol(co->numSymbolsPresent[teamNumber],co->numColorsPresent[teamNumber]);
        
    };
    
    void reset(){
        baskets.clear();
    };
    

    
};
