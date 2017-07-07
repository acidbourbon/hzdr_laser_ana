// this is the compare.C for the 



Bool_t file_exists(TString fname){
  
  fstream src_file(fname.Data());
  return src_file.good();
}

std::vector<TString> file_to_str_array(TString fname) {
  
  ifstream in;
  in.open(fname);
  
  std::vector<TString> return_obj;
  
  string line;
  
  while (getline(in, line)) {
    return_obj.push_back(line);
  }
  
  in.close();
  return return_obj;
}

TString from_env(TString env_var,TString default_val){
  if(gSystem->Getenv(env_var)){
    return gSystem->Getenv(env_var);
  } 
  return default_val;
}



void draw_and_save(TObject *hist,TString name,TString outdir,TString draw_options) {
  
//   if(DRAW_PNG) {
    TCanvas *c = new TCanvas("c_"+name,"c_"+name,200,10,1024,786);
    c->cd();
    hist->Draw(draw_options);
    c->Print(outdir+name+".png");
//   }
//   c->Print(outdir+name+".pdf");
    c->Print(outdir+name+".svg");
}


void get_gauss_params(TH1F* histogram, Float_t* target_mu, Float_t* target_sigma) {
  histogram->Fit("gaus");
  TF1 *fit = histogram->GetFunction("gaus");
  Float_t chi2 = fit->GetChisquare();
  Float_t p1 = fit->GetParameter(1);
  Float_t e1 = fit->GetParError(1);
  Float_t p2 = fit->GetParameter(2);
  Float_t e2 = fit->GetParError(2);
//   cout << "Gauss fit sigma: " << p2 << " +- " << e2 << "ns" << endl;
  *target_mu = p1;
  *target_sigma = p2;
}


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



