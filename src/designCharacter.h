#include "ofMain.h"
#include "common.h"

class DesignACharacter : public commonFunctions{
    
public:
    vector<Button>*buttons;
    void setup(commonObjects*_co, vector<Button>*b, string colorFile, int numColors){
        buttons = b;
        co = _co;
        
        
        ofTexture tex;
        ofPixels pix;
        
        colors.resize(numColors);
        
        if(ofLoadImage(tex, colorFile))cout << "succes"<<endl;
        
        tex.readToPixels(pix);
        
        int indx = tex.getWidth() / numColors;
        int c = 0;
        for(int i = 0; i<numColors;i++){
            colors[i]=pix.getColor(i*indx + indx/2 ,tex.getHeight()/2);
      
          //  cout << colors[c].r <<endl;
        }
        //cout << indx<<endl;
    };
    bool isDone(){
        // do not drain here...
        return true;
    };
    
    void update(){
        for(int i=0; i<buttons->size(); i++) {
            ofVec3f data = buttons->at(i).getRawData();
            int x = ofMap(data.x,0,1,0,colors.size()) ;
            int y = ofMap(data.y,0,1,0,colors.size()) ; // symbol
            x=CLAMP(x,0,colors.size()-1);
            y=CLAMP(y,0,colors.size()-1);
            if(y==x){
                y++;
                y=y%colors.size();
            };
        //    int z =CLAMP( ofMap(data.z,0,2*PI,0,colors.size()), 0 , colors.size()-1);
            buttons->at(i).color[0]=colors[CLAMP(x,0,colors.size()-1)];
            buttons->at(i).color[1]=colors[CLAMP(y,0,colors.size()-1)];
            //buttons->at(i).symbol=&co->characterSymbols[CLAMP(y,0,co->characterSymbols.size()-1)];
          //  buttons->at(i).colors[2]=colors[CLAMP(z,0,colors.size()-1)];
            
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
    
    void begin(){
        
    };
    void reset(){
        
    };
    
    commonObjects * co;
    vector<ofColor>colors;
    
};
