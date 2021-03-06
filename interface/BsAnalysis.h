#ifndef __BSANALYSIS_H
#define __BSANALYSIS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

#include "TLorentzVector.h"

const double kmass = 0.493; // GeV
const double phi_polemass = 1.019445; // GeV

// Holds information about a Phi Candidate
struct PhiInfo 
{
  // track pair indices
  int indx1;
  int indx2;

  // difference from nominal mass
  double dmass;

  // position difference between track pair
  double dxy;
  double dz;

  // deltaR between track pair
  double dr;
  
  // sum of track pair vectors
  TLorentzVector v;

  // average of track vertices
  double vertexX;
  double vertexY;
  double vertexZ;  
};

// holds information about a Bs
struct BsInfo 
{
  PhiInfo phi1;
  PhiInfo phi2;
};

// Function objects - comparator 
class LVPtComparator {
public:
  bool operator() (const TLorentzVector& a, const TLorentzVector& b) {
    return a.Pt() > b.Pt();
  }
};
template <class T>
class PtComparator {
public:
  bool operator() (const T& a, const T& b) {
    return a.pt > b.pt;
  }
};
class PhiInfoComparator {
  public:
    bool operator() (const PhiInfo& a, const PhiInfo& b) const {
      return (a.dmass < b.dmass);
    }
};

class TFile;
class TChain;
class TH1;
class TProfile;

class BsAnalysis {
  
  public:
  BsAnalysis(); 
  ~BsAnalysis();
  
  int setInputFile(const std::string& fname);
  void setTreeBranches();
  void bookHistograms();
  void saveHistograms();  
  void printResults(std::ostream& os=std::cout) const; 
  bool beginJob();
  void endJob();
  void eventLoop();
  bool openFiles();
  void closeFiles();
  bool readJob(const std::string& jobFile, int& nFiles);
  void printJob(std::ostream& os=std::cout) const; 
  // ---------------------------------------
  // Get total number of events in the chain
  // --------------------------------------
  int getEntries() const
  {
    return static_cast<int>(chain_->GetEntries());
  }
  void findPhiCandidates(const std::map<std::string, double>& trkCutMap, 
			 const std::map<std::string, double>& phiCutMap, 
			 const std::vector<TTStudy::Track>* trackList, 
			 std::vector<PhiInfo>& phiList, int indx=0);
  bool selectEvent(const std::map<std::string, double>& trkCutMap, 
		   const std::map<std::string, double>& phiCutMap, 
		   const std::map<std::string, double>& bsCutMap, 
		   const std::vector<TTStudy::Track>* trackList, int ishift, int indx=0);
  void readGenParticle();
  void plotGen(const std::vector<TTStudy::Track>* tracksBr);
  void clearEvent();
  void clearLists();
  void clearGenLists();
  void getKaonList(const std::vector<TTStudy::Track>* trackList, const BsInfo& info, std::vector<TLorentzVector>& kaonList);
  void fillKaonInfo(const std::vector<TLorentzVector>& kaonList);
  void fillKaonTrackInfo(const std::vector<TTStudy::Track>* tracksBr, const BsInfo& info);
  void computeIsolation(const std::vector<TTStudy::Track>* tracksBr, const BsInfo& info, double cone=0.3);
  void fillGenInfo();

  void checkPhiKaonBs(const PhiInfo& info_i, const PhiInfo& info_j, const std::vector<TTStudy::Track>* tracksBr);
  void checkMatchingPhi(int ntrk) const;
  void printTrackProperties(const std::vector<TTStudy::Track>* tracksBr) const;
  void checkConsistency(const std::vector<TTStudy::Track>* tracksBr, int ntrk) const;
  bool genFilter(double minPt=2.0) const;
  void printTrk(const std::vector<TTStudy::Track>* tracksBr, size_t i) const;
  void printGenParticle(size_t i) const;
   int doTrkGenMatch(const std::vector<TLorentzVector>& kaonList);
  bool isGenKaonMatched(const TLorentzVector& trk_lv, double& pt_diff);
  void getTV(const std::vector<TTStudy::Track>* tracksBr, unsigned int indx, TVector3& lv) const;
  void getLV(const std::vector<TTStudy::Track>* tracksBr, unsigned int indx, TLorentzVector& lv) const;
  void plotSignalProperties(const std::vector<TTStudy::Track>* tracksBr);
  void plotGenVertex();

