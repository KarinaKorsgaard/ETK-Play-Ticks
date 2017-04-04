#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    //set common objects to point at fonts and button map
    syphon.setName("tick games");
    //load fonts
    font_x_small.load("fonts/GT.ttf", 20);
    
    
    font_small.load("fonts/GT.ttf", 30);
    font_medium.load("fonts/GT.ttf", 65);
    font_large.load("fonts/GT.ttf", 140);
    
    //set common objects to point at fonts and button map
    co.font_small = &font_small;
    co.font_medium = &font_medium;
    co.font_large = &font_large;
    
    co.characterImgs.resize(2);
    co.characterImgs[0].load("characters/body.png");
    co.characterImgs[1].load("characters/dead.png");
 //   co.characterImgs[2].load("characters/belly.png");
    
    co.characterSymbols.resize(5);
    for(int i = 0 ; i< 5 ; i++)co.characterSymbols[i].load("characters/symbols/s-0"+ofToString(i+1)+".png");
    co.sceneNumber = 0;
    
    ofxXmlSettings xml;
    xml.load("config.xml");
    xml.pushTag("config");
    
    for(int i = 0; i<2;i++){
        teams[i].setup(i,&co);
        teams[i].time = 0;//xml.getValue("startTime", 0.0);
        co.size_break = xml.getValue("size_break_point", 0.0);
        co.size_lim = xml.getValue("size_limit", 0.0);
    }
    float startRad = xml.getValue("size", 0.0);
    float startVal = xml.getValue("startValue", 0.0);
    
    PORT = xml.getValue("PORT", 0);
    NUM_TABLES = xml.getValue("NUM_TABLES", 0);
    BUTTONS_PR_TABLE = xml.getValue("BUTTONS_PR_TABLE", 0);
    
    co.NUM_TABLES = NUM_TABLES;
    co.BUTTONS_PR_TABLE = BUTTONS_PR_TABLE;
    //co.refillCoef = xml.getValue("refillCoef", 0.0);
    
    receiver.setup(PORT);
    int portout = xml.getValue("PORT_OUT", 0);
    string ip = xml.getValue("PORT_OUT_IP", "");
    cout << ip + ofToString(portout)<<endl;
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
            
            
            b.setup(j,i,teamNum, getAdress(0, i, j), getAdress(1, i, j), startVal, startRad, &teams[teamNum].box2d);
            b.size_lim = co.size_lim;
            b.size_break= co.size_break;
            b.img = &co.characterImgs;
            b.box2Dcircle.get()->setData(new ButtonData());
            ButtonData * sd = (ButtonData*)b.box2Dcircle.get()->getData();
            sd->buttonID = indx - (teamNum * ((BUTTONS_PR_TABLE*NUM_TABLES)/2));
            sd->teamID = teamNum;
            sd->bHit	= false;
            b.setPosition(ofRandom(1920), ofRandom(-100,-50));
            indx++;
            
            teams[teamNum].buttons.push_back(b);
            
        }
    }
    
    teams[0].setupScenes();
    teams[1].setupScenes();
    
    ofAddListener(teams[0].box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(teams[0].box2d.contactEndEvents, this, &ofApp::contactEnd);
    
    ofAddListener(teams[1].box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(teams[1].box2d.contactEndEvents, this, &ofApp::contactEnd);

    pingPong.setup(&co,&teams[0].buttons,&teams[1].buttons);
    pushGame.setup(&co);
    push2.setup(&co);
    
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
            co.sMap[ofSplitString(name, "-")[1]]=i;
           //cout <<" map  "+ ofSplitString(name, "-")[1] << endl;
        }
    }
    
    p_b_scenes.resize(b_scenes.size());
    
    physics.setName("physics");
    physics.add(co.attraction.set("attraction",1,0,500));
    physics.add(co.fc.set("fc position",0.05,0.01,0.4));
    
    
    gameMechs.setName("game controls");
    //gameMechs.add(co.lessIsMore.set("less is more", false));
    gameMechs.add(co.deadTimer.set("dead time",5,1,30));
//    gameMechs.add(co.refill1.set("refill 1", false));
//    gameMechs.add(co.refill2.set("refill 2", false));
    gameMechs.add(co.refillCoef.set("refill amount",startVal/2,0,startVal));
