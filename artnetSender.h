//
//  artnetSender.h
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#ifndef artnetSender_h
#define artnetSender_h

#include "ofxOceanodeNodeModel.h"
#include "ofxArtNode.h"

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
    
    void setup() override;
    void update(ofEventArgs &a) override;
    
    virtual void presetSave(ofJson &json) override{
        vector<string> outputNode;
        outputNode.resize(inputMap.size());
        for(auto param : universeMap){
            outputNode[param.first] = nodeOptions[param.second];
        }
        json["outputNode"] = outputNode;
    }
    
    virtual void loadCustomPersistent(ofJson &json) override{
        if(json.count("outputNode") == 1){
            vector<string> outputNode = json["outputNode"];
            for(auto input : inputMap){
                ptrdiff_t pos = find(nodeOptions.begin(), nodeOptions.end(), outputNode[input.first]) - nodeOptions.begin();
                if(pos < nodeOptions.size()) {
                    universeMap[input.first] = pos;
                }
            }
        }
    }
    
    virtual void presetRecallAfterSettingParameters(ofJson &json) override{
        for(auto input : inputMap){
            input.second = {-1};
        }
    }
    
private:
    void inputListener(int index);
    
    ofxArtNode artnet;
    void sendPoll();
    void nodeAdded(ofxArtNode::NodeEntry &node);
    void nodeErased(ofxArtNode::NodeEntry &node);
    
    void loadManualNodes();
    
    ofEventListeners eventListeners;
    
    ofParameter<void>   pollButton;
    
    ofParameter<string> ip;
    map<int, ofParameter<int>> universeMap;
    map<int, ofParameter<vector<float>>> inputMap;
    map<int, bool> ifNewCreatedChecker;
    
    map<int, ofEventListener> listeners;
    
    vector<string> nodeOptions;
    vector<nodeOptionStruct> nodeOptionStructs;
    
    bool manualNodes;
    vector<bool> isPollPerIndex;
};

#endif /* artnetSender_h */

