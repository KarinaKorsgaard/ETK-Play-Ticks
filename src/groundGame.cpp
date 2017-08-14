//
//  groundGame.cpp
//  _ETK_March_2017
//
//  Created by Karina Jensen on 01/08/2017.
//
//

#include <stdio.h>
void updateGroundGame(){
    int aliveCounter = 0;
    
    if(groundVideo.isFrameNew())
        groundPixels = groundVideo.getPixels();
    
    for(int t = 0; t < 2 ; t++){
        for(int i = 0; i < teams[t].buttons.size(); i++){
            
            Button *b = &teams[t].buttons[i];
            
            b->update(co.attraction, false);
            
            if(!b->isDead() && b->isPlaying && !isGroundDone){
                float x = b->getBiquadPos().x;
                float y = b->getBiquadPos().y;
                
                if(x<1920){
                    x *= groundVideo.getWidth()/1920.;
                    y *= groundVideo.getHeight()/1080.;
                    
                    if(groundPixels.getColor(x,y).r < 10){
                        b->setValue(0);
                    }
                }
            }
            if(!b->isDead() && b->isPlaying)aliveCounter++;
        }
    }
    
    if(aliveCounter != 1)
        groundVideo.update();
    else isGroundDone = true;
}