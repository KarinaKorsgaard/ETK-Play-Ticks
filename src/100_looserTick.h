//
//  100_looserTick.h
//  _ETK_March_2017
//
//  Created by Karina Jensen on 10/03/2018.
//
//

#ifndef _00_looserTick_h
#define _00_looserTick_h

#include "ofMain.h"
#include "Scene.h"



class LooserTick : public Scene{
    
public:

    int looser ;
    ofImage forGround;
    bool done;
    ofVec2f pos;
    bool setIsDone;
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    };
    
    
    bool isDone(bool b = false) {
        return setIsDone;
    };
    
    void update(){
        if(!setIsDone){
            buttons->at(looser).update(co->attraction, false, ofRectangle(0, 0, 1920*2, 1080));
            pos = buttons->at(looser).getBiquadPos();
        }
    }
    
    void draw(){
        buttons->at(looser).draw();
        if(teamNumber==0)if(forGround.isAllocated())forGround.draw(0 , 0 , 1920 * 2, 1080);
    };
    
    void begin(ofxBox2d * world = nullptr){
        setIsDone = false;
        forGround.load("img/specialAssets/100_LooserTickForGround.png");
        
        looser = -1;
        vector<int>randomLooser;
        
        for (int i = 0; i<buttons->size();i++){
            if (buttons->at(i).isPlaying){
                
                randomLooser.push_back(i);
                if (buttons->at(i).isLooser){
                    looser = i;
                    cout << looser << " the looser tick was found "<<teamNumber<< endl;
                }
            }
        }
        if(looser == -1){
            int l = randomLooser.size() == 0 ? 0 : randomLooser[int(ofRandom(randomLooser.size()-1))];
            looser = l;
            cout << looser << " the looser was not found is looser from "<<teamNumber<< endl;
        }
        
        
       
        
        ofxSVG svg;
        svg.load("svg/100_looserTick.svg");
        if(teamNumber == 0)solidPolys = getPolyline(svg, true);
        
        for (int i = 0; i < buttons->size(); i++) {
            buttons->at(i).box2Dcircle->alive = false;
           
        }
        buttons->at(looser).box2Dcircle->alive = true;
        int xbegin = teamNumber == 0 ? 20: 3820;
        buttons->at(looser).setPosition(xbegin, 1080/2);
    };
    
    void reset(){
        looser = -1;
        forGround.clear();
        
        for (int i = 0; i < buttons->size(); i++) {
            buttons->at(i).box2Dcircle->alive = true;
        }
    
    };
  
    
    
};

#endif /* _00_looserTick_h */
