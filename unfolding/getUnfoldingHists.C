#include "VariableList.h"
bool passDileptonKinematics(double pt1,double pt2,double eta1,double eta2);
double CalcInvMass(double pt1,double eta1,double phi1,double m1,double pt2,double eta2,double phi2,double m2);
bool GenToRecoMatch(int genIndex,int &recoIndex);
void counter(Long64_t i, Long64_t N,TString printName);
void getDistributions(SampleType sampleType,LepType lepType);


void getUnfoldingHists()
{
 TH1::SetDefaultSumw2();
 TTimeStamp ts_start;
 cout << "[Start Time(local time): " << ts_start.AsString("l") << "]" << endl;
 TStopwatch totaltime;
 totaltime.Start();
 gStyle->SetOptStat(0);

 getDistributions(DATA,ELE);

 totaltime.Stop();
 Double_t TotalCPURunTime = totaltime.CpuTime();
 Double_t TotalRunTime = totaltime.RealTime();
 TTimeStamp ts_end;
 cout << endl;
 cout << "**************************************************************************" << endl;
 cout << "Total CPU RunTime: " << TotalCPURunTime/60 << " minutes" << endl;
 cout << "Total Real RunTime: " << TotalRunTime/60 << " minutes" << endl;
 cout << "[End Time(local time): " << ts_end.AsString("l") << "]" << endl;   
 cout << "**************************************************************************" << endl;
 cout << endl;

}
void getDistributions(SampleType sampleType,LepType lepType)
{

 std::vector<TString> dirNames;
 TString counterName;
 if(sampleType==LL){
  dirNames = dirNamesLL;
  xSec = xSecLL;
  counterName = "Getting LL events";
 }
 else if(sampleType==EW){
  dirNames = dirNamesEW;
  xSec = xSecEW;
  counterName = "Getting EW events";
 }
 else if(sampleType==TT){
  dirNames = dirNamesTT;
  xSec = xSecTT;
  counterName = "Getting TT events";
 }
 else if(sampleType==FAKES){
  dirNames = dirNamesFakes;
  xSec = xSecFakes;
  counterName = "Getting FAKE events";
 }
 else if(sampleType==DATA){
  dirNames = dirNamesData;
  xSec = xSecData;
  counterName = "Getting DATA events";
 }
 int dirSize = dirNames.size();
 if(sampleType==LL){
  if(lepType==ELE){
   for(int i=0;i<dirSize;i++){
    dirNames.at(i)+= "/EE";
   }
  }
  else if(lepType==MUON){
   for(int i=0;i<dirSize;i++){
    dirNames.at(i)+= "/MuMu";
   }
  }
  else if(lepType==TAU){
   for(int i=0;i<dirSize;i++){
    dirNames.at(i)+= "/TauTau";
   }
  }
 }//end if sampletype


 //-----Defining branches-----//
 TBranch*b_Nelectrons;
 TBranch*b_Electron_pT;
 TBranch*b_Electron_eta;
 TBranch*b_Electron_phi;
 TBranch*b_Electron_passMediumID;
 TBranch*b_HLT_ntrig;
 TBranch*b_HLT_trigType;
 TBranch*b_HLT_trigFired;
 TBranch*b_GENEvt_weight;
 TBranch*b_nVertices;
 TBranch*b_nPileUp;
 TBranch*b_GENnPair;
 TBranch*b_GENLepton_eta;
 TBranch*b_GENLepton_phi;
 TBranch*b_GENLepton_pT;
 TBranch*b_GENLepton_ID;
 TBranch*b_GENLepton_isHardProcess;
 TBranch*b_GENLepton_fromHardProcessFinalState;

 cout << "Loading ntuples" << endl;
 cout << "Begin loading trees:" << endl;

 bool isMC = true;
 if(sampleType==DATA){
  isMC = false;
 }

 const int numChains = dirNames.size();
 TString files;
 Long64_t subDirectorySize;
 Long64_t totalentries = -1;

 TString fileNames;
 if(isMC) fileNames = "/*.root";
 else fileNames = "/skims_0002/*.root";
 vector <TString> *subFiles[numChains];
 for(int iChain=0;iChain<numChains;iChain++){
  subFiles[iChain] = new vector<TString>;
  if(sampleType==LL && iChain==M10to50){
   subFiles[iChain]->push_back(dirNames.at(iChain)+"/ext1v1");
   subFiles[iChain]->push_back(dirNames.at(iChain)+"/v1");
   subFiles[iChain]->push_back(dirNames.at(iChain)+"/v2");
  }
  else if(sampleType==LL && iChain==M50to100){
   subFiles[iChain]->push_back(dirNames.at(iChain)+"/base");
  }
  else if(sampleType==EW && iChain==W_PLUS_JETS) {
   subFiles[iChain]->push_back(dirNames.at(iChain));
   subFiles[iChain]->push_back(dirNames.at(iChain)+"_ext");
   subFiles[iChain]->push_back(dirNames.at(iChain)+"_ext2v5");
  }
  else if(sampleType==DATA && iChain==RUN_H) {
   subFiles[iChain]->push_back(dirNames.at(iChain)+"ver2");
   subFiles[iChain]->push_back(dirNames.at(iChain)+"ver3");
  }
  else if(sampleType==TT && iChain==TT0to700) {
   subFiles[iChain]->push_back(dirNames.at(iChain));
   subFiles[iChain]->push_back(dirNames.at(iChain)+"Backup");
  }
  else subFiles[iChain]->push_back(dirNames.at(iChain));
 }//end loop over iChains 

 totalentries = 0;
 TChain*chains[numChains];
 for(int iChain=0;iChain<numChains;iChain++){
  chains[iChain] = new TChain(treeName);
  subDirectorySize = subFiles[iChain]->size();
  for(int k=0;k<subDirectorySize;k++){
   files=subFiles[iChain]->at(k);
   files+=fileNames;
   chains[iChain]->Add(files);
   cout << files << endl;
   cout << chains[iChain]->GetEntries() << " events loaded" << endl;
   if(chains[iChain]->GetEntries()==0){
    cout << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "ERROR: Broken files or files not found in: " << endl;
    cout << files << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << endl;
    return 0;
   }
  }//end loop over files
  totalentries=totalentries+chains[iChain]->GetEntries();


  //-----Setting addresses for branches-----//
  chains[iChain]->SetBranchAddress("Nelectrons", &Nelectrons, &b_Nelectrons);
  chains[iChain]->SetBranchAddress("nVertices", &nVertices, &b_nVertices);
  chains[iChain]->SetBranchAddress("nPileUp", &nPileUp, &b_nPileUp);
  chains[iChain]->SetBranchAddress("Electron_pT", &Electron_pT, &b_Electron_pT);
  chains[iChain]->SetBranchAddress("Electron_eta",&Electron_eta, &b_Electron_eta);
  chains[iChain]->SetBranchAddress("Electron_phi",&Electron_phi, &b_Electron_phi);
  chains[iChain]->SetBranchAddress("Electron_passMediumID",&Electron_passMediumID,
    &b_Electron_passMediumID);
  chains[iChain]->SetBranchAddress("HLT_ntrig",&HLT_ntrig,&b_HLT_ntrig);
  chains[iChain]->SetBranchAddress("HLT_trigType",&HLT_trigType,&b_HLT_trigType);
  chains[iChain]->SetBranchAddress("HLT_trigFired",&HLT_trigFired,&b_HLT_trigFired);
  chains[iChain]->SetBranchAddress("HLT_trigName",&pHLT_trigName);   
  if(isMC){
   chains[iChain]->SetBranchAddress("GENEvt_weight",&GENEvt_weight,&b_GENEvt_weight);
   chains[iChain]->SetBranchAddress("GENnPair", &GENnPair, &b_GENnPair);
   chains[iChain]->SetBranchAddress("GENLepton_eta", &GENLepton_eta, &b_GENLepton_eta);
   chains[iChain]->SetBranchAddress("GENLepton_phi",&GENLepton_phi, &b_GENLepton_phi);
   chains[iChain]->SetBranchAddress("GENLepton_pT",&GENLepton_pT, &b_GENLepton_pT);
   chains[iChain]->SetBranchAddress("GENLepton_ID",&GENLepton_ID, &b_GENLepton_ID);
   chains[iChain]->SetBranchAddress("GENLepton_isHardProcess",&GENLepton_isHardProcess,
    &b_GENLepton_isHardProcess);
   chains[iChain]->SetBranchAddress
    ("GENLepton_fromHardProcessFinalState",&GENLepton_fromHardProcessFinalState,
    &b_GENLepton_fromHardProcessFinalState);
  }
 }//end chain loop  
 cout << "Total Events Loaded: " << totalentries << endl;
 cout << endl;

 //-----Initialize histograms for unfolding distributions-----//
 TH1D*hReco = new TH1D("hReco","",nLogBins2,massbins2);
 TH1D*hTrue = new TH1D("hTrue","",nLogBins,massbins);
 TH2D*hMatrix = new TH2D("hMatrix","",nLogBins,massbins,nLogBins2,massbins2);

 //-----Get histograms for pileup and SF weights-----//
 TFile*pileupRatioFile  = new TFile(pileupRatioName);
 TH1F*hPileupRatio = (TH1F*)pileupRatioFile->Get("hPileupRatio");
 TFile*fileLeg2SF = new TFile(leg2SFName);
 TH2F*hLeg2SF = (TH2F*)fileLeg2SF->Get("EGamma_SF2D");
 TFile*fileMedIDSF = new TFile(medIDSFName);
 TH2F*hMedIDSF = (TH2F*)fileMedIDSF->Get("EGamma_SF2D");
 TFile*fileRecoSF = new TFile(recoSFName);
 TH2F*hRecoSF = (TH2F*)fileRecoSF->Get("EGamma_SF2D");
 
 cout << "Starting Event Loop" << endl;
 //-----Initialize important variables-----//
 double varGenWeight,localEntry,sumGenWeight,sumRawGenWeight,totalWeight,sfWeight,xSecWeight,
  genWeight,pileupWeight;
 Long64_t nentries;
 Long64_t count = 0;
 TString compareHLT = "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*";
 TString trigName;
 int trigNameSize;
 double lumi = dataLuminosity;//luminosity for xsec weighting
 double sfReco1,sfReco2,sfID1,sfID2,sfHLT;//efficiency scale factors
 double eEta1, eEta2, ePt1, ePt2;//eta and pt of the electrons in each event

 //-----Loop over samples-----//
 for(int iChain=0;iChain<numChains;iChain++) {
  cout << endl;
  cout << "Processing chain: " << dirNames[iChain] << endl;
  cout << endl;
  nentries = chains[iChain]->GetEntries();
  cout << "Number of Entries: " << nentries << endl; 

  //-----Find normalized genWeights,sums,variances-----//
  if(isMC){
   sumGenWeight = 0.0;
   sumRawGenWeight = 0.0;
   varGenWeight = 0.0;
   for(Long64_t i=0;i<nentries;i++){
    localEntry = chains[iChain]->LoadTree(i);
    b_GENEvt_weight->GetEntry(localEntry);
    genWeight = GENEvt_weight/fabs(GENEvt_weight);//normalized genweight
    sumGenWeight += genWeight;
    varGenWeight += GENEvt_weight*GENEvt_weight;//variance of genweights
    sumRawGenWeight += GENEvt_weight; 
   }          
  }
  //-----Event loop-----//
  for(Long64_t i=0;i<nentries;i++) {      
   counter(count,totalentries,counterName);
   count = count+1; 
   chains[iChain]->GetEntry(i);
    
   int idxGenEle1 = -1;
   int idxGenEle2 = -1;
   int idxGenEleFS1 = -1;
   int idxGenEleFS2 = -1;
   int nGenDielectrons = 0;
   double invMassHard = 0;
   if(isMC){ 
    //-----Gen loop-----//
    for(int kLep=0;kLep<GENnPair;kLep++){
     for(int lLep=kLep+1;lLep<GENnPair;lLep++){
      if(!(abs(GENLepton_ID[kLep])==11 && abs(GENLepton_ID[lLep])==11))
      continue;
      if(GENLepton_ID[kLep]*GENLepton_ID[lLep]>0) continue;
      if(GENLepton_isHardProcess[kLep]==1 && GENLepton_isHardProcess[lLep]==1){
       idxGenEle1 = kLep;
       idxGenEle2 = lLep;
       nGenDielectrons++;
      }//end if hard process
      if(GENLepton_fromHardProcessFinalState[kLep]==1 && 
       GENLepton_fromHardProcessFinalState[lLep]==1){
       idxGenEleFS1 = kLep;
       idxGenEleFS2 = lLep;
      }//end if FSR
     }//end lLep loop
    }//end kLep loop
   
    //-----Make sure there are only two gen-level electrons-----//
    if(nGenDielectrons!=1){
     cout << "!!!!!!!!!!WARNING!!!!!!!!!!" << endl;
     cout << "Gen level produces too many or too few electron pairs" << endl;
     continue;
    }
   }

   //-----Make sure all events are within acceptance-----//
   bool passAcceptance = true;
   if(!passDileptonKinematics(GENLepton_pT[idxGenEle1],GENLepton_pT[idxGenEle2],
    GENLepton_eta[idxGenEle1],GENLepton_eta[idxGenEle2])) passAcceptance = false;

   //-----Calculate gen-level invariant masses-----//
   if(passAcceptance&&isMC) invMassHard = CalcInvMass(GENLepton_pT[idxGenEle1],
                                                      GENLepton_eta[idxGenEle1],
                                                      GENLepton_phi[idxGenEle1],eMass,
                                                      GENLepton_pT[idxGenEle2],
                                                      GENLepton_eta[idxGenEle2],
                                                      GENLepton_phi[idxGenEle2],eMass);

   //-----HLT criteria-----//
   trigNameSize = pHLT_trigName->size();
   bool passHLT = kFALSE;	  
   for(int iHLT=0;iHLT<trigNameSize;iHLT++) {
    trigName = pHLT_trigName->at(iHLT);	  
    if(trigName.CompareTo(compareHLT)==0) {
     if(HLT_trigFired[iHLT]==1) passHLT = kTRUE;
     else passHLT = kFALSE;		     
     break; 
    }
   }//end loop over triggers 
  
   int numDielectrons = 0;
   int subEle = -1;
   int leadEle = -1;
   double invMass = 0;
   TLorentzVector recoP4;

   //-----Reco Electron loop-----//
   //Find reconstructed electrons within acceptance passing medium ID criteria
   //Determine which is leading and which is subleading
   for(int iEle = 0; iEle < Nelectrons; iEle++) {
   //Does this part need to require opposite charge of dielectrons?
    if(!Electron_passMediumID[iEle]) continue;
    for(int jEle = iEle+1; jEle < Nelectrons; jEle++) {
     if(!Electron_passMediumID[jEle]) continue;
     if(passDileptonKinematics(Electron_pT[iEle],Electron_pT[jEle],Electron_eta[iEle],
      Electron_eta[jEle])){
      numDielectrons++;
      if(Electron_pT[iEle]>Electron_pT[jEle]){
       leadEle = iEle; 
       subEle = jEle;
      }
      else {
       leadEle = jEle; 
       subEle = iEle;
      }	  
     }
    }//end jEle loop
   }//end iEle loop

   //-----Calculate reconstructed invariant mass-----//
   invMass = CalcInvMass(Electron_pT[leadEle],Electron_eta[leadEle],
                         Electron_phi[leadEle],eMass,Electron_pT[subEle],Electron_eta[subEle],
                         Electron_phi[subEle],eMass);

   //-----Gen to reco matching-----//
   int closestTrackLep1, closestTrackLep2;
   closestTrackLep1 = closestTrackLep2 = -1;
   bool genToRecoMatchedLep1 = GenToRecoMatch(idxGenEleFS1,closestTrackLep1);
   bool genToRecoMatchedLep2 = GenToRecoMatch(idxGenEleFS2,closestTrackLep2);


   //-----All cuts-----//
   //place cut events into underflow bins
   if(!(genToRecoMatchedLep1 && genToRecoMatchedLep2)) invMass=0;
   if(!Electron_passMediumID[closestTrackLep1])        invMass=0;
   if(!Electron_passMediumID[closestTrackLep2])        invMass=0;
   if(numDielectrons!=1)                               invMass=0;
   if(leadEle<0||subEle<0)                             invMass=0;
   if(!passHLT)                                        invMass=0;

   //-----Defining eta and pt for SF calculation-----//
   eEta1 = Electron_eta[leadEle];
   eEta2 = Electron_eta[subEle];
   ePt1 = Electron_pT[leadEle];
   ePt2 = Electron_pT[subEle];
 
   //-----Moves pt on the edge of the SF histograms to just inside-----//
   if(ePt1<ptBinLow) ePt1 = ptBinLow;
   if(ePt2<ptBinLow) ePt2 = ptBinLow;
   if(ePt1>ptBinHigh) ePt1 = ptBinHigh;
   if(ePt2>ptBinHigh) ePt2 = ptBinHigh;

   sfWeight = 1.0;
   genWeight = 1.0;
   //-----Determining weighting factors-----//
   pileupWeight = hPileupRatio->GetBinContent(hPileupRatio->FindBin(nPileUp));
   sfReco1=hRecoSF->GetBinContent(hRecoSF->FindBin(eEta1,ePt1));
   sfReco2=hRecoSF->GetBinContent(hRecoSF->FindBin(eEta2,ePt2));
   sfID1=hMedIDSF->GetBinContent(hMedIDSF->FindBin(eEta1,ePt1));
   sfID2=hMedIDSF->GetBinContent(hMedIDSF->FindBin(eEta2,ePt2));
   sfHLT=(hLeg2SF->GetBinContent(hLeg2SF->FindBin(eEta1,ePt1)))*
         (hLeg2SF->GetBinContent(hLeg2SF->FindBin(eEta2,ePt2)));
   if(isMC){ 
    xSecWeight=lumi*(xSec.at(iChain)/1.0);//xSecWeight when used with genWeight 
    genWeight = (GENEvt_weight/fabs(GENEvt_weight))/sumGenWeight;
    sfWeight = sfReco1*sfReco2*sfID1*sfID2*sfHLT;
    totalWeight = genWeight*xSecWeight*pileupWeight;
   }
   else{
    xSecWeight=lumi*(xSec.at(iChain)/chains[iChain]->GetEntries());
    totalWeight = xSecWeight;
   }
   
   //-----Fill histograms-----//
   hReco->Fill(invMass,totalWeight*sfWeight);
   if(isMC){
    hTrue->Fill(invMassHard,totalWeight);
    hMatrix->Fill(invMassHard,invMass,totalWeight*sfWeight);
    hMatrix->Fill(invMassHard,0.0,totalWeight*(1-sfWeight));
   }
   
  }//end event loop
 }//end chain loop

 //-----Save histograms to file-----//
 TString saveName;
 if(isMC) saveName = "data/migrationMatrix.root";
 else saveName = "data/inputData.root";
 TFile *rootFile = new TFile(saveName,"RECREATE");
 rootFile->cd();
 hReco->Write();
 if(isMC){
  hTrue->Write();
  hMatrix->Write();
 }
 rootFile->Write();
 rootFile->Close();
 
}

