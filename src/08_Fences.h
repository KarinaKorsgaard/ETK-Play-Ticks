// gravity!!

#include "ofMain.h"
#include "Scene.h"

class Fences : public Scene{
    
public:
    
    ofRectangle winningArea;
    int theWinner;
    double isDoneCounter;
    float solidHeigth;
    vector<ofPolyline>dispensorPoly;
    ofxBox2d * thisWorld;
    
    vector<ofImage>dispensorImages;
    
    ofImage escalatorImg[3];
    
    vector<shared_ptr<ofxBox2dRect>>escalators;
    vector<shared_ptr<ofxBox2dPolygon>>dispensors;
    
    vector<shared_ptr<ofxBox2dRect>>pitHole;
    vector<ofPolyline>movingPolys;
    vector<ofPolyline>pitPolys;
    vector<bool>inPits;
    ofRectangle start;
    double time;
    bool moveEscalators;
    bool p_moveEscalators;
    
    bool destroyFirst = true;
    
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
    
    bool isDone(bool b = false){
        bool isInside=false;
        for(int i = 0; i<buttons->size();i++){
            Button *b =&buttons->at(i);
            //if(!b->isPlaying || !b->on)continue; // if b is not playing or not on, dont check.
            if(winningArea.inside(b->getBiquadPos())){
                isInside = true;
                theWinner = i;
                break;
            }
        }
        return isInside;
    };
    
//    bool isDone(bool b = false){
//        if(reached())isDoneCounter+=ofGetLastFrameTime();
//        else isDoneCounter = 0;
//        return isDoneCounter>1.f;
//    }
    
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
                        if(destroyFirst){
                            destroyFirst=false;
                            dispensors[0]->destroy();
                            dispensors.erase(dispensors.begin());
                            dispensorPoly.erase(dispensorPoly.begin());
                            dispensorImages.erase(dispensorImages.begin());
                        }
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
                    if(dispensors.size()>0){
                        dispensors[0]->destroy();
                        dispensors.erase(dispensors.begin());
                        dispensorPoly.erase(dispensorPoly.begin());
                        dispensorImages.erase(dispensorImages.begin());
                        
                    }
                    
                }
            }
        }else{ // if is not doen
            buttons->at(theWinner).isWinner = true;
        }
        if(moveEscalators){
            time += ofGetLastFrameTime();
            for(int i = 0; i<escalators.size();i++){
                int floor = movingPolys[i].getBoundingBox().y + movingPolys[i].getBoundingBox().height;
                
                float escalatorSpeed = co->escalatorSpeed * float(1920 - floor) * 0.001;
                float ypos = abs(sin(time*escalatorSpeed)) * floor;
                
                escalators[i]->setPosition(escalators[i]->getPosition().x ,ypos);
                
            }
        }
    }
    
    void draw(){
        
        ofSetColor(0);
        
        
        
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
        for(int i = 0 ;i<dispensorImages.size();i++)
            dispensorImages[i].draw(dispensorPoly[i].getBoundingBox());
        
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
        
        ofSetColor(255);
        for(int i = 0; i<pitHole.size();i++)
            pitHole[i]->draw();
    };
    
    void begin(ofxBox2d * world = nullptr){
        time=0.0;
        moveEscalators = false;
        p_moveEscalators = false;
        thisWorld = world;
        
        destroyFirst = true;
        
        overlay.load("img/specialAssets/08_EscalatorOverlay.png");
        
        float addX = teamNumber == 0 ? 0 : 1920;
        
        ofxSVG svg;
        
        svg.load("svg/08_EscalatorMoving.svg");
        movingPolys = getPolyline(svg);
        
        
        svg.load("svg/08_EscalatorSolids.svg");
        solidPolys = getPolyline(svg);
        
        theWinner=-1;
        
        
        //world->setGravity(0,co->gravity);
        createScene(world,movingPolys);
        
        
        escalatorImg[0].load("img/specialAssets/07_escalatorPads-01.png");
        escalatorImg[1].load("img/specialAssets/07_escalatorPads-02.png");
        escalatorImg[2].load("img/specialAssets/07_escalatorPads-03.png");
        
        svg.load("svg/08_EscalatorPits.svg");
        pitPolys = getPolyline(svg);
        cout << "PITPOLYS "<< pitPolys.size()<<endl;
        
        vector<int>rTickOrder;
        for(int i = 0; i<36;i++){
            inPits[i]=false;
            rTickOrder.push_back(i);
        }
        std::random_shuffle ( rTickOrder.begin(), rTickOrder.end() );

        buttonsInPits.resize(pitPolys.size());
        
        svg.load("svg/08_TickDispensor.svg");
        
        dispensorPoly=getPolyline(svg);
        createDispensor(dispensorPoly);
        

        solidHeigth = 400;
        int count = 0;
        ofRectangle d = ofRectangle(teamNumber*1920, 1000, 1920, 10);
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(rTickOrder[i]).isPlaying){
                
                buttons->at(rTickOrder[i]).setPosition(ofRandom(d.width - 50)+d.x +25, ofRandom(d.height-50)+d.y+25);
                buttons->at(rTickOrder[i]).isWinner = false;
                buttons->at(rTickOrder[i]).getOutOfPolys(solidPolys);
                
                count ++;
                if(count > 4)d = dispensorPoly[0].getBoundingBox();
                if(count > 12)d = dispensorPoly[1].getBoundingBox();
                if(count > 20)d = dispensorPoly[2].getBoundingBox();
            }
        }
        for(int i = 0;i<dispensorPoly.size();i++){
            ofImage im;
            im.load("img/specialAssets/08_EscalatorDispensor-0"+ofToString(i+1)+".png");
            dispensorImages.push_back(im);
        }
        
        
    }
    
    void reset(){
        dispensorImages.clear();
        dispensorPoly.clear();
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
        
        for(int i = 0; i<dispensors.size();i++)
            dispensors[i]->destroy();
        dispensors.clear();
    }
    
    
    
    void createDispensor(vector<ofPolyline>polys, float d = 0.,float f = 0.,float b = 0.){
        
        
        for(int i = 0 ;i <polys.size();i++){
            ofRectangle r = polys[i].getBoundingBox();
            if(polys[i].getVertices().size()>3 && (r.width<1900 || r.height < 1070)){
                
                shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                
                vector<ofPoint>verts;
                for(auto p : polys[i].getVertices())
                    verts.push_back(ofPoint(p.x, p.y));
                
                poly->addVertices(verts);
                poly->setPhysics(d,f,b);
                poly->triangulatePoly();
                
                poly->create(thisWorld->getWorld());
                dispensors.push_back(poly);
            }
        }
        
    }
    
    void createPit(ofxBox2d * world ,ofPolyline polys){
        
        ofRectangle rect = polys.getBoundingBox();
        
        rect.x += rect.width/2;
        rect.y += 5;
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
