#include "ns3/ssid.h"
#include "ns3/wifi-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/nstime.h"
#include "ns3/dsr-module.h"

#include <iostream>
#include <vector>

using namespace ns3;
using namespace std;


std::ofstream outfile;

//  Default Values
int PACKET_SIZE_KB = 10;
int PACKET_PER_SECOND = 3;
string DATA_RATE = to_string(PACKET_SIZE_KB * 8 * PACKET_PER_SECOND) + "kbps";

//  Params
bool verbose = false;

void superComputation(int nNode, string fileName)
{
    cout << endl << endl << "---" << endl;
    cout << "nNode: " << nNode << endl;

    // double rss = -80;
    double txpDistance = 20.0;
    string phyMode = "DsssRate1Mbps";

    //  Important, otherwise NS3 becomes cringy
    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
    Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (txpDistance));

    NodeContainer adhocNodes;
    adhocNodes.Create(nNode);

    WifiHelper wifi;
    if (verbose)    wifi.EnableLogComponents ();  // Turn on all Wifi logging
    
    wifi.SetStandard (WIFI_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();

    wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel");
    wifiPhy.SetChannel (wifiChannel.Create ());

    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue (phyMode),
                                "ControlMode", StringValue (phyMode));
    // Set it to adhoc mode
    wifiMac.SetType ("ns3::AdhocWifiMac");

    NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

    uint gridWidth = sqrt(nNode);
    //  Mobility Model Settings
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(0.5),
                                  "DeltaY", DoubleValue(0.5),
                                  "GridWidth", UintegerValue(gridWidth),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(adhocNodes);

    InternetStackHelper internet;
    internet.Install(adhocNodes);

    // IPv4 Helper
    Ipv4AddressHelper address;
    address.SetBase("13.4.0.0", "255.255.255.0");
    Ipv4InterfaceContainer adhocInterfaces;
    adhocInterfaces = address.Assign(adhocDevices);

    Ptr<Node> serverNode;
    Ipv4Address serverIp, clientIp;

    Ptr<Node> clientNode;
    srand(time(NULL));

    int serverIndex = rand() % nNode;
    int clientIndex = rand() % nNode;

    serverNode = adhocNodes.Get(serverIndex);
    serverIp = adhocInterfaces.GetAddress(serverIndex);
    clientNode = adhocNodes.Get(clientIndex);
    clientIp = adhocInterfaces.GetAddress(clientIndex);

    if (verbose)
    {
        cout << "Server IP: " << serverIp << endl;
        cout << "Client IP: " << clientIp << endl;
    }

    int SERVER_PORT = 9;

    /* Install TCP Receiver on the access point */
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), SERVER_PORT));
    ApplicationContainer sinkApp = sinkHelper.Install(clientNode);

    /* Install TCP/UDP Transmitter on the station */
    OnOffHelper server("ns3::TcpSocketFactory", (InetSocketAddress(clientIp, SERVER_PORT)));

    server.SetAttribute("PacketSize", UintegerValue(PACKET_SIZE_KB * 1024));
    server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute("DataRate", DataRateValue(DataRate(DATA_RATE)));
    ApplicationContainer serverApp = server.Install(serverNode);

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    /* Start Applications */
    sinkApp.Start(Seconds(0.0));
    serverApp.Start(Seconds(1.0));

    Simulator::Stop(Seconds(8.0));
    Simulator::Run();

    // flowHelper.SerializeToXmlFile(fileName + ".xml", false, false);

    //  Output Measurement Varibales
    int total = 0;
    double avgTp = 0;
    double totalTp = 0;
    double tp = 0;
    int sentPkt = 0;
    int lostPkt = 0;
    int recvPkt = 0;
    int dropPkt = 0;
    int sent = 0;
    int received = 0;
    int lost = 0;
    int drop = 0;
    Time avgDelay;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    for (auto iter = stats.begin(); iter != stats.end(); ++iter)
    {
        // Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(iter->first);

        sent = iter->second.txPackets;
        received = iter->second.rxPackets;
        lost = iter->second.lostPackets;
        drop = sent - received - lost;
        tp = (iter->second.rxBytes * 8.0) / ((iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds()) * 1024);

        sentPkt += sent;
        lostPkt += lost;
        dropPkt += drop;
        recvPkt += received;
        totalTp += tp;
        avgDelay += iter->second.delaySum;

        total++;
    }

    avgTp = totalTp / total;
    if (recvPkt != 0)
        avgDelay = avgDelay / recvPkt;
    else 
        avgDelay = avgDelay;

    float dropRatio = (dropPkt * 100.00)/sentPkt;
    float deliveryRatio = (recvPkt * 100.00)/sentPkt;

    cout << endl;
    cout << "Aggregated Final Data" << endl;
    cout << "---------------------" << endl;
    cout << "Total Sent Packets: " << sentPkt << endl;
    cout << "Total Received Packets: " << recvPkt << endl;
    cout << "Total Lost Packets: " << lostPkt << endl;
    cout << "Total Drop Packets: " << dropPkt << endl;

    cout << "Packet Drop Ratio %: " << dropRatio << endl;
    cout << "Packet Delivery Ratio %: " << deliveryRatio << endl;
    cout << "Network Throughput: " << avgTp << " kbps" << endl;
    cout << "End To End Delay: " << avgDelay << endl;
    cout << "Total Flow id: " << total << endl;
    cout << endl << endl;

    // "Node, Throughput(kbps), Delay (ns), Drop Ratio, Delivery Ratio"
    outfile << nNode << "," << avgTp << "," << avgDelay.GetNanoSeconds() << "," << dropRatio << "," << deliveryRatio << endl;

    Simulator::Destroy();
}

int main(int argc, char *argv[])
{
    CommandLine cmd;
    cmd.AddValue("verbose", "Verbose output", verbose);
    cmd.Parse(argc, argv);

    /*
    *      & & && && & &&              all wireless                  & && & && & &
    *          nLeft     ---- leftAP ---- CENTER ---- rightAP ----    nRight
    */

    string fileName = "66_Wireless_High_Node";

    outfile.open(fileName + ".dat", ios_base::app);
    outfile << "Node,Throughput (kbps),Delay (ns),Drop Ratio,Delivery Ratio" << endl;

    vector<int> nNode = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (size_t i=0; i < nNode.size(); i++)
    {
        int nodeCount = nNode[i];
        superComputation(nodeCount, fileName);
    }

    outfile.close();
    return 0;
}
