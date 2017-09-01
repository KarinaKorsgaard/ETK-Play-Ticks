#include "ofMain.h"
#include "Scene.h"

class ReDesign : public Scene{
    
public:
    
    int p_numPlayers;
    int maxSymbols = 0;
    int maxColors = 0;
    vector<ofColor>colors;
    
    int looserTick ;
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
    };
    
    
    bool isDone(bool b = false){
        
        int diffCount = 0;
        
        for (int i = 0; i<buttons->size();i++){
            if (buttons->at(i).isPlaying && i != looserTick){
                
                for (int j = i+1; j<buttons->size();j++){
                    if ( buttons->at(j).isPlaying && j != looserTick){
                        
                        if (buttons->at(i).symbolInt != buttons->at(j).symbolInt &&
                            buttons->at(i).colorInt != buttons->at(j).colorInt){
                            diffCount++;
                            
                        }
                    }
                    else if (j == looserTick && i != looserTick){
                        if (buttons->at(i).symbolInt == buttons->at(j).symbolInt &&
                            buttons->at(i).colorInt == buttons->at(j).colorInt){
                            diffCount++;
                        }
                    }
                }
            }
        }
        
        done = diffCount == 0;
        
        return done;
        
    };
    
    void update(){
        
        int c = colors.size();
        int s = co->characterSymbols.size();
        
        int numPlayers = co->numPresentButtons[teamNumber];
        
        maxColors = co->lookUp[numPlayers][0];
        maxSymbols = co->lookUp[numPlayers][1];
        
        int minColors = maxColors == maxSymbols ? maxColors : maxColors-1;
        minColors=MAX(minColors,1);
        //int howManyGetsMore = numPlayers % minColors;
        int howManyGetsMore = numPlayers-(minColors*maxSymbols);
        int count = 0;
        for(int i=0; i<buttons->size(); i++) {
            if (i != looserTick){
                
                ofVec3f data = buttons->at(i).getRawData();
                float normX = data.x > 1 ? data.x - 1. : data.x;
                
                int x = ofMap (normX,0,1,0,minColors);
                int y = ofMap (data.z,0,2*PI,0,maxSymbols); // symbol
                
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
    };
    
    void begin(ofxBox2d * world = nullptr){
        co->designDone1 = false;
        co->designDone2 = false;
        
        forGround.load("img/specialAssets/11_ReDesignForeground.png");
        
        looserTick = -1;
        vector<int>randomLooser;
        for (int i = 0; i<buttons->size();i++){
            if (buttons->at(i).isPlaying){
                randomLooser.push_back(i);
                if (buttons->at(i).isLooser){
                    looserTick = i;
                }
            }
        }
        if(looserTick == -1){
            int l = randomLooser.size() == 0 ? 0 : randomLooser[int(ofRandom(randomLooser.size()-1))];
            looserTick = l;
        }
    };
    
    void reset(){
        
    };
    
    
    
};
