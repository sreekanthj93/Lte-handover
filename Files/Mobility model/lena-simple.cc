/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/radio-environment-map-helper.h>
#include <ns3/phy-stats-calculator.h>
//#include "ns3/gtk-config-store.h"

using namespace ns3;
FILE *fr=fopen("sinr","w");
FILE *fp=fopen("datadl","w");
/*void func(Ptr<RadioEnvironmentMapHelper> remHelper){
        //std::cout<<"yes\n";
        remHelper->pause=true;
        if((remHelper->pos_sinr).begin ()->z==1.5){std::cout<<"kn\n"<<(remHelper->pos_sinr).begin ()->sinr<<"\n";}
        if((remHelper->pos_sinr).end ()->z==1.5){std::cout<<"bk\n"<<(remHelper->pos_sinr).end ()->sinr<<"\n";}
        for (remHelper->iter = (remHelper->pos_sinr).begin (); remHelper->iter != (remHelper->pos_sinr).end (); remHelper->iter++)
        {
                fprintf(fr,"Positions: %lf...%lf...%lf\n",(remHelper->iter)->x,(remHelper->iter)->y,(remHelper->iter)->z);
                fprintf(fr,"SINR:%lf\n",(remHelper->iter)->sinr);
                std::cout<<"Positions: "<<(remHelper->iter)->x<<"..."<<(remHelper->iter)->y<<"..."<<(remHelper->iter)->z<<"\n";
                std::cout<<"SINR:"<<(remHelper->iter)->sinr<<"\n";
        }
        remHelper->pause=false;
}*/
void func2(Ptr<LteHelper> ltehelper){
        int i,j;
        ltehelper->as();
        printf("cellid:%d\n",ltehelper->finalid_cellid);
        for(i=0;i<ltehelper->finalid_cellid;i++){
                for(j=0;j<ltehelper->finalid_rnti;j++){
                        fprintf(fp,"cellid:%d..rnti:%d..sinr:%lf..rsrp:%lf..\n",i,j,ltehelper->finalSinr[i][j],ltehelper->finalRsrp[i][j]);
                }   
        }
        Simulator::Schedule(Seconds(1),&func2,ltehelper);
}
int main (int argc, char *argv[])
{	
  CommandLine cmd;
  cmd.Parse (argc, argv);
	
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  //
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // Parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // Uncomment to enable logging
//  lteHelper->EnableLogComponents ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (2);
  ueNodes.Create (2);
  enbNodes.Get(0)->femto=false;
  enbNodes.Get(0)->femto=false;

  Ptr<BuildingsMobilityModel> mm1;
  int i;
  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::BuildingsMobilityModel");
  mobility.Install (enbNodes);
  mobility.SetMobilityModel ("ns3::BuildingsMobilityModel");
  mobility.Install (ueNodes);
  for(i=0;i<(int)enbNodes.GetN();i++){
        mm1 = enbNodes.Get (i)->GetObject<BuildingsMobilityModel> ();
        if(i==0)
                mm1->m_vel=Vector(0,0,0);
        if(i==1)
                mm1->m_vel=Vector(0,0,0);
        mm1->constraint=false;
  } 
  Vector pos;
  //NS_LOG_LOGIC ("installing mobility for HomeEnbs"<<mm1);
  for(i=0;i<(int)ueNodes.GetN();i++){
        mm1 = ueNodes.Get (i)->GetObject<BuildingsMobilityModel> ();
        mm1->m_vel=Vector(-10,11,10);
        pos=Vector(20,10,30);
        if(i==0)
                mm1->SetPosition (pos);
        pos=Vector(10,-30,20);
        if(i==1)
                mm1->SetPosition (pos);
        mm1->constraint=false;
  }

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  // Default scheduler is PF, uncomment to use RR
  //lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  
  enbDevs = lteHelper->InstallEnbDevice (enbNodes.Get(0));
  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");
  enbDevs.Add(lteHelper->InstallEnbDevice (enbNodes.Get(1)));
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  lteHelper->Attach (ueDevs.Get(0), enbDevs.Get (0));
  lteHelper->Attach (ueDevs.Get(1), enbDevs.Get (1));

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  lteHelper->EnableTraces ();
  
  Simulator::Stop (Seconds (4)); 
  //Ptr <ChannelList> list= GetObject<ChannelList>();
  //int ws=list->GetNChannels();
  //fprintf(fr,"no of channels:%d\n",ws);
  /*Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper> ();
  
  remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/0"));
  remHelper->SetAttribute ("OutputFile", StringValue ("rem.out"));
  remHelper->SetAttribute ("XMin", DoubleValue (-2000.5));
  remHelper->SetAttribute ("XMax", DoubleValue (+2000.5));
  remHelper->SetAttribute ("YMin", DoubleValue (-2000.5));
  remHelper->SetAttribute ("YMax", DoubleValue (+2000.5));
  remHelper->SetAttribute ("Z", DoubleValue (1.5));
  remHelper->pause=false;
  remHelper->Install ();
  */
  //Simulator::Schedule(Seconds(0.00366),&func,remHelper);
  Simulator::Schedule(Seconds(0.01),&func2,lteHelper);
  Simulator::Run ();

  // GtkConfigStore config;
  // config.ConfigureAttributes ();

  Simulator::Destroy ();
  return 0;
}
