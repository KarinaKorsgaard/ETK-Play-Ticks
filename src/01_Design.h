#include "ofMain.h"
#include "Scene.h"

class Design : public Scene{
    
public:
    
    int p_numPlayers;
    int maxSymbols = 0;
    int maxColors = 0;
    vector<ofColor>colors;
   
    void setup(commonObjects*_co, vector<Button>*b, string colorFile, int numColors){
        buttons = b;
        co = _co;
        
        
        ofTexture tex;
        ofPixels pix;
        
        colors.resize(numColors);
        if(ofLoadImage(tex, colorFile)){
            
            tex.readToPixels(pix);
            
            int indx = tex.getWidth() / numColors;
            int c = 0;
            for(int i = 0; i<numColors;i++){
                colors[i]=pix.getColor(i*indx + indx/2 ,tex.getHeight()/2);
                
            }
        }
    };
    
    bool isDone(bool b = false){
        bool isDifferent = true;
        int indx =0;
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                indx++;
                
                for(int j = i; j<buttons->size();j++){
                    if((i!=j) && buttons->at(j).isPlaying ){
                        
                        if((buttons->at(i).symbolInt == buttons->at(j).symbolInt)&&
                           (buttons->at(i).colorInt == buttons->at(j).colorInt)){
                            isDifferent=false;
                            break;
                        }
                    }
                }
            }
        }
        if(indx<5)isDifferent=false;
        return isDifferent;
       // return false;
    };
    
    void update(){
        int c = colors.size();
        int s = co->characterSymbols.size();

        int numPlayers = co->numPresentButtons[teamNumber];
        if(numPlayers != p_numPlayers){
            
            maxColors = 0;
            maxSymbols = 0;
            
            if(numPlayers <= c){
                maxColors = numPlayers;
                maxSymbols = 1;
            }else{
                maxColors = c;
                
                maxSymbols = floor(ceil( float(numPlayers) / float(maxColors) ) + 0.1);
            }
            p_numPlayers = numPlayers;
        }

        co->numSymbolsPresent[teamNumber] = maxSymbols;
        co->numColorsPresent[teamNumber] = maxColors;
        
        for(int i=0; i<buttons->size(); i++) {
            ofVec3f data = buttons->at(i).getRawData();
            float normX = data.x > 1 ? data.x - 1. : data.x;
       
            int x = ofMap (normX,0,1,0,maxColors);
            int y = ofMap (data.y,0,1,0,maxSymbols); // symbol
            
            x = CLAMP (x,0,c-1);
            y = CLAMP (y,0,s-1);
            
        //    int z =CLAMP( ofMap(data.z,0,2*PI,0,colors.size()), 0 , colors.size()-1);
            buttons->at(i).color[0]=colors[x];
            buttons->at(i).symbol=&co->characterSymbols[y];
           
            buttons->at(i).colorInt = x;
            buttons->at(i).symbolInt = y;

            
        }
    }
    
    void draw(){
        
        for(int i = 0 ; i<buttons->size();i++) {
            
            Button b = buttons->at(i);
            if(b.isPlaying){
                ofPushMatrix();
                ofTranslate(b.getGridPos(   b.table - (b.table%2) * b.teamNumber , b.ID )   );
                ofTranslate(800,0);
                b.draw(false, false, true);
                if(co->debug)
                    co->font_small->drawString(ofToString(b.table+1)+" :"+ofToString(b.ID+1), -10, 20);
                ofPopMatrix();
            }
        }
        
    };
    
    void begin(ofxBox2d * world = nullptr){
        
    };
    void reset(){
        
    };
    

    
};
