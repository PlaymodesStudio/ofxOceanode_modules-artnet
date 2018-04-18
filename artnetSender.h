//
//  artnetSender.h
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#ifndef artnetSender_h
#define artnetSender_h

#include "ofxOceanodeNodeModel.h"
#include "ofxArtNet.h"

struct nodeOptionStruct{
    int subnet;
    int universe;
    string ip;
    nodeOptionStruct(int s, int u, string i) : subnet(s), universe(u), ip(i){};
    string getString(){return "Sub:" + ofToString(subnet) + " Univ:" + ofToString(universe) + " IP:" + ip;};
};

class artnetSender : public ofxOceanodeNodeModel{
public:
    artnetSender();
    ~artnetSender();
    
private:
    ofxArtNet artnet;
    void sendArtnet(vector<float> &vf, int inputIndex);
    void sendPoll();
    void receivePollReply(ofxArtNetNodeEntry &node);

    vector<ofEventListener> eventListeners;
    
    ofParameter<void>   pollButton;
    vector<ofParameter<vector<float>>>  inputs;
    ofParameter<int> startSubnet;
    ofParameter<int> startUniverse;
    ofParameter<string> targetIp;
    vector<ofParameter<int>> universeChooser;
    
    vector<string> nodeOptions;
    vector<nodeOptionStruct> nodeOptionStructs;
};

#endif /* artnetSender_h */
