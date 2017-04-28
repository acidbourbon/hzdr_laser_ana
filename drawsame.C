



void drawsame(void){
    
    Int_t color_contrast_array[200]={kBlack,kRed,kBlue,kGreen+1,kOrange,kMagenta,kYellow+1,kViolet,kOrange+7,kRed+1,kRed+2,kRed-6,kOrange-3,kMagenta+1,kMagenta+2,kMagenta-9,kBlue+2,kBlue-7,kGreen+2,kGreen-6,kYellow+2,kGray,kGray+1, kGray+2};
  
TFile * t1 = (TFile*)gROOT->GetListOfFiles()->First(); 
cout << gROOT->GetListOfFiles()->GetEntries() << endl;
for(int f=0 ; f < gROOT->GetListOfFiles()->GetEntries() ; f++)
  {
    cout <<  " adding " << t1->GetName() << " with " ;
    t1->cd();
    TH1F* CentA_t1 = ((TH1F*) t1->Get("Histograms/Sec_1483/Sec_1483_Ch05_t1"));  
    CentA_t1->SetLineColor(color_contrast_array[f]);
    CentA_t1->Rebin(4);
    if(f > 0) {
        CentA_t1->DrawCopy("same");
    } else {
        CentA_t1->DrawCopy();
    }
        t1 = (TFile*)gROOT->GetListOfFiles()->After(t1); 
  }






    
    
    
    
//   TFile *f = new TFile(fname);
// 
//   cout << "contents: "<< endl; 
//   f->ls();
// 
//   double isect = get_toa_offset(CentA_t1);
//   cout << "toa_intersect : " << isect <<endl;
      
    
    
}