//    gameMechs.add(co.refillTime.set("refill animation time",5,1,10));
    
    
    gameMechs.add(co.drainCoefficient1.set("drain team 1",1,0,5));
    gameMechs.add(co.drainCoefficient2.set("drain team 2",1,0,5));

    gravity.setName("gravity game");
    gravity.add(co.gravity.set("gravity",1,0,50));
    gravity.add(co.jump.set("jumpiness",1,0,10));
    gravity.add(co.x_jump.set("attraction to x",0.001,0,.01));
    gravity.add(co.thresY_gravity.set("dy threshold",0.001,0,0.1));
    gravity.add(co.gravityReward.set("gravityReward",50,0,100));
    
    
    spyGame.setName("spy game");
    spyGame.add(co.spyDrainer.set("drain",0.2,0.,2));
    spyGame.add(co.spySpeed.set("Spy Speed",30.,0.,500));
    
    market.setName("market control");
    
    market.add(co.marketReward.set("reward",0.5,0.,2));
    market.add(co.divideTimeTime.set("divideTimeToButtons",50.,1.,100.));
    market.add(co.marketDone1.set("finish market 1",false));
    market.add(co.marketDone2.set("finish market 2",false));
    
    push.setName("PushGame");
    push.add(co.blockForce.set("repel force",0.2,0.,200));
    
