//
//  Scene.h
//  _ETK_March_2017
//
//  Created by Karina Jensen on 30/07/2017.
//
//

#include "Poco/TextConverter.h"
#include "Poco/TextEncoding.h"
#include "Poco/Unicode.h"
#include "Poco/UTF8String.h"
#include "Poco/String.h"

# include "ofxTrueTypeFontUC.h"
# include "button.h"
# include "ofxSvg.h"
# include "ofxBox2d.h"

#ifndef Scene_h
#define Scene_h
class Scene {
    public :
    
    Scene(){};
    virtual ~Scene(){};
    
    vector<ofPolyline> solidPolys;
    int teamNumber;
    
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void begin(ofxBox2d * world = nullptr) = 0;
    virtual void reset() = 0;
    
    virtual bool isDone(bool b = false) = 0;
    
    virtual float ease(float t, float b, float c, float d) {
        c -=b;
        t /= d;
        return c*t*t + b;
    }
    
    
    
    virtual vector<string> transformToCollumn(string str, int w, ofxTrueTypeFontUC * font ){
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
    virtual string timeToString(double time){
        time = floorf(time);
        if(time<0)time = 0;
        
        string timeString = "";
        int min = time / 60 ;
        int sec = time - min*60;
        
        timeString = ofToString(min,2,'0')+":";
        timeString.append(ofToString(sec,2,'0'));
        
        return timeString;
    }
    
    
    virtual string drawCollumn(vector<string> s, int x, int y, ofxTrueTypeFontUC * font ){
        //ofPushMatrix();
        //ofTranslate(x,y);
        
        //vector<string>words=ofSplitString(s,"#");
        
        for(int i = 0; i< s.size();i++){
            font->drawString(s[i], x, y+i*font->getLineHeight());
            
        }
        
        //ofPopMatrix();
    }
    virtual void test(ofxTrueTypeFontUC * font,int x,int y){font->drawString("test",x,y);}
    
    
    virtual string encode(string input, const string& inputEncoding, const string& outputEncoding) {
        Poco::TextEncoding::Ptr ie = Poco::TextEncoding::find(inputEncoding);
        Poco::TextEncoding::Ptr oe = Poco::TextEncoding::find(outputEncoding);
        Poco::TextConverter converter(*ie, *oe);
        string output;
        converter.convert(input, output);
        return output;
    }
    
    
    
    virtual void drawGridLetters(int t, int b, ofxTrueTypeFontUC * f){
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
    
    
    
    virtual vector<ofPolyline> getPolyline(ofxSVG svg, bool doble = false){
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
    
    virtual string getLine(string fileName, int num_line){
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

#endif /* Scene_h */
