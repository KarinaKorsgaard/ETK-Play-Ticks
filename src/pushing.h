#include "ofMain.h"
#include "common.h"



#include "ofMain.h"
#include "common.h"
#include "ofxBox2d.h"


class Pushing : public commonFunctions{
    
public:
    
    vector<shared_ptr<ofxBox2dRect>> rects;
//    vector <shared_ptr<ofxBox2dPolygon> > polyShapes;
//    
    ofxBox2d world;
    vector<Button>*buttons;
    commonObjects * co;
//    vector<ofPolyline>polys;
//    ofxSVG svg;
    float deadZone;
    
    void setup(commonObjects*_co ){
        co = _co;
        
        world.init();
        world.setGravity(0, 0);
        world.createBounds(ofRectangle(0,0,1920, 1080));
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
        
      //  if(beginTime<.2)beginTime += ofGetLastFrameTime();
        
        for(int i = 0 ; i< buttons->size();i++){
            Button * b = &buttons->at(i);
            //if(beginTime>.1)
            b->update(b->getValue() * b->getValue(), ofRectangle(deadZone,deadZone,1920-deadZone*2,1080-deadZone*2), false);
            
            if(b->getPos().x < deadZone           )b->setValue(0);
            else if(b->getPos().x > 1920-deadZone)b->setValue(0);
            
            if(b->getPos().y<deadZone            )b->setValue(0);
            else if(b->getPos().y > 1080-deadZone)b->setValue(0);
            
            b->drain(co->drainCoefficient1);
            
            
            if(b->on && b->radius > 0){
                b->radius = (b->size_lim - (b->size_break/b->getValue())) / (1 + (b->size_break/b->getValue()));
                if(b->box2Dcircle->getRadius() < b->radius)b->box2Dcircle->setRadius(b->box2Dcircle->getRadius()+1);
                else b->box2Dcircle->setRadius(b->radius);
                b->box2Dcircle->alive = true;
            }
            if (b->getValue() <= 0 || !b->on || b->isDead()) {
                b->box2Dcircle->setRadius(0);
                b->box2Dcircle->alive = false;
            }
            
            
        }
        
        
//        // cout << rects[0]->getPosition() << endl;
        for(int i = 0; i<rects.size();i++){
            
            if(rects[i]->getPosition().x < deadZone+100 )
                rects[i]->addForce(ofVec2f(10,0),co->blockForce);
            if(rects[i]->getPosition().x > 1920-(deadZone+100) )
                rects[i]->addForce(ofVec2f(-10,0),co->blockForce);
            
            if(rects[i]->getPosition().y < deadZone+100 )
                rects[i]->addForce(ofVec2f(0,10),co->blockForce);
            if(rects[i]->getPosition().y > 1080-(deadZone+100) )
                rects[i]->addForce(ofVec2f(0,-10),co->blockForce);
        }
        
        world.update();
    }
    
    void draw(){
        ofSetColor(10,10,80);
        ofDrawRectangle(0,0,deadZone,1080);
        ofDrawRectangle(1920 - deadZone,0,deadZone,1080);
        
        ofDrawRectangle(0,0,1920,deadZone);
        ofDrawRectangle(0,1080 - deadZone,1920,deadZone);
//        
        ofSetColor(80,80,120);
        for(int i = 0 ; i< rects.size();i++)rects[i]->draw();
//
        //svg.draw();
        
        for(int i = 0 ; i< buttons->size();i++){
            if(co->debug)buttons->at(i).drawDebug(ofRectangle(deadZone,deadZone,1920-deadZone*2,1080-deadZone*2));
            else buttons->at(i).draw();
        }
        
        
    };
    
    void begin(vector<Button>*b1){
        
        buttons = b1;
        
        
        for(int i = 0 ; i<buttons->size();i++){
            buttons->at(i).box2Dcircle->destroy();
            
            buttons->at(i).box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
            buttons->at(i).box2Dcircle.get()->setPhysics(3., 0.0, 400.);
            buttons->at(i).box2Dcircle.get()->setup(world.getWorld(), ofRandom(300,1920-300), ofRandom(300,1920-300), buttons->at(i).getValue());
            buttons->at(i).setPosition(1920/2,1080/2);
        }
        
        
//        
//        createScene(getPolyline(svg, true));
//        
        
        for(int i = 0; i<7 ; i++){
            
            shared_ptr<ofxBox2dRect> r = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
            r.get()->setPhysics(600., 0.1 , 100.);
            r.get()->setup(world.getWorld(), ofRandom(1920) , ofRandom(1080) , 40 , 200);
            r.get()->alive = true;
            //r->setMassFromShape=false;
            //r->setVelocity(ofVec2f(0,0));
            rects.push_back(r);
        }
//
//        for(int i = 0 ; i< buttons->size();i++){
//            if(i<buttons->size()/2)
//                buttons->at(i).setPosition(200,ofRandom(80,1000));
//            else
//                buttons->at(i).setPosition( 1920*2 - 200 ,ofRandom(80,1000));
//            
//            buttons->at(i).box2Dcircle->setRadius(0);
//        }
//        beginTime= 0;
    };
    void reset(){
//        if(polyShapes.size()>0){
//            polyShapes.clear();
//        }
        for(int i = 0; i<6 ; i++){
            rects[i]->destroy();
        }
        rects.clear();
        
    };
//    
//    void createScene(vector<ofPolyline>polys){
//        
//        if(polyShapes.size()==0){
//            
//            for(int i = 0 ;i <polys.size();i++){
//                ofRectangle r = polys[i].getBoundingBox();
//                if(polys[i].getVertices().size()>3 && (r.width<1900*2 && r.height < 1070)){
//                    shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
//                    
//                    poly->addVertices(polys[i].getVertices());
//                    poly->setPhysics(0.0, 0.0, 0.0);
//                    poly->triangulatePoly();
//                    
//                    poly->create(world.getWorld());
//                    polyShapes.push_back(poly);
//                }
//            }
//            
//        }
//    }
//    
    
    float beginTime;
};

