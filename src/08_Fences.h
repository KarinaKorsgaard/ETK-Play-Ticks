// gravity!!

#include "ofMain.h"
#include "Scene.h"

class Fences : public Scene{
    
public:
    
    ofRectangle winningArea;
    int theWinner;
    double isDoneCounter;
    float solidHeigth;

    vector<shared_ptr<ofxBox2dRect>>escalators;
    vector<ofPolyline>movingPolys;
    ofRectangle start;
    bool moveEscalators;

    vector<int>upDown;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        ofxSVG svg;
        
        svg.load("svg/08_EscalatorMoving.svg");
        movingPolys = getPolyline(svg);

        svg.load("svg/08_EscalatorWinArea.svg");
        winningArea = getPolyline(svg)[0].getBoundingBox();
        
        svg.load("svg/08_EscalatorStartArea.svg");
        start = getPolyline(svg)[0].getBoundingBox();
        
        svg.load("svg/08_EscalatorSolids.svg");
        solidPolys = getPolyline(svg);
        
        theWinner=-1;
        solidHeigth = solidPolys[0].getBoundingBox().height;
        
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
            moveEscalators = false;
            int count = 0;
            for(int i=0; i<buttons->size(); i++) {
                buttons->at(i).updateFences(co->attraction);
                
                if(!moveEscalators){
                    if (start.inside(buttons->at(i).getPos()))
                        count++;
                    if (count>3)
                        moveEscalators = true;
                }
            }
        }else{
            buttons->at(theWinner).isWinner = true;
        }
        if(moveEscalators){
            for(int i = 0; i<escalators.size();i++){
                escalators[i]->setPosition(escalators[i]->getPosition() + ofVec2f(0,upDown[i]));
                if(escalators[i]->getPosition().y > 1080 - solidHeigth - escalators[i]->getHeight())upDown[i]=-1;
                if(escalators[i]->getPosition().y < 0 )upDown[i]=1;
            }
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
        
        ofDrawRectangle(start);
    };
    
    void begin(ofxBox2d * world = nullptr){
        
        float addX = teamNumber == 0 ? 0 : 1920;
        
        world->setGravity(0,co->gravity);
        createScene(world,movingPolys);
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                buttons->at(i).setPosition(ofRandom(800,1120) + addX, 1080);
                buttons->at(i).isWinner = false;
                buttons->at(i).getOutOfPolys(solidPolys);
            }
        }
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
