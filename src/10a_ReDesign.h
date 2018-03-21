#include "Scene.h"
#include "ofMain.h"

class ReDesign : public Scene {

  public:
    int p_numPlayers;
    int maxSymbols = 0;
    int maxColors = 0;
    vector<ofColor> colors;

    int looserTick;
    ofImage forGround;
    vector<string> letters;
    bool done;
    string colorFile;

    int culpritColor, culpritSymbol;
    int numColors;

    void setup(commonObjects *_co, vector<Button> *b, string _colorFile,
               int _numColors) {
        buttons = b;
        co = _co;
        colorFile = _colorFile;
        numColors = _numColors;

        string abc = "A-B-C-D-E-F";
        letters = ofSplitString(abc, "-");

        ofTexture tex;
        ofPixels pix;
        colors.resize(numColors);
        if (ofLoadImage(tex, colorFile)) {

            tex.readToPixels(pix);

            int indx = tex.getWidth() / numColors;
            int c = 0;
            for (int i = 0; i < numColors; i++) {
                colors[i] =
                    pix.getColor(i * indx + indx / 2, tex.getHeight() / 2);
            }
        }
    };

    bool isDone(bool b = false) {

        done = true;
        for (int i = 0; i < buttons->size(); i++) {
            if (buttons->at(i).isPlaying && i != looserTick) {
                if (buttons->at(i).symbolInt != culpritSymbol ||
                    buttons->at(i).colorInt != culpritColor) {
                    done = false;
                }
            }
        }

        if (!co->startScene)
            done = false;
        // cout << diffCount<< "team" << teamNumber << done << endl;
        return done;
    };

    void update() {
        if (!done && co->startScene) {
            int c = colors.size();
            int s = co->characterSymbols.size();

            int numPlayers = co->numPresentButtons[teamNumber];

            maxColors = co->lookUp[numPlayers][0];
            maxSymbols = co->lookUp[numPlayers][1];

            int minColors = MAX(maxColors - 1, 1);

            int howManyGetsMore = numPlayers - (minColors * maxSymbols);
            // howManyGetsMore = maxSymbols - howManyGetsMore;

            int count = 0;
            for (int i = 0; i < buttons->size(); i++) {
                // if(buttons->at(i).isPlaying){
                ofVec3f data = buttons->at(i).getRawData();

                float delta_rotation = buttons->at(i).getRotationDelta();
                if (abs(delta_rotation) > 5)
                    delta_rotation = 0;
                
                buttons->at(i).designNumber += delta_rotation*co->rotationSpeed;
                if(buttons->at(i).designNumber<0)
                    buttons->at(i).designNumber=(minColors * maxSymbols + howManyGetsMore);
                
                if (buttons->at(i).designNumber > minColors * maxSymbols + howManyGetsMore)
                    buttons->at(i).designNumber = 0;


                int dn = buttons->at(i).designNumber;

                int x = dn / MAX(maxSymbols, 1);

                int y = dn - x * maxSymbols;

                buttons->at(i).color = colors[x];
                buttons->at(i).symbol = &co->characterSymbols[CLAMP(
                    y + teamNumber * 6, 0, co->characterSymbols.size() - 1)];

                if (i == looserTick && !co->canCulpritMove) {
                    x = culpritColor;
                    y = culpritSymbol;
                }

                buttons->at(i).colorInt = x;
                buttons->at(i).symbolInt = y;
            }
        }
    }

    void draw() {
        int hSpace = 125;
        int vSpace = 125;
        int leftSide = (1920 - 6 * vSpace) / 2 + vSpace / 2;
        int topSide = (1080 - 6 * hSpace) / 2 + hSpace / 2 + 10;
        leftSide += teamNumber * 1920;
        ofPushStyle();
        for (int i = 0; i < buttons->size(); i++) {

            Button b = buttons->at(i);
            if (b.isPlaying) {
                ofPushMatrix();

                int table =
                    ceil((b.table + 1 + (teamNumber * -1 + 1)) / 2) + 0.1 - 1;

                ofTranslate((b.ID * hSpace) + leftSide,
                            table * vSpace + topSide);

                ofSetColor(b.color);

                int w = 132;
                int h = 128;
                b.legs[0]->draw(-w / 2, -h / 2, w, h);

                b.img->at(0).draw(-25, -25, 25 * 2, 25 * 2);

                ofSetColor(255);
                if (b.symbol != NULL)
                    b.symbol->draw(-25, -25, 25 * 2, 25 * 2);

                if (co->debug)
                    co->font_small->drawString(ofToString(b.ID + 1) + " :" +
                                                   ofToString(b.table + 1),
                                               -10, 20);
                ofPopMatrix();
            }
        }
        
        ofSetColor(255);
        if (forGround.isAllocated())
            forGround.draw(teamNumber * 1920, 0, 1920, 1080);
        ofPopStyle();
    };

    void begin(ofxBox2d *world = nullptr) {
        co->designDone1 = false;
        co->designDone2 = false;

        forGround.load("img/specialAssets/11_ReDesignForeground.png");

        looserTick = -1;
        vector<int> randomLooser;

        for (int i = 0; i < buttons->size(); i++) {
            if (buttons->at(i).isPlaying) {
                buttons->at(i).symbol =
                    &co->characterSymbols[buttons->at(i).symbolInt];
                randomLooser.push_back(i);
                if (buttons->at(i).isLooser) {
                    looserTick = i;
                    cout << looserTick << " the looser tick was found "
                         << teamNumber << endl;
                }
            }
        }
        if (looserTick == -1) {
            int l = randomLooser.size() == 0
                        ? 0
                        : randomLooser[int(ofRandom(randomLooser.size() - 1))];
            looserTick = l;
        }

        culpritColor = buttons->at(looserTick).colorInt;
        culpritSymbol = buttons->at(looserTick).symbolInt;
        cout << looserTick << " the looser was not found is looser from "
             << teamNumber << endl;
    };

    void reset(){

    };
};
