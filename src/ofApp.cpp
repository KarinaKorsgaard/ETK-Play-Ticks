#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    //set common objects to point at fonts and button map
    
    //load fonts
    font_x_small.load("fonts/GT.ttf", 20);
    
    
    font_small.load("fonts/GT.ttf", 30);
    font_medium.load("fonts/GT.ttf", 65);
    font_large.load("fonts/GT.ttf", 140);
    
    //set common objects to point at fonts and button map
    co.font_small = &font_small;
    co.font_medium = &font_medium;
    co.font_large = &font_large;
    
    co.sceneNumber = 2;
    
    ofxXmlSettings xml;
    xml.load("config.xml");
    xml.pushTag("config");
    
    for(int i = 0; i<2;i++){
        teams[i].setup(i,&co);
        teams[i].time = xml.getValue("startTime", 0.0);
        //teams[i].distance = xml.getValue("startDist", 0.0);
    }
    float startDist = xml.getValue("startDist", 0.0);
    PORT = xml.getValue("PORT", 0);
    NUM_TABLES = xml.getValue("NUM_TABLES", 0);
    BUTTONS_PR_TABLE = xml.getValue("BUTTONS_PR_TABLE", 0);
    
    //co.refillCoef = xml.getValue("refillCoef", 0.0);
    
    receiver.setup(PORT);
    
    // set up buttons
    int indx = 0;
    int tableIndx = 0;
    
    for(int i =0; i < NUM_TABLES; i++){
        int teamNum = (i+1)>NUM_TABLES/2 ? 1:0;
        
        tableIndx = i;
        if(i+1>NUM_TABLES/2) tableIndx-= NUM_TABLES/2;
        
        for(int j = 0; j<BUTTONS_PR_TABLE;j++){
            
            Button b = * new Button;
            
            teams[teamNum].buttons.push_back(b);
            teams[teamNum].buttons.back().setup(j,tableIndx,teamNum, getAdress(0, i, j), getAdress(1, i, j), startDist/2, &teams[teamNum].box2d);
            teams[teamNum].buttons.back().box2Dcircle.get()->setData(new ButtonData());
            ButtonData * sd = (ButtonData*)teams[teamNum].buttons.back().box2Dcircle.get()->getData();
            sd->buttonID = indx - (teamNum * ((BUTTONS_PR_TABLE*NUM_TABLES)/2));
            sd->teamID = teamNum;
            sd->bHit	= false;
            indx++;
            
        }
    }
    
    teams[0].setupScenes();
    teams[1].setupScenes();
    
    ofAddListener(teams[0].box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(teams[0].box2d.contactEndEvents, this, &ofApp::contactEnd);
    
    ofAddListener(teams[1].box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(teams[1].box2d.contactEndEvents, this, &ofApp::contactEnd);

    
    
    //allocate framebuffer
    fbo.allocate(1920*2, 1080, GL_RGBA);
    ofEnableAntiAliasing();
    
    scenes.setName("scenes");
        
    for(int i = 0; i<10;i++){
        string name = cc.getLine("sceneNames.txt", i);
        if(name.length()>0){
            ofParameter<bool>p;
            p.set(name,false);
            scenes.add(p);
            b_scenes.push_back(p);
        }
    }
    p_b_scenes.resize(b_scenes.size());
    
    physics.setName("physics");
    physics.add(co.attraction.set("attraction",1000,0,5000));
    physics.add(co.fc.set("low pass position",0.05,0.00,1.));
    
    physics.add(co.gravity.set("gravity",1,0,50));
    
    gameMechs.setName("game controls");
    //gameMechs.add(co.lessIsMore.set("less is more", false));
    gameMechs.add(co.refill1.set("refill 1", false));
    gameMechs.add(co.refill2.set("refill 2", false));
    gameMechs.add(co.refillCoef.set("refill amount",startDist/2,0,startDist));
    gameMechs.add(co.refillTime.set("refill animation time",5,1,30));
    
    gameMechs.add(co.drainCoefficient1.set("drain team 1",1,0,10));
    gameMechs.add(co.drainCoefficient2.set("drain team 2",1,0,10));

    spyGame.setName("spy game");
    spyGame.add(co.spyDrainer.set("drain on collide",0.2,0.,2));
    
    market.setName("market control");
    market.add(co.marketDone1.set("calculate market 1",false));
    market.add(co.marketDone2.set("calculate market 2",false));
    
    guiScenes.setup(scenes);
    guiScenes.saveToFile("scenes.xml");
    
    gui.setup();
    gui.add(co.logReport.set("log report",true));
    gui.add(time_energy.set("time/energy balance",0.5,0,1));
    gui.add(physics);
    gui.add(spyGame);
    gui.add(gameMechs);
    
    gui.add(market);
    
    gui.loadFromFile("settings.xml");
    
    guiScenes.setPosition(gui.getWidth()+20,gui.getPosition().x);
    
    // load the 8 sfx soundfile
    for(int i=0; i<N_SOUNDS; i++) {
        sound[i].load("sfx/"+ofToString(i)+".mp3");
        sound[i].setMultiPlay(true);
        sound[i].setLoop(false);
        sound[i].setVolume(0.1f);
    }
    receivingTables.resize(NUM_TABLES);
    for(int i = 0;i<receivingTables.size();i++){
        receivingTables[i]=false;
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    int resent = -1;
    for(int i = 0; i<b_scenes.size();i++){
        if(b_scenes[i] && !p_b_scenes[i]){
            p_b_scenes[i]=true;
            resent = i;
            co.sceneNumber = i;
        }
    }
    if(resent!=-1){
        for(int i = 0; i<b_scenes.size();i++){
            if(i!=resent){
                b_scenes[i]=false;
                p_b_scenes[i]=false;
            }
        }
        handleSceneChange();
    }
    double beforeOSC;
    if(co.debug)beforeOSC = ofGetElapsedTimef();
    // update from incoming osc
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(co.debug)cout<<m.getAddress()<<endl;
        // set button values
        if(!co.lock){
            for(int t = 0; t < 2; t++){
                for(int i = 0; i < teams[t].buttons.size(); i++){
                    
                    Button *b = &teams[t].buttons[i];
                    if (m.getAddress()==b->secondAdress){
                        if(m.getArgAsInt32(0)==0){
//                            if(teams[t].s04.spyId == -1)
//                                teams[t].s04.spyId=i;
//                            b->on = true;
//                            b->isPlaying = true;
//                        }
//                        else{
                            b->on = false;
                        }
                        
                    }
                    else if( m.getAddress()==b->address ){
                        b->on = true;
                        b->isPlaying = true;
                        b->set(m.getArgAsFloat(0)/127.0f, m.getArgAsFloat(1)/127.0f, m.getArgAsFloat(2));
                        int table = b->table + teams[t].teamId*NUM_TABLES/2;
                        receivingTables[table]=true;
                        
                        if(teams[t].s04.spyId == -1)
                            teams[t].s04.spyId=i;
                        
                    }
                    
                }
            }
        }
    }
    //if(co.debug)cout<< ofGetElapsedTimef()-beforeOSC << endl;
    
    if(!co.refill1){
        teams[0].update();
        if(co.sceneNumber == 4){
            if(teams[0].s04.areWeDone() && co.logReport){
                co.log("the spy outsmarted team 0");
            }
        }
    }
    if(!co.refill2){
        teams[1].update();
        if(co.sceneNumber == 4){
            if(teams[1].s04.areWeDone() && co.logReport){
                co.log("the spy outsmarted team 1");
            }
        }
    }
    
    
//    for(int i = 0; i<2;i++){
//        for(int b = 0; b<teams[i].buttons.size();b++){
//            if(teams[i].buttons[b].isColliding() && !sound[i].isPlaying()){
//               // sound[i].play();
//            }
//        }
//    }

    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    
    ofClear(0);
    ofSetColor(225, 220, 255);
    ofDrawRectangle(0,0,1920,1080);
    ofSetColor(255, 220, 220);
    ofDrawRectangle(1920,0,1920,1080);

        
    if(co.refill1){
        refill(0,easeRefill1);
        
    }else{
        teams[0].draw();
        easeRefill1 = ofGetElapsedTimef();
        teams[0].recordValues();
    }
    
    if(co.sceneNumber != 7){
        if(co.refill2){
            refill(1,easeRefill2);
        }
        else {
            ofPushMatrix();
            ofTranslate(1920, 0);
            teams[1].draw();
            ofPopMatrix();
            
            easeRefill2 = ofGetElapsedTimef();
            teams[1].recordValues();
        }
    }
  //  drawScores();
    fbo.end();
    
   // fbo.draw(0,0,fbo.getWidth()/3,fbo.getHeight()/3);
    syphon.publishTexture(&fbo.getTexture());
    gui.draw();
   // ofTranslate(gui.getWidth()+20, 0);
    guiScenes.draw();
    
    for(int i = 0 ; i<NUM_TABLES;i++){
        int red = receivingTables[i]?20:200;
        ofSetColor(red, 200-red , 0);
        ofDrawRectangle(gui.getWidth()*2 + 40, i*20+20, 18, 18);
    }

}

void ofApp::handleSceneChange(){
    
    for(int i = 0;i<receivingTables.size();i++){
        receivingTables[i]=false;
    }
    
    //if(key-'0'<10){
     //   co.sceneNumber = key-'0';
    co.log("");
    co.log("scene "+ofToString(co.sceneNumber)+" began at: " + ofGetTimestampString(co.timeStamp));
    co.log("team 1 collected value: "+ofToString(teams[0].getDistVal()));
    co.log("team 2 collected value: "+ofToString(teams[1].getDistVal()));
   // }
    // spy fuckups....
    
    if(teams[0].s04.spyId!=-1 && teams[1].s04.spyId!=-1){
        if(co.sceneNumber == 4){
            if(teams[0].s05.theWinner!=-1)teams[0].s04.spyId = teams[0].s05.theWinner;
            if(teams[1].s05.theWinner!=-1)teams[1].s04.spyId = teams[1].s05.theWinner;
            
            Button a = teams[0].buttons[teams[0].s04.spyId]; // 5
            Button b = teams[1].buttons[teams[1].s04.spyId]; // 8
            
            if(a.teamNumber == 0 && b.teamNumber == 1){
                if(a.isDead())a.value+=10;
                if(b.isDead())b.value+=10;
                
                teams[0].buttons[teams[0].s04.spyId] = b;
                teams[1].buttons[teams[1].s04.spyId] = a;
                
                shared_ptr <ofxBox2dCircle> c = teams[0].buttons[teams[0].s04.spyId].box2Dcircle;
                shared_ptr <ofxBox2dCircle> d = teams[1].buttons[teams[1].s04.spyId].box2Dcircle;
                
                teams[0].buttons[teams[0].s04.spyId].box2Dcircle = d;
                teams[1].buttons[teams[1].s04.spyId].box2Dcircle = c;
                
            }
        }
        if(co.sceneNumber != 4){
            
            Button a = teams[0].buttons[teams[0].s04.spyId];
            Button b = teams[1].buttons[teams[1].s04.spyId];
            if(a.teamNumber == 1 && b.teamNumber == 0){
                teams[0].buttons[teams[0].s04.spyId] = b;
                teams[1].buttons[teams[1].s04.spyId] = a;
                
                shared_ptr <ofxBox2dCircle> c = teams[0].buttons[teams[0].s04.spyId].box2Dcircle;
                shared_ptr <ofxBox2dCircle> d = teams[1].buttons[teams[1].s04.spyId].box2Dcircle;
                
                teams[0].buttons[teams[0].s04.spyId].box2Dcircle = d;
                teams[1].buttons[teams[1].s04.spyId].box2Dcircle = c;
                
            }
        }
    }
    
    if(co.sceneNumber == 7){
        teamSize=teams[1].buttons.size();
        
        if(teams[0].buttons.size() == teamSize ){
            for(int i = 0;i<teams[1].buttons.size();i++){
                Button b = * new Button;
                b = teams[1].buttons[i];
                // b.box2Dcircle->destroy();
                
                b.box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
                //virtual void setPhysics(float density, float bounce, float friction);
                b.box2Dcircle.get()->setPhysics(3.0, 0.0, 400.0);
                b.box2Dcircle.get()->setup(teams[0].box2d.getWorld(), 0, 0, 0);
                b.box2Dcircle.get()->alive = false;
                teams[0].buttons.push_back(b);
                
                teams[0].buttons.back().box2Dcircle.get()->setData(new ButtonData());
                ButtonData * sd = (ButtonData*)teams[0].buttons.back().box2Dcircle.get()->getData();
                sd->buttonID = teams[0].buttons.size() - 1;
                sd->teamID = 0;
                sd->bHit	= false;
            }
        }
    }
    
    if(co.sceneNumber != 7 && teamSize!=-1){
        if(teams[0].buttons.size() != teamSize ){
            for(int i = 0;i<teamSize;i++){
                teams[1].buttons[i].value = teams[0].buttons[i+teamSize].value;
                teams[0].buttons.back().box2Dcircle->destroy();
                teams[0].buttons.pop_back();
            }
        }
        //finaleWorld.disableEvents();
    }
}

void ofApp::refill(int team, float timef){
    float t=ofGetElapsedTimef()-timef;
    for(int i=0; i<teams[team].buttons.size(); i++) {
        
        Button * b = &teams[team].buttons.at(i);
        if(b->isPlaying){
            
            float oldValue = b->beforeRefillValue;
            
            float newValue = oldValue + co.refillCoef;
//            cout << oldValue<< endl;
//            cout << newValue<< endl;
//            cout << "" << endl;
            //  if(co.lessIsMore)newValue =oldValue + (b->beginningValue - b->value) * co.refillCoef;
            //  else newValue = oldValue + b->value * co.refillCoef;
            b->value=CLAMP(ease(t,oldValue,newValue,co.refillTime),0,newValue);
            
//            cout << b->value << endl;
//            cout << "" << endl;
            
            b->updateRadius();
            
            
            ofPushMatrix();
            ofTranslate(b->getGridPos());
            ofTranslate(1920 * team,0);
            ofTranslate(800,0);
            b->draw(false, true, true);
            ofPopMatrix();
        }
    }
    if(t>co.refillTime && team == 0)co.refill1=false;
    if(t>co.refillTime && team == 1)co.refill2=false;
}

void ofApp::exit(){

    if(co.logReport){
        string path = ofToString("reports/"+ofGetTimestampString("%m-%d-%M")+".txt");
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
    
    if(key == 'd')co.debug=!co.debug;
    if(key == 'p')co.lock=!co.lock;
    
}
void ofApp::drawScores(){
    float a = teams[0].getDistVal() * time_energy; // this goes down.
    float b = teams[1].getDistVal() * time_energy;
    
    float a_time =- teams[0].time * (1.f-time_energy); // this goes up.
    float b_time =- teams[1].time * (1.f-time_energy);
    
    float a_tot = a+a_time;
    float b_tot = b+b_time;
    
    float grandTot = a_tot+b_tot;
    
    ofSetColor(100, 100, 255);
    float a_w = ofMap(a_tot, 0, grandTot, 0, 1920*2);
    ofDrawRectangle(0, 10, a_w, 20);
    ofSetColor(255, 100, 100);
    float b_w = ofMap(b_tot, 0, grandTot, 0, 1920*2);
    ofDrawRectangle(a_w, 10, b_w, 20);
    
    
    float max = MAX(a+a_time,b+b_time);
    
    

}
//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        // if we collide with the ground we do not
        // want to play a sound. this is how you do that
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
            
            ButtonData * aData = (ButtonData*)e.a->GetBody()->GetUserData();
            ButtonData * bData = (ButtonData*)e.b->GetBody()->GetUserData();
            
            
            if(aData && bData) {
                
                //sound[aData->teamID].play();
                if(!teams[bData->teamID].buttons[bData->buttonID].isDead())aData->bHit = true;
                if(!teams[aData->teamID].buttons[aData->buttonID].isDead())bData->bHit = true;
                
                if(co.sceneNumber==7 && aData->bHit && bData->bHit){
                    
                    Button * b1 = &teams[aData->teamID].buttons[aData->buttonID];
                    Button * b2 = &teams[bData->teamID].buttons[bData->buttonID];
                    
//                    if(teams[0].finale.getTeamOne()!=0 || teams[0].finale.getTeamTwo()!=0){
//                        if(b1->teamNumber != b2->teamNumber){
//                            if(b1->value > b2->value){
//                                b1->value++;
//                                b2->value--;
//                            }
//                            else if(b1->value < b2->value){
//                                b2->value++;
//                                b1->value--;
//                            }else{
//                                b1->value--;
//                                b2->value--;
//                            }
//                        }
//                    }else{
                        if(b1->value > b2->value){
                            b1->value++;
                            b2->value--;
                        }
                        else if(b1->value < b2->value){
                            b2->value++;
                            b1->value--;
                        }else{
                            b1->value--;
                            b2->value--;
                        }
                   // }
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

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

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
