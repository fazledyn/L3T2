/*
* This is demo-1 of the NS3 CSE322 Update 1
*/

#include "ns3/ssid.h"
#include "ns3/wifi-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

#define TOTAL_NODES 5

int
main(int argc, char *argv[])
{

    bool tracing = true;

    /**
     * 
     *      123.13.1.0     121.11.1.0    122.12.1.0
     *      &   &   &   & ------------- &   &   &
     *      |   |   |   |      p2p      |   |   |
     *                  STA             STA
    */

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NS_LOG_UNCOND("Starting code");

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper p2p;
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));

    NetDeviceContainer p2pDevices;
    p2pDevices = p2p.Install(p2pNodes);

    NodeContainer leftStaNodes;
    leftStaNodes.Create(3);
    NodeContainer rightStaNodes;
    rightStaNodes.Create(2);

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

    NodeContainer leftApNode = p2pNodes.Get(0);
    NetDeviceContainer leftStaDevices;
    leftStaDevices = wifi.Install(phy, mac, leftStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssidLeft));
    NetDeviceContainer leftApDevice;
    leftApDevice = wifi.Install(phy, mac, leftApNode);

    Ssid ssidRight = Ssid("ssid-right");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssidRight),
                "ActiveProbing", BooleanValue(false));

    NodeContainer rightApNode = p2pNodes.Get(1);
    NetDeviceContainer rightStaDevices;
    rightStaDevices = wifi.Install(phy, mac, rightStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssidRight));
    NetDeviceContainer rightApDevice;
    rightApDevice = wifi.Install(phy, mac, rightApNode);

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
    InternetStackHelper internet;
    // Ipv4ListRoutingHelper routing;

    // routing.Add(olsr, 1);
    internet.SetRoutingHelper(olsr);

    internet.Install(leftStaNodes);
    internet.Install(rightStaNodes);
    internet.Install(p2pNodes);
    // internet.Install(leftApNode);
    // internet.Install(rightApNode);

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer p2pInterfaces;
    address.SetBase("121.11.1.0", "255.255.255.0");
    p2pInterfaces = address.Assign(p2pDevices);

    Ipv4InterfaceContainer leftApInterface, leftStaInterface;
    address.SetBase("123.13.1.0", "255.255.255.0");
    leftApInterface = address.Assign(leftApDevice);
    leftStaInterface = address.Assign(leftStaDevices);

    Ipv4InterfaceContainer rightApInterface, rightStaInterface;
    address.SetBase("122.12.1.0", "255.255.255.0");
    rightApInterface = address.Assign(rightApDevice);
    rightStaInterface = address.Assign(rightStaDevices);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApp = echoServer.Install(leftApNode.Get(0));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(leftApInterface.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApp = echoClient.Install(rightApNode.Get(0));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(10.0));

    if (tracing) {
        AsciiTraceHelper trace;
        p2p.EnableAsciiAll(trace.CreateFileStream("demo-1-p2p.tr"));
        p2p.EnablePcapAll("demo-1-p2p");

        phy.EnableAsciiAll(trace.CreateFileStream("demo-1-phy.tr"));
        phy.EnablePcapAll("demo-1-phy");
    }

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();
    flowMonitor->SetAttribute("DelayBinWidth", DoubleValue (0.001));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    flowMonitor->SerializeToXmlFile("flow-demo-1.xml", false, false);

    Simulator::Destroy();
    return 0;
}
