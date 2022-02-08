#include "ns3/ssid.h"
#include "ns3/wifi-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-helper.h"

using namespace ns3;
using namespace std;

int
main(int argc, char *argv[])
{
    bool tracing = true;
    float interval = 1.0;
    int maxPackets = 2;
    int packetSize = 1024;

    int nLeft = 5;
    int nRight = 5;
    int from = 0;
    int to = 4;

    CommandLine cmd;
    cmd.AddValue("packetSize", "Packet size in bytes",     packetSize);
    cmd.AddValue("interval", "Interval between packets",   interval);
    cmd.AddValue("maxPackets", "Max no. of packet sent",   maxPackets);
    cmd.AddValue("nLeft", "No. of nodes on left",          nLeft);
    cmd.AddValue("nRight", "No. of nodes on right",        nRight);
    cmd.AddValue("from", "Node to send packet from",       from);
    cmd.AddValue("to", "Node to receive packet in",        to);
    cmd.Parse(argc, argv);

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    cout << "Program start" << endl;

    //
    //     10.8.4.0       11.8.4.0        12.8.4.0
    //  *   *   *   *   # ------- #   *   *   *   *
    //      STAs        @   p2p   @      STAs
    //

    NodeContainer wiredNodes;
    wiredNodes.Create(2);

    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(wiredNodes);

    NodeContainer leftStaNodes;
    leftStaNodes.Create(nLeft);

    NodeContainer rightStaNodes;
    rightStaNodes.Create(nRight);

    NodeContainer leftApNode = wiredNodes.Get(0);
    NodeContainer rightApNode = wiredNodes.Get(1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssidLeft = Ssid("ssid-left");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssidLeft),
                "ActiveProbing", BooleanValue(false));
    NetDeviceContainer leftStaDevices = wifi.Install(phy, mac, leftStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssidLeft));
    NetDeviceContainer leftApDevice = wifi.Install(phy, mac, leftApNode);


    Ssid ssidRight = Ssid("ssid-right");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssidRight),
                "ActiveProbing", BooleanValue(false));
    NetDeviceContainer rightStaDevices = wifi.Install(phy, mac, rightStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssidRight));
    NetDeviceContainer rightApDevice = wifi.Install(phy, mac, rightApNode);

    MobilityHelper mobility;
    mobility.SetPositionAllocator  ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue(0.0),
                                    "MinY", DoubleValue(0.0),
                                    "DeltaX", DoubleValue(5.0),
                                    "DeltaY", DoubleValue(7.0),
                                    "GridWidth", UintegerValue(3),
                                    "LayoutType", StringValue("RowFirst"));
    
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                            "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(leftStaNodes);
    mobility.Install(rightStaNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(leftApNode);
    mobility.Install(rightApNode);


    OlsrHelper olsr;
    Ipv4StaticRoutingHelper staticRouting;

    olsr.ExcludeInterface(leftApNode.Get(0), 1);
    olsr.ExcludeInterface(rightApNode.Get(0), 1);

    Ipv4ListRoutingHelper list;
    list.Add(staticRouting, 0);
    list.Add(olsr, 10);

    InternetStackHelper internetOlsr;
    internetOlsr.SetRoutingHelper(list);

    internetOlsr.Install(NodeContainer(leftStaNodes, leftApNode));
    internetOlsr.Install(NodeContainer(rightStaNodes, rightApNode));


    // Assgging IP Address

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer p2pInterfaces;
    address.SetBase("11.8.4.0", "255.255.255.0");
    p2pInterfaces = address.Assign(csmaDevices);

    Ipv4InterfaceContainer leftApInterface, leftStaInterface;
    address.SetBase("10.8.4.0", "255.255.255.0");
    leftStaInterface = address.Assign(leftStaDevices);
    leftApInterface = address.Assign(leftApDevice);

    Ipv4InterfaceContainer rightApInterface, rightStaInterface;
    address.SetBase("12.8.4.0", "255.255.255.0");
    rightStaInterface = address.Assign(rightStaDevices);
    rightApInterface = address.Assign(rightApDevice);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApp = echoServer.Install(leftStaNodes.Get(to));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(leftStaInterface.GetAddress(to), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(maxPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(interval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientApp = echoClient.Install(leftStaNodes.Get(from));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(10.0));
    
    if (tracing) {
        AsciiTraceHelper trace;
        csma.EnableAsciiAll(trace.CreateFileStream("beta_csma.tr"));
        csma.EnablePcapAll("beta_csma");

        phy.EnableAsciiAll(trace.CreateFileStream("beta_phy.tr"));
        phy.EnablePcapAll("beta_phy");
    }

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    flowMonitor->SerializeToXmlFile("beta.flowmonitor", false, false);

    Simulator::Destroy();
    return 0;
}
