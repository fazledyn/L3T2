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
    int nLeft = 5;
    int nRight = 5;

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    cout << "Program start" << endl;

    //
    //     10.8.4.0       11.8.4.0        12.8.4.0
    //  *   *   *   *   # ....... #   *   *   *   *
    //      STAs        @   p2p   @      STAs
    //

    NodeContainer leftApNode, rightApNode;
    NodeContainer leftStaNodes, rightStaNodes;

    leftApNode.Create(1);
    rightApNode.Create(1);

    leftStaNodes.Create(nLeft);
    rightStaNodes.Create(nRight);

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



    Ssid ssidCenter = Ssid("ssid-center");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssidCenter),
                "ActiveProbing", BooleanValue(false));
    NetDeviceContainer centerStaDevice = wifi.Install(phy, mac, rightApNode);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssidCenter));
    NetDeviceContainer centerApDevice = wifi.Install(phy, mac, leftApNode);


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
    internet.SetRoutingHelper(olsr);

    internet.Install(leftStaNodes);
    internet.Install(leftApNode);
    internet.Install(rightStaNodes);
    internet.Install(rightApNode);

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer leftApInterface, leftStaInterface;
    address.SetBase("10.8.4.0", "255.255.255.0");
    leftStaInterface = address.Assign(leftStaDevices);
    leftApInterface = address.Assign(leftApDevice);

    Ipv4InterfaceContainer centerApInterface, centerStaInterface;
    address.SetBase("11.8.4.0", "255.255.255.0");
    centerStaInterface = address.Assign(centerStaDevice);
    centerApInterface = address.Assign(centerApDevice);

    Ipv4InterfaceContainer rightApInterface, rightStaInterface;
    address.SetBase("12.8.4.0", "255.255.255.0");
    rightStaInterface = address.Assign(rightStaDevices);
    rightApInterface = address.Assign(rightApDevice);


    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApp = echoServer.Install(leftApNode.Get(0));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(leftApInterface.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(4));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApp = echoClient.Install(leftStaNodes.Get(0));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(10.0));
    
    if (tracing) {
        AsciiTraceHelper trace;
        phy.EnableAsciiAll(trace.CreateFileStream("alpha_phy.tr"));
        phy.EnablePcapAll("alpha_phy");
    }

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    flowMonitor->SerializeToXmlFile("alpha.flowmonitor", false, false);

    Simulator::Destroy();
    return 0;
}
