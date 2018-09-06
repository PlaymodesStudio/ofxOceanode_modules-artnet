//
//  artnetSender.cpp
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#include "artnetSender.h"

artnetSender::artnetSender() : ofxOceanodeNodeModel("Artnet Sender"){
    isPoll = false;
}

artnetSender::~artnetSender(){
    if(artnet.isRunning()){
        artnet.stop();
        artnet.close();
    }
}

void artnetSender::setup(){
    artnet.init();
    artnet.setSubNet(0);
    //Brodcast;
    //    artnet.setNodeType(ARTNET_TYPE_RAW);
    artnet.setPortType(0, ARTNET_PORT_ENABLE_INPUT, ARTNET_DATA_DMX);
    artnet.setPortAddress(0, ARTNET_PORT_INPUT, 0);
    
    eventListeners.push(artnet.pollReply.newListener(this, &artnetSender::receivePollReply));
    artnet.start();
    
    nodeOptions.clear();
    nodeOptions.push_back("None");
    
    
    parameters->add(pollButton.set("Poll Devices"));
    
    int numInputs = 4;
    isPollPerIndex.resize(4, false);
    
    for(int i = 0; i < numInputs; i++){
        universeMap[i] = ofParameter<int>();
        addParameterToGroupAndInfo(createDropdownAbstractParameter("Output " + ofToString(i), nodeOptions, universeMap[i])).isSavePreset = false;
        inputMap[i] = ofParameter<vector<float>>();
        addParameterToGroupAndInfo(inputMap[i].set("Input " + ofToString(i), {-1}, {0}, {1})).isSavePreset = false;
        ifNewCreatedChecker[i] = true;
    }
    for(int i = 0; i < numInputs; i++){
        listeners[i] = inputMap[i].newListener([&, i](vector<float> &val){
            inputListener(i);
        });
    }
    sendPoll();
    
    eventListeners.push(pollButton.newListener(this, &artnetSender::sendPoll));
}

void artnetSender::inputListener(int index){
    //    if(inputMap[index].get()[0] != -1 && !ifNewCreatedChecker[index]){
    //        int newCreatedIndex = -1;
    //        for(int i = 0 ; newCreatedIndex == -1 ; i++){
    //            if(inputMap.count(i) == 0){
    //                newCreatedIndex = i;
    //            }
    //        }
    //        universeMap[newCreatedIndex] = ofParameter<int>();
    //        parameters->add(createDropdownAbstractParameter("Output " + ofToString(newCreatedIndex), nodeOptions, universeMap[newCreatedIndex]));
    //        inputMap[newCreatedIndex] = ofParameter<vector<float>>();
    //        addParameterToGroupAndInfo(inputMap[newCreatedIndex].set("Input " + ofToString(newCreatedIndex), {-1}, {0}, {1})).isSavePreset = false;
    //        ifNewCreatedChecker[newCreatedIndex] = false;
    //        listeners[newCreatedIndex] = inputMap[newCreatedIndex].newListener([&, newCreatedIndex](vector<float> &val){
    //            inputListener(newCreatedIndex);
    //        });
    //        ifNewCreatedChecker[index] = true;
    //        ofNotifyEvent(parameterGroupChanged);
    //    }
    //    else if(inputMap[index].get()[0] == -1){
    //        int removeIndex = -1;
    //        for(auto param : inputMap){
    //            if(param.second.get()[0] == -1 && param.first > removeIndex){
    //                removeIndex = param.first;
    //            }
    //        }
    //        parameters->remove(inputMap[removeIndex].getEscapedName());
    //        parameters->remove("Output_" + ofToString(removeIndex) + "_Selector");
    //        listeners.erase(removeIndex);
    //        inputMap.erase(removeIndex);
    //        universeMap.erase(removeIndex);
    //        ifNewCreatedChecker.erase(removeIndex);
    //        if(index != removeIndex){
    //            ifNewCreatedChecker[index] = false;
    //        }
    //        ofNotifyEvent(parameterGroupChanged);
    //    }else
    if(isPollPerIndex[index]){
        if(parameters->contains("Output " + ofToString(index) + " Selector")){
            string optionsString;
            for(auto opt : nodeOptions){
                optionsString += opt + "-|-";
            }
            optionsString.erase(optionsString.end()-3, optionsString.end());
            string name = "Output " + ofToString(index) + " Selector";
            parameters->getGroup(name).getString(0).set(optionsString);
            parameters->getGroup(name).getInt(1).setMax(nodeOptions.size()-1);
            ofNotifyEvent(dropdownChanged, name);
        }
        isPollPerIndex[index] = false;
    }
    if(inputMap[index].get()[0] != -1){
        //        unsigned char data[inputMap[index].get().size()];
        vector<unsigned char> data;
        data.resize(512, 0);
        for(int i = 0; i < inputMap[index].get().size(); i++){
            data[i] = inputMap[index].get()[i]  * 255;
        }
        
        //Unicast
        if(universeMap[index] != 0 && universeMap[index] < nodeOptionStructs.size()){
            nodeOptionStruct option = nodeOptionStructs[universeMap[index]-1];
            artnet.sendDmx_by_SU(0, option.subnet, option.universe, option.ip.data(), data.data(), 512);
        }
    }
}

void artnetSender::sendArtnet(vector<float> &vf, int inputIndex){
    unsigned char data[vf.size()];
    for(int i = 0; i < vf.size(); i++){
        data[i] = vf[i]  * 255;
    }
    
    //Unicast
    if(universeChooser[inputIndex] != 0 && universeChooser[inputIndex] < nodeOptionStructs.size()){
        nodeOptionStruct option = nodeOptionStructs[universeChooser[inputIndex]-1];
        artnet.sendDmx_by_SU(0, option.subnet, option.universe, option.ip.data(), data, 512);
    }
    
    //Broadcast
    //artnet.sendDmxRaw(startSubnet*16 + startUniverse, data, 512);
}

void artnetSender::sendPoll(){
    if(!artnet.isRunning()){
        artnet.start();
    }
    artnet.sendPoll();
    nodeOptions.clear();
    nodeOptions.push_back("None");
};

void artnetSender::receivePollReply(ofxArtNetNodeEntry &node){
    cout<<"NODE FOUND! " << "IP: " <<node.getIp()<< " - Subnet: "<<node.getSubnet()<<" Universes: -" ;
    for(int i = 0; i < node.getPortCount(); i++){
        nodeOptionStructs.push_back(nodeOptionStruct(node.getSubnet(), node.getUniverseOutput(i)%16, node.getIp()));
        nodeOptions.push_back("Sub:" + ofToString(node.getSubnet()) + " Univ:" + ofToString(node.getUniverseOutput(i)%16) + " IP:" + node.getIp());
        cout<<node.getUniverseOutput(i)%16<<"-";
    }
    std::fill(isPollPerIndex.begin(), isPollPerIndex.end(), true);
    cout<<endl;
    //    for(int i = 0 ; parameters->contains("Output " + ofToString(i) + " Selector"); i++){
    //        string optionsString;
    //        for(auto opt : nodeOptions){
    //            optionsString += opt + "-|-";
    //        }
    //        optionsString.erase(optionsString.end()-3, optionsString.end());
    //        string name = "Output " + ofToString(i) + " Selector";
    //        parameters->getGroup(name).getString(0).set(optionsString);
    //        parameters->getGroup(name).getInt(1).setMax(nodeOptions.size());
    //        ofNotifyEvent(dropdownChanged, name);
    //    }
}

