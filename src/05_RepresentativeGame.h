#include "ofMain.h"
#include "Scene.h"

struct Spot{
    ofVec2f pos;
    bool isTaken;
    int buttonInt;
};

class Representative : public Scene{
    
public:

    int frontHall, doorLimit;
    vector<shared_ptr<ofxBox2dRect>>doors;
    vector<ofPolyline>movingPolys;
    
    vector<Spot>spots;
    vector<ofPoint>originalPoints;
    vector<ofPoint>midtPoints;
    Representative(){};
    ~Representative(){};
    
    
    ofImage overlay;
    
    ofImage doorImg[2];
    ofVec2f midt;

    bool doneFormation = false;
    bool breakFormation = false;
    bool done = false;
    
    vector<bool>tempTaken;
    
    float getDist(ofVec2f v, ofVec2f o){
        ofVec2f a = o - v;
        return a.dot(a) ;
    }
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
        
        midt = ofVec2f(1920 / 2 + (1920 * teamNumber) , 1080 / 2);
        frontHall = 480 + teamNumber * 1920;
        doorLimit = frontHall + 1000;

    };
    
    bool isDone(bool b = false){
        return done;
    }
    
    void updateIsDone(){
        
        
        if(!doneFormation){
            vector<int> buttonsOutside;
            for(int b = 0; b<buttons->size();b++){
                if(buttons->at(b).isPlaying){
                    if(buttons->at(b).getBiquadPos().x > frontHall){
                        buttonsOutside.push_back(b);
                        
                    }
                }
            }
            
            doneFormation = buttonsOutside.size() == spots.size() ? true : false;
            
            if(!doneFormation)
            {
                for(int s = 0; s<spots.size();s++){
                    tempTaken[s] = false;
                    for(int i = 0; i<buttonsOutside.size();i++){
                        if (getDist(spots[s].pos, buttons->at(buttonsOutside[i]).getBiquadPos())<200){
                            tempTaken[s] = true;
                        }
                    }
                }
            }
            
            for(int i = 0; i<spots.size();i++){
                spots[i].isTaken = false;
                spots[i].buttonInt = -1;
            }
            
            if(doneFormation){
               
                for(int i = 0; i<buttonsOutside.size();i++){
                    for(int s = 0; s<spots.size();s++){
                        
                        if(!spots[s].isTaken && getDist(spots[s].pos, buttons->at(buttonsOutside[i]).getBiquadPos())<200){
                            spots[s].isTaken = true;
                            spots[s].buttonInt = buttonsOutside[i];
                        }
                    }
                    
                }
                for (int s = 0; s<spots.size();s++){
                    if (!spots[s].isTaken)
                        doneFormation=false;
                }
            }
            
        }
        
        breakFormation = false;

        if(doneFormation){
            int count = 0;
            for(int b = 0; b<buttons->size();b++){
                if(buttons->at(b).isPlaying && buttons->at(b).getBiquadPos().x > frontHall &&
                   buttons->at(b).getBiquadPos().x<doorLimit && !buttons->at(b).isWinner){
                    count++;
                }
            }
            breakFormation = count > spots.size() ? true : false;
           // cout << count << "count> "<< spots.size()<<endl;
            
            for(int b = 0; b<buttons->size();b++){
                ofVec2f bpos = buttons->at(b).getBiquadPos();
                if(bpos.x > doorLimit && !buttons->at(b).isWinner){
                    
                    buttons->at(b).isWinner = true;
                    
                    bool isErased = false;
                    for(int i = 0; i<spots.size();i++){
                        
                        if(spots[i].buttonInt == b){
                            spots.erase (spots.begin()+i);
                            isErased=true;
                            break;
                        }
                    }
                    if(!isErased)
                        spots.erase (spots.begin()+int(ofRandom(spots.size())));
                
                }
                
            }
        }
        done = spots.size() == 0 ? true : false;
        
    };
    
    void update(){
        updateIsDone();
        
        
        for (int i = 0; i<doors.size();i++){
            if(doneFormation && !breakFormation ){
                if(abs(doors[i]->getPosition().y - originalPoints[i].y)<100){
                    int up = i * 2 - 1;
                    up*=-1;
                    doors[i]->setPosition(doors[i]->getPosition().x, doors[i]->getPosition().y-up);
                    
                    ofxOscMessage m;
                    m.setAddress("/doorsOpening"+ofToString(teamNumber+1));
                    m.addFloatArg(1.f);
                    co->oscOut.sendMessage(m);
                }
            }
            else if(abs(doors[i]->getPosition().y - originalPoints[i].y )>6){
                int up = i * 2 - 1;
                ofxOscMessage m;
                m.setAddress("/doorsClosing"+ofToString(teamNumber+1));
                m.addFloatArg(1.f);
                co->oscOut.sendMessage(m);
                doors[i]->setPosition(doors[i]->getPosition().x, doors[i]->getPosition().y-up*10.);
            }
           // doors[i]->setRotation(0);
            doors[i]->update();
            
        }
        
        if(!done)
            for(int i=0; i<buttons->size(); i++){
                 buttons->at(i).update(co->attraction);
            }
    }
    
    void draw(){
        ofSetColor(255);

        
        for(int i = 0; i<spots.size();i++){
            if(spots[i].isTaken || tempTaken[i]){
                ofSetColor(255, 200);
                ofDrawCircle(spots[i].pos,40);
            }
        }
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
        
        ofDrawLine(frontHall, 0, frontHall, 1080);
        
        for(int i = 0; i<doors.size();i++){
            int x = doors[i]->getPosition().x;
            int y = doors[i]->getPosition().y;
            int w = doors[i]->getWidth();
            int h = doors[i]->getHeight();
            x -= w/2;
            y -= h/2;
            doorImg[i].draw(x,y,w,h);
            //doors[i]->draw();
        }
        ofSetColor(255);
        if (overlay.isAllocated())overlay.draw(1920*teamNumber , 0, 1920, 1080);
    };
    
    void begin(ofxBox2d * world = nullptr){
       
        ofxSVG svg;
       
        float addX = teamNumber == 0 ? 0 : 1920;
        svg.load("svg/05_RepresentativeMoving.svg");
        movingPolys = getPolyline(svg);
        createScene(world,movingPolys);
        
        doorLimit = doors[0]->getPosition().x + doors[0]->getWidth();
        
        vector<ofPolyline>spotPos;
        svg.load("svg/05_RepresentativePositions.svg");
        spotPos=getPolyline(svg);
        
        for(int i = 0; i<spotPos.size();i++){
            Spot s = *new Spot;
            s.pos = ofVec2f( spotPos[i].getCentroid2D() );
            s.isTaken = false;
            spots.push_back(s);
        }
            
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).isWinner = false;
                buttons->at(i).setPosition(frontHall ,buttons->at(i).getPos().y);
        }
        
        doorImg[0].load("img/specialAssets/05_Door-01.png");
        doorImg[1].load("img/specialAssets/05_Door-02.png");
        
        overlay.load("img/specialAssets/05_RepresentativeOverlay.png");
        tempTaken.resize(spots.size());
    };
    
    void reset(){
        done = false;
        doneFormation = false;
        breakFormation = false;
        done = false;
        spots.clear();
        for(int i = 0; i< doors.size();i++)
            doors[i]->destroy();
        
        doors.clear();
        movingPolys.clear();
        solidPolys.clear();
        midtPoints.clear();
        originalPoints.clear();
        
        doorImg[0].clear();
        doorImg[1].clear();
    }
    
    void createScene(ofxBox2d * world ,vector<ofPolyline>polys){
        
        
        for(int i = 0 ;i <polys.size();i++){
            ofRectangle rect = polys[i].getBoundingBox();
            
            rect.x += rect.width/2;
            rect.y += rect.height/2;
            
            if(polys[i].getVertices().size()>3 && (rect.width<1900 && rect.height < 1070)){
                
                shared_ptr<ofxBox2dRect> r = shared_ptr<ofxBox2dRect>(new ofxBox2dRect);
                r->setPhysics(0., 0., 0.);
                r->setup(world->getWorld(), rect);
                
                doors.push_back(r);
                originalPoints.push_back(doors.back()->getPosition());
                
            }
        }
        midtPoints.resize(originalPoints.size());
   
        int top = originalPoints[0].y + doors[0]->getHeight()/2;
        int bot = originalPoints[1].y - doors[1]->getHeight()/2;
        midtPoints[0].y = originalPoints[0].y + (bot-top)/2;
        midtPoints[1].y = originalPoints[0].y - (bot-top)/2;
        
       
        
    }

};
/*
 #include "ofMain.h"
 #include "Scene.h"
 
 
 
 class Representative : public Scene{
 
 public:
 
 int bigRadius,smallRadius;
 int bigRadiusWin,smallRadiusWin;
 
 Representative(){};
 ~Representative(){};
 
 ofVec2f midt;
 
 bool done = false;
 
 
 void setup(commonObjects*_co, vector<Button>*b){
 buttons = b;
 co = _co;
 
 midt = ofVec2f(1920 / 2 + (1920 * teamNumber) , 1080 / 2);
 
 };
 
 bool isDone(bool b = false){
 return done;
 }
 
 void updateIsDone(){
 
 bool isInside = true;
 int ousideCount = 0;
 vector<int> checkThisOne;
 vector<int> checkThisOneDist;
 
 for(int i = 0; i<buttons->size();i++){
 
 if(buttons->at(i).isPlaying){
 ofPoint p = buttons->at(i).getBiquadPos();
 
 ofVec2f a = p - midt;
 float dist = a.dot(a);
 
 if(dist > bigRadius || dist < smallRadius){
 checkThisOne.push_back(i);
 checkThisOneDist.push_back(dist);
 ousideCount ++;
 }
 
 
 if(ousideCount > 6){
 isInside = false;
 done = false;
 break;
 }
 }
 }
 
 done = checkThisOne.size() == 6;
 // << checkThisOne.size() << endl;
 if(done){
 for(int i = 0; i<checkThisOne.size();i++){
 if(isInside){
 float dist = checkThisOneDist[i];
 
 if(dist < bigRadiusWin && dist > smallRadiusWin){
 buttons->at(checkThisOne[i]).isWinner = true;
 }else
 done = false;
 }
 }
 }
 };
 
 void update(){
 updateIsDone();
 
 if(!done){
 for(int i=0; i<buttons->size(); i++) {
 buttons->at(i).update(co->attraction);
 }
 }
 }
 
 void draw(){
 ofSetColor(255);
 
 for(int i=0; i<buttons->size(); i++) {
 buttons->at(i).draw();
 
 if(co->debug){
 buttons->at(i).drawDebug();
 
 ofSetColor(255,0,0);
 ofNoFill();
 //winArea.draw();
 ofDrawCircle(midt, bigRadius);
 ofDrawCircle(midt, smallRadius);
 
 ofDrawCircle(midt, bigRadiusWin);
 ofDrawCircle(midt, smallRadiusWin);
 ofFill();
 }
 }
 };
 
 void begin(ofxBox2d * world = nullptr){
 int radius = 1;
 int numPlayers = 0;
 
 for(int i=0; i<buttons->size(); i++) {
 buttons->at(i).isWinner = false;
 
 if(buttons->at(i).isPlaying){
 radius = buttons->at(i).beginningRad;
 numPlayers ++;
 }
 }
 numPlayers = CLAMP(numPlayers + 5, 0, 36);
 float l = numPlayers * radius * 2;
 float d = l / PI;
 d/=2;
 cout << d << endl;
 bigRadius = d == 0 ? 286 : d;
 
 bigRadius += radius;
 smallRadius = bigRadius - 20;
 
 
 // big small win radius
 l = 10 * radius * 2;
 d = l / PI;
 d/=2;
 bigRadiusWin = d;
 bigRadiusWin += radius;
 smallRadiusWin = bigRadiusWin-20;
 
 };
 
 void reset(){
 done = false;
 };
 
 
 
 };
*/
