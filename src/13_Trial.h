#include "ofMain.h"
#include "Scene.h"

class Trial : public Scene{
    
public:
    
    bool done = false;
    int looserTick;

    int vote[2];
    ofPolyline votingAreas[2];
    
    void setup(commonObjects*_co, vector<Button>*b){
        buttons = b;
        co = _co;
    };
    
    
    bool isDone(bool b = false){
  
        vote[0] = 0;
        vote[1] = 0;
        int fullAmount = 0;
        
        for(int i=0; i<buttons->size(); i++) {
            if (i != looserTick && buttons->at(i).isPlaying)
            {
                if (votingAreas[0].inside( buttons->at(i).getBiquadPos()))
                    vote[0]++;
                else if (votingAreas[1].inside( buttons->at(i).getBiquadPos()))
                    vote[1]++;
                
                fullAmount ++;
            }
        }
        
        done = false;
        if (vote[0] == fullAmount || vote[1] == fullAmount)
            done = true;
        
        return done;
    };
    
    
    void update(){
        if(!done){
            for(int i=0; i<buttons->size(); i++) {
                if (i != looserTick)
                    buttons->at(i).update(co->attraction);
                else
                    buttons->at(i).setPosition(1920*teamNumber + 998 , 445);
            }
            
            
            ofxOscMessage m;
            m.setAddress("/TrialTeam"+ofToString(teamNumber+1));
            m.addInt32Arg(vote[0]);
            m.addInt32Arg(vote[1]);
            co->oscOut.sendMessage(m);
            
        }
        else
        {
            ofxOscMessage m;
            int s = vote[0] > vote[1] ? 1 : 0;
            m.setAddress("/TrialTeamVote"+ofToString(teamNumber+1));
            m.addIntArg(s);
            co->oscOut.sendMessage(m);
        
        }
    }
    
    
    void draw(){
        ofSetColor(255);
        for(int i=0; i<buttons->size(); i++) {
            buttons->at(i).draw();
            if(co->debug){
                buttons->at(i).drawDebug();
            }
        }
    }
    
    
    void begin(ofxBox2d * world = nullptr){
        done = false;
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
        
        ofxSVG svg;
        svg.load("svg/13_Trial.svg");
        vector<ofPolyline>p  = getPolyline(svg);
        if (p.size()>1){
            int x1 = p[0].getCentroid2D().x;
            int x2 = p[1].getCentroid2D().x;
            
            if (x2 > x1){
                votingAreas[0] = p[0];
                votingAreas[1] = p[1];
            }
            else
            {
                votingAreas[0] = p[1];
                votingAreas[1] = p[0];
            }
        }
        else
            cout << "something is wrong in the vote!"<< endl;
    }
    
    
    void reset(){

    }
};
