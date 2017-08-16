#include "ofMain.h"
#include "Scene.h"

class Design : public Scene{
    
public:
    
    int p_numPlayers;
    int maxSymbols = 0;
    int maxColors = 0;
    vector<ofColor>colors;
    vector<ofColor>colors2;
    vector<bool>drawCircles;
    double countDown = 5*60;
    ofImage forGround;
    vector<string>letters;
    
    string colorFile;
    string colorFile2;
    int numColors;
    void setup(commonObjects*_co, vector<Button>*b, string _colorFile, string _colorFile2, int _numColors){
        buttons = b;
        co = _co;
        colorFile = _colorFile;
        colorFile2 = _colorFile2;
        numColors = _numColors;
        
        string abc = "A-B-C-D-E-F";
        letters = ofSplitString(abc,"-");
    };
    
    
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
        
        if(isDifferent)countDown = 0.;
        return isDifferent;
       // return false;
    };
    
    void update(){
        
        if(countDown > 0)countDown -= ofGetLastFrameTime();
        else countDown = 0;
        
        int c = colors.size();
        int c2 = colors2.size();

        int numPlayers = co->numPresentButtons[teamNumber];

        maxColors = co->lookUp[numPlayers][0];
        maxSymbols = co->lookUp[numPlayers][1];
        
        //co->numSymbolsPresent[teamNumber] = maxSymbols;
        //co->numColorsPresent[teamNumber] = maxColors;
        
        
        for(int i=0; i<buttons->size(); i++) {
            ofVec3f data = buttons->at(i).getRawData();
            float normX = data.x > 1 ? data.x - 1. : data.x;
       
            int x = ofMap (normX,0,1,0,maxColors);
            int y = ofMap (data.y,0,1,0,maxSymbols); // symbol
            
            x = CLAMP (x,0,c-1);
            y = CLAMP (y,0,c2-1);
            
        //    int z =CLAMP( ofMap(data.z,0,2*PI,0,colors.size()), 0 , colors.size()-1);
            buttons->at(i).color[0]=colors[x];
            buttons->at(i).color[1]=colors2[y];
            // buttons->at(i).symbol=&co->characterSymbols[ CLAMP (y + teamNumber * 6,0,co->characterSymbols.size() )];
           
            buttons->at(i).colorInt = x;
            buttons->at(i).symbolInt = y;

            
        }
    }
    
    void draw(){
        int hSpace = 125;
        int vSpace = 125;
        int leftSide = (1920 - 6 * vSpace )/2 + vSpace/2;
        int topSide  = (1080 - 6 * hSpace )/2 + hSpace/2 + 10;
        leftSide+=teamNumber * 1920;
        for(int i = 0 ; i<buttons->size();i++) {
            
            Button b = buttons->at(i);
            if(b.isPlaying){
                ofPushMatrix();
                
                
                 int table =ceil( (b.table+1 + (teamNumber*-1 + 1))/ 2)+0.1 - 1;
                
                ofTranslate( (table * hSpace) + leftSide, b.ID * vSpace + topSide );
                
                
                ofSetColor(255);
                if(drawCircles[i] && countDown == 0)ofDrawCircle(0,0,30);
                b.draw(false);
                if(co->debug)
                    co->font_small->drawString(ofToString(b.table+1)+" :"+ofToString(b.ID+1), -10, 20);
                ofPopMatrix();
            }
        }
        forGround.draw(teamNumber * 1920 , 0 , 1920 , 1080);
        for(int i = 0; i<6; i++)
            co->font_small->drawString(letters[i], leftSide - 125*2, topSide + i*vSpace +10);
        
        for(int i = 0; i<6; i++)
            co->font_small->drawString(ofToString((i+1)*2 - (teamNumber*-1+1)), leftSide + hSpace*i - 10, topSide - 100);
        
    };
    
    void begin(ofxBox2d * world = nullptr){
        countDown = 5*60;
        co->designDone1 = false;
        co->designDone2 = false;
        
        
        drawCircles.resize(buttons->size());
        
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
        tex.clear();
        pix.clear();
        colors2.resize(numColors);
        cout << colorFile2 << endl;
        if(ofLoadImage(tex, colorFile2)){
            
            tex.readToPixels(pix);
            
            int indx = tex.getWidth() / numColors;
            int c = 0;
            for(int i = 0; i<numColors;i++){
                colors2[i]=pix.getColor(i*indx + indx/2 ,tex.getHeight()/2);
                
            }
        }
        forGround.load("img/specialAssets/01_DesignForeground.png");
    };
    
    void reset(){
        drawCircles.clear();
        colors.clear();
    };
    

    
};
