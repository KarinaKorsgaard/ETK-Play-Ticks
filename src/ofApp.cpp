#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    co.sceneNumber = 12;
    trailShader.load("trail");
    
    box2d.init();
    box2d.setGravity(0, 0);
    box2d.createBounds(ofRectangle(0,0,1920 * 2,1080));
    box2d.enableEvents();
    
    //set common objects to point at fonts and button map
    syphon.setName("tick games");
    font_small.load("fonts/font.ttf", 30);
    font_medium.load("fonts/font.ttf", 65);

    co.fillLookUp();

    co.font_small = &font_small;
    co.font_medium = &font_medium;

    co.characterImgs.resize(2);
    co.characterImgs[0].load("img/characters/body.png");
    co.characterImgs[1].load("img/characters/dead.png");
    //co.avergaTick.load("characters/averageTick.png");
    
    co.legs[0].load("img/characters/arms/arm1.png");
    co.legs[1].load("img/characters/arms/arm2.png");
    co.legs[2].load("img/characters/arms/arm3.png");
    co.legs[3].load("img/characters/arms/arm4.png");
    
    co.celebration[0].load("videos/celebrationDefaultWinner.mov");
    co.celebration[1].load("videos/celebrationDefaultLooser.mov");
    
    
    co.numPresentButtons[0] = 0;
    co.numPresentButtons[1] = 0;
    
    co.characterSymbols.resize(12);
    for(int i = 0 ; i< 12 ; i++)
        co.characterSymbols[i].load("img/symbols/marker-"+ofToString(i+1, 2, '0')+".png");
    
    
    
    ofxXmlSettings xml;
    xml.load("config.xml");
    xml.pushTag("config");
    
    for(int i = 0; i<2;i++){
        teams[i].setup(i,&co);
        teams[i].box2d = &box2d;
        teams[i].time = 0;//xml.getValue("startTime", 0.0);
        co.size_break = xml.getValue("size_break_point", 0.0);
        co.size_lim = xml.getValue("size_limit", 0.0);
    }
    
    float startRad = xml.getValue("size", 0.0);
    float startVal=0.;
    startVal+=xml.getValue("startValue", 0.0);
    startVal=CLAMP(startVal, 1, 200);
    
    PORT = xml.getValue("PORT", 0);
    NUM_TABLES = xml.getValue("NUM_TABLES", 0);
    BUTTONS_PR_TABLE = xml.getValue("BUTTONS_PR_TABLE", 0);
    
    co.NUM_TABLES = NUM_TABLES;
    co.BUTTONS_PR_TABLE = BUTTONS_PR_TABLE;
    //co.refillCoef = xml.getValue("refillCoef", 0.0);
    
    receiver.setup(PORT);
    int portout = xml.getValue("PORT_OUT", 0);
    string ip = xml.getValue("PORT_OUT_IP", "");
    co.oscOut.setup(ip, portout);
    int portIn = xml.getValue("PORT_IN", 0);
    co.oscIn.setup(portIn);
    
    // set up buttons
    int indx = 0;
    int tableIndx = 0;
    
    for(int i =0; i < NUM_TABLES; i++){
        int teamNum = i%2;
        
        tableIndx = i;
        if(i+1>NUM_TABLES/2) tableIndx-= NUM_TABLES/2;
        
        for(int j = 0; j<BUTTONS_PR_TABLE;j++){
            
            Button b = * new Button;
            b.setup(j,i,teamNum, getAdress(0, i, j), getAdress(1, i, j), startVal, startRad, &box2d);
            
            b.img = &co.characterImgs;
            b.legs[0] = &co.legs[0];
            b.legs[1] = &co.legs[1];
            b.legs[2] = &co.legs[2];
            b.legs[3] = &co.legs[3];
            b.symbol = &co.characterSymbols[teamNum];
            
            b.box2Dcircle.get()->setData(new ButtonData());
            ButtonData * sd = (ButtonData*)b.box2Dcircle.get()->getData();
            sd->buttonID = indx - (teamNum * ((BUTTONS_PR_TABLE*NUM_TABLES)/2));
            sd->teamID = teamNum;
            sd->bHit	= false;
            sd->isPlaying = false;
            sd->contactStart = ofRandom(5);
            b.setPosition(ofRandom(1940), ofRandom(-100,-50));
            indx++;
            
            teams[teamNum].buttons.push_back(b);
            
        }
    }
    

    
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);

    
    //allocate framebuffer
    fbo.allocate(1920*2, 1080, GL_RGBA);
    ofEnableAntiAliasing();
    
    
    scenes.setName("scenes");
    for(int i = 0; i<16;i++){
        string name = cc.getLine("sceneNames.txt", i);
        if(name.length()>0){
            ofParameter<bool>p;
            p.set(name,false);
            scenes.add(p);
            b_scenes.push_back(p);
            co.sMap[i]=ofSplitString(name, "-")[1];
            cout << co.sMap[i] << endl;
          
        }else{
            break;
        }
    }
    
    p_b_scenes.resize(b_scenes.size());
    
    physics.setName("general");
    
    physics.add(reverseX.set("reverse X",true));
    physics.add(reverseY.set("reverse Y",true));
    physics.add(co.playSound.set("play sound",false));
    physics.add(co.sendAverageData.set("sendAverageData",false));
    
    physics.add(co.attraction.set("attraction",1,0,2500));
    physics.add(co.fc.set("fc position",0.05,0.01,0.4));
    physics.add(co.moveThemOut.set("move ticks out of walls", false));
    physics.add(co.logReport.set("logreport", false));
    //physics.setName("game controls");
    physics.add(co.deadTimer.set("dead time",5,1,30));
    physics.add(co.refillCoef.set("refill amount",startVal/2,0,startVal));
    //physics.add(co.drainCoefficient1.set("drain team 1",1,0,5));
    //physics.add(co.drainCoefficient2.set("drain team 2",1,0,5));

    
    design.setName("Design");
    design.add(co.designDone1.set("team 1 ready",false));
    design.add(co.designDone2.set("team 2 ready",false));
    
    gravity.setName("escalator and trail");
    gravity.add(co.moveBall.set("move ball",false));
    gravity.add(co.ballSpeed.set("ball speeed",1.,0.,1.));
    gravity.add(co.gravity.set("gravity",1,0,50));
    gravity.add(co.escalatorSpeed.set("escalator speed",1.,0.,1.));
    gravity.add(co.maxTrailRadius.set("maxTrailRadius", 0.001, 0., 0.05));
    //gravity.add(co.trailRadius[1].set("trailRadius2", 0.1, 0., 1.));
    
    logic.setName("logic");
    logic.add(co.logicPrecision.set("logic precision", 20., 0.01, 100.f));
    for(int i = 0 ; i<6; i++){
        ofParameter<float>t;
        co.targetCircleRot.push_back(t);
        logic.add(co.targetCircleRot.back().set("circle"+ofToString(i+1),0,0,360));
        
        ofParameter<float>f;
        co.factoryRotation.push_back(f);
        market.add(co.factoryRotation.back().set("factory"+ofToString(i+1),0,0,360));
    }
    logic.add(co.showLogicTargets.set(false));

    market.setName("factories");
    market.add(co.marketDone1.set("finish market 1",false));
    market.add(co.marketDone2.set("finish market 2",false));
    
    charades.setName("charades");
    charades.add(co.idleA.set("a is done",false));
    charades.add(co.idleB.set("b is done",false));
    
    guiScenes.setup(scenes);
    guiScenes.saveToFile("scenes.xml");
    
    gui.setup();
    gui.add(restartApp.set("record state",false));
    gui.add(resetApp.set("RESET",false));
    gui.add(co.startScene.set("START SCENE",false));
    gui.add(co.startTime.set("START time",false));
    gui.add(co.startMovement.set("START movement",false));
    
    gui.add(physics);
    gui.add(design);
    gui.add(logic);
    gui.add(gravity);
    gui.add(charades);
    gui.add(market);
    
    gui.loadFromFile("settings.xml");
    
    guiScenes.setPosition(gui.getWidth()+20,gui.getPosition().x);
    
    // load the 8 sfx soundfile
    for(int i=0; i<2; i++) {
        sound[i].load("sounds/team_"+ofToString(i)+"_collision.mp3");
        sound[i].setMultiPlay(true);
        sound[i].setLoop(false);
        sound[i].setVolume(0.1f);
    }
    
    co.deathSound.load("sounds/deathSound.mp3");
    co.deathSound.setMultiPlay(true);
    co.deathSound.setLoop(false);
    co.deathSound.setVolume(0.1f);
    
    receivingTables.resize(NUM_TABLES);

    for(int i = 0;i<receivingTables.size();i++){
        receivingTables[i]=false;
    }
    
    groundPixels.setNumChannels(3);
    
    teams[0].setupScenes();
    teams[1].setupScenes();
    
    loadFromRestart();
}

