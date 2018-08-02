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
    
    void setup() override;
    
    virtual void presetSave(ofJson &json) override{
        vector<int> indexs;
        for(auto param : universeMap) indexs.push_back(param.first);
        json["Inputs"] = indexs;
        
        vector<string> outputNode;
        outputNode.resize(*max_element(indexs.begin(), indexs.end())+1);
        for(auto param : universeMap){
            outputNode[param.first] = nodeOptions[param.second];
        }
        json["outputNode"] = outputNode;
    }
    
    virtual void presetRecallBeforeSettingParameters(ofJson &json) override{
        if(json.count("Inputs") == 1){
            vector<int> indexs = json["Inputs"];
           
            for(auto param : inputMap){
                parameters->remove(param.second.getEscapedName());
                parameters->remove("Output_" + ofToString(param.first) + "_Selector");
                listeners.erase(param.first);
                ifNewCreatedChecker.erase(param.first);
            }
            for(auto param : universeMap){
                parameters->remove(param.second.getEscapedName());
            }
            universeMap.clear();
            inputMap.clear();
            ofNotifyEvent(parameterGroupChanged);
            int maxIndex = 0;
            for(int i : indexs){
                universeMap[i] = ofParameter<int>();
                addParameterToGroupAndInfo(createDropdownAbstractParameter("Output " + ofToString(i), nodeOptions, universeMap[i])).isSavePreset = false;
                inputMap[i] = ofParameter<vector<float>>();
                addParameterToGroupAndInfo(inputMap[i].set("Input " + ofToString(i), {-1}, {0}, {1})).isSavePreset = false;
                ifNewCreatedChecker[i] = true;
                listeners[i] = inputMap[i].newListener([&, i](vector<float> &val){
                    inputListener(i);
                });
                if(i > maxIndex) maxIndex = i;
            }
            ofNotifyEvent(parameterGroupChanged);
            ifNewCreatedChecker[maxIndex] = false;
            
            if(json.count("outputNode") == 1){
                vector<string> outputNode = json["outputNode"];
                for(auto i : indexs){
                    ptrdiff_t pos = find(nodeOptions.begin(), nodeOptions.end(), outputNode[i]) - nodeOptions.begin();
                    if(pos < nodeOptions.size()) {
                        universeMap[i] = pos;
                    }
                }
            }
        }
    }
    
private:
    void inputListener(int index);
    
    ofxArtNet artnet;
    void sendArtnet(vector<float> &vf, int inputIndex);
    void sendPoll();
    void receivePollReply(ofxArtNetNodeEntry &node);

    ofEventListeners eventListeners;
    
    ofParameter<void>   pollButton;
    vector<ofParameter<vector<float>>>  inputs;
    ofParameter<int> startSubnet;
    ofParameter<int> startUniverse;
    ofParameter<string> targetIp;
    vector<ofParameter<int>> universeChooser;
    
    
    map<int, ofParameter<int>> universeMap;
    map<int, ofParameter<vector<float>>> inputMap;
    map<int, bool> ifNewCreatedChecker;
    
    map<int, ofEventListener> listeners;
    
    vector<string> nodeOptions;
    vector<nodeOptionStruct> nodeOptionStructs;
    
    bool isPoll;
};

#endif /* artnetSender_h */