bool passDileptonKinematics(double pt1,double pt2,double eta1,double eta2)
{
 if(abs(eta1)>etaGapLow && abs(eta1)<etaGapHigh) return false;
 if(abs(eta2)>etaGapLow && abs(eta2)<etaGapHigh) return false;
 if(abs(eta1)>etaHigh||abs(eta2)>etaHigh) return false;
 if(!((pt1>ptLow && pt2>ptHigh)||(pt1>ptHigh && pt2>ptLow))) return false;
 return true;
}

double CalcInvMass(double pt1,double eta1,double phi1,double m1,double pt2,double eta2,double phi2,double m2)
{
 TLorentzVector v1;
 TLorentzVector v2;
 v1.SetPtEtaPhiM(pt1,eta1,phi1,m1);
 v2.SetPtEtaPhiM(pt2,eta2,phi2,m2);
 return (v1+v2).M();
}


bool GenToRecoMatch(int genIndex,int &recoIndex)
{
 double dR,deta,dphi;
 float dRMin = 100000;
 recoIndex=-1;
 for(int iEle=0;iEle<Nelectrons;iEle++){
  deta=Electron_eta[iEle]-GENLepton_eta[genIndex];
  dphi=abs(Electron_phi[iEle]-GENLepton_phi[genIndex]);
  if(dphi>pi) dphi=2*pi-dphi;
  dR=sqrt(deta*deta+dphi*dphi);
  if(dR<dRMin){
   recoIndex=iEle;
   dRMin=dR;
  }
 }//end for loop

 bool matchFound = true;
 if(dRMin>=dRMinCut){
  recoIndex=-1;
  matchFound=false;
 }

 return matchFound;
}

void counter(Long64_t i, Long64_t N,TString printName)
{
 int P = 100*(i)/(N);  
 TTimeStamp eventTimeStamp;
 if(i%(N/100)==0)
  cout << printName << " " << "[Time: " << eventTimeStamp.AsString("s") << "] " << P << "%" << endl;
 return;
}

