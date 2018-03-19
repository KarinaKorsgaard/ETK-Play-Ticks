#include "Scene.h"
#include "ofMain.h"

class circle {
  public:
    // Button * b;
    // ofPath p;
    float targetRotation;
    bool isTarget = false;

    float p_rotation;
    float cur_rotation;
    float startRot;
    ofImage ring;

    void setup(float start, float target) {
        targetRotation = target;
        cur_rotation = start;
        p_rotation = start;
        startRot = start;
    }
    void setTarget(float t) { targetRotation = t; }

    float update(float precision, float attraction, float z) {
        if (cur_rotation < precision || cur_rotation > 360 - precision)
            isTarget = true;
        else
            isTarget = false;

        float dr = ofRadToDeg(z) - p_rotation;

        if (abs(dr) < 355)
            cur_rotation += dr;
        if (cur_rotation > 360.f)
            cur_rotation = 0.f;
        if (cur_rotation < 0.f)
            cur_rotation = 360.f;

        p_rotation = ofRadToDeg(z);

        return dr;
    }

    void draw(ofVec2f midt) {
        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(cur_rotation);
        int s = ring.getWidth();
        ring.draw(-s / 2, -s / 2, s, s);
        ofPopMatrix();
    }

    void drawTargets(ofVec2f midt) {
        ofPushMatrix();
        ofTranslate(midt);
        ofRotate(targetRotation);
        int s = ring.getWidth();
        ofSetColor(255, 100);
        ring.draw(-s / 2, -s / 2, s, s);
        ofPopMatrix();
    }
};

class Logic : public Scene {

  public:
    Logic(){};
    ~Logic(){};

    vector<circle> circles;
    vector<int> winnerButtons;
    float collectedRotation = 0.f;

    void setup(commonObjects *_co, vector<Button> *b) {
        buttons = b;
        co = _co;

        for (int i = 0; i < 6; i++) {
            circle c = *new circle;
            c.setup(ofRandom(360), 0);
            circles.push_back(c);
            circles.back().ring.load("img/specialAssets/rings/ring-0" +
                                     ofToString(i + 1) + ".png");
        }
    }

    bool isDone(bool b = false) {

        bool isInside = true;
        for (int i = 0; i < circles.size(); i++) {
            if (!circles[i].isTarget)
                isInside = false;
        }
        return isInside;
    }

    void update() {

        float temp = collectedRotation;
        collectedRotation = 0.f;

        for (int i = 0; i < winnerButtons.size(); i++) {

            if (!isDone())
                collectedRotation += circles[i].update(
                    co->logicPrecision, co->attraction,
                    buttons->at(winnerButtons[i]).getRawData().z);
            // if(co->showLogicTargets)
        }
        if (collectedRotation != temp) {
            ofxOscMessage m;
            m.setAddress("/collectedRotation" + ofToString(teamNumber + 1));
            m.addFloatArg(collectedRotation);
            co->oscOut.sendMessage(m);
        }
    }

    void draw() {
        ofSetColor(255);
        for (int i = 0; i < circles.size(); i++) {
            ofVec2f m = ofVec2f(1920 / 2 + teamNumber * 1920, 1080 / 2);
            circles[i].draw(m);
            if (co->showLogicTargets)
                circles[i].drawTargets(m);
        }
        for (int i = 0; i < winnerButtons.size(); i++) {
            ofPushMatrix();

            if (i > 2)
                ofTranslate(1920 + teamNumber * 1920 - 340,
                            324 + (i - 3) * 216);
            else
                ofTranslate(teamNumber * 1920 + 340, 324 + i * 216);
            ofRotateZ(ofRadToDeg(buttons->at(winnerButtons[i]).getRawData().z));
            buttons->at(winnerButtons[i]).draw(false, 2.2);
            if (co->debug)
                co->font_small->drawString(
                    ofToString(buttons->at(winnerButtons[i]).table) + " " +
                        ofToString(buttons->at(winnerButtons[i]).ID),
                    0, 0);

            ofPopMatrix();
        }
    }

    void begin(ofxBox2d *world = nullptr) {

        winnerButtons.clear();

        vector<int> firstOn;
        int count = 0;

        for (int b = 0; b < buttons->size(); b++) {

            // cout << firstOn.back() << endl;
            if (buttons->at(b).isPlaying) {
                if (buttons->at(b).isWinner && winnerButtons.size() < 6) {
                    // circles[count].b = &buttons->at(b);
                    winnerButtons.push_back(b);
                    cout << "------------winbutton " << count << " is " << b
                         << " from team " << teamNumber + 1 << endl;

                } else {
                    firstOn.push_back(b);
                }
            }
        }

        if (firstOn.size() < 6) {
            for (int i = 0; i < 36; i++) {
                bool exists = false;
                for (int u = 0; u < firstOn.size(); u++) {
                    if (i == firstOn[u])
                        exists = true;
                }
                if (!exists)
                    firstOn.push_back(i);
            }
        }

        int i = 0;
        while (winnerButtons.size() < MIN(firstOn.size(), 6)) {
            int setTo = firstOn[i];
            i++;
            buttons->at(setTo).isWinner = true;
            winnerButtons.push_back(setTo);
            cout << "winbutton " << i << " was not found and is set to "
                 << setTo << "on team " << teamNumber + 1 << endl;
        }
        cout << "-----------------there are " << winnerButtons.size()
             << " winners" << endl;

        for (int i = 0; i < 6; i++) {
            circles[i].isTarget = false;

            float pRot = i > 0 ? circles[i - 1].p_rotation : 0;
            circles[i].cur_rotation = ofRandom(360);
            circles[i].p_rotation = pRot + ofRandom(60, 160);
            circles[i].p_rotation = int(circles[i].p_rotation) % 360;
            circles[i].setTarget(0);
        }
    }

    void reset() {

        for (int i = 0; i < buttons->size(); i++) {
            buttons->at(i).isWinner = false;
        }

        for (int i = 0; i < 6; i++) {
            circles[i].isTarget = false;
        }
    }
};
