//
//  artnetSender.cpp
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#include "artnetSender.h"

artnetSender::artnetSender() : ofxOceanodeNodeModel("Artnet Sender"){
    isPoll = false;
    isLoopback = false;
}

artnetSender::~artnetSender(){
    
}

void artnetSender::setup(){
    
    artnet.setup(ofxArtNode::getInterfaceAddr(0), "255.255.255.0");
    eventListeners.push(artnet.nodeAdded.newListener(this, &artnetSender::newNodeReceived));
    
    nodeOptions.clear();
    nodeOptions.push_back("None");
    
    if(ofxArtNode::getInterfaceAddr(0) == "127.0.0.1"){ //WE ARE WORKING WITH LOOPBACK
        isLoopback = true;
        for(int i = 0; i < 16; i++) nodeOptions.push_back("Sub 0 : Uni "  + ofToString(i));
    }else{
        parameters->add(pollButton.set("Poll Devices"));
    }
    
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
        
        if(isLoopback && universeMap[index] != 0){
            ArtDmx * dmx = artnet.createArtDmx(0, 0, universeMap[index]-1);
            for(int i = 0; i < inputMap[index].get().size(); i++){
                dmx->Data[i] = inputMap[index]->at(i)  * 255;
            }
            artnet.sendUniCast("127.0.0.1", 6454, (char*)dmx, dmx->getSize());
        }
        else if(universeMap[index] != 0 && universeMap[index] < (nodeOptionStructs.size()+1)){
            nodeOptionStruct option = nodeOptionStructs[universeMap[index]-1];
//            artnet.sendDmx_by_SU(0, option.subnet, option.universe, option.ip.data(), data.data(), 512);
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
//        artnet.sendDmx_by_SU(0, option.subnet, option.universe, option.ip.data(), data, 512);
    }
    
    //Broadcast
    //artnet.sendDmxRaw(startSubnet*16 + startUniverse, data, 512);
}

void artnetSender::sendPoll(){
    artnet.sendPoll();
    nodeOptions.clear();
    nodeOptions.push_back("None");
};

//void artnetSender::receivePollReply(ofxArtNetNodeEntry &node){
//    cout<<"NODE FOUND! " << "IP: " <<node.getIp()<< " - Subnet: "<<node.getSubnet()<<" Universes: -" ;
//    for(int i = 0; i < node.getPortCount(); i++){
//        nodeOptionStructs.push_back(nodeOptionStruct(node.getSubnet(), node.getUniverseOutput(i)%16, node.getIp()));
//        nodeOptions.push_back("Sub:" + ofToString(node.getSubnet()) + " Univ:" + ofToString(node.getUniverseOutput(i)%16) + " IP:" + node.getIp());
//        cout<<node.getUniverseOutput(i)%16<<"-";
//    }
//    std::fill(isPollPerIndex.begin(), isPollPerIndex.end(), true);
//    cout<<endl;
//}

void artnetSender::newNodeReceived(ofxArtNode::NodeEntry &node){
    cout<<"NODE FOUND! " << "IP: " <<node.address << " - Subnet: "<<node.pollReply.SubSwitch <<" Universes: -" ;
//    for(int i = 0; i < node.getPortCount(); i++){
//        nodeOptionStructs.push_back(nodeOptionStruct(node.getSubnet(), node.getUniverseOutput(i)%16, node.getIp()));
//        nodeOptions.push_back("Sub:" + ofToString(node.getSubnet()) + " Univ:" + ofToString(node.getUniverseOutput(i)%16) + " IP:" + node.getIp());
//        cout<<node.getUniverseOutput(i)%16<<"-";
//    }
    std::fill(isPollPerIndex.begin(), isPollPerIndex.end(), true);
    cout<<endl;
}
