#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv6-flow-classifier.h"
#include "ns3/flow-monitor-helper.h"
#include <ns3/lr-wpan-error-model.h>
#include "ns3/network-module.h"

#include <string>
#include <iostream>
#include <vector>

using namespace ns3;
using namespace std;

ofstream outfile;

class MyApp : public Application
{
public:
  MyApp();
  virtual ~MyApp();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId(void);
  void Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void ScheduleTx(void);
  void SendPacket(void);

  Ptr<Socket> m_socket;
  Address m_peer;
  uint32_t m_packetSize;
  uint32_t m_nPackets;
  DataRate m_dataRate;
  EventId m_sendEvent;
  bool m_running;
  uint32_t m_packetsSent;
};

MyApp::MyApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId(void)
{
  static TypeId tid = TypeId("MyApp")
                          .SetParent<Application>()
                          .SetGroupName("Tutorial")
                          .AddConstructor<MyApp>();
  return tid;
}

void MyApp::Setup(Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void MyApp::StartApplication(void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType(m_peer))
  {
    m_socket->Bind();
  }
  else
  {
    m_socket->Bind6();
  }
  m_socket->Connect(m_peer);
  SendPacket();
}

void MyApp::StopApplication(void)
{
  m_running = false;

  if (m_sendEvent.IsRunning())
  {
    Simulator::Cancel(m_sendEvent);
  }

  if (m_socket)
  {
    m_socket->Close();
  }
}

void MyApp::SendPacket(void)
{
  Ptr<Packet> packet = Create<Packet>(m_packetSize);
  m_socket->Send(packet);

  if (++m_packetsSent < m_nPackets)
  {
    ScheduleTx();
  }
}

void MyApp::ScheduleTx(void)
{
  if (m_running)
  {
    Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
    m_sendEvent = Simulator::Schedule(tNext, &MyApp::SendPacket, this);
  }
}

int nFlow = 20;
double nodeSpeed = 0.1;

int nWirelessNodes = 20;
int TOTAL_PACKET = 100;
int PACKET_SIZE_KB = 10;
int PACKET_PER_SECOND = 3;
string DATA_RATE = to_string(PACKET_SIZE_KB * 8 * PACKET_PER_SECOND) + "kbps";

uint16_t sinkPort = 9;
double start_time = 0;
double simulationTime = 50;
uint64_t packetpersecond = 50;

void superComputation(int nWirelessNodes)
{
  NodeContainer WirelessNodes;
  WirelessNodes.Create(nWirelessNodes);

  std::cout << "wsn nodes: " << WirelessNodes.GetN() << std::endl;

  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
  channel->SetPropagationDelayModel(delayModel);

  int gridWidth = sqrt(nWirelessNodes);
  MobilityHelper mobility;

  cout << "GridWidth: " << gridWidth << endl;

  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(0.0),
                                "MinY", DoubleValue(0.0),
                                "DeltaX", DoubleValue(1.0),
                                "DeltaY", DoubleValue(1.0),
                                "GridWidth", UintegerValue(gridWidth),
                                "LayoutType", StringValue("RowFirst"));

  stringstream ssSpeed, ssPause;
  ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
  ssPause << "ns3::ConstantRandomVariable[Constant=0]";

  mobility.SetMobilityModel("ns3::GaussMarkovMobilityModel",
                            "MeanVelocity", StringValue(ssSpeed.str()),
                            "Bounds", BoxValue(Box(0, 25, 0, 25, 0, 25)),
                            "TimeStep", TimeValue(Seconds(3)));

  mobility.Install(WirelessNodes);

  LrWpanHelper lrWpanHelper;
  lrWpanHelper.SetChannel(channel);

  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install(WirelessNodes);
  lrWpanHelper.AssociateToPan(lrwpanDevices, 0);

  InternetStackHelper internetv6;
  internetv6.Install(WirelessNodes);

  SixLowPanHelper sixLowPanHelper;
  NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install(lrwpanDevices);

  Ipv6AddressHelper ipv6address;
  ipv6address.SetBase(Ipv6Address("2001:f00d::"), Ipv6Prefix(64));
  Ipv6InterfaceContainer WirelessDeviceInterfaces;
  WirelessDeviceInterfaces = ipv6address.Assign(sixLowPanDevices);
  WirelessDeviceInterfaces.SetForwarding(0, true);
  WirelessDeviceInterfaces.SetDefaultRouteInAllNodes(0);

  for (uint32_t i = 0; i < sixLowPanDevices.GetN(); i++)
  {
    Ptr<NetDevice> dev = sixLowPanDevices.Get(i);
    dev->SetAttribute("UseMeshUnder", BooleanValue(true));
    dev->SetAttribute("MeshUnderRadius", UintegerValue(10));
  }

  // flow
  int index = 0;
  for (int i = 0; i < nFlow/2; i++)
  {
    Address sinkAddress(Inet6SocketAddress(WirelessDeviceInterfaces.GetAddress(0, 1), sinkPort));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", Inet6SocketAddress(Ipv6Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = sinkHelper.Install(WirelessNodes.Get(0));

    sinkApps.Start(Seconds(start_time));
    sinkApps.Stop(Seconds(simulationTime));

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(WirelessNodes.Get(index + 1), TcpSocketFactory::GetTypeId());

    Ptr<MyApp> app = CreateObject<MyApp>();
    app->Setup(ns3TcpSocket, sinkAddress, PACKET_SIZE_KB * 1024, TOTAL_PACKET, DataRate(DATA_RATE));
    WirelessNodes.Get(index + 1)->AddApplication(app);
    app->SetStartTime(Seconds(start_time + 1));
    app->SetStopTime(Seconds(simulationTime));
  
    Simulator::Stop(Seconds(simulationTime));

    sinkPort++;
    index++;
    if (index == nWirelessNodes - 1)
    {
      index = 0;
    }
  }

  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

  Simulator::Run();
  
  Ptr<Ipv6FlowClassifier> classifier = DynamicCast<Ipv6FlowClassifier>(flowHelper.GetClassifier6());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

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

  cout << "stats size: " << stats.size() << endl;

  for (auto iter = stats.begin(); iter != stats.end(); ++iter)
  {
    // Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(iter->first);

    sent = iter->second.txPackets;
    received = iter->second.rxPackets;
    lost = iter->second.lostPackets;
    drop = sent - received - lost;
    tp = (iter->second.rxBytes * 8.0) / (abs(iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds()) * 1024);

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

  float dropRatio = (dropPkt * 100.00) / sentPkt;
  float deliveryRatio = (recvPkt * 100.00) / sentPkt;

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
  cout << endl
       << endl;

  // "Node, Throughput(kbps), Delay (ns), Drop Ratio, Delivery Ratio"
  outfile << nWirelessNodes << "," << avgTp << "," << avgDelay.GetNanoSeconds() << "," << dropRatio << "," << deliveryRatio << endl;

  Simulator::Destroy();
}


int main(int argc, char **argv)
{
  Packet::EnablePrinting();

  bool verbose;
  CommandLine cmd(__FILE__);
  cmd.AddValue("verbose", "verbose output", verbose);
  cmd.Parse(argc, argv);

  string fileName = "66_Wireless_Low_Node.dat";
  outfile.open(fileName, ios::app);
  outfile << "Node,Throughput (kbps),Delay (ns),Drop Ratio, Delivery Ratio" << endl;

  vector<int> nNode = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
  for (size_t i=0; i < nNode.size(); i++)
  {
    int nodeCount = nNode[i];
    superComputation(nodeCount);
  }

  outfile.close();
  return 0;
}
