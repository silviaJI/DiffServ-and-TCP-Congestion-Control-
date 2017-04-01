// DiffServ and TCP Congestion Control
// Aim at setup five TCP flows
//                
//              
// ======== n10 ------ n11 ----- n12 ------ n13  ------------------   n14
//  |    |                              
// n0   n1             |          |          |              =======================
//                     n2         n3         n4              |    |    |    |    |
//                                                           n5   n6   n7   n8   n9
// n0 to n5
// n1 to n6                                  
// n2 to n7
// n3 to n8
// n4 to n9

#include <iostream>  
#include <fstream>  
#include <string>  
#include <cassert>  
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/animation-interface.h"
#include "ns3/netanim-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/packet-sink.h"
#include "ns3/queue.h"

using namespace ns3;
using namespace std;
 
//声明了一个日志构件，通过引用操作，可以实现打开或者关闭控制台日志的输出。 
NS_LOG_COMPONENT_DEFINE ("TcpControlScript");

class MyApp : public Application 
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, 
  uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_nPackets (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void 
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void 
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

/*
static void PrintTxQueue (Ptr <Queue> q0,Ptr <Queue> q1, Ptr <Queue> q2, Ptr <Queue> q3, Ptr <Queue> q4)
{
Simulator::Schedule (Seconds (0.001), &PrintTxQueue, q0, q1, q2, q3, q4);
std::cout << Simulator::Now ().GetSeconds () << "\t" << q0->GetNPackets()<< "\t" << q1->GetNPackets()<< "\t" << q2->GetNPackets()<< "\t" << q3->GetNPackets()<< "\t" << q4->GetNPackets()<<"\n";
}
*/
/**/
//cwnd
static void
CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
}