  static void calculateDeltaPos(const TTStudy::Track& trki, const TTStudy::Track& trkj, double& dxy, double& dz);
  static void calculateDeltaPos(const PhiInfo& infoi, const PhiInfo& infoj, double& dxy, double& dz);
  static double calculateDeltaR(const TTStudy::Track& trki, const TTStudy::Track& trkj);
  static double calculateDeltaR(const TTStudy::GenParticle& gpi, const TTStudy::GenParticle& gpj);
  static void scaleHistogram(TH1F* th, double fac);
  static TLorentzVector phiLV(const TTStudy::Track& trki, const TTStudy::Track& trkj);
  static double genInvMass(const TTStudy::GenParticle&, const TTStudy::GenParticle&);
  static double getPoissonError(double k, double N);
  static double getBinomailError(double k, double N);

  TChain* chain_;
  TFile* outputFile_;
  std::string dataType_;
  bool isSignal_;
  bool studyGen_;
  bool dumpGenInfo_;
  bool studyOffline_;
  std::string histFile_;
  std::string logFile_;
  int maxEvent_;
  int verbosity_;
  bool applyTrkQuality_;
  int maxEvt_;
  std::vector<std::string> fileList_;

  std::map<std::string, double> trkSelCutMap_;
  std::map<std::string, double> phiSelCutMap_;
  std::map<std::string, double> bsSelCutMap_;

  // Branches
  TTStudy::Event* eventBr_;
  std::vector<TTStudy::SimTrack>* simTracksBr_;  
  std::vector<TTStudy::Track>* tracksBr_;
  std::vector<TTStudy::GenParticle>* genParticleBr_;

  // other lists
  std::vector<TTStudy::GenParticle> genKaonList_;  
  std::vector<std::vector<TLorentzVector> > phiCandList_;
  std::vector<TTStudy::GenParticle> genPhiCandList_;
  std::vector<BsInfo> bsList_;

  long nEntries_;
  bool bookedHistograms_; 
  double scaleFactor_;
  int nEvents_;
  std::ofstream fLog_;   

  TH1F* evcountH_;
  TH1F* centralH_;
  TH1F* fwdH_;
  TH1F* nH_;
  TH1F* ptDiffH_;

  TH1F* ntrkH_;
  TH1F* trkVertexZH_;
  TH1F* trkVertexXYH_;
  TH1F* trkPtH_;
  TH1F* trkChi2H_;

  TH1F* dzTrackPairH_;
  TH1F* dzTrackPair2H_;
  TH1F* dxyTrackPairH_;
  TH1F* dxyTrackPair2H_;

  TH1F* drTrackPairH_;

  TH1F* phiCandPtH_;
  TH1F* phimass0H_;
  TH1F* phimassH_;
  TH1F* nPhiCandH_;

  TH1F* dxyPhiPairH_;
  TH1F* dzPhiPairH_;
  TH1F* drPhiPairH_;

  TH1F* drPhi1TrackPairH_;
  TH1F* drPhi2TrackPairH_;

  TH1F* bsmass0H_;
  TH1F* bsmassH_;

  TH1F* phi1PtH_;
  TH1F* phi2PtH_;
  TH2D* phiPtH_;
  
  TH1F* dxyPhi1TrackPairH_;
  TH1F* dzPhi1TrackPairH_;

  TH1F* dxyPhi2TrackPairH_;
  TH1F* dzPhi2TrackPairH_;
  // end selectEvent