//--------------------------------------------------------------
void ofApp::update(){
 
    if(restartApp)restart();
    if(resetApp)reset();
    
    int fr = ofGetFrameRate();
    ofSetWindowTitle(ofToString(fr));
    
    
    // make tabels red by interval
    alertCounter ++;
    if(alertCounter > ofGetFrameRate()*6){
        alertCounter=0;
        for(int i = 0;i<receivingTables.size();i++){
            receivingTables[i]=false;
        }
    }
    
    updateOsc();
    handleSceneChange();
    
    if(co.startScene)
    {
        co.startMovement = true;
        co.startTime = true;
    }
    
    
    teams[0].update();
    teams[1].update();
    
    
    if(co.sMap[co.sceneNumber] == "GroundGame")
    {
        updateGroundGame();
    }
    else
    {
        teams[0].reviveTicks(co.deadTimer,co.refillCoef);
        teams[1].reviveTicks(co.deadTimer,co.refillCoef);
    }
    if(co.sMap[co.sceneNumber] == "Fight")
    {
        if(co.moveBall){
            co.moveBall = false;
            int ran = ofRandom(2)>1 ? -1 : 1;
            fightBall->setVelocity(1*ran,0);
        }
        if(co.startMovement){
            ofVec2f v = fightBall->getVelocity();
         
            v.y*=0.95;
            v.normalize();
            v*=co.ballSpeed;
            //fightBall->getVelocity().x * 1.01, fightBall->getVelocity().y * 0.9
            fightBall->setVelocity(v);
            fightBall->update();
        }
    }
    
    
    box2d.update();
    
    

}

