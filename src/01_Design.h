#include "ofMain.h"
#include "Scene.h"

class Design : public Scene{
    
public:
    
    int p_numPlayers;
    int maxSymbols = 0;
    int maxColors = 0;
    vector<ofColor>colors;
    vector<bool>drawCircles;
    double countDown = 5*60;
    ofImage forGround;
    vector<string>letters;
    bool done;
    string colorFile;
    
    int numColors;
    void setup(commonObjects*_co, vector<Button>*b, string _colorFile, int _numColors){
        buttons = b;
        co = _co;
        colorFile = _colorFile;
        numColors = _numColors;
        
        string abc = "A-B-C-D-E-F";
        letters = ofSplitString(abc,"-");
        
        
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
    }
    
    
    bool isDone(bool b = false){
        for(int i = 0; i<drawCircles.size();i++)
            drawCircles[i]=false;
        
        bool isDifferent = true;
        
        for(int i = 0; i<buttons->size();i++){
            if(buttons->at(i).isPlaying){
                
                for(int j = i+1; j<buttons->size();j++){
                    if( buttons->at(j).isPlaying ){
                        
                        if (buttons->at(i).symbolInt == buttons->at(j).symbolInt &&
                            buttons->at(i).colorInt == buttons->at(j).colorInt){
                            
                            drawCircles[i]=true;
                            drawCircles[j]=true;
                            isDifferent=false;
                            break;
                        }
                    }
                }
            }
        }
        
        
        
        if(!b)isDifferent=false;
        
        // if(isDifferent)countDown = 0.;
        done = isDifferent;
        return isDifferent;
        // return false;
    }
    
    void update(){
        
        if(co->startScene){
            if(countDown > 0 && !done )countDown -= ofGetLastFrameTime();
            else countDown = 0;
        }
        if(!done){
            int c = colors.size();
            int s = co->characterSymbols.size();
            
            int numPlayers = co->numPresentButtons[teamNumber];
            
            maxColors = co->lookUp[numPlayers][0];
            maxSymbols = co->lookUp[numPlayers][1];
            
            //int minColors = MIN(maxColors,maxSymbols);
            int minColors = maxColors == maxSymbols ? maxColors : maxColors-1;
            
            minColors=MAX(minColors,1);
            int howManyGetsMore = numPlayers-(minColors*maxSymbols);
            
            int count = 0;
            for(int i=0; i<buttons->size(); i++) {
                ofVec3f data = buttons->at(i).getRawData();
                float normX = data.x > 1 ? data.x - 1. : data.x;
                int x = ofMap (normX,0,1,0,minColors);
                int y = ofMap (data.y,0,1,0,maxSymbols); // symbol
                if(count < howManyGetsMore){
                    
                    x = ofMap (normX,0,1,0,MAX(maxColors,maxSymbols));
                    
                }
                if(buttons->at(i).isPlaying)count++;
                x = CLAMP (x,0,maxColors);
                y = CLAMP (y,0,maxSymbols-1);
                buttons->at(i).color=colors[x];
                buttons->at(i).symbol=&co->characterSymbols[ CLAMP (y + teamNumber * 6,0,co->characterSymbols.size()-1 )];
                
                buttons->at(i).colorInt = x;
                buttons->at(i).symbolInt = y;
            }
            
        }
    }
    
    void draw(){
        int hSpace = 125;
        int vSpace = 125;
        int leftSide = (1920 - 6 * vSpace )/2 + vSpace/2;
        int topSide  = (1080 - 6 * hSpace )/2 + hSpace/2 + 10;
        leftSide+=teamNumber * 1920;
        ofPushStyle();
        for(int i = 0 ; i<buttons->size();i++) {
            
            Button b = buttons->at(i);
            if(b.isPlaying){
                ofPushMatrix();
                
                
                int table =ceil( (b.table+1 + (teamNumber*-1 + 1))/ 2)+0.1 - 1;
                
                ofTranslate( (b.ID * hSpace) + leftSide, table * vSpace + topSide );
                
                
                ofSetColor(255);
                if(drawCircles[i])ofDrawCircle(0,0,30);
                
                ofSetColor(b.color);
                
                int w = 132 ;
                int h = 128 ;
                b.legs[0]->draw(-w/2,-h/2 , w,h);
                
                b.img->at(0).draw(-25,-25,25*2,25*2);
                
                ofSetColor(255);
                b.symbol->draw(-25,-25,25*2,25*2);
                
                
                if(co->debug)
                    co->font_small->drawString(ofToString(b.ID+1)+" :"+ofToString(b.table+1), -10, 20);
                ofPopMatrix();
            }
        }
        ofSetColor(255);
        if(forGround.isAllocated())forGround.draw(teamNumber * 1920 , 0 , 1920 , 1080);
        
        for(int i = 0; i<6; i++)
            co->font_small->drawString(ofToString((i+1)*2 - (teamNumber*-1+1)), leftSide - 125*2, topSide + i*vSpace +10);
        
        for(int i = 0; i<6; i++)
            co->font_small->drawString(letters[i], leftSide + hSpace*i - 10, topSide - 100);
        
        ofPopStyle();
    }
    
    void begin(ofxBox2d * world = nullptr){
        countDown = 5*60;
        co->designDone1 = false;
        co->designDone2 = false;
        
        
        drawCircles.resize(buttons->size());
        
        
        
        forGround.load("img/specialAssets/01_DesignForeground.png");
    }
    
    void reset(){
        drawCircles.clear();
    }
    
    
    
};
