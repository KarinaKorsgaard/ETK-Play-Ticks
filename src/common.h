//
//  common.h
//  ETK_2017
//
//  Created by Karina Jensen on 14/01/17.
//
//

#ifndef common_h
#define common_h


#include "Poco/TextConverter.h"
#include "Poco/TextEncoding.h"
#include "Poco/Unicode.h"
#include "Poco/UTF8String.h"
#include "Poco/String.h"

# include "ofxTrueTypeFontUC.h"
# include "button.h"
# include "ofxSvg.h"

class commonObjects{
public:
    bool lock;
    bool start;
    bool debug;
    bool logDone = false;
    vector<string>logs;

    string timeStamp = "%H:%M";
    
    int sceneNumber;
    float size_break;
    float size_lim;
    
    ofxOscSender oscOut;
    ofxOscReceiver oscIn;
    
    ofParameter<bool>logReport;
    ofParameter<bool>sendAverageData;
    ofParameter<int> attraction;
    ofParameter<float>fc;
    ofParameter<float>refillTime;
    ofParameter<float>drainCoefficient1;
    ofParameter<float>drainCoefficient2;
    ofParameter<float>refillCoef;
    ofParameter<float>blockForce;
    ofParameter<int>gravity;
    ofParameter<int>gravityReward;
    ofParameter<bool>lessIsMore;
    ofParameter<float>divideTimeTime;
    
    ofParameter<bool>marketDone1;
    ofParameter<bool>marketDone2;
    ofParameter<float>marketReward;
    ofParameter<bool>idleA;
    ofParameter<bool>idleB;
    
    ofParameter<float>spyDrainer;
    ofParameter<float>spySpeed;
    ofParameter<bool>pauseTeam1;
    ofParameter<bool>pauseTeam2;
    ofParameter<float>thresY_gravity;
    
    ofParameter<bool>refill1;
    ofParameter<bool>refill2;
    ofParameter<float>deadTimer;

    ofParameter<float>jump;
    ofParameter<float>x_jump;
    
    ofParameter<bool>playSound;
    
    ofParameter<bool>startFinale;
    ofParameter<float>deadTimeFinale;

    ofxTrueTypeFontUC * font_x_small;
    ofxTrueTypeFontUC * font_small;
    ofxTrueTypeFontUC * font_medium;
    ofxTrueTypeFontUC * font_large;
    
    ofSoundPlayer deathSound;
    vector<ofImage> characterImgs;
    vector<ofImage> characterSymbols;
    
    map<string, int>sMap;
    
    int NUM_TABLES;
    int BUTTONS_PR_TABLE;
    
    ofImage avergaTick;
    
    void log(string l){
        if(logReport){
            logs.push_back(l);
        }
    }
private:
    
    
};

class commonFunctions{
public:
    float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    

    
    vector<string> transformToCollumn(string str, int w, ofxTrueTypeFontUC * font ){
        vector<string> result;
        string appending="";
        for(int i = 0; i<str.length();i++){
            string c = str.substr(i, 1);
            if (c == "\303") {
                c = str.substr(i, 2);
                i++;
            }
            appending.append(c);
            if(font->getStringBoundingBox(appending, 0, 0).width > w && ( c == " " || c == "-") ){
                //font->drawString(appending, 0, 0);
                result.push_back(appending);
              //  result.append("#");
                //ofTranslate(0, font->getLineHeight());
                appending.clear();
            }
        }
        //font->drawString(appending, 0, 0);
        result.push_back(appending);
               
        return result;
        
    }
    string timeToString(double time){
        time = floorf(time);
        if(time<0)time = 0;
        
        string timeString = "";
        int min = time / 60 ;
        int sec = time - min*60;
        
        timeString = ofToString(min,2,'0')+":";
        timeString.append(ofToString(sec,2,'0'));
        
        return timeString;
    }

    
    string drawCollumn(vector<string> s, int x, int y, ofxTrueTypeFontUC * font ){
        //ofPushMatrix();
        //ofTranslate(x,y);
        
        //vector<string>words=ofSplitString(s,"#");
        
        for(int i = 0; i< s.size();i++){
            font->drawString(s[i], x, y+i*font->getLineHeight());
            
        }
        
        //ofPopMatrix();
    }
    void test(ofxTrueTypeFontUC * font,int x,int y){font->drawString("test",x,y);}
    
    
    string encode(string input, const string& inputEncoding, const string& outputEncoding) {
        Poco::TextEncoding::Ptr ie = Poco::TextEncoding::find(inputEncoding);
        Poco::TextEncoding::Ptr oe = Poco::TextEncoding::find(outputEncoding);
        Poco::TextConverter converter(*ie, *oe);
        string output;
        converter.convert(input, output);
        return output;
    }
    
    
    
    void drawGridLetters(int t, int b, ofxTrueTypeFontUC * f){
        for(int i = 0 ; i<t; i++){
            //return ofPoint(table * 70 + int(table/4)*20 + (1920 -( 70*12+20*3 + 70) ), row * 70 + 70);
            ofPushMatrix();
            ofTranslate(i * 70 + int(i/4)*20 + (1920 -( 70*12+20*3 + 70) ) - f->getStringBoundingBox(ofToString(i+1), 0, 0).width/2 , f->getLineHeight() + 30);
            f->drawString(ofToString(i+1),0,0);
            ofPopMatrix();
            
        }
        vector<string>letters = {"A","B","C","D","E","F"};
        for(int i = 0 ; i<b; i++){
            //return ofPoint(table * 70 + int(table/4)*20 + (1920 -( 70*12+20*3 + 70) ), row * 70 + 70);
            ofPushMatrix();
            ofTranslate((1920 -( 70*12+20*3 + 70) ) - 90 , i * 70 + 140  + (70-f->getLineHeight())/2 );
            f->drawString(letters[i],0,0);
            ofPopMatrix();
        }
    }
    
//    void drawGrid(int t, int b, map<int,Button>*buttons){
//        ofPushStyle();
//        ofSetLineWidth(4);
//        ofSetColor(255);
//        ofNoFill();
//        for(auto b: *buttons){
//            ofPushMatrix();
//            ofTranslate(b.second.getGridPos());
//            ofDrawRectangle(-35,-35,70,70);
//            
//            ofPopMatrix();
//        }
//        ofPopStyle();
//    }
    
    
    vector<ofPolyline> getPolyline(ofxSVG svg, bool doble = false){
       // vector<ofPolyline> result;
        vector<ofPolyline>polys;
        
        for(int j = 0; j<svg.getNumPath();j++){
            ofPath p = svg.getPathAt(j);
            p.setPolyWindingMode(OF_POLY_WINDING_ODD);
            vector<ofPolyline>& lines = const_cast<vector<ofPolyline>&>(p.getOutline());
            int w = doble ? 1919*2 : 1919;
            if(lines[0].getBoundingBox().width<w || lines[0].getBoundingBox().height<1079)
                polys.push_back(lines[0]);
        }
        return polys;

    }
    
    string getLine(string fileName, int num_line){
        ofFile file;
        file.open(ofToDataPath(fileName));
        ofBuffer b = file.readToBuffer();

        
        string str = "";
        int indx = 0;
        
        for (auto line : b.getLines()) {
            if(indx == num_line){
                str = line;
                break;
            }
            indx++;
        }
        cout << str <<endl;
        return str;
    }
    
};


#endif /* common_h */