//--------------------------------------------------------------
void ofApp::draw(){

    
    fbo.begin();
    ofClear(0);
    
    //ofBackground(200);
    
    
    ofSetColor(255);
    int w = 1920;
    if(co.sMap[co.sceneNumber] == "Idle" || co.sMap[co.sceneNumber] == "Fight")
        w = 1920 * 2;
    
    if(co.background.isAllocated() && !co.debug){
        co.background.draw(0 , 0, w, 1080);
        if(w == 1920)co.background.draw(1920 , 0, 1920, 1080);
    }
    
    if(co.sMap[co.sceneNumber] == "GroundGame")groundVideo.draw(0, 0, 1920, 1080);
    
    if(co.sMap[co.sceneNumber] == "Trail"){
        Trail * trail1 = static_cast<Trail *>(teams[0].scenes[co.sMap[co.sceneNumber]]);
        Trail * trail2 = static_cast<Trail *>(teams[1].scenes[co.sMap[co.sceneNumber]]);
        
        trailShader.begin();
        trailShader.setUniform2f("u_resolution", 1920*2, 1080);
        trailShader.setUniform2f("u_mask1", trail1->filter.value().x, trail1->filter.value().y);
        
        
        trailShader.setUniform2f("u_mask2", trail2->filter.value().x, trail2->filter.value().y);
        trailShader.setUniform1f("u_radius1", co.trailRadius[0]);
        trailShader.setUniform1f("u_radius2", co.trailRadius[1]);
        
        
        trailShader.setUniform2f("u_beginLight1", trail1->trailStart.x, trail1->trailStart.y);
        trailShader.setUniform2f("u_beginLight2", trail2->trailStart.x, trail2->trailStart.y);
        
        trailShader.setUniform1f("u_time", ofGetElapsedTimef());
        ofSetColor(255);
        ofFill();
        ofDrawRectangle(0,0,1920*2 , 1080);
        trailShader.end();
    }
    if(co.sMap[co.sceneNumber] == "Fight" && co.teamIsDone.size() == 0 && co.startScene)
        fightBall->draw();
    
    teams[0].draw();
    teams[1].draw();
    
    fbo.end();
    
    ofDrawBitmapString(unemployedMessage, 10, gui.getHeight() + 30);
    ofDrawBitmapString(unemployedMessage1, 10, gui.getHeight() + 45);
    ofDrawBitmapString(unemployedMessage2, 10, gui.getHeight() + 60);
    syphon.publishTexture(&fbo.getTexture());
    
    
    gui.draw();
    guiScenes.draw();
    
    //ofDrawBitmapString("version "+ofToString(VERSION), 10, gui.getHeight() + 30);
    for(int i = 0 ; i<NUM_TABLES;i++){
        int red = receivingTables[i]?20:200;
        ofSetColor(red, 200-red , 0);
        ofDrawRectangle(gui.getWidth()*2 + 40, i*20+20, 18, 18);
    }
    
    
}
//--------------------------------------------------------------
void ofApp::handleSceneChange(){

    
    // toggle scenechanges
    int resent = -1;
    for(int i = 0; i<b_scenes.size();i++){
        if(b_scenes[i] && !p_b_scenes[i]){
            p_b_scenes[i]=true;
            resent = i;
            co.sceneNumber = i;
        }
    }
    
    if(resent!=-1){
        restart();
        co.startScene = false;
        co.startMovement = false;
        co.startTime = false;
        for(int i = 0; i<b_scenes.size();i++){
            if(i!=resent){
                b_scenes[i]=false;
                p_b_scenes[i]=false;
            }
        }
        if (co.sMap[co.sceneNumber] == "Maze" || co.sMap[co.sceneNumber] == "AverageMaze"|| co.sMap[co.sceneNumber] == "Fences"){
            box2d.disableEvents();
        }
        else{
            box2d.enableEvents();
        }

        if(co.sMap[co.sceneNumber] == "Fight"){
            fightBall = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
            fightBall.get()->setPhysics(10., 5., 10.0);
            fightBall.get()->setup(box2d.getWorld(), 1920,1080/2 , 40);
            fightBall.get()->alive = true;
            
            fightBall.get()->setVelocity(5, 5);
            
            Fight * f1 = static_cast<Fight *>(teams[0].scenes["Fight"]);
            Fight * f2 = static_cast<Fight *>(teams[1].scenes["Fight"]);
            
            f1->ball = fightBall;
            f2->ball = fightBall;
   
        }else if(co.sMap[p_sceneNumber] == "Fight"){
            fightBall->destroy();
        }
        
        
        if(co.sMap[co.sceneNumber] == "GroundGame"){
            if(!groundVideo.isLoaded()){
                groundVideo.load("videos/ground.mov");
                groundPixels.allocate(groundVideo.getWidth(), groundVideo.getHeight(), GL_RGB);
            }
            
            groundVideo.setLoopState(OF_LOOP_NONE);
            groundVideo.play();
            isGroundDone = false;
            for(int t = 0; t < 2 ; t++){
                for(int i = 0; i < teams[t].buttons.size(); i++){
                    
                    Button *b = &teams[t].buttons[i];
                    if(b->teamNumber == 1)
                        b->setPosition(b->getPos().x-1920  , b->getPos().y);
                }
            }
            
            
        }else if(co.sMap[p_sceneNumber] == "GroundGame"){
            groundVideo.stop();
        }
        
        if(co.sMap[p_sceneNumber] == "Factories" ){
            string abc = "A-B-C-D-E-F";
            vector<string>letters = ofSplitString(abc,"-");
            unemployedMessage = "unemplyed ticks:";
            unemployedMessage1 = ("team 1 : table "+ofToString(co.isUnemployed[0].x + 1) +" id: "+letters[co.isUnemployed[0].y]);
            unemployedMessage2 = ("team 2 : table "+ofToString(co.isUnemployed[1].x + 1) +" id: "+letters[co.isUnemployed[1].y]);
        }
        
        p_sceneNumber = co.sceneNumber;

    }
}
//--------------------------------------------------------------
void ofApp::refill(int team, float timef){
    float t=ofGetElapsedTimef()-timef;
    for(int i=0; i<teams[team].buttons.size(); i++) {
        
        Button * b = &teams[team].buttons.at(i);
        if(b->isPlaying){
            
            float oldValue = b->beforeRefillValue;
            
            float newValue = oldValue + co.refillCoef;
            b->setValue(CLAMP(ease(t,oldValue,newValue,co.refillTime),0,newValue));

            ofPushMatrix();
            ofTranslate(b->getGridPos( b->table - b->table%2*team , b->ID));
            ofTranslate(1920 * team,0);
            ofTranslate(800,0);
            b->draw(false);
            ofPopMatrix();
           
        }
    }
    if(t>co.refillTime && team == 0)co.refill1=false;
    if(t>co.refillTime && team == 1)co.refill2=false;
}
//--------------------------------------------------------------
void ofApp::exit(){

    co.oscIn.~ofxOscReceiver();
    co.oscOut.~ofxOscSender();
    if(co.logReport){
        string path = ofToString("reports/"+ofGetTimestampString("%m-%d_%H-%M")+".txt");
        ofFile newFile(ofToDataPath(path),ofFile::WriteOnly); //file doesn't exist yet
        newFile.create();

        for(int i = 0; i< co.logs.size(); i++){
            newFile <<co.logs[i]+"\n";

        }
        newFile.close();
    }
    
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 's')co.startScene = true;
    if(key == 'd')co.debug=!co.debug;

}