int
main (int argc, char *argv[])
{
  
  uint32_t packetSize = 1400;
  uint32_t packetnum = 3000;
  /**模擬環境參數設定**/
  /*Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpReno"));
  Config::SetDefault ("ns3::DropTailQueue::Mode", EnumValue(DropTailQueue::QUEUE_MODE_PACKETS));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (4));*/
  Config::SetDefault ("ns3::RedQueue::Mode", StringValue ("QUEUE_MODE_PACKETS"));
  Config::SetDefault ("ns3::RedQueue::MinTh", DoubleValue (1));
  Config::SetDefault ("ns3::RedQueue::MaxTh", DoubleValue (3));
  Config::SetDefault ("ns3::RedQueue::QueueLimit", UintegerValue (4));
  Config::SetDefault ("ns3::TcpSocketBase::MaxWindowSize", UintegerValue(20*1000));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue(1000)); //TCP maximum segment size in bytes (may be adjusted based on MTU discovery)
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue(1)); //TCP initial congestion window size (segments)    
  

 /**建立模擬環境**/
 NodeContainer nodes;
 nodes.Create (15);

 /*Install network stacks on the nodes*/
  InternetStackHelper internet;
  internet.InstallAll ();

 /**建立连线**/
 //将pc和router相连
 NodeContainer n0n10=NodeContainer(nodes.Get(0),nodes.Get(10));  
 NodeContainer n1n10=NodeContainer(nodes.Get(1),nodes.Get(10)); 
 NodeContainer n2n11=NodeContainer(nodes.Get(2),nodes.Get(11));  
 NodeContainer n3n12=NodeContainer(nodes.Get(3),nodes.Get(12)); 
 NodeContainer n4n13=NodeContainer(nodes.Get(4),nodes.Get(13));  
 NodeContainer n5n14=NodeContainer(nodes.Get(5),nodes.Get(14));
 NodeContainer n6n14=NodeContainer(nodes.Get(6),nodes.Get(14));  
 NodeContainer n7n14=NodeContainer(nodes.Get(7),nodes.Get(14));
 NodeContainer n8n14=NodeContainer(nodes.Get(8),nodes.Get(14));  
 NodeContainer n9n14=NodeContainer(nodes.Get(9),nodes.Get(14));
 //router间相连
 NodeContainer n10n11=NodeContainer(nodes.Get(10),nodes.Get(11));  
 NodeContainer n11n12=NodeContainer(nodes.Get(11),nodes.Get(12));
 NodeContainer n12n13=NodeContainer(nodes.Get(12),nodes.Get(13));  
 NodeContainer n13n14=NodeContainer(nodes.Get(13),nodes.Get(14));

  /**設定DataRate與Delay**/
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("100us"));
  
  pointToPoint.SetQueue ("ns3::RedQueue","LinkBandwidth",StringValue ("10Mbps"),"LinkDelay", StringValue ("100us"));
  NetDeviceContainer d0d10 = pointToPoint.Install (n0n10);
  NetDeviceContainer d1d10 = pointToPoint.Install (n1n10);
  NetDeviceContainer d2d11 = pointToPoint.Install (n2n11);
  NetDeviceContainer d3d12 = pointToPoint.Install (n3n12);
  NetDeviceContainer d4d13 = pointToPoint.Install (n4n13);
  NetDeviceContainer d5d14 = pointToPoint.Install (n5n14);
  NetDeviceContainer d6d14 = pointToPoint.Install (n6n14);
  NetDeviceContainer d7d14 = pointToPoint.Install (n7n14);
  NetDeviceContainer d8d14 = pointToPoint.Install (n8n14);
  NetDeviceContainer d9d14 = pointToPoint.Install (n9n14);
  NetDeviceContainer d10d11 = pointToPoint.Install (n10n11);
  NetDeviceContainer d11d12 = pointToPoint.Install (n11n12);
  NetDeviceContainer d12d13 = pointToPoint.Install (n12n13);
  NetDeviceContainer d13d14 = pointToPoint.Install (n13n14);

  /**設定node的IP Address**/
  Ipv4AddressHelper ipv4;  

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i10 = ipv4.Assign (d0d10);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i10 = ipv4.Assign (d1d10);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i11 = ipv4.Assign (d2d11);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i12 = ipv4.Assign (d3d12);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i13 = ipv4.Assign (d4d13);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i5i14 = ipv4.Assign (d5d14);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i6i14 = ipv4.Assign (d6d14);
  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i7i14 = ipv4.Assign (d7d14);
  ipv4.SetBase ("10.1.9.0", "255.255.255.0");
  Ipv4InterfaceContainer i8i14 = ipv4.Assign (d8d14);
  ipv4.SetBase ("10.1.10.0", "255.255.255.0");
  Ipv4InterfaceContainer i9i14 = ipv4.Assign (d9d14);
  ipv4.SetBase ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer i10i11 = ipv4.Assign (d10d11);
  ipv4.SetBase ("10.1.12.0", "255.255.255.0");
  Ipv4InterfaceContainer i11i12 = ipv4.Assign (d11d12);
  ipv4.SetBase ("10.1.13.0", "255.255.255.0");
  Ipv4InterfaceContainer i12i13 = ipv4.Assign (d12d13);
  ipv4.SetBase ("10.1.14.0", "255.255.255.0");
  Ipv4InterfaceContainer i13i14 = ipv4.Assign (d13d14);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /**建立接收端來接收封包**/
  uint16_t port = 50000;
  ApplicationContainer sinkApp;  
  Address sinkLocalAddress(InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);

  sinkApp.Add(sinkHelper.Install(n5n14.Get(0)));  
  sinkApp.Add(sinkHelper.Install(n6n14.Get(0)));
  sinkApp.Add(sinkHelper.Install(n7n14.Get(0)));  
  sinkApp.Add(sinkHelper.Install(n8n14.Get(0)));
  sinkApp.Add(sinkHelper.Install(n9n14.Get(0)));  

  sinkApp.Start (Seconds (0.5));
  sinkApp.Stop (Seconds (19.5));
    
  /**建立TCP Socket Client**/
  Ptr<Node> clientNodea = nodes.Get(0);
  Ptr<Socket> clientSocketa = Socket::CreateSocket(clientNodea,TcpSocketFactory::GetTypeId());
     
  Ptr<Node> clientNodeb = nodes.Get(1);
  Ptr<Socket> clientSocketb = Socket::CreateSocket(clientNodeb,TcpSocketFactory::GetTypeId());
          
  Ptr<Node> clientNodec = nodes.Get(2);
  Ptr<Socket> clientSocketc = Socket::CreateSocket(clientNodec,TcpSocketFactory::GetTypeId());
  
  Ptr<Node> clientNoded = nodes.Get(3);
  Ptr<Socket> clientSocketd = Socket::CreateSocket(clientNoded,TcpSocketFactory::GetTypeId());
  
  Ptr<Node> clientNodee = nodes.Get(4);
  Ptr<Socket> clientSockete = Socket::CreateSocket(clientNodee,TcpSocketFactory::GetTypeId());      
  
  clientSocketa->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));
    //clientSocketb->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));
    //clientSocketc->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));
    //clientSocketd->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));
    //clientSockete->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));    
  /**建立Application**/
  Ptr<MyApp> clientAppa = CreateObject<MyApp>();
  Ptr<MyApp> clientAppb = CreateObject<MyApp>();
  Ptr<MyApp> clientAppc = CreateObject<MyApp>();
  Ptr<MyApp> clientAppd = CreateObject<MyApp>();
  Ptr<MyApp> clientAppe = CreateObject<MyApp>();

  /**設定連線**/
  Ipv4Address clientAddressa = i0i10.GetAddress (0);
  AddressValue localAddressa (InetSocketAddress (clientAddressa, port));
  Ipv4Address serverAddressa = i5i14.GetAddress (0);
  Address remoteAddressa (InetSocketAddress (serverAddressa, port));
  
  Ipv4Address clientAddressb = i1i10.GetAddress (0);
  AddressValue localAddressb (InetSocketAddress (clientAddressb, port));
 
  Ipv4Address serverAddressb = i6i14.GetAddress (0);
  Address remoteAddressb (InetSocketAddress (serverAddressb, port));

  Ipv4Address clientAddressc = i2i11.GetAddress (0);
  AddressValue localAddressc (InetSocketAddress (clientAddressc, port));
  
  Ipv4Address serverAddressc = i7i14.GetAddress (0);
  Address remoteAddressc (InetSocketAddress (serverAddressc, port));

  Ipv4Address clientAddressd = i3i12.GetAddress (0);
  AddressValue localAddressd (InetSocketAddress (clientAddressd, port));
  Ipv4Address serverAddressd = i8i14.GetAddress (0);
  Address remoteAddressd (InetSocketAddress (serverAddressd, port));

  Ipv4Address clientAddresse = i4i13.GetAddress (0);
  AddressValue localAddresse (InetSocketAddress (clientAddresse, port));
  Ipv4Address serverAddresse = i9i14.GetAddress (0);
  Address remoteAddresse (InetSocketAddress (serverAddresse, port));

  /**綁定socket與application並連接server**/
  /**設定sending date rate and packet size**/
  clientAppa->Setup(clientSocketa, remoteAddressa, packetSize,packetnum, DataRate("2Mbps"));
  clientAppb->Setup(clientSocketb, remoteAddressb, packetSize,packetnum, DataRate("2Mbps"));
  clientAppc->Setup(clientSocketc, remoteAddressc, packetSize,packetnum, DataRate("2Mbps"));
  clientAppd->Setup(clientSocketd, remoteAddressd, packetSize,packetnum, DataRate("2Mbps"));
  clientAppe->Setup(clientSockete, remoteAddresse, packetSize,packetnum, DataRate("2Mbps"));

  /**安裝application到Client**/
  clientNodea->AddApplication(clientAppa);
  clientNodeb->AddApplication(clientAppb);
  clientNodec->AddApplication(clientAppc);
  clientNoded->AddApplication(clientAppd);
  clientNodee->AddApplication(clientAppe);

  /**設定application啟動時間結束數時間**/
  clientAppa->SetStartTime(Seconds(1.0));
  clientAppa->SetStopTime(Seconds(19.0));
  clientAppb->SetStartTime(Seconds(1.0));
  clientAppb->SetStopTime(Seconds(19.0));
  clientAppc->SetStartTime(Seconds(1.0));
  clientAppc->SetStopTime(Seconds(19.0));
  clientAppd->SetStartTime(Seconds(1.0));
  clientAppd->SetStopTime(Seconds(19.0));
  clientAppe->SetStartTime(Seconds(1.0));
  clientAppe->SetStopTime(Seconds(19.0));

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("proj2_red.tr"));
  
  // Flow Monitor
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

