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
    
    void setup(commonObjects*_co ){
        co = _co;
        
        world.init();
        world.setGravity(0, 1);
        world.createBounds(ofRectangle(0,0,1920*2, 1080));
        svg.load("svg/push_wall.svg");

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
    
        beginTime += ofGetLastFrameTime();
        
        for(int i = 0 ; i< buttons->size();i++){

            if(beginTime>10) buttons->at(i).update(co->attraction, true);
            
            if(i<buttons->size()/2)
                buttons->at(i).lastPos = ofVec2f(150,ofRandom(80,1000));
            else
                buttons->at(i).lastPos = ofVec2f(1920*2 - 150 ,ofRandom(80,1000));
            

            if(i<buttons->size()/2 && buttons->at(i).getPos().x<150)buttons->at(i).value = 0;
            else if(buttons->at(i).getPos().x > 1920*2-150)buttons->at(i).value = 0;
        }

        world.update();
    }
    
    void draw(){
        ofSetColor(100);
        ofDrawRectangle(0,0,150,1080);
        ofDrawRectangle(1920*2 - 150,0,150,1080);
        
        ofSetColor(255);
        for(int i = 0 ; i< rects.size();i++)rects[i]->draw();
        
        svg.draw();
        
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
            //virtual void setPhysics(float density, float bounce, float friction);
            buttons->at(i).box2Dcircle.get()->setPhysics(buttons->at(i).value * 0.01, 0.0, 0.1);
            buttons->at(i).box2Dcircle.get()->setup(world.getWorld(), 0, 0, 0);
            
            
//            buttons->at(i).box2Dcircle.get()->setData(new ButtonData());
//            ButtonData * sd = (ButtonData*)buttons->at(i).box2Dcircle.get()->getData();
//            sd->buttonID = i;
//            sd->bHit   = false;
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

