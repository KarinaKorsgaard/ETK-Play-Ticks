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
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    };
    
    
    bool isDone(bool b = false) {
        done = true;
    };
    
    void update(){
        buttons->at(looser).update(co->attraction);
    }
    
    void draw(){
        buttons->at(looser).draw();
        if(forGround.isAllocated())forGround.draw(teamNumber * 1920 , 0 , 1920 , 1080);
    };
    
    void begin(ofxBox2d * world = nullptr){
        
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
        }
        
        
        cout << looser << " the looser was not found is looser from "<<teamNumber<< endl;
    };
    
    void reset(){
        looser = -1;
        forGround.clear();
    };
  
    
    
};

#endif /* _00_looserTick_h */
