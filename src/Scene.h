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
    
    vector<Button>*buttons;
    commonObjects * co;
    
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
    
    virtual void moveOutOfSolids(vector<ofPolyline>solidPolys){
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).getOutOfPolys(solidPolys);
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
            if(lines[0].getBoundingBox().width<w || lines[0].getBoundingBox().height<1079){
                
                vector<ofPoint>verts;
                for(auto p : lines[0].getVertices())
                    verts.push_back(ofPoint(p.x + (1920 * teamNumber), p.y));
                ofPolyline poly;
                poly.addVertices(verts);
                
                polys.push_back(poly);
            
            }
        }
        return polys;
        
    }
    
    


};

#endif /* Scene_h */
