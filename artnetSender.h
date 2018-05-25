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
    
//    void saveParameterArrange(ofJson &json){
//        vector<int> indexs;
//        for(auto param : parameterVector) indexs.push_back(param.first);
//        json["MultiPresetArrange"] = indexs;
//    }
//    
//    void loadParameterArrange(ofJson &json){
//        if(json.count("MultiPresetArrange") == 1){
//            vector<int> indexs = json["MultiPresetArrange"];
//            for(auto param : parameterVector){
//                group->remove(param.second.getEscapedName());
//                listeners.erase(param.first);
//                ifNewCreatedChecker.erase(param.first);
//            }
//            parameterVector.clear();
//            for(int i : indexs){
//                parameterVector[i] = ofParameter<T>();
//                parameterVector[i].set(baseParameter.getName() + " " + ofToString(i), baseParameter);
//                ifNewCreatedChecker[i] = false;
//                group->add(parameterVector[i]);
//                listeners[i] = parameterVector[i].newListener([&, i](T &val){
//                    inputListener(i);
//                });
//                ifNewCreatedChecker[i] = true;
//            }
//            ofNotifyEvent(parameterGroupChanged);
//        }
//    }
    
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
};

#endif /* artnetSender_h */
