#include <ns3/object.h>
#include <ns3/epc-s1ap-sap.h>
#include <ns3/epc-s11-sap.h>

#include <map>
#include <list>

namespace ns3 {

class Node;
class NetDevice;

class FemtoGW : public object
{

public:
  
  /** 
   * Constructor
   */
  FemtoGW ();

  /** 
   * Destructor
   */  
  virtual ~FemtoGW ();

  /** 
   * Add a new ENB to the MME. 
   * \param imsi the unique identifier of the UE
   * \param enbS1apSap the ENB side of the S1-AP SAP 
   */
  void AddEnb (uint16_t egci, Ipv4Address enbS1UAddr, EpcS1apSapEnb* enbS1apSap); 
  
  /** 
   * Add a new UE to the HEnb. This is the equivalent of storing the UE
   * credentials before the UE is ever turned on. 
   * 
   * \param imsi the unique identifier of the UE
   */
  void AddUe (uint64_t imsi);  
void 
FemtoGW::DoInitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t gci);
void 
FemtoGW::DoPathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t gci, std::list<EpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList);
void 
FemtoGW::DoInitialContextSetupRequest (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<EpcS1apSapEnb::ErabToBeSetupItem> erabToBeSetupList);
void 
FemtoGW::DoPathSwitchRequestAcknowledge (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t gci, std::list<EpcS1apSapEnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList);
	
private:
	
  /**
   * Hold info on a UE
   * 
   */
  struct UeInfo : public SimpleRefCount<UeInfo>
  {
    uint64_t mmeUeS1Id;
    uint16_t enbUeS1Id;
    uint64_t imsi;
    uint16_t cellId;
    std::list<BearerInfo> bearersToBeActivated;
    uint16_t bearerCounter;
  };

  /**
   * UeInfo stored by IMSI
   * 
   */  
  std::map<uint64_t, Ptr<UeInfo> > m_ueInfoMap;
  /**
   * Hold info on a ENB
   * 
   */
  struct EnbInfo : public SimpleRefCount<EnbInfo>
  {
    uint16_t gci;
    Ipv4Address s1uAddr;
    EpcS1apSapEnb* s1apSapEnb;
  };

  /**
   * EnbInfo stored by EGCI
   * 
   */
  std::map<uint16_t, Ptr<EnbInfo> > m_enbInfoMap;

  /**
   * MME side of the S1-AP SAP
   * 
   */
  EpcS1apSapMme* m_s1apSapMme;