//--------------------------------------------------------------
// SPECIAL SCENES
//--------------------------------------------------------------
void ofApp::updateGroundGame(){
    //int aliveCounter = 0;
    int deadCounter = 0;
    
    if(groundVideo.isFrameNew())
        groundPixels = groundVideo.getPixels();
    
    
    
    for(int t = 0; t < 2 ; t++){
        for(int i = 0; i < teams[t].buttons.size(); i++){
            
            Button *b = &teams[t].buttons[i];
            b->update(co.attraction, false);
            if(groundPixels.isAllocated()){
                
                // kill
                if(!b->isDead() && b->isPlaying && !isGroundDone){
                    float x = b->getBiquadPos().x;
                    float y = b->getBiquadPos().y;
                    
                    if(x<1920){
                        x *= groundVideo.getWidth()/1920.;
                        y *= groundVideo.getHeight()/1080.;
                        
                        if(groundPixels.getColor(x,y).r < 10){
                            b->setValue(0);
                        }
                    }
                }
                if(b->isDead() && b->isPlaying)deadCounter++;
                
                if(deadCounter == co.numPresentButtons[0]+co.numPresentButtons[1] - 1){
                    //cout << "i tried to kill everyone" << endl;
                    isGroundDone = true;
                    break;
                }
            }
            if(isGroundDone)break;
        }
    }
    if(!isGroundDone && co.startMovement)groundVideo.update();
}