//    finale.add(co.startFinale.set("begin",false));
//    finale.add(co.deadTimeFinale.set("push time",0.1,0.,2));
//    finale.add(finalePushDrain.set("push drain",0.01,0.,0.1));
    
    idle.setName("charades");
    idle.add(co.idleA.set("a is done",false));
    idle.add(co.idleB.set("b is done",false));
    
    guiScenes.setup(scenes);
    guiScenes.saveToFile("scenes.xml");
    
    gui.setup();
    gui.add(reverseX.set("reverse X",false));
    gui.add(reverseY.set("reverse Y",false));
    gui.add(alertDialog.set("Alert Dialogs",false));
    gui.add(co.logReport.set("log report",false));
    gui.add(co.playSound.set("play sound",false));
    
    gui.add(co.sendAverageData.set("sendAverageData",false));
    
  //  gui.add(time_energy.set("time energy balance",0.5,0,1));
    gui.add(physics);
    gui.add(gameMechs);
    gui.add(gravity);
    gui.add(spyGame);
    gui.add(push);
    gui.add(idle);
    
    
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
    alive_counter.resize(NUM_TABLES);
    
    for(int i = 0;i<receivingTables.size();i++){
        receivingTables[i]=false;
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    alertCounter ++;
    if(alertCounter > ofGetFrameRate()*6){
        alertCounter=0;
        for(int i = 0;i<receivingTables.size();i++){
            receivingTables[i]=false;
        }
    }
    
    while(co.oscIn.hasWaitingMessages()){
        ofxOscMessage m;
        co.oscIn.getNextMessage(m);
        if(m.getAddress() == "/next")b_scenes[CLAMP(co.sceneNumber+1,0,b_scenes.size())]=true;
    }
    
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
        //if(co.sceneNumber == co.sMap["GoOffEdge"]) Catastrophy? 
        handleSceneChange();
    }
    
    double beforeOSC;
    if(co.debug)beforeOSC = ofGetElapsedTimef();
    // update from incoming osc
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        //if(co.debug)cout << m.getAddress() << endl;
        for (int i = 0 ; i<alive.size(); i++) {
            if(!receivingTables[i])if(m.getAddress() == alive[i])receivingTables[i]=true;
        }
        
        if(!co.lock){
            int uT=2;
            
            if(co.sceneNumber == co.sMap["PushGame"] ||
               co.sceneNumber == co.sMap["GoOffEdge"]
               )uT = 1;
            
            for(int t = 0; t < uT ; t++){
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
                        
                        double x = reverseX ? 1.- (m.getArgAsFloat(0)/127.0f) : m.getArgAsFloat(0)/127.0f;
                        double y = reverseY ? 1.- (m.getArgAsFloat(1)/127.0f) : m.getArgAsFloat(1)/127.0f;
                        b->set( x , y , m.getArgAsFloat(2) );
                        
                        if(!b->isPlaying){
                            b->isPlaying = true;
                            b->box2Dcircle->setPosition(x, 10);
                        }
                        //int table = b->table + teams[t].teamId*NUM_TABLES/2;
                        
                        
                        if(teams[t].spy04.spyId == -1)
                            teams[t].spy04.spyId=i;
                        
                        break;
                    }
                    
                }
            }
        }
    }
    //if(co.debug)cout<< ofGetElapsedTimef()-beforeOSC << endl;
    if(co.sceneNumber != co.sMap["PushGame"] && co.sceneNumber != co.sMap["GoOffEdge"]){
        if(!co.refill1){
            teams[0].update();
            if(co.sceneNumber == co.sMap["SpyGame"]){
                if(teams[0].spy04.areWeDone() && co.logReport){
                    co.log("the spy outsmarted team 0");
                }
            }
        }
        if(!co.refill2){
            teams[1].update();
            if(co.sceneNumber == co.sMap["SpyGame"]){
                if(teams[1].spy04.areWeDone() && co.logReport){
                    co.log("the spy outsmarted team 1");
                }
            }
        }
        
//        if(teams[0].allAreDead() && teams[0].deadTime>co.deadTimer && !co.refill1){
//            co.refill1 = true;
//            ofxOscMessage m;
//            m.setAddress("/dead1");
//            co.oscOut.sendMessage(m);
//        }
//        if(teams[1].allAreDead() && teams[1].deadTime>co.deadTimer && !co.refill2){
//            co.refill2 = true;
//            ofxOscMessage m;
//            m.setAddress("/dead2");
//            co.oscOut.sendMessage(m);
//        }
    }
    if(co.sceneNumber == co.sMap["PingPong"] && !co.refill2 && !co.refill1){
        pingPong.update();
    }
    if(co.sceneNumber == co.sMap["PushGame"] && !co.refill2 && !co.refill1){
        pushGame.update();
    }
    if(co.sceneNumber == co.sMap["GoOffEdge"] && !co.refill2 && !co.refill1){
        push2.update();
    }
    if(co.sceneNumber != co.sMap["PushGame"] && co.sceneNumber != co.sMap["SpyGame"]){
        teams[0].reviveTicks(co.deadTimer,co.refillCoef);
        teams[1].reviveTicks(co.deadTimer,co.refillCoef);
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
//    if(alertDialog){
//        for(int i = 0;i<receivingTables.size();i++){
//            if(!receivingTables[i])alive_counter[i]+=ofGetLastFrameTime();
//            else alive_counter[i]=0;
//            
//            if(alive_counter[i]>15)ofSystemAlertDialog("table "+ofToString(i+1));
//        }
//    }
    
    if(co.playSound){
        for(int i = 0; i<2; i++){
            for(int j = 0 ; j<teams[i].buttons.size();j++){
                if(teams[i].buttons[j].isDead()){
                    if(teams[i].buttons[j].deadSoundCheck){
                        teams[i].buttons[j].deadSoundCheck=false;
                        co.deathSound.play();
                    }else{
                        teams[i].buttons[j].deadSoundCheck=true;
                    }
                    
                }
            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    

    fbo.begin();
    
    ofClear(0);
    ofSetColor(225, 220, 255);
    ofDrawRectangle(0,0,1920,1080);
    ofSetColor(255, 220, 220);
    ofDrawRectangle(1920,0,1920,1080);

    if(co.sceneNumber!= co.sMap["PingPong"] && co.sceneNumber!=co.sMap["PushGame"] && co.sceneNumber!=co.sMap["GoOffEdge"] ){
//        if(co.refill1){
//            refill(0,easeRefill1);
//            
//        }else{
            teams[0].draw();
        //    easeRefill1 = ofGetElapsedTimef();
           // teams[0].recordValues();
  //      }
        
        
//        if(co.refill2){
//            refill(1,easeRefill2);
//        }
//        else {
            ofPushMatrix();
            ofTranslate(1920, 0);
            teams[1].draw();
            ofPopMatrix();
            
       //     easeRefill2 = ofGetElapsedTimef();
         //   teams[1].recordValues();
     //   }
        
    }
    if(co.sceneNumber==co.sMap["PingPong"])
        pingPong.draw();
    if (co.sceneNumber==co.sMap["PushGame"])
        pushGame.draw();
    if (co.sceneNumber==co.sMap["GoOffEdge"])
        push2.draw();
    
    fbo.end();

    syphon.publishTexture(&fbo.getTexture());
    gui.draw();
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
    
    if(co.sceneNumber == co.sMap["PingPong"])pingPong.begin();
    if(p_sceneNumber  == co.sMap["PingPong"]) pingPong.reset();

    
    
    if( (p_sceneNumber == co.sMap["PushGame"] && teamSize!=-1) ){
        cout << "team 0 was " + ofToString(teams[0].buttons.size()) + " big"<<endl;
        pushGame.reset();
        
        if(teams[0].buttons.size() != teamSize ){
            for(int i = 0;i<teamSize;i++){
                
                
                teams[0].buttons.at(i).box2Dcircle->destroy();
                teams[0].buttons.at(i).box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
                //virtual void setPhysics(float density, float bounce, float friction);
                teams[0].buttons.at(i).box2Dcircle.get()->setPhysics(3., 0.0, 40.);
                teams[0].buttons.at(i).box2Dcircle.get()->setup(teams[0].box2d.getWorld(), teams[0].buttons.at(i).lastPos.x, teams[0].buttons.at(i).lastPos.y, 1);
                teams[0].buttons.at(i).box2Dcircle.get()->setRadius(teams[0].buttons.at(i).beginningRad);
                
                
                teams[1].buttons[i].setValue( teams[0].buttons[i+teamSize].getValue() ); // update value
                teams[0].buttons.back().box2Dcircle->destroy(); // destroy and remove.
                teams[0].buttons.pop_back();
                
            }
            
        }
        cout << "PUSH GAME 1 team 0 is now " + ofToString(teams[0].buttons.size()) + " big"<<endl;
    }
    if( (p_sceneNumber== co.sMap["GoOffEdge"] && teamSize!=-1) ){
        cout << "team 0 was " + ofToString(teams[0].buttons.size()) + " big"<<endl;
        push2.reset();
        
        if(teams[0].buttons.size() != teamSize ){
            for(int i = 0;i<teamSize;i++){
                
                
                teams[0].buttons.at(i).box2Dcircle->destroy();
                teams[0].buttons.at(i).box2Dcircle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
                //virtual void setPhysics(float density, float bounce, float friction);
                teams[0].buttons.at(i).box2Dcircle.get()->setPhysics(3., 0.0, 40.);
                teams[0].buttons.at(i).box2Dcircle.get()->setup(teams[0].box2d.getWorld(), teams[0].buttons.at(i).lastPos.x, teams[0].buttons.at(i).lastPos.y, 1);
                teams[0].buttons.at(i).box2Dcircle.get()->setRadius(teams[0].buttons.at(i).beginningRad);
                
                teams[1].buttons[i].setValue( teams[0].buttons[i+teamSize].getValue() ); // update value
                teams[0].buttons.back().box2Dcircle->destroy(); // destroy and remove.
                teams[0].buttons.pop_back();
                
            }
            
        }
        cout << "PUSH GAME 2 team 0 is now " + ofToString(teams[0].buttons.size()) + " big"<<endl;
    }

    
    
    //if(key-'0'<10){
     //   co.sceneNumber = key-'0';
    co.log("");
    co.log("scene "+ofToString(co.sceneNumber)+" began at: " + ofGetTimestampString(co.timeStamp));
    co.log("team 1 collected value: "+ofToString(teams[0].getDistVal()));
    co.log("team 2 collected value: "+ofToString(teams[1].getDistVal()));
   // }
    // spy fuckups....
    
    if(teams[0].spy04.spyId!=-1 && teams[1].spy04.spyId!=-1){
        if(co.sceneNumber == co.sMap["SpyGame"]){
            if(teams[0].gravity03.theWinner!=-1)teams[0].spy04.spyId = teams[0].gravity03.theWinner;
            if(teams[1].gravity03.theWinner!=-1)teams[1].spy04.spyId = teams[1].gravity03.theWinner;
            
            Button a = teams[0].buttons[teams[0].spy04.spyId]; // 5
            Button b = teams[1].buttons[teams[1].spy04.spyId]; // 8
            
            if(a.teamNumber == 0 && b.teamNumber == 1){
                if(a.isDead())a.addValue(20);
                if(b.isDead())b.addValue(20);
                
                teams[0].buttons[teams[0].spy04.spyId] = b;
                teams[1].buttons[teams[1].spy04.spyId] = a;
                
                shared_ptr <ofxBox2dCircle> c = teams[0].buttons[teams[0].spy04.spyId].box2Dcircle;
                shared_ptr <ofxBox2dCircle> d = teams[1].buttons[teams[1].spy04.spyId].box2Dcircle;
                
                teams[0].buttons[teams[0].spy04.spyId].box2Dcircle = d;
                teams[1].buttons[teams[1].spy04.spyId].box2Dcircle = c;
                
            }
        }
        if(p_sceneNumber == co.sMap["SpyGame"]){
            
            Button a = teams[0].buttons[teams[0].spy04.spyId];
            Button b = teams[1].buttons[teams[1].spy04.spyId];
            if(a.teamNumber == 1 && b.teamNumber == 0){
                teams[0].buttons[teams[0].spy04.spyId] = b;
                teams[1].buttons[teams[1].spy04.spyId] = a;
                
                shared_ptr <ofxBox2dCircle> c = teams[0].buttons[teams[0].spy04.spyId].box2Dcircle;
                shared_ptr <ofxBox2dCircle> d = teams[1].buttons[teams[1].spy04.spyId].box2Dcircle;
                
                teams[0].buttons[teams[0].spy04.spyId].box2Dcircle = d;
                teams[1].buttons[teams[1].spy04.spyId].box2Dcircle = c;
                
            }
        }
    }
    
    if(co.sceneNumber == co.sMap["PushGame"] ){
        
        teamSize=teams[1].buttons.size();
        
       // teams[0].box2d.getWorld()->ClearForces();
        
        cout << "team 0 was " + ofToString(teams[0].buttons.size()) + " big"<<endl;
        
        if(teams[0].buttons.size() == teamSize ){
            
            for(int i = 0;i<teams[1].buttons.size();i++){
                Button * old = &teams[1].buttons[i];
                
                Button b = * new Button;
            
                b.setup(old->ID, old->table , old->teamNumber, old->address, old->secondAdress, old->beginningValue, old->beginningRad, &teams[0].box2d);
                b.color = old->color;
                b.img = old->img;
                b.symbol = old->symbol;
                b.radius = old->radius;
                b.setValue(old->getValue());
                b.isPlaying = old->isPlaying;
                b.on = old->on;
                b.size_lim = old->size_lim;
                b.size_break = old->size_break;
//
                teams[0].buttons.push_back(b);

                
            }
            cout << "team 0 is now " + ofToString(teams[0].buttons.size()) + " big"<<endl;
            
            pushGame.begin(&teams[0].buttons);
            
            for(int b = 0; b<teams[1].buttons.size();b++){
                teams[1].buttons[b].box2Dcircle->alive = false;
               // teams[1].buttons[b].box2Dcircle->setRadius(0);
            }
            
            
            
        }
    }

    
    //
    
    if(co.sceneNumber == co.sMap["GoOffEdge"] ){
        
        teamSize=teams[1].buttons.size();
        
      //  teams[0].box2d.getWorld()->ClearForces();
        
        cout << "team 0 was " + ofToString(teams[0].buttons.size()) + " big"<<endl;
        
        if(teams[0].buttons.size() == teamSize ){
            
            for(int i = 0;i<teams[1].buttons.size();i++){
                Button * old = &teams[1].buttons[i];
                
                Button b = * new Button;
                
                b.setup(old->ID, old->table , old->teamNumber, old->address, old->secondAdress, old->beginningValue, old->beginningRad, &teams[0].box2d);
                b.color = old->color;
                b.img = old->img;
                b.symbol = old->symbol;
                b.radius = old->radius;
                b.setValue(old->getValue());
                b.isPlaying = old->isPlaying;
                b.on = old->on;
                b.size_lim = old->size_lim;
                b.size_break = old->size_break;
                //
                teams[0].buttons.push_back(b);
                
                
            }
            cout << "team 0 is now " + ofToString(teams[0].buttons.size()) + " big"<<endl;
            
            push2.begin(&teams[0].buttons);
            
            for(int b = 0; b<teams[1].buttons.size();b++){
                teams[1].buttons[b].box2Dcircle->alive = false;
              //  teams[1].buttons[b].box2Dcircle->setRadius(0);
            }
  
        }
    }
    
    
    
    
    p_sceneNumber = co.sceneNumber;
}

void ofApp::refill(int team, float timef){
    float t=ofGetElapsedTimef()-timef;
    for(int i=0; i<teams[team].buttons.size(); i++) {
        
        Button * b = &teams[team].buttons.at(i);
        if(b->isPlaying){
            
            float oldValue = b->beforeRefillValue;
            
            float newValue = oldValue + co.refillCoef;
            b->setValue(CLAMP(ease(t,oldValue,newValue,co.refillTime),0,newValue));
            b->updateRadius();
            
            
            ofPushMatrix();
            ofTranslate(b->getGridPos( b->table - b->table%2*team , b->ID));
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
    
    if(key == 'd')co.debug=!co.debug;
    if(key == 'p')co.lock=!co.lock;
    
}
void ofApp::drawScores(){
//    float a = teams[0].getDistVal() * time_energy; // this goes down.
//    float b = teams[1].getDistVal() * time_energy;
//    
//    float a_time =- teams[0].time * (1.f-time_energy); // this goes up.
//    float b_time =- teams[1].time * (1.f-time_energy);
//    
//    float a_tot = a+a_time;
//    float b_tot = b+b_time;
//    
//    float grandTot = a_tot+b_tot;
//    
//    ofSetColor(100, 100, 255);
//    float a_w = ofMap(a_tot, 0, grandTot, 0, 1920*2);
//    ofDrawRectangle(0, 10, a_w, 20);
//    ofSetColor(255, 100, 100);
//    float b_w = ofMap(b_tot, 0, grandTot, 0, 1920*2);
//    ofDrawRectangle(a_w, 10, b_w, 20);
//    
//    
//    float max = MAX(a+a_time,b+b_time);
//    
//    

}
//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
            
            ButtonData * aData = (ButtonData*)e.a->GetBody()->GetUserData();
            ButtonData * bData = (ButtonData*)e.b->GetBody()->GetUserData();
            
            
            if(aData && bData) {
                
                //sound[aData->teamID].play();
                if(!teams[bData->teamID].buttons[bData->buttonID].isDead() &&
                   !teams[aData->teamID].buttons[aData->buttonID].isDead()
                   )
                {
                    aData->bHit = true;
                    bData->bHit = true;
                    
                    if(co.playSound){
                        sound[bData->teamID].play();
                    }
                    
                }
                
//                if(co.sceneNumber==7 && aData->bHit && bData->bHit){
//                    
//                    Button * b1 = &teams[aData->teamID].buttons[aData->buttonID];
//                    Button * b2 = &teams[bData->teamID].buttons[bData->buttonID];
//                    
//
//                        if(b1->finaleValue > b2->finaleValue){
//                            b2->freezeUpdate = true;
//                            b2->freezeTimer = 0.;
//                            b2->box2Dcircle->setVelocity(0, 0);
//                            //b1->value-= b2->value*finalePushDrain;
//                        }
//                        else if(b1->finaleValue < b2->finaleValue){
//                            b1->freezeUpdate = true;
//                            b1->freezeTimer = 0.;
//                            b1->box2Dcircle->setVelocity(0, 0);
//                           // b2->value-= b1->value*finalePushDrain;
//                        }
//                    if(abs(b1->finaleValue-b2->finaleValue)>0){
//                        b1->finaleValue-=b2->value;
//                        b1->updatefinaleValue = true;
//                        b1->finaleValueTimer=0.;
//                        
//                        b2->finaleValue-=b1->value;
//                        b2->updatefinaleValue = true;
//                        b2->finaleValueTimer=0.;
//
//                    }
//                }
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
