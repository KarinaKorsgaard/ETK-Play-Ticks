#include "ofMain.h"
#include "common.h"

class Finale : public commonFunctions{
    
public:
    vector<Button>*buttons;
    vector<int>players;
    ofRectangle board ;
    float rememberAttraction;
    void setup(commonObjects*_co){

     
        co = _co;
        board = ofRectangle(100,100,1920-200,1080-200);
        head = transformToCollumn(getLine("text/finale.txt",0),800, co->font_medium);
        bread = transformToCollumn(getLine("text/finale.txt",1),800, co->font_small);
    };
    
    bool isDone(){
        int alive=0;

        for(int i = 0; i<players.size();i++){
            if(!buttons->at(players[i]).isDead())alive++;
        }
        return alive == 1;
    };
    
    void update(){
        for(int i=0; i<players.size(); i++) {
            Button * b = &buttons->at(players[i]);
            b->update(co->attraction);
            
            if(!board.inside(b->getPos()) && co->startFinale &&  b->on && b->value>0){
                 b->value-=5;
                if(b->value<0)b->value=0;
            }
            b->freezeControl(co->deadTimeFinale);
        }  
    }

    void draw(){
        
        ofSetColor(255);
        ofNoFill();
        ofDrawRectangle(board);
        ofFill();
        
        for(int i=0; i<players.size(); i++) {
            buttons->at(players[i]).draw();
            
//            if(!buttons->at(players[i]).isDead() && co->debug){
//                ofPushMatrix();
//                ofTranslate(buttons->at(players[i]).getBiquadPos());
//                string s = ofToString(buttons->at(players[i]).value,0);
//                ofSetColor(255);
//                ofRectangle r = co->font_medium->getStringBoundingBox(s, 0, 0);
//                co->font_medium->drawString(s,-r.width/2,0);
//                
//                ofPopMatrix();
//
//                buttons->at(players[i]).drawDebug();
//            }
        }
        
    };
    
    void begin(vector<Button>*buttonsArg){
        buttons = buttonsArg;
        //cout <<" fuck this!! "+ofToString(buttons->size()) << endl;
        co->startFinale = false;
        rememberAttraction = co->attraction ;
        //co->attraction  = 6;
        players.clear();
        
        int table = 0;
        int max = 0;
        int maxWinner = -1;
        
        for(int i = 0; i<buttons->size();i++){
            Button * b = &buttons->at(i);
            
            b->box2Dcircle->setRadius(0);
            b->box2Dcircle->alive = false;
            b->box2Dcircle->setPosition(1920/2,1080/2);
            buttons->at(i).box2Dcircle->friction = 400.;
           
            if(table != b->table || i==buttons->size()-1){
                table=b->table;
                if(maxWinner!=-1){
                    players.push_back(maxWinner);
                    
                    cout << "button: "+ofToString(buttons->at(maxWinner).ID)+" from table: "+ofToString(buttons->at(maxWinner).table)+ " in battle" << endl;
                    co->log("button: "+ofToString(buttons->at(maxWinner).ID)+" from table: "+ofToString(buttons->at(maxWinner).table)+ " in battle");
                }
                max = 0;
                maxWinner = -1;
                
            }
            if(b->value>max && b->isPlaying){
                max = b->value;
                maxWinner = i;
            }
            
            
            
        }
        cout << "amount of players in battle royale: "+ofToString(players.size()) << endl;
    }
    
    void reset(){
        for(int i = 0; i<buttons->size();i++){
            Button * b = &buttons->at(i);
            b->box2Dcircle->alive = true;
            b->freezeUpdate=false;
            
            b->box2Dcircle->friction = 40.;
            
        }
        co->attraction  = rememberAttraction;
    };
    
    commonObjects * co;

    vector<string> head;
    vector<string> bread;
    
};
