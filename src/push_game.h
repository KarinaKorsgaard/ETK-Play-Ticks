#include "ofMain.h"
#include "common.h"



#include "ofMain.h"
#include "common.h"
#include "ofxBox2d.h"


class PushGame : public commonFunctions{
    
public:
    
    vector<shared_ptr<ofxBox2dRect>> rects;
    vector <shared_ptr<ofxBox2dPolygon> > polyShapes;
    
    ofxBox2d world;
    vector<Button>*buttons;
    commonObjects * co;
    vector<ofPolyline>polys;
    ofxSVG svg;
    float deadZone;
    
    void setup(commonObjects*_co ){
        co = _co;
        
        world.init();
        world.setGravity(0, 1);
        world.createBounds(ofRectangle(0,0,1920*2, 1080));
        svg.load("svg/push_wall.svg");
        deadZone = 150;
    };
    
    bool isDone(){
        bool oneSurvivor = true;
        int count = 0;
        for(int i = 0 ; i< buttons->size();i++){
            if(buttons->at(i).isPlaying && !buttons->at(i).isDead())count++;
            if(count > 1)break;
        }
        return count = 1;
    };
    
    void update(){
    
        if(beginTime<.2)beginTime += ofGetLastFrameTime();
        
        for(int i = 0 ; i< buttons->size();i++){

            if(beginTime>.1) buttons->at(i).update(co->attraction, deadZone+60);
            
            if(i<buttons->size()/2 && buttons->at(i).getPos().x<deadZone-20)buttons->at(i).setValue(0);
            else if(buttons->at(i).getPos().x+20 > 1920*2-deadZone)buttons->at(i).setValue(0);
            
            buttons->at(i).drain(co->drainCoefficient1);
        }
       // cout << rects[0]->getPosition() << endl;
        for(int i = 0; i<rects.size();i++){
            
            if(rects[i]->getPosition().x > 1920 + (960 - deadZone) )
                rects[i]->addForce(ofVec2f(-10,0),co->blockForce);
            if(rects[i]->getPosition().x < 960+deadZone )
                rects[i]->addForce(ofVec2f(10,0),co->blockForce);
        }

        world.update();
    }
    
    void draw(){
        ofSetColor(10,10,80);
        ofDrawRectangle(0,0,deadZone,1080);
        ofDrawRectangle(1920*2 - deadZone,0,deadZone,1080);
        
        ofSetColor(80,80,120);
        for(int i = 0 ; i< rects.size();i++)rects[i]->draw();
        
        //svg.draw();
        
        for(int i = 0 ; i< buttons->size();i++){
            if(co->debug)buttons->at(i).drawDebug();
            else buttons->at(i).draw();
        }
        
        
    };
    
    void begin(vector<Button>*b1){
        
        buttons = b1;
        
        
        for(int i = 0 ; i<buttons->size();i++){
            buttons->at(i).box2Dcircle->destroy();
            
            buttons->at(i).box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
            buttons->at(i).box2Dcircle.get()->setPhysics(buttons->at(i).getValue() * 0.01, 0.0, 0.1);
            buttons->at(i).box2Dcircle.get()->setup(world.getWorld(), 0, 0, 0);
            

        }
        

        
        createScene(getPolyline(svg, true));
        
        
        for(int i = 0; i<6 ; i++){
            
            shared_ptr<ofxBox2dRect> r = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
            r.get()->setPhysics(6., 0.1 , .1);
            r.get()->setup(world.getWorld(), 1920 , 94 + 168*i + 10*i  , 1920 , 165);
            r.get()->alive = true;
            //r->setMassFromShape=false;
            //r->setVelocity(ofVec2f(0,0));
            rects.push_back(r);
        }
        
        for(int i = 0 ; i< buttons->size();i++){
            if(i<buttons->size()/2)
                buttons->at(i).setPosition(200,ofRandom(80,1000));
            else
                buttons->at(i).setPosition( 1920*2 - 200 ,ofRandom(80,1000));
            
            buttons->at(i).box2Dcircle->setRadius(0);
        }
        beginTime= 0;
    };
    void reset(){
        if(polyShapes.size()>0){
            polyShapes.clear();
        }
        for(int i = 0; i<6 ; i++){
            rects[i]->destroy();
        }
        rects.clear();
        
    };
    
    void createScene(vector<ofPolyline>polys){
        
        if(polyShapes.size()==0){
            
            for(int i = 0 ;i <polys.size();i++){
                ofRectangle r = polys[i].getBoundingBox();
                if(polys[i].getVertices().size()>3 && (r.width<1900*2 && r.height < 1070)){
                    shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    
                    poly->addVertices(polys[i].getVertices());
                    poly->setPhysics(0.0, 0.0, 0.0);
                    poly->triangulatePoly();
                    
                    poly->create(world.getWorld());
                    polyShapes.push_back(poly);
                }
            }
            
        }
    }


    float beginTime;
};

