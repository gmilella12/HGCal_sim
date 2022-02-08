// system include files
#include <memory>
#include <algorithm>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"

#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include <memory>
#include <vector>
#include <cmath>
#include "TLorentzVector.h"


//
// class declaration
//

using namespace std;
using namespace edm;

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class HGCal_analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit HGCal_analyzer(const edm::ParameterSet&);
  ~HGCal_analyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  edm::Service<TFileService> fs;

  std::string detector;
  bool rawRecHits;
  hgcal::RecHitTools recHitTools;
  
  // ----------member data ---------------------------
  //TOKEN, TREE DEFINITION 

  edm::EDGetTokenT<HGCRecHitCollection> _recHitsBH;
  edm::ESHandle<HGCalGeometry> HGcal_geometry;
  
  std::shared_ptr<TTree> HGCal_tree;

  //VARIABLES TO BE STORED IN THE HGCALTREE                                                                                                                                                                
  std::vector<int> rechit_layer;
  std::vector<int> rechit_ring;
  std::vector<int> rechit_zside;
  std::vector<int> rechit_scint_type;
  std::vector<int> rechit_sipm;

  //HISTOGRAMS

  TH1F* h_Occupancy;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HGCal_analyzer::HGCal_analyzer(const edm::ParameterSet& iConfig) : 
  detector(iConfig.getParameter<std::string >("detector")),
  rawRecHits(iConfig.getParameter<bool>("rawRecHits"))
{

  usesResource("TFileService");
  
  //CONFIGURATION OF THE TOKEN WITH THE LABEL
  
  _recHitsBH = consumes<HGCRecHitCollection>(edm::InputTag("HGCalRecHit","HGCHEBRecHits"));

  
}

HGCal_analyzer::~HGCal_analyzer() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void HGCal_analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  //recHitTools.getEventSetup(iSetup);

  // RECHIT HANDLE
  Handle<HGCRecHitCollection> recHitHandleBH;
  iEvent.getByToken(_recHitsBH,recHitHandleBH);
  const HGCRecHitCollection& rechitsBH = *recHitHandleBH;

  // GEOMETRY LOADING
  iSetup.get<IdealGeometryRecord>().get(HGCal_geometry);
   
  if (recHitHandleBH.isValid() )
    {
      std::cout << rechitsBH.size() << std::endl;
      for (HGCRecHitCollection::const_iterator rechit = rechitsBH.begin(); rechit < rechitsBH.end(); ++rechit)
	{
	  const HGCScintillatorDetId hitdetid = rechit->detid();
	  	  
	  //unsigned int layer = detid.layer();//recHitTools.getLayerWithOffset(detid);
	  //h_Occupancy->Fill(layer);
	  
	  rechit_layer.push_back(hitdetid.layer());
	  rechit_ring.push_back(hitdetid.ring());
	  rechit_zside.push_back(hitdetid.zside());
	  rechit_scint_type.push_back(hitdetid.type());
	  rechit_sipm.push_back(hitdetid.sipm());
	  

	}
    }
  
  HGCal_tree->Fill();
  
}


void HGCal_analyzer::beginJob() {
  
  h_Occupancy = fs->make<TH1F>("h_Occupancy", "", 60, 0., 60.);
  HGCal_tree = std::shared_ptr<TTree>(fs->make<TTree>("HGCalTree","HGCal Tree"));

  rechit_layer.clear();
  rechit_ring.clear();
  rechit_zside.clear();
  rechit_scint_type.clear();
  rechit_sipm.clear();

  //HGCAL TREE BRANCHES
  
  HGCal_tree->Branch("HGCal_rechit_layer", &rechit_layer);
  HGCal_tree->Branch("HGCal_rechit_ring", &rechit_ring);
  HGCal_tree->Branch("HGCal_rechit_zside", &rechit_zside);
  HGCal_tree->Branch("HGCal_rechit_scint_type", &rechit_scint_type);
  HGCal_tree->Branch("HGCal_rechit_sipm", &rechit_sipm);

}

// ------------ method called once each job just after ending the event loop  ------------
void HGCal_analyzer::endJob() {

  HGCal_tree->GetCurrentFile()->Write();

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HGCal_analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HGCal_analyzer);
