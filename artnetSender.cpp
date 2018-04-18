//
//  artnetSender.cpp
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#include "artnetSender.h"

artnetSender::artnetSender() : ofxOceanodeNodeModel("Artnet Sender"){
    parameters->add(pollButton.set("Poll Devices"));
    
    inputs.resize(1);
    parameters->add(inputs[0].set("Input 0", {0}, {0}, {1}));
    universeChooser.resize(1);
    parameters->add(createDropdownAbstractParameter("Output 0", {""}, universeChooser[0]));
    
    
    artnet.init();
    artnet.setSubNet(0);
    //Brodcast;
//    artnet.setNodeType(ARTNET_TYPE_RAW);
    artnet.setPortType(0, ARTNET_PORT_ENABLE_INPUT, ARTNET_DATA_DMX);
    artnet.setPortAddress(0, ARTNET_PORT_INPUT, 0);
    
    eventListeners.push_back(artnet.pollReply.newListener(this, &artnetSender::receivePollReply));
    artnet.start();
    sendPoll();
    
    eventListeners.push_back(pollButton.newListener(this, &artnetSender::sendPoll));
    eventListeners.push_back(inputs[0].newListener([&](vector<float> &vf){
        sendArtnet(vf, 0);
    }));
}

artnetSender::~artnetSender(){
    artnet.stop();
    artnet.close();
}

void artnetSender::sendArtnet(vector<float> &vf, int inputIndex){
    unsigned char data[vf.size()];
    for(int i = 0; i < vf.size(); i++){
        data[i] = vf[i]  * 127;
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
    cout<<endl;
    for(int i = 0 ; parameters->contains("Output " + ofToString(i)); i++){
        string optionsString;
        for(auto opt : nodeOptions){
            optionsString += opt + "-|-";
        }
        optionsString.erase(optionsString.end()-3, optionsString.end());
        string name = "Output " + ofToString(i);
        parameters->getGroup(name).getString(0).set(optionsString);
        parameters->getGroup(name).getInt(1).setMax(nodeOptions.size());
        ofNotifyEvent(dropdownChanged, name);
    }
}