  TH1F* trk1PtH_;
  TH1F* trk2PtH_;
  TH1F* trk3PtH_;
  TH1F* trk4PtH_;

  TH1F* trk1EtaH_;
  TH1F* trk2EtaH_;
  TH1F* trk3EtaH_;
  TH1F* trk4EtaH_;

  TH1F* trk1PhiH_;
  TH1F* trk2PhiH_;
  TH1F* trk3PhiH_;
  TH1F* trk4PhiH_;

  TH1F* trk1Chi2H_;
  TH1F* trk2Chi2H_;
  TH1F* trk3Chi2H_;
  TH1F* trk4Chi2H_;

  TH1F* trk1Chi2RedH_;
  TH1F* trk2Chi2RedH_;
  TH1F* trk3Chi2RedH_;
  TH1F* trk4Chi2RedH_;

  TH1F* trk1nStubH_;
  TH1F* trk2nStubH_;
  TH1F* trk3nStubH_;
  TH1F* trk4nStubH_;

  TH1F* trk1nStubPSH_;
  TH1F* trk2nStubPSH_;
  TH1F* trk3nStubPSH_;
  TH1F* trk4nStubPSH_;
  
  TH1F* drKaonPairH_;
  TH1F* isol1H_;
  TH1F* isol2H_;
  TH1F* isol3H_;
  TH1F* isol4H_;

  TH1F* bsCandListH_;
  TH1F* anglePlanesH_;

  TH1F* genKPt1H_;
  TH1F* genKPt2H_;
  TH1F* genKPt3H_;
  TH1F* genKPt4H_;
  TH1F* genKPtCheckH_;

  TH1F* genKEta1H_;
  TH1F* genKEta2H_;
  TH1F* genKEta3H_;
  TH1F* genKEta4H_;

  TH1F* genKPhi1H_;
  TH1F* genKPhi2H_;
  TH1F* genKPhi3H_;
  TH1F* genKPhi4H_;
  
  TH1F* genPhiMH_;
  TH1F* genPhiPt1H_;
  TH1F* genPhiPt2H_;

  TH1F* genPhiEta1H_;
  TH1F* genPhiEta2H_;
  TH1F* genPhiPhi1H_;
  TH1F* genPhiPhi2H_;

  TH1F* genDrKPairH_;
  TH1F* genDrPhiPairH_;
  TH1F* drPhiGenPhiH_;
  TH1F* genBsPtH_;
  TH1F* genBsEtaH_;
  TH1F* genBsPhiH_;

  TH1F* phiVXYH_;
  TH1F* phiVZH_;
  TH1F* BsVXYH_;
  TH1F* BsVZH_;

  TH1F* mDr_K;
  TH1F* mDpt_K;
  TH1F* mDphi_K;
  TH1F* mDeta_K;
  TH1F* mDr_phi;
  TH1F* mDpt_phi;
  TH1F* mDphi_phi;
  TH1F* mDeta_phi;
  
  TH1F* signalPt1H_;
  TH1F* signalPt2H_;
  TH1F* signalPt3H_;
  TH1F* signalPt4H_;
  TH1F* signalDrH_;
  TH1F* signalPhiMH_;
  TH1F* signalNtrkH_  ;
  TH1F* signalCentralH_ ;
  TH1F* signalFwdH_ ;
  TH1F* signal_VZH_;
  TH1F* signal_VXYH_;
  TH1F* signal_chiH_;
  TH1F* signalDPT_H;
  
  TH1F* signalDXYPVH_;
  TH1F* signalDZPVH_;
  TH2D* signal2DH_;
  TH1F* signalDXYH_;
  TH1F* signalDZH_;
  TH1F* allDXYPVH_;
  TH1F* allDZPVH_;
  TH2D* all2DH_;
  TH1F* allDXYH_;
  TH1F* allDZH_;

  TProfile* drVsMatchedTrkH_;
};
#endif
