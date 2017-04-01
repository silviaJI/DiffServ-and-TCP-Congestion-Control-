# DiffServ-and-TCP-Congestion-Control-
Use  NS-3 Simulator  to setup five TCP flows (A to A, B to B, etc.) in the above parking-lot configuration. Each physical link is assumed to have a bandwidth of 10 Mbps, and each link has propagation delay 100 us.
// ======== R1 ------  R2 -----   R3 ------  R4  ------------------   R5
//  |    |                              
//  A    B             |          |          |              =======================
//                     C          D          E              |    |    |    |    |
//                                                          A    B    C    D    E       
Assume A, B, C, D,  and E generate Poisson traffic, respectively. Each TCP flow has
       (1) Mean packet generation rate = 2 Mbps. 
       (2) Max IP packet length = 1400 bytes.
       (3) Queuing discipline in routers: FIFO with Drop Tail. 
       (4) Buffer size = adjustable up to 5 packets.   
ECN: Once congestion occurs in a router, the ECN field in IP header is set. The ECE (ECN Echo) bit in TCP header is set by the receiver on every ACK packet until the CWR flag is received.
