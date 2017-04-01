BEGIN{
	#n10
	n10n0R=0;
	n10n1R=0;
	n10n11R=0;
	n10D=0;

	#n11
	n11n10R=0;
	n11n2R=0;
	n11n12R=0;
	n11D=0;

	#n12
	n12n11R=0;
	n12n13R=0;
	n12n3R=0;
	n12D=0;

	#n13
	n13n12R=0;
	n13n14R=0;
	n13n4R=0;
	n13D=0;

	#n14
	n14n13R=0;
	n14n5R=0;
	n14n6R=0;
	n14n7R=0;
	n14n8R=0;
	n14n9R=0;
	n14n13D=0;
	n14D=0;
}
{
	RTx = $1;
	path = $3;

	if(RTx == "r"){
		#n10
		if(path =="/NodeList/10/DeviceList/1/$ns3::PointToPointNetDevice/MacRx"){n10n11R++;}
		else if(path =="/NodeList/10/DeviceList/2/$ns3::PointToPointNetDevice/MacRx"){n10n1R++;}
		else if(path =="/NodeList/10/DeviceList/3/$ns3::PointToPointNetDevice/MacRx"){n10n0R++;}
		

		#n11
		else if(path =="/NodeList/11/DeviceList/1/$ns3::PointToPointNetDevice/MacRx"){n11n2R++;}
		else if(path =="/NodeList/11/DeviceList/2/$ns3::PointToPointNetDevice/MacRx"){n11n10R++;}
		else if(path =="/NodeList/11/DeviceList/3/$ns3::PointToPointNetDevice/MacRx"){n11n12R++;}

		#n12
		else if(path =="/NodeList/12/DeviceList/1/$ns3::PointToPointNetDevice/MacRx"){n12n3R++;}
		else if(path =="/NodeList/12/DeviceList/2/$ns3::PointToPointNetDevice/MacRx"){n12n11R++;}
		else if(path =="/NodeList/12/DeviceList/3/$ns3::PointToPointNetDevice/MacRx"){n12n13R++;}

		#n13
		else if(path =="/NodeList/13/DeviceList/1/$ns3::PointToPointNetDevice/MacRx"){n13n4R++;}
		else if(path =="/NodeList/13/DeviceList/2/$ns3::PointToPointNetDevice/MacRx"){n13n12R++;}
		else if(path =="/NodeList/13/DeviceList/3/$ns3::PointToPointNetDevice/MacRx"){n13n14R++;}

		#n14
		else if(path =="/NodeList/14/DeviceList/1/$ns3::PointToPointNetDevice/MacRx"){n14n5R++;}
		else if(path =="/NodeList/14/DeviceList/2/$ns3::PointToPointNetDevice/MacRx"){n14n6R++;}
		else if(path =="/NodeList/14/DeviceList/3/$ns3::PointToPointNetDevice/MacRx"){n14n7R++;}
		else if(path =="/NodeList/14/DeviceList/4/$ns3::PointToPointNetDevice/MacRx"){n14n8R++;}
		else if(path =="/NodeList/14/DeviceList/5/$ns3::PointToPointNetDevice/MacRx"){n14n9R++;}
		else if(path =="/NodeList/14/DeviceList/6/$ns3::PointToPointNetDevice/MacRx"){n14n13R++;}
	}

	else if(RTx == "d"){
		#n10
		if(path =="/NodeList/10/DeviceList/3/$ns3::PointToPointNetDevice/TxQueue/Drop"){n10D++;}
		
		#n11
		else if(path =="/NodeList/11/DeviceList/3/$ns3::PointToPointNetDevice/TxQueue/Drop"){n11D++;}

		#n12
		else if(path =="/NodeList/12/DeviceList/3/$ns3::PointToPointNetDevice/TxQueue/Drop"){n12D++;}

		#n13
		else if(path =="/NodeList/13/DeviceList/3/$ns3::PointToPointNetDevice/TxQueue/Drop"){n13D++;}

		#n14
		else if(path =="/NodeList/14/DeviceList/3/$ns3::PointToPointNetDevice/TxQueue/Drop"){n14D++;}
	}
}
END{
	printf("router1 received packets(A,B,router2): %d,%d,%d\n",n10n0R,n10n1R,n10n11R);
	printf("router1 dropped packets: %d\n",n10D);
	printf("router1 transmit packets total: %d\n",n10D+n10n0R+n10n1R+n10n11R);
	printf("router1 packet loss rate: %f%%\n",n10D/(n10D+n10n0R+n10n1R+n10n11R)*100);
	printf("\n");	
	
	printf("router2 received packets(router1,C,router3): %d,%d,%d\n",n11n10R,n11n2R,n11n12R);
	printf("router2 dropped packets: %d\n",n11D);
	printf("router2 transmit packets total: %d\n",n11D+n11n10R+n11n2R+n11n12R);
	printf("router2 packet loss rate: %f%%\n",n11D/(n11D+n11n10R+n11n2R+n11n12R)*100);	
	printf("\n");

	printf("router3 received packets(router2,D,router4): %d,%d,%d\n",n12n11R,n12n3R,n12n13R);
	printf("router3 dropped packets: %d\n",n12D);
	printf("router3 transmit packets total: %d\n",n12D+n12n11R+n12n3R+n12n13R);
	printf("router3 packet loss rate: %f%%\n",n12D/(n12D+n12n11R+n12n3R+n12n13R)*100);
	printf("\n");

	printf("router4 received packets(router3,E,router5): %d,%d,%d\n",n13n12R,n13n4R,n13n14R);
	printf("router4 dropped packets: %d\n",n13D);
	printf("router4 transmit packets total: %d\n",n13D+n13n12R+n13n4R+n13n14R);
	printf("router4 packet loss rate: %f%%\n",n13D/(n13D+n13n12R+n13n4R+n13n14R)*100);
	printf("\n");
	printf("router5 received packets(router4,A,B,C,D,E): %d,%d,%d,%d,%d,%d\n",n14n13R,n14n5R,n14n6R,n14n7R,n14n8R,n14n9R);
	printf("router5 dropped packets: %d\n",n14D);
	printf("router5 transmit packets total: %d\n",n14D+n14n13R+n14n5R+n14n6R+n14n7R+n14n8R+n14n9R);
	printf("router5 packet loss rate: %f%%\n",n14D/(n14D+n14n13R+n14n5R+n14n6R+n14n7R+n14n8R+n14n9R)*100);
	printf("\n");	
}