//--------------------------------------------------------------
void ofApp::updateOsc(){
    // change scene by incoming OSC
    while(co.oscIn.hasWaitingMessages()){
        ofxOscMessage m;
        co.oscIn.getNextMessage(m);
        if(m.getAddress() == "/next")
            b_scenes[CLAMP(co.sceneNumber+1,0,b_scenes.size()-1)]=true;
    }
    
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        for (int i = 0 ; i<alive.size(); i++) {
            if(!receivingTables[i])if(m.getAddress() == alive[i])receivingTables[i]=true;
        }
        
        if(co.startMovement){
            for(int t = 0; t < 2 ; t++){
                for(int i = 0; i < teams[t].buttons.size(); i++){
                    
                    Button *b = &teams[t].buttons[i];
                    
                    if (m.getAddress()==b->secondAdress){
                        if(m.getArgAsInt32(0)==0){
                            b->on = false;
                            break;
                        }
                    }
                    else if( m.getAddress()==b->address ){
                        b->on = true;
                        
                        float x = reverseX ? 1.f- (m.getArgAsFloat(0)/127.0f) : m.getArgAsFloat(0)/127.0f;
                        float y = reverseY ? 1.f- (m.getArgAsFloat(1)/127.0f) : m.getArgAsFloat(1)/127.0f;
                        b->set( x , y , m.getArgAsFloat(2) );
                        
                        if(b->on && !b->isPlaying){
                            b->isPlaying = true;
                            ButtonData *bd =(ButtonData*)b->box2Dcircle.get()->getData();
                            bd->isPlaying = true;
                            // prevent them getting stuck on edges
                            x = CLAMP(x, 0.1, 0.9);
                            y = CLAMP(y, 0.1, 0.9);
                            ofVec2f p = ofVec2f(x * 1920 + b->teamNumber*1920, y*1080);
                            
                            if (co.sMap[co.sceneNumber]=="Fences" || co.sMap[co.sceneNumber]=="AverageMaze")p.y = 1000;
                            if (co.sMap[co.sceneNumber]=="Maze")p.y = 60;
                            if (co.sMap[co.sceneNumber]=="Representative")p.x = 100;
                            if (co.sMap[co.sceneNumber]=="Factories")
                                p = ofVec2f(1920/2+b->teamNumber*1920, 1080/2);
                            
                            b->setPosition(p);
                            co.numPresentButtons[b->teamNumber]++;
                        }
                        
                        break;
                    }
                }
            }
        }
    }
}