/*   
  //queue length
  Ptr<PointToPointNetDevice> net0 = DynamicCast <PointToPointNetDevice> (d0d10.Get (1));
  Ptr <Queue> q0 = net0->GetQueue();
  Ptr<PointToPointNetDevice> net1 = DynamicCast <PointToPointNetDevice> (d11d12.Get (0));
  Ptr <Queue> q1 = net1->GetQueue();
  Ptr<PointToPointNetDevice> net2 = DynamicCast <PointToPointNetDevice> (d12d13.Get (0));
  Ptr <Queue> q2 = net2->GetQueue();
  Ptr<PointToPointNetDevice> net3 = DynamicCast <PointToPointNetDevice> (d13d14.Get (0));
  Ptr <Queue> q3 = net3->GetQueue();
  Ptr<PointToPointNetDevice> net4 = DynamicCast <PointToPointNetDevice> (d13d14.Get (1));
  Ptr <Queue> q4 = net4->GetQueue();
  Simulator::Schedule (Seconds (0.0001), &PrintTxQueue, q0, q1, q2, q3, q4);
*/     
  NS_LOG_INFO ("Run Simulation.");       
  Simulator::Stop (Seconds (20));       
  Simulator::Run ();
/*
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {            
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";        
    std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
    std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
    std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 18.0 / 1024 / 1024  << " Mbps\n";
  }
*/
 Simulator::Destroy ();
 
 return 0;
}


