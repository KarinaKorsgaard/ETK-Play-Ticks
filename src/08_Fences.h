// gravity!!

#include "ofMain.h"
#include "Scene.h"

class Fences : public Scene{
    
public:
    
    ofRectangle winningArea;
    int theWinner;
    double isDoneCounter;
    float solidHeigth;
    ofxBox2d * thisWorld;

    ofImage escalatorImg[3];

    vector<shared_ptr<ofxBox2dRect>>escalators;
    
    vector<shared_ptr<ofxBox2dRect>>pitHole;
    vector<ofPolyline>movingPolys;
    vector<ofPolyline>pitPolys;
    vector<bool>inPits;
    ofRectangle start;
    double time;
    bool moveEscalators;
    bool p_moveEscalators;
    
    ofImage overlay;
    
    vector<int> buttonsInPits;

    vector<int>upDown;
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        ofxSVG svg;
        svg.load("svg/08_EscalatorWinArea.svg");
        winningArea = getPolyline(svg)[0].getBoundingBox();
        
        svg.load("svg/08_EscalatorStartArea.svg");
        start = getPolyline(svg)[0].getBoundingBox();
        
        inPits.resize(36);
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
            for(int i = 0;i<buttonsInPits.size();i++)
                buttonsInPits[i]=0;
            
            for(int i=0; i<buttons->size(); i++) {
                
                float attractionDivider = inPits[i] ? 0.001 : 1;
                buttons->at(i).updateFences(co->attraction * attractionDivider, co->gravity);
                
                bool isButtonInAPit = false;
                for(int pit = 0; pit < pitPolys.size();pit++){
                    
                    if (pitPolys[pit].inside(buttons->at(i).getBiquadPos())){
                        isButtonInAPit = true;
                        buttonsInPits[pit]++;
                    }
                }
                inPits[i] = isButtonInAPit;
                    
                
                
                if(!moveEscalators){
                    if (start.inside(buttons->at(i).getPos()))
                        count++;
                    if (count>3){
                        moveEscalators = true;
                    }
                }
                if(p_moveEscalators != moveEscalators){
                    p_moveEscalators = moveEscalators;
                    
                    ofxOscMessage m;
                    string  s = moveEscalators ? "On" : "Off";
                    m.setAddress("/elevator"+s+ofToString(teamNumber+1));
                    co->oscOut.sendMessage(m);
                
                }
                
            }
            
            for(int i = 0;i<buttonsInPits.size();i++){
                if (buttonsInPits[i]==4){
                    createPit(thisWorld, pitPolys[i]);
                    pitPolys.erase(pitPolys.begin() + i);
                }
            }
            
            
            
        }else{
            buttons->at(theWinner).isWinner = true;
        }
        if(moveEscalators){
            time+= ofGetLastFrameTime();
            for(int i = 0; i<escalators.size();i++){
                float ypos = abs(sin(time*co->escalatorSpeed)) * (1080 - i*200);
              //  escalators[i]->setPosition(escalators[i]->getPosition().x + ofVec2f(0,upDown[i] * co->escalatorSpeed *10.f* ofGetLastFrameTime()));
                escalators[i]->setPosition(escalators[i]->getPosition().x ,ypos);
            //    if(escalators[i]->getPosition().y > (1080 - i*solidHeight) )upDown[i]=-1;
            //    if(escalators[i]->getPosition().y < 0 )upDown[i] = 1;
            }
        }
    }
    
    void draw(){
        
        ofSetColor(0);
        
        for(int i = 0; i<pitHole.size();i++)
            pitHole[i]->draw();
        
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if (inPits[i]) {
                ofSetColor(0,100);
                ofDrawCircle(buttons->at(i).getBiquadPos(),25);
                ofSetColor(255);

            }
            if (co->debug){
                buttons->at(i).drawDebug();
                ofSetColor(255,255,0);
                if(theWinner!=-1)
                    ofDrawCircle(buttons->at(theWinner).getBiquadPos(),20);
            }
        }
        
        for(int i = 0; i< MIN(3 , escalators.size());i++){

            int x = escalators[i]->getPosition().x;
            int y = escalators[i]->getPosition().y;
            int w = escalators[i]->getWidth();
            int h = escalators[i]->getHeight();
            x -= w/2;
            y -= h/2;
            escalatorImg[i].draw(x,y,w,h);
    
        }
        ofNoFill();
        ofDrawRectangle(start);
        ofFill();
        
        overlay.draw(teamNumber * 1920, 0, 1920, 1080);
    };
    
    void begin(ofxBox2d * world = nullptr){
        time=0.0;
        moveEscalators = false;
        p_moveEscalators = false;
        thisWorld = world;
        
        overlay.load("img/specialAssets/08_EscalatorOverlay.png");
        
        float addX = teamNumber == 0 ? 0 : 1920;
        
        ofxSVG svg;
        
        svg.load("svg/08_EscalatorMoving.svg");
        movingPolys = getPolyline(svg);
        
        svg.load("svg/08_EscalatorSolids.svg");
        solidPolys = getPolyline(svg);
        
        theWinner=-1;
        solidHeigth = solidPolys[0].getBoundingBox().height;
        
        //world->setGravity(0,co->gravity);
        createScene(world,movingPolys);
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                buttons->at(i).setPosition(ofRandom(800,1120) + addX, 1080);
                buttons->at(i).isWinner = false;
                buttons->at(i).getOutOfPolys(solidPolys);
            }
        }
        escalatorImg[0].load("img/specialAssets/07_escalatorPads-01.png");
        escalatorImg[1].load("img/specialAssets/07_escalatorPads-02.png");
        escalatorImg[2].load("img/specialAssets/07_escalatorPads-03.png");

        svg.load("svg/08_EscalatorPits.svg");
        pitPolys = getPolyline(svg);
        cout << "PITPOLYS "<< pitPolys.size()<<endl;
        for(int i = 0; i<36;i++)
            inPits[i]=false;
        
        buttonsInPits.resize(pitPolys.size());

    };
    void reset(){
        overlay.clear();
        isDoneCounter=0.;
        for(int i = 0; i< escalators.size();i++)
            escalators[i]->destroy();
        
        escalators.clear();
        movingPolys.clear();
        solidPolys.clear();
        pitPolys.clear();
        
        escalatorImg[0].clear();
        escalatorImg[1].clear();
        escalatorImg[2].clear();
        
        for(int i = 0; i<pitHole.size();i++)
            pitHole[i]->destroy();
        pitHole.clear();
    }
    
    void createPit(ofxBox2d * world ,ofPolyline polys){
       
            ofRectangle rect = polys.getBoundingBox();
            
            rect.x += rect.width/2;
            rect.y += rect.height/2;
            rect.height = 10;
        
            if(polys.getVertices().size()>3 && (rect.width<1900 && rect.height < 1070)){
                
                shared_ptr<ofxBox2dRect> r = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
                r->setPhysics(0., 0., 0.);
                r->setup(world->getWorld(), rect);
                
                pitHole.push_back(r);
           
                
            }
        
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
