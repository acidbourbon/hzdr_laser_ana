


Float_t get_toa_offset(TH1F* toa0) {
    
  TH1* cum_toa0 = toa0->GetCumulative();
  Int_t cum_toa0_sum = cum_toa0->GetBinContent(cum_toa0->GetEntries());
//   cout << "toa0 sum " << cum_toa0_sum <<endl;
  
  Float_t cum_toa0_up   = ((Float_t) cum_toa0_sum) *0.6;
  Float_t cum_toa0_down = ((Float_t) cum_toa0_sum) *0.1;
  
  Int_t cum_toa0_left_bin   = cum_toa0->FindFirstBinAbove(cum_toa0_down,2);
  Int_t cum_toa0_right_bin  = cum_toa0->FindFirstBinAbove(cum_toa0_up,2);
  
//   for (Int_t i = 1; i<= cum_toa0->GetEntries(); i++){
//     Float_t cur_val = cum_toa0->GetBinContent(i);
//     if (cur_val < cum_toa0_down) {
//       cum_toa0_left_bin = i;
//     }
//     if (cur_val > cum_toa0_up) {
//       cum_toa0_right_bin = i;
//       break;
//     }
//   }
  TAxis *xaxis = cum_toa0->GetXaxis();
  Float_t cum_toa0_left  = xaxis->GetBinCenter(cum_toa0_left_bin);
  Float_t cum_toa0_right = xaxis->GetBinCenter(cum_toa0_right_bin);
  
//   cout << "toa0 entries  " << cum_toa0->GetEntries() <<endl;
//   cout << "toa0 up   " << cum_toa0_up <<endl;
//   cout << "toa0 down " << cum_toa0_down <<endl;
//   cout << "toa0 left_bin  " << cum_toa0_left_bin <<endl;
//   cout << "toa0 right_bin " << cum_toa0_right_bin <<endl;
//   cout << "toa0 left  " << cum_toa0_left <<endl;
//   cout << "toa0 right " << cum_toa0_right <<endl;
  
  TF1 *fa = new TF1("fa","[0] + [1]*x",0,5000);

  // This creates a function of variable x with 2 parameters. The parameters must be initialized via:
  fa->SetParameter(0,0);
  fa->SetParameter(1,1);
  
  cum_toa0->Fit(fa,"","", cum_toa0_left,cum_toa0_right);
  
  Float_t x_intersect = -fa->GetParameter(0)/fa->GetParameter(1);
  
//   cout << "x intersect " << x_intersect << endl;
  
//   draw_and_save(cum_toa0,"cum_toa0","./","");
  
  return x_intersect;

  
}




void intersect(void){
    
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