//--------------------------------------------------------------
string ofApp:: getAdress(int _firstSecond, int _tabel, int _button){
    ofFile file;
    file.open(ofToDataPath("oscAdresses.txt"));
    ofBuffer b = file.readToBuffer();
    string result = "";
    int indx = 0;
    
    
    for (auto line : b.getLines()) {
        string str = line;
#ifdef TARGET_WIN32
        str = cc.encode(str, "UTF-8", "Windows-1252");
#endif
        if(alive.size()<12)alive.push_back(str + "/alive");
        
        if(indx == _tabel){
            result = str;
            if(_firstSecond == 1)
                result.append("/switch");
        }
        if(indx == 12+_button && _firstSecond == 0)
            result.append(str);
        if(indx == 12+6+_button && _firstSecond == 1)
            result.append(str);
        indx++;
    }
   // ofLog() << result <<endl;
    return result;
}

void ofApp:: restart(){
    
    
    ofxXmlSettings xml;
    xml.addValue("sceneNumber",co.sceneNumber);
    xml.addValue("team0time",teams[0].time);
    xml.addValue("team1time",teams[1].time);
    xml.addValue("numpeople0",co.numPresentButtons[0]);
    xml.addValue("numpeople1",co.numPresentButtons[1]);
    
    for(int u = 0; u<2;u++){
        for(int i = 0; i<36;i++){
            string team = "team"+ofToString(u)+"button";
            Button * b = &teams[u].buttons.at(i);
            xml.addValue(team+ofToString(i)+"isPlaying",b->isPlaying);
            xml.addValue(team+ofToString(i)+"symbol",b->symbolInt);
            xml.addValue(team+ofToString(i)+"color",b->colorInt);
            xml.addValue(team+ofToString(i)+"winner",b->isWinner);
            
            xml.addValue(team+ofToString(i)+"xpos",b->getPos().x);
            xml.addValue(team+ofToString(i)+"ypos",b->getPos().y);
            
        }
    }
    xml.save("restart.xml");
    restartApp = false;
}

void ofApp:: reset(){
    resetApp = false;
    ofxXmlSettings xml;
    xml.addValue("sceneNumber",0);
    xml.addValue("team0time",0);
    xml.addValue("team1time",0);
    xml.addValue("numpeople0",0);
    xml.addValue("numpeople1",0);
    
    for(int u = 0; u<2;u++){
        for(int i = 0; i<36;i++){
            string team = "team"+ofToString(u)+"button";
            Button * b = &teams[u].buttons.at(i);
            xml.addValue(team+ofToString(i)+"isPlaying",0);
            xml.addValue(team+ofToString(i)+"symbol",0);
            xml.addValue(team+ofToString(i)+"color",0);
            xml.addValue(team+ofToString(i)+"winner",0);
            
            xml.addValue(team+ofToString(i)+"xpos",ofRandom(1920*2));
            xml.addValue(team+ofToString(i)+"ypos",-300);
            
        }
    }
    xml.save("restart.xml");
    loadFromRestart();
    
}

void ofApp:: loadFromRestart(){
    ofxXmlSettings xml;
    cout << "loading xml"<< endl;
    if(ofFile::doesFileExist("restart.xml")){
        xml.load("restart.xml");
        cout << "RESTARTING"<< endl;
        
        teams[0].time = xml.getValue("team0time", 0);
        teams[1].time = xml.getValue("team1time", 0);
        cout << teams[0].time << endl;
        cout << xml.getValue("team0", 0)<< endl;
      
        co.numPresentButtons[0] = xml.getValue("numpeople0",0);
        co.numPresentButtons[1] = xml.getValue("numpeople1",0);
        

        for(int u = 0; u < 2; u++){
            for(int i = 0; i<36;i++){
                Button * b = &teams[u].buttons.at(i);
                string team = "team"+ofToString(u)+"button";
                
                teams[u].buttons.at(i).isPlaying = xml.getValue(team+ofToString(i)+"isPlaying",b->isPlaying);
                teams[u].buttons.at(i).symbolInt = xml.getValue(team+ofToString(i)+"symbol",0);
                teams[u].buttons.at(i).colorInt = xml.getValue(team+ofToString(i)+"color",0);
                teams[u].buttons.at(i).isWinner = xml.getValue(team+ofToString(i)+"winner",0);
                if(teams[u].buttons.at(i).isPlaying)
                    cout <<"symbol " << b->symbolInt << " color "<< b->colorInt << endl;
               
                float x = xml.getValue(team+ofToString(i)+"xpos",0);
                float y = xml.getValue(team+ofToString(i)+"ypos",0);
                
                cout <<"XX " << x << " YY "<< y << endl;
                teams[u].buttons.at(i).setPosition(x, y);
                
                
                Design * designColor = static_cast<Design *>(teams[u].scenes["Design"]);
                teams[u].buttons.at(i).color = designColor->colors[b->colorInt];
                teams[u].buttons.at(i).symbol = &co.characterSymbols[b->symbolInt];
            }
        }
        
        co.sceneNumber = xml.getValue("sceneNumber", 0);
        b_scenes[co.sceneNumber]=true;
        
        ofFile::removeFile("restart.xml");
    
    }

}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        if(co.sMap[co.sceneNumber] == "Fight"){
            if(e.a->GetType() != b2Shape::e_circle && e.a->GetType()!= b2Shape::e_edge  && e.b->GetType() == b2Shape::e_circle) {

                RacketData * racketData = (RacketData*)e.a->GetBody()->GetUserData();
                ButtonData * buttonData = (ButtonData*)e.b->GetBody()->GetUserData();
                
                
                if(!buttonData && racketData) {
                    ofxOscMessage m;
                    m.setAddress("/racket"+ofToString(racketData->teamID+1));
                    co.oscOut.sendMessage(m);
                    
                }
            }
        }

        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {

            ButtonData * aData = (ButtonData*)e.a->GetBody()->GetUserData();
            ButtonData * bData = (ButtonData*)e.b->GetBody()->GetUserData();


            if(aData && bData) {

                //sound[aData->teamID].play();
                if(!teams[bData->teamID].buttons[bData->buttonID].isDead() &&
                   !teams[aData->teamID].buttons[aData->buttonID].isDead() &&
                   teams[bData->teamID].buttons[bData->buttonID].on &&
                   teams[aData->teamID].buttons[aData->buttonID].on 
                   )
                {
                    float f = ofGetElapsedTimef();
                    if(co.playSound && !bData->bHit && !aData->bHit
                       && aData->contactStart!=bData->contactStart
                       ){
                        aData->contactStart = f;
                        bData->contactStart = f;
                        sound[bData->teamID].play();
                    }
                    aData->bHit = true;
                    bData->bHit = true;
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {

        ButtonData * aData = (ButtonData*)e.a->GetBody()->GetUserData();
        ButtonData * bData = (ButtonData*)e.b->GetBody()->GetUserData();

        if(aData) {
            aData->bHit = false;

        }

        if(bData) {
            bData->bHit = false;

        }
    }
}