void compare(void) {
    Int_t color_contrast_array[200]={kBlack,kRed,kBlue,kGreen+1,kOrange,kMagenta,kYellow+1,kViolet,kOrange+7,kRed+1,kRed+2,kRed-6,kOrange-3,kMagenta+1,kMagenta+2,kMagenta-9,kBlue+2,kBlue-7,kGreen+2,kGreen-6,kYellow+2,kGray,kGray+1, kGray+2};
  

  
//      _       __ _                                   
//     | |     / _(_)                                  
//   __| | ___| |_ _ _ __   ___                        
//  / _` |/ _ \  _| | '_ \ / _ \                       
// | (_| |  __/ | | | | | |  __/                       
//  \__,_|\___|_| |_|_| |_|\___|                       
//                                                     
//                                                     
//  _     _     _                                      
// | |   (_)   | |                                     
// | |__  _ ___| |_ ___   __ _ _ __ __ _ _ __ ___  ___ 
// | '_ \| / __| __/ _ \ / _` | '__/ _` | '_ ` _ \/ __|
// | | | | \__ \ || (_) | (_| | | | (_| | | | | | \__ \
// |_| |_|_|___/\__\___/ \__, |_|  \__,_|_| |_| |_|___/
//                        __/ |                        
//                       |___/                         
// 

//   Int_t charge_bins = 100;
//   Float_t charge_start = 0;
//   Float_t charge_stop  = 5e-12;
//   Int_t thresh_bins = 128;
//   Float_t thresh_start = 0;
//   Float_t thresh_stop  = 127;
    
  TString TDC=from_env("TDC","1482");
  TString chan=from_env("chan","05");
    
  TString outdir=from_env("outdir","./");
  TString scan_x=from_env("scan_x","false");
  TString scan_z=from_env("scan_z","false");
  TString scan_thr=from_env("scan_thr","false");
  
//   TGraph2DErrors *tot0 = new TGraph2DErrors();
//   tot0->SetTitle("Time Over Threshold Ch 0");
//   tot0->SetName("tot0");
//   tot0->GetXaxis()->SetTitle("injected charge (C)");
//   tot0->GetYaxis()->SetTitle("threshold");
//   tot0->GetZaxis()->SetTitle("ToT (ns)");
//   
//   TGraph2D *tot_rms0 = new TGraph2D();
//   tot_rms0->SetTitle("Time Over Threshold RMS Ch 0");
//   tot_rms0->SetName("tot_rms0");
//   tot_rms0->GetXaxis()->SetTitle("injected charge (C)");
//   tot_rms0->GetYaxis()->SetTitle("threshold");
//   tot_rms0->GetZaxis()->SetTitle("ToT (ns)");
//   
//   TGraph2D *toa_rms1 = new TGraph2D();
//   toa_rms1->SetTitle("Time of arrival RMS Ch 1 vs Ch 0");
//   toa_rms1->SetName("toa_rms1");
  
  
  TGraph *tg_ChX_t1 = new TGraph();
  tg_ChX_t1->SetTitle("t1 Ch"+chan);
  tg_ChX_t1->SetName("tg_Ch"+chan+"_t1");
  tg_ChX_t1->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1->GetYaxis()->SetTitle("t1 (ns)");
  //tg_ChX_t1->GetZaxis()->SetTitle("ToT (ns)");
  
  TGraphErrors *tg_ChX_t1_means = new TGraphErrors();
  tg_ChX_t1_means->SetTitle("t1 Ch"+chan+" means");
  tg_ChX_t1_means->SetName("tg_Ch"+chan+"_t1_means");
  tg_ChX_t1_means->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1_means->GetYaxis()->SetTitle("t1 (ns)");
  
  TGraphErrors *tg_ChX_t1_gauss = new TGraphErrors();
  tg_ChX_t1_gauss->SetTitle("t1 Ch"+chan+" gauss");
  tg_ChX_t1_gauss->SetName("tg_Ch"+chan+"_t1_gauss");
  tg_ChX_t1_gauss->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1_gauss->GetYaxis()->SetTitle("t1 (ns)");
  

  TGraphErrors *tg_ChX_tot_means = new TGraphErrors();
  tg_ChX_tot_means->SetTitle("tot Ch"+chan+" means");
  tg_ChX_tot_means->SetName("tg_Ch"+chan+"_tot_means");
  tg_ChX_tot_means->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_tot_means->GetYaxis()->SetTitle("tot (ns)");
  
  TGraph *tg_ChX_t1_std = new TGraph();
  tg_ChX_t1_std->SetTitle("t1 Ch"+chan+" StdDevs");
  tg_ChX_t1_std->SetName("tg_ChX_t1_std");
  tg_ChX_t1_std->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1_std->GetYaxis()->SetTitle("t1 StdDev (ns)");
  
  TGraph *tg_ChX_counts = new TGraph();
  tg_ChX_counts->SetTitle("Ch"+chan+" Counts");
  tg_ChX_counts->SetName("tg_ChX_counts");
  tg_ChX_counts->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_counts->GetYaxis()->SetTitle("counts");
  
  TGraph *tg_intensity = new TGraph();
  tg_intensity->SetTitle("Laser Intensity");
  tg_intensity->SetName("tg_intensity");
  tg_intensity->GetXaxis()->SetTitle("y-pos (um)");
  tg_intensity->GetYaxis()->SetTitle("intensity (nJ)");
  
  if( scan_x == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("x-pos (um)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("x-pos (um)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("x-pos (um)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("x-pos (um)");
    tg_ChX_counts->GetXaxis()->SetTitle("x-pos (um)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("x-pos (um)");
    tg_intensity->GetXaxis()->SetTitle("x-pos (um)");
  }
  if( scan_z == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("z-pos (um)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("z-pos (um)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("z-pos (um)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("z-pos (um)");
    tg_ChX_counts->GetXaxis()->SetTitle("z-pos (um)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("z-pos (um)");
    tg_intensity->GetXaxis()->SetTitle("z-pos (um)");
  }
  if( scan_thr == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_counts->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("threshold (LSB)");
    tg_intensity->GetXaxis()->SetTitle("threshold (LSB)");
  }
  
  
  
  
  TFile *f_out = new TFile(outdir+"/compare.root","RECREATE");
  
  
//   _                                          
//  | |                                         
//  | | ___   ___  _ __     _____   _____ _ __  
//  | |/ _ \ / _ \| '_ \   / _ \ \ / / _ \ '__| 
//  | | (_) | (_) | |_) | | (_) \ V /  __/ |    
//  |_|\___/ \___/| .__/   \___/ \_/ \___|_|    
//                | |                           
//                |_|                           
//                   _      __ _ _              
//                  | |    / _(_) |             
//   _ __ ___   ___ | |_  | |_ _| | ___  ___    
//  | '__/ _ \ / _ \| __| |  _| | |/ _ \/ __|   
//  | | | (_) | (_) | |_  | | | | |  __/\__ \   
//  |_|  \___/ \___/ \__| |_| |_|_|\___||___/   
//                                              
//                                              
  
  std::vector<TString> list = file_to_str_array("root_files.txt");
  std::vector<TString> xlist;
  std::vector<TString> ylist;
  std::vector<TString> zlist;
  std::vector<TString> intensitylist;
  std::vector<TString> thr_list; 
  
//   std::vector<TH1F*> hist_list;
  
  if ( scan_thr == "true" ){
    thr_list = file_to_str_array("thr_list.txt");
  } else {
    xlist = file_to_str_array("xlist.txt");
    ylist = file_to_str_array("ylist.txt");
    zlist = file_to_str_array("zlist.txt");
  }
  
  intensitylist = file_to_str_array("intensity_list.txt");
  
  TLegend* leg = new TLegend(0.1,0.7,0.48,0.9);
//   leg->SetHeader("The Legend Title"); // option "C" allows to center the header
//   leg->AddEntry(h1,"Histogram filled with random numbers","f");
//   leg->AddEntry("f1","Function abs(#frac{sin(x)}{x})","l");
//   leg->AddEntry("gr","Graph with error bars","lep");
  
  
  for (Int_t i = 0; i< list.size(); i++){
    
    
//     TString fname   = ((class TObjString*) list[i].Tokenize(";")->At(0))->GetString();
//     TString comment = ((class TObjString*) list[i].Tokenize(";")->At(1))->GetString();
    TString fname   = list[i];
    TString comment = "blah";
    cout << "fname: " << fname << " comment: "<< comment << endl;
    
    
    cout << "opening file: "<< fname <<endl; 
    if (not(file_exists(fname))){
      cout << fname << " does not exist" << endl;
      continue;
    }
    
    
    TFile *f = new TFile(fname);
    
    f->cd();
    
    TH1F* CentA_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_t1"));  
    TH1F* CentA_tot = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_tot"));  
//     CentA_t1->Rebin(4);
    
    
    Double_t t1 = get_toa_offset(CentA_t1);
    Double_t t1_mean = CentA_t1->GetMean();
    Double_t counts = CentA_t1->GetEntries();
    Double_t t1_std  = CentA_t1->GetStdDev();
    Double_t tot_mean = CentA_tot->GetMean();
    Double_t tot_std  = CentA_tot->GetStdDev();
    
    Float_t t1_gauss_mu =0;
    Float_t t1_gauss_sigma =0;
    
//     get_gauss_params(CentA_t1,&t1_gauss_mu,&t1_gauss_sigma);
    
    
  if ( scan_thr == "true" ){
    cout << "thresh: " << thr_list[i] << endl;
  } else {
    cout << "x pos: " << xlist[i] << endl;
    cout << "y pos: " << ylist[i] << endl;
    cout << "z pos: " << zlist[i] << endl;
  }
//     if( t1 > 100 || t1 < -100) {
//         t1 = -100;
//     }
    
    Double_t graph_x;
    if( scan_thr == "true") {
      graph_x = thr_list[i].Atoi();
    } else if( scan_x == "true") {
      graph_x = xlist[i].Atoi();
    } else if( scan_z == "true") {
      graph_x = zlist[i].Atoi();
    } else {
      graph_x = ylist[i].Atoi();
    }
    
    Double_t intensity = intensitylist[i].Atof();
//     cout << "intensity" << intensitylist[i] << endl;
//     Double_t intensity = i;
    
    if( zlist[i].Atoi() == 4500 || true) { // select only points in the anode plane
        static Int_t point_no = 0;
        tg_ChX_t1->SetPoint(point_no,graph_x,t1);
        tg_ChX_t1_means->SetPoint(point_no,graph_x,t1_mean);
        tg_ChX_t1_means->SetPointError (point_no, 0, t1_std);
        tg_ChX_tot_means->SetPoint(point_no,graph_x,tot_mean);
        tg_ChX_tot_means->SetPointError (point_no, 0, tot_std);
        tg_ChX_t1_std->SetPoint(point_no, graph_x, t1_std);
        
        tg_ChX_counts->SetPoint(point_no,graph_x,counts);
        
        tg_ChX_t1_gauss->SetPoint(point_no,graph_x,t1_gauss_mu);
        tg_ChX_t1_gauss->SetPointError (point_no, 0, t1_gauss_sigma);
        
        tg_intensity->SetPoint(point_no,graph_x,intensity);
        
        TH1F* hist_new = (TH1F*) CentA_t1->Clone();
        TString new_hist_name;
        new_hist_name.Form("%04.1f_t1_hist",graph_x);
        hist_new->SetName(new_hist_name);
        f_out->cd();
        hist_new->Write();
        f->cd();
        
        
        point_no++;
    }
    
    TString a;
    a.Form("%04.1f_t1_hist",graph_x);
    //draw_and_save(CentA_t1,a,outdir,"");
    cout << "t1   : " << t1 << endl;
    
    
    leg->AddEntry(CentA_t1,"entry","l");
    CentA_t1->SetLineColor(color_contrast_array[i]);
    CentA_t1->GetYaxis()->SetRangeUser(0,1000);
    
//     if(i > 0) {
//         CentA_t1->DrawCopy("same");
//     } else {
//         CentA_t1->DrawCopy();
//     }
    
//     leg->Draw();

//     cout << "contents: "<< endl; 
//     f->ls();
    
//     Float_t pulse_charge =  ((TH1F*)f->Get("pulse_charge"))->GetMean();
//     Float_t discr_thr    =  ((TH1F*)f->Get("discr_thr"))->GetMean();
    
//     if (discr_thr < 20) {
//       continue;
//     }
    
    
//     tot0->SetPoint(i,pulse_charge,discr_thr,      ((TH1F*)f->Get("pwd0"))->GetMean()
//     );
//     tot0->SetPointError(i,
//                    0,0,((TH1F*)f->Get("pwd0"))->GetRMS()
//     );
//     tot_rms0->SetPoint(i,pulse_charge,discr_thr,
//                        ((TH1F*)f->Get("pwd0"))->GetRMS() 
//                       );
//     
//     
//     toa_rms1->SetPoint(i,pulse_charge,discr_thr,      ((TH1F*)f->Get("toa1"))->GetRMS()
//     );
    
    
    f->Close();
    cout << endl;

  }
  
//  tg_ChX_t1->SetMarkerSize(13);
//  tg_ChX_t1->SetMarkerColor(1);
//  tg_ChX_t1->SetMarkerStyle(21+i);
  
  tg_ChX_t1->SetLineColor(2);
  tg_ChX_t1->SetLineWidth(4);
  tg_ChX_t1->SetMarkerColor(4);
  tg_ChX_t1->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1,"tg_Ch"+chan+"_t1",outdir,"AP");
//   tg_ChX_t1->Draw("AP");
  
  tg_ChX_t1_means->SetLineColor(2);
  tg_ChX_t1_means->SetLineWidth(4);
  tg_ChX_t1_means->SetMarkerColor(4);
  tg_ChX_t1_means->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1_means,"tg_Ch"+chan+"_t1_means",outdir,"AP");
  
  tg_ChX_t1_gauss->SetLineColor(2);
  tg_ChX_t1_gauss->SetLineWidth(4);
  tg_ChX_t1_gauss->SetMarkerColor(4);
  tg_ChX_t1_gauss->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1_gauss,"tg_Ch"+chan+"_t1_gauss",outdir,"AP");
  
  tg_ChX_tot_means->SetLineColor(2);
  tg_ChX_tot_means->SetLineWidth(4);
  tg_ChX_tot_means->SetMarkerColor(4);
  tg_ChX_tot_means->SetMarkerStyle(21);
  draw_and_save(tg_ChX_tot_means,"tg_Ch"+chan+"_tot_means",outdir,"AP");
  
  tg_ChX_t1_std->SetLineColor(2);
  tg_ChX_t1_std->SetLineWidth(4);
  tg_ChX_t1_std->SetMarkerColor(4);
  tg_ChX_t1_std->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1_std,"tg_Ch"+chan+"_t1_std",outdir,"AP");
  
  tg_ChX_counts->SetLineColor(2);
  tg_ChX_counts->SetLineWidth(4);
  tg_ChX_counts->SetMarkerColor(4);
  tg_ChX_counts->SetMarkerStyle(21);
  draw_and_save(tg_ChX_counts,"tg_Ch"+chan+"_counts",outdir,"AP");
  
  tg_intensity->SetLineColor(2);
  tg_intensity->SetLineWidth(4);
  tg_intensity->SetMarkerColor(4);
  tg_intensity->SetMarkerStyle(21);
  draw_and_save(tg_intensity,"tg_intensity",outdir,"AP");

//                                      _ _           _             
//                          ___        | (_)         | |            
//   ___  __ ___   _____   ( _ )     __| |_ ___ _ __ | | __ _ _   _ 
//  / __|/ _` \ \ / / _ \  / _ \/\  / _` | / __| '_ \| |/ _` | | | |
//  \__ \ (_| |\ V /  __/ | (_>  < | (_| | \__ \ |_) | | (_| | |_| |
//  |___/\__,_| \_/ \___|  \___/\/  \__,_|_|___/ .__/|_|\__,_|\__, |
//                                             | |             __/ |
//                                             |_|            |___/ 
//       _          __  __                                          
//      | |        / _|/ _|                                         
//   ___| |_ _   _| |_| |_                                          
//  / __| __| | | |  _|  _|                                         
//  \__ \ |_| |_| | | | |                                           
//  |___/\__|\__,_|_| |_|                                           
//                                                                  
//                                                                  
  
//   dummy->GetXaxis()->SetRangeUser(0,5);
//   dummy->GetYaxis()->SetRangeUser(0,5);
//   dummy->GetZaxis()->SetRangeUser(0,500);
  
  
//   TCanvas *c01 = new TCanvas("c_1","c_1",200,10,600,400);
//   tot0->SetMarkerStyle(20);
//   (new TH3F("name","Time Over Threshold",10,0,5e-12,10,0,120,10,0,500))->Draw();
// //   dummy->Draw();
// //   tot0->Draw("same,pcol");
//   tot0->Draw("tri1");
//   
//   
//   
//   
//   TCanvas *c02 = new TCanvas("c_2","c_2",200,10,600,400);
// //   tot_rms0->SetMarkerStyle(20);
// //   dummy->GetZaxis()->SetRangeUser(0,100);
// //   dummy->Draw();
// //   (new TH3F("name2","ToT RMS (jitter)",10,0,5e-12,10,0,120,10,0,0.5))->Draw();
// //   tot_rms0->Draw("same,pcol");
//   tot_rms0->Draw("tri1");
//   
//   TCanvas *c03 = new TCanvas("c_3","c_3",200,10,600,400);
// //   toa_rms1->SetMarkerStyle(20);
// //   dummy->GetZaxis()->SetRangeUser(0,100);
// //   dummy->Draw();
// //   (new TH3F("name3","T1 RMS ch0 vs ch1",10,0,5e-12,10,0,120,10,0,2))->Draw();
// //   toa_rms1->Draw("same,pcol");
//   toa_rms1->Draw("tri1");
//   
//   
  
  f_out->cd();
  tg_ChX_t1->Write();
  tg_ChX_t1_means->Write();
  tg_ChX_t1_std->Write();
  tg_ChX_tot_means->Write();
  
  
  f_out->Write();
  
}