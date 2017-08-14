// gravity!!

#include "ofMain.h"
#include "Scene.h"

class Fences : public Scene{
    
public:
    
    ofRectangle winningArea;
    int theWinner;
    double isDoneCounter;

    vector<shared_ptr<ofxBox2dRect>>escalators;
    vector<ofPolyline>movingPolys;

    vector<int>upDown;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        ofxSVG movingSvg;
        movingSvg.load("svg/08_EscalatorMoving.svg");
        movingPolys = getPolyline(movingSvg);
        
        
        ofxSVG svg2;
        svg2.load("svg/08_EscalatorWinArea.svg");
        winningArea = getPolyline(svg2)[0].getBoundingBox();
        
        ofxSVG svg;
       // svg.load("svg/05_fences.svg");
        svg.load("svg/08_EscalatorSolids.svg");
        
        solidPolys = getPolyline(svg);
        theWinner=-1;
        
    };
    
    bool reached(){
        bool isInside=false;
        for(int i = 0; i<buttons->size();i++){
            Button *b =&buttons->at(i);
            if(!b->isPlaying || !b->on)continue; // if b is not playing or not on, dont check.
            if(winningArea.inside(b->getBiquadPos())){
                isInside = true;
                theWinner = i;
                break;
            }
        }
        return isInside;
    };
    
    bool isDone(bool b = false){
        if(reached())isDoneCounter+=ofGetLastFrameTime();
        else isDoneCounter = 0;
        return isDoneCounter>5.f;
    }
    
    void update(){
        if(!isDone()){ // freeze when has reached...
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).updateFences(co->attraction);
                
            }
        }else{
            buttons->at(theWinner).isWinner = true;
        }
        
        for(int i = 0; i<escalators.size();i++){
            escalators[i]->setPosition(escalators[i]->getPosition() + ofVec2f(0,upDown[i]));
            if(escalators[i]->getPosition().y > 1080)upDown[i]=-1;
            if(escalators[i]->getPosition().y <0 )upDown[i]=1;
        }
    }
    
    void draw(){
        
        ofSetColor(255);

        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
                ofSetColor(255,255,0);
                if(theWinner!=-1)
                    ofDrawCircle(buttons->at(theWinner).getBiquadPos(),20);
            }
        }
        for(int i = 0; i< escalators.size();i++)
            escalators[i]->draw();
            
    };
    
    void begin(ofxBox2d * world = nullptr){
        
        float addX = teamNumber == 0 ? 0 : 1920;
        
        world->setGravity(0,co->gravity);
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                buttons->at(i).setPosition(ofRandom(60,400) + addX, 1080-110);
                buttons->at(i).isWinner = false;
            }
        }
        createScene(world,movingPolys);
    };
    void reset(){
        isDoneCounter=0.;
        for(int i = 0; i< escalators.size();i++)
            escalators[i]->destroy();
        
        escalators.clear();
    }
    
    
    
    void createScene(ofxBox2d * world ,vector<ofPolyline>polys){
        
        
        for(int i = 0 ;i <polys.size();i++){
            ofRectangle rect = polys[i].getBoundingBox();
            
            if(teamNumber == 1)rect.x+=1920;
            rect.x += rect.width/2;
            
            if(polys[i].getVertices().size()>3 && (rect.width<1900 && rect.height < 1070)){
                
                shared_ptr<ofxBox2dRect> r = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
                r->setPhysics(0., 1., 0.);
                r->setup(world->getWorld(), rect);
                
                escalators.push_back(r);
                upDown.push_back(1);
  
            }
        }
        
        //cout << ofToString(polyShapes.size())+ " fences size in " + co->sMap[co->sceneNumber]<< endl;
    }

};
