// this is the compare.C for the 

#define use_intersect false // use fit+ intersect methond, else use 50% of cumulant
#define REFCHAN "08"
#define DRAW_ALL_T1 false

#include <iostream>
#include <fstream>



// Bool_t file_exists(TString fname){
//   
//   fstream src_file(fname.Data());
//   return src_file.good();
// }


TLatex* plotTopLegend(char* label,Float_t x=-1,Float_t y=-1,Float_t size=0.06,Int_t color=1,Float_t angle=0.0, Int_t align=11)
{
    // coordinates in NDC! (pad bottom left corner = (0,0); pad top right corner = (1,1);)
    // plots the string label in position x and y in NDC coordinates
    // size is the text size
    // color is the text color
 
    if(x<0||y<0)
    {   // defaults
      x=gPad->GetLeftMargin()*1.15;
      y=(1-gPad->GetTopMargin())*1.04;
    }
    y= y-y*gPad->GetTopMargin();
 
    TLatex* text=new TLatex(x,y,label);
    text->SetTextSize(size);
    text->SetNDC();
    text->SetTextColor(color);
    text->SetTextAngle(angle);
    text->SetTextAlign(align);
    text->Draw();
    return text;
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


void HistXCut(TH1F *hist, Float_t min, Float_t max) {
  
    //  cutting ghots signals with long drift times t1 > 90 ns
    Int_t binNumber = hist->GetNbinsX() ;
    for(Int_t ibin =  hist->FindBin(max) ; ibin < binNumber ; ibin++){
      hist->SetBinContent(ibin,0);    
      }
    for(Int_t ibin = 1 ; ibin < hist->FindBin(min)   ; ibin++){
      hist->SetBinContent(ibin,0);    
      }
      
    hist->SetEntries((Int_t) hist->Integral());
}

Float_t cut_around_maximum(TH1F *hist, Float_t below, Float_t above) {
  
  Int_t binmax = hist->GetMaximumBin();
  Double_t x = hist->GetXaxis()->GetBinCenter(binmax);
  HistXCut(hist,x-below,x+above);
  
  return x;
  
}


void draw_and_save(TObject *hist,TString name,TString outdir,TString draw_options) {
  
//   if(DRAW_PNG) {
	//~ gStyle->SetNumberContours(255);   // finer palette
	//~ gStyle->SetPalette(1,0); 
	//~ // //create pretty High contrast color palette 
		{
			const Int_t NRGBs = 5;
			const Int_t NCont = 80; 
			Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
			Double_t reds[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
			Double_t greens[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
			Double_t blues[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
			Int_t  FI = TColor::CreateGradientColorTable(NRGBs, stops, reds, greens, blues, NCont);
			gStyle->SetNumberContours(NCont);
		}
    TCanvas *c = new TCanvas("c_"+name,"c_"+name,200,10,640,640);
    c->cd();
    hist->Draw(draw_options);
    c->Print(outdir+name+".png");
//   }
//   c->Print(outdir+name+".pdf");
//     c->Print(outdir+name+".svg");
}


void get_gauss_params(TH1F* histogram, Float_t* target_mu, Float_t* target_sigma) {
//   histogram->Fit("gaus");
//   TF1 *fit = histogram->GetFunction("gaus");
  
  Float_t prior_mean = histogram->GetMean();
  Float_t prior_ampl = histogram->GetEntries();
  
  TF1 *fit = new TF1("myfit","gaus", prior_mean-60, prior_mean+60);

  fit->SetParName(0,"ampl");
  fit->SetParName(1,"mu");
  fit->SetParName(2,"sigma");
  fit->SetParameter(0, prior_ampl);
  fit->SetParameter(1, prior_mean);
  fit->SetParameter(2, 10);

  histogram->Fit("myfit");
  
  
  
  Float_t chi2 = fit->GetChisquare();
  Float_t p1 = fit->GetParameter(1);
  Float_t e1 = fit->GetParError(1);
  Float_t p2 = fit->GetParameter(2);
  Float_t e2 = fit->GetParError(2);
//   cout << "Gauss fit sigma: " << p2 << " +- " << e2 << "ns" << endl;
  *target_mu = p1;
  *target_sigma = p2;
}


void get_toa_offset(TH1F* toa0, Float_t* intersect, Float_t* midpoint) {
    
  TCanvas * dummy_canvas = new TCanvas("dummy","dummy",100,100);
  TH1* cum_toa0 = toa0->GetCumulative();
  Int_t cum_toa0_sum = cum_toa0->GetBinContent(cum_toa0->GetEntries());
//   cout << "toa0 sum " << cum_toa0_sum <<endl;
  
  Float_t cum_toa0_up   = ((Float_t) cum_toa0_sum) *0.6;
  Float_t cum_toa0_down = ((Float_t) cum_toa0_sum) *0.1;
  Float_t cum_toa0_mid = ((Float_t) cum_toa0_sum) *0.5;
  
  Int_t cum_toa0_left_bin   = cum_toa0->FindFirstBinAbove(cum_toa0_down,2);
  Int_t cum_toa0_right_bin  = cum_toa0->FindFirstBinAbove(cum_toa0_up,2);
  Int_t cum_toa0_mid_bin    = cum_toa0->FindFirstBinAbove(cum_toa0_mid,2);
  
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
  *midpoint = xaxis->GetBinCenter(cum_toa0_mid_bin);
  
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
  
  cum_toa0->Fit(fa,"q","", cum_toa0_left,cum_toa0_right);
  
  *intersect = -fa->GetParameter(0)/fa->GetParameter(1);
  dummy_canvas->Delete();
  
//   cout << "x intersect " << x_intersect << endl;
  
//   draw_and_save(cum_toa0,"cum_toa0","./","");
  
//   return x_intersect;

  
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
    
    TH1::AddDirectory(kFALSE);
    
    
  TString TDC=from_env("TDC","1482");
  TString chan=from_env("chan","05");
  
  TString t1_source=from_env("t1_source","");
    
  TString outdir=from_env("outdir","./");
  TString scan_x=from_env("scan_x","false");
  TString scan_z=from_env("scan_z","false");
  TString scan_thr=from_env("scan_thr","false");
  
  TString graph_x_unit       = "mm";
  TString graph_x_observable = "y";
  
  ofstream graph_x_txt (outdir+"/graph_x.txt");
  ofstream t1_mean_txt (outdir+"/t1_mean.txt");
  ofstream t1_txt (outdir+"/t1.txt");
  ofstream t1_std_txt (outdir+"/t1_std.txt");
  ofstream tot_mean_txt (outdir+"/tot_mean.txt");
  ofstream tot_std_txt (outdir+"/tot_std.txt");
  ofstream tot_untrig_mean_txt (outdir+"/tot_untrig_mean.txt");
  ofstream tot_untrig_std_txt (outdir+"/tot_untrig_std.txt");
//   if (myfile.is_open())
//   {
//     myfile << "This is a line.\n";
//     myfile << "This is another line.\n";
//     for(int count = 0; count < size; count ++){
//         myfile << x[count] << " " ;
//     }
//     myfile.close();
//   }
//   

  
  TGraph *tg_ChX_t1 = new TGraph();
  tg_ChX_t1->SetTitle("t1 Ch"+chan);
  tg_ChX_t1->SetName("tg_Ch"+chan+"_t1");
  tg_ChX_t1->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_t1->GetYaxis()->SetTitle("t1 (ns)");
  //tg_ChX_t1->GetZaxis()->SetTitle("ToT (ns)");
  
  TGraphErrors *tg_ChX_t1_means = new TGraphErrors();
  tg_ChX_t1_means->SetTitle("t1 Ch"+chan+" means");
  tg_ChX_t1_means->SetName("tg_Ch"+chan+"_t1_means");
  tg_ChX_t1_means->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_t1_means->GetYaxis()->SetTitle("t1 (ns)");
  
  TGraphErrors *tg_ChX_t1_gauss = new TGraphErrors();
  tg_ChX_t1_gauss->SetTitle("t1 Ch"+chan+" gauss");
  tg_ChX_t1_gauss->SetName("tg_Ch"+chan+"_t1_gauss");
  tg_ChX_t1_gauss->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_t1_gauss->GetYaxis()->SetTitle("t1 (ns)");
  

  TGraphErrors *tg_ChX_tot_means = new TGraphErrors();
  tg_ChX_tot_means->SetTitle("tot Ch"+chan+" means");
  tg_ChX_tot_means->SetName("tg_Ch"+chan+"_tot_means");
  tg_ChX_tot_means->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_tot_means->GetYaxis()->SetTitle("tot (ns)");
  
  TGraphErrors *tg_ChX_tot_untrig_means = new TGraphErrors();
  tg_ChX_tot_untrig_means->SetTitle("tot untrig Ch"+chan+" means");
  tg_ChX_tot_untrig_means->SetName("tg_Ch"+chan+"_tot_untrig_means");
  tg_ChX_tot_untrig_means->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_tot_untrig_means->GetYaxis()->SetTitle("tot (ns)");
  
  TGraphErrors *tg_ChX_t1_std = new TGraphErrors();
  tg_ChX_t1_std->SetTitle("t1 Ch"+chan+" StdDevs");
  tg_ChX_t1_std->SetName("tg_ChX_t1_std");
  tg_ChX_t1_std->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_t1_std->GetYaxis()->SetTitle("t1 StdDev (ns)");
  
  TGraph *tg_ChX_counts = new TGraph();
  tg_ChX_counts->SetTitle("Ch"+chan+" Counts");
  tg_ChX_counts->SetName("tg_ChX_counts");
  tg_ChX_counts->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_counts->GetYaxis()->SetTitle("counts");
  
  TGraph *tg_ChX_efficiency = new TGraph();
  tg_ChX_efficiency->SetTitle("Ch"+chan+" Efficiency");
  tg_ChX_efficiency->SetName("tg_ChX_efficiency");
  tg_ChX_efficiency->GetXaxis()->SetTitle("y-pos (mm)");
  tg_ChX_efficiency->GetYaxis()->SetTitle("efficiency");
  
  TGraph *tg_intensity = new TGraph();
  tg_intensity->SetTitle("Laser Intensity");
  tg_intensity->SetName("tg_intensity");
  tg_intensity->GetXaxis()->SetTitle("y-pos (mm)");
  tg_intensity->GetYaxis()->SetTitle("intensity (nJ)");
  
  if( scan_x == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_tot_untrig_means->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_counts->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("x-pos (mm)");
    tg_intensity->GetXaxis()->SetTitle("x-pos (mm)");
    tg_ChX_efficiency->GetXaxis()->SetTitle("x-pos (mm)");
    graph_x_observable = "x";
  }
  if( scan_z == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_tot_untrig_means->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_counts->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("z-pos (mm)");
    tg_intensity->GetXaxis()->SetTitle("z-pos (mm)");
    tg_ChX_efficiency->GetXaxis()->SetTitle("z-pos (mm)");
    graph_x_observable = "z";
  }
  if( scan_thr == "true") {
    tg_ChX_t1->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_std->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_means->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_tot_means->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_tot_untrig_means->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_counts->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_t1_gauss->GetXaxis()->SetTitle("threshold (LSB)");
    tg_intensity->GetXaxis()->SetTitle("threshold (LSB)");
    tg_ChX_efficiency->GetXaxis()->SetTitle("threshold (LSB)");
    graph_x_observable = "thr";
    graph_x_unit = "LSB";
  }
  
  
  
  
  TFile *f_out = new TFile(outdir+"/compare.root","RECREATE");
  f_out->cd();
  TCanvas * c_t1_all = new TCanvas("t1_all","t1_all",1500,900);
  
  
  
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
  
  c_t1_all->Divide(3,list.size()/3+1);
  
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
  
  
  Int_t point_no = 0;
  for (Int_t i = 0; i< list.size(); i++){
    
    
//     TString fname   = ((class TObjString*) list[i].Tokenize(";")->At(0))->GetString();
//     TString comment = ((class TObjString*) list[i].Tokenize(";")->At(1))->GetString();
    TString fname   = list[i];
    TString comment = "blah";
    cout << "fname: " << fname << " comment: "<< comment << endl;
    
    
    cout << "opening file: "<< fname <<endl; 
//     if (not(file_exists(fname))){
//       cout << fname << " does not exist" << endl;
//       continue;
//     }
    
    
    TFile *f = new TFile(fname);
    
    f->cd();
    
    TH1F* CentA_t1;
    if( t1_source != "" ){
      CentA_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_"+t1_source));  
    } else {
      CentA_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_t1"));  
    }
    
    TH2F* CentA_potato = ((TH2F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_potato"));  
    
    //HistXCut(CentA_t1,-50,90);
    //cut_around_maximum(CentA_t1,10,10);
    
    TH1F* CentA_tot = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_tot"));  
//     CentA_t1->Rebin(4);
    TH1F* CentA_tot_untrig = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_tot_untrig"));  
    TH1F* RefChan_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+REFCHAN+"_t1"));  
    
    
    Float_t intersect = 0;
    Float_t midpoint = 0;
    get_toa_offset(CentA_t1, &intersect, &midpoint);

    Float_t t1 = midpoint;
    if(use_intersect) {
      t1 = intersect;
    }
    Double_t t1_mean = CentA_t1->GetMean();
    Double_t counts = CentA_t1->Integral();
    Double_t ref_counts = RefChan_t1->Integral();
    Double_t t1_std  = CentA_t1->GetStdDev();
    Double_t tot_mean = CentA_tot->GetMean();
    Double_t tot_std  = CentA_tot->GetStdDev();
    Double_t tot_untrig_mean = CentA_tot_untrig->GetMean();
    Double_t tot_untrig_std  = CentA_tot_untrig->GetStdDev();
    
    Double_t efficiency = 0;
    
    Float_t t1_gauss_mu = midpoint;
    Float_t t1_gauss_sigma =0;
    
    //get_gauss_params(CentA_t1,&t1_gauss_mu,&t1_gauss_sigma);
    
    
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
      graph_x = xlist[i].Atof();
    } else if( scan_z == "true") {
      graph_x = zlist[i].Atof();
    } else {
      graph_x = ylist[i].Atof();
    }
    
    TString graph_x_str;
    graph_x_str.Form("%06.2f",graph_x);
    if(DRAW_ALL_T1){ draw_and_save(CentA_t1,"CentA_t1"+graph_x_str,outdir,""); }
    
    Double_t intensity = intensitylist[i].Atof();
//     cout << "intensity" << intensitylist[i] << endl;
//     Double_t intensity = i;
    cout << "test" << endl;
    if( true) { // select only points in the anode plane
       
        tg_ChX_t1->SetPoint(point_no,graph_x,t1);
        tg_ChX_t1_means->SetPoint(point_no,graph_x,t1_mean);
        tg_ChX_t1_means->SetPointError (point_no, 0, t1_std);
        tg_ChX_tot_means->SetPoint(point_no,graph_x,tot_mean);
        tg_ChX_tot_means->SetPointError (point_no, 0, tot_std);
        tg_ChX_tot_untrig_means->SetPoint(point_no,graph_x,tot_untrig_mean);
        tg_ChX_tot_untrig_means->SetPointError (point_no, 0, tot_untrig_std);
        
        tg_ChX_counts->SetPoint(point_no,graph_x,counts);
        
        
        tg_intensity->SetPoint(point_no,graph_x,intensity);
        
        TH1F* t1_clone = (TH1F*) CentA_t1->Clone();
        TH2F* potato_clone = (TH2F*) CentA_potato->Clone();
        TH1F* tot_clone = (TH1F*) CentA_tot->Clone();
        TH1F* tot_untrig_clone = (TH1F*) CentA_tot_untrig->Clone();
        TString new_hist_name;
        new_hist_name.Form("%04.1f_t1_hist",graph_x);
        t1_clone->SetName(new_hist_name);
        new_hist_name.Form("%04.1f_potato_hist",graph_x);
        potato_clone->SetName(new_hist_name);
        new_hist_name.Form("%04.1f_tot_hist",graph_x);
        tot_clone->SetName(new_hist_name);
        new_hist_name.Form("%04.1f_tot_untrig_hist",graph_x);
        tot_untrig_clone->SetName(new_hist_name);
        f_out->cd();
//         t1_clone->Write();
        potato_clone->Write();
        tot_clone->Write();
        tot_untrig_clone->Write();
        
//         c_t1_all->Clear() ;
//         c_t1_all->Divide(8,3) ;
        c_t1_all->cd(1+i) ;
        
        t1_clone->Draw();
        t1_clone->Fit("gaus","WW q","",t1 - 20,t1 + 20);
        float fit_mean =  t1_clone->GetFunction("gaus")->GetParameter(1);
        float fit_sigma =  t1_clone->GetFunction("gaus")->GetParameter(2);	
        t1_clone->Fit("gaus","WW","",fit_mean-2*fit_sigma,fit_mean+2*fit_sigma);
        t1_clone->GetXaxis()->SetRangeUser(fit_mean-10*fit_sigma,fit_mean+10*fit_sigma);
        t1_gauss_mu = t1_clone->GetFunction("gaus")->GetParameter(1);
        t1_gauss_sigma = t1_clone->GetFunction("gaus")->GetParameter(2);
        float t1_gauss_sigma_error = t1_clone->GetFunction("gaus")->GetParError(2);
        plotTopLegend(Form(graph_x_observable+" = %4.1f "+graph_x_unit, graph_x), 0.1,1.01);
        
        counts=t1_clone->Integral(t1_clone->FindBin(fit_mean-10*fit_sigma),t1_clone->FindBin(fit_mean+10*fit_sigma));
        
        if (ref_counts > 0) {
              efficiency = counts/ref_counts;
            }
        
        
        tg_ChX_t1_gauss->SetPoint(point_no,graph_x,t1_gauss_mu);
        tg_ChX_t1_gauss->SetPointError (point_no, 0, t1_gauss_sigma);
        tg_ChX_t1_std->SetPoint(point_no, graph_x, t1_gauss_sigma);
        tg_ChX_t1_std->SetPointError(point_no, 0, t1_gauss_sigma_error);
        tg_ChX_efficiency->SetPoint(point_no,graph_x,efficiency);
        f->cd();
        
        graph_x_txt << graph_x << endl;
        t1_txt << t1 << endl;
        t1_mean_txt << t1_mean << endl;
        t1_std_txt << t1_std << endl;
        tot_mean_txt << tot_mean << endl;
        tot_std_txt << tot_std << endl;
        tot_untrig_mean_txt << tot_untrig_mean << endl;
        tot_untrig_std_txt << tot_untrig_std << endl;
        
        
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

  
  Int_t i = 0;
  
  TCanvas * c_results_all = new TCanvas("results_all","results_all",1024,786);
  c_results_all->Divide(2,2);

  
  tg_ChX_t1->SetLineColor(2);
  tg_ChX_t1->SetLineWidth(4);
  tg_ChX_t1->SetMarkerColor(4);
  tg_ChX_t1->SetMarkerStyle(21);
//   tg_ChX_t1->GetYaxis()->SetRangeUser(-10,20);
  if(TDC == "1483") tg_ChX_t1->GetYaxis()->SetRangeUser(-20,-5);
//   draw_and_save(tg_ChX_t1,"tg_Ch"+chan+"_t1",outdir,"AP");
//   tg_ChX_t1->Draw("AP");
  
  tg_ChX_t1_means->SetLineColor(2);
  tg_ChX_t1_means->SetLineWidth(4);
  tg_ChX_t1_means->SetMarkerColor(4);
  tg_ChX_t1_means->SetMarkerStyle(21);
//   tg_ChX_t1_means->GetYaxis()->SetRangeUser(-10,20);
  if(TDC == "1483") tg_ChX_t1_means->GetYaxis()->SetRangeUser(-20,-5);
//   draw_and_save(tg_ChX_t1_means,"tg_Ch"+chan+"_t1_means",outdir,"AP");
  
  tg_ChX_t1_gauss->SetLineColor(2);
  tg_ChX_t1_gauss->SetLineWidth(4);
  tg_ChX_t1_gauss->SetMarkerColor(4);
  tg_ChX_t1_gauss->SetMarkerStyle(21);
  tg_ChX_t1_gauss->GetYaxis()->SetRangeUser(-800-20,-725-20);
  draw_and_save(tg_ChX_t1_gauss,"tg_Ch"+chan+"_t1_gauss",outdir,"APL");
  c_results_all->cd(++i);
  tg_ChX_t1_gauss->Draw("AP");
  
  
  

  
  
  tg_ChX_tot_untrig_means->SetLineColor(2);
  tg_ChX_tot_untrig_means->SetLineWidth(4);
  tg_ChX_tot_untrig_means->SetMarkerColor(4);
  tg_ChX_tot_untrig_means->SetMarkerStyle(21);
  tg_ChX_tot_untrig_means->GetYaxis()->SetRangeUser(0,600);
  if(TDC == "1483") tg_ChX_tot_untrig_means->GetYaxis()->SetRangeUser(0,200);
//   draw_and_save(tg_ChX_tot_untrig_means,"tg_Ch"+chan+"_tot_untrig_means",outdir,"AP");
  
  
  
  tg_ChX_t1_std->SetLineColor(2);
  tg_ChX_t1_std->SetLineWidth(4);
  tg_ChX_t1_std->SetMarkerColor(4);
  tg_ChX_t1_std->SetMarkerStyle(21);
  tg_ChX_t1_std->GetYaxis()->SetRangeUser(0,12);
//   if(TDC == "1483") tg_ChX_t1_std->GetYaxis()->SetRangeUser(0,6);
  draw_and_save(tg_ChX_t1_std,"tg_Ch"+chan+"_t1_std",outdir,"APL");
  c_results_all->cd(++i);
  tg_ChX_t1_std->Draw("APL");
  
  
  
  
  tg_ChX_tot_means->SetLineColor(2);
  tg_ChX_tot_means->SetLineWidth(4);
  tg_ChX_tot_means->SetMarkerColor(4);
  tg_ChX_tot_means->SetMarkerStyle(21);
  tg_ChX_tot_means->GetYaxis()->SetRangeUser(0,600);
  if(TDC == "1483") tg_ChX_tot_means->GetYaxis()->SetRangeUser(0,200);
//   draw_and_save(tg_ChX_tot_means,"tg_Ch"+chan+"_tot_means",outdir,"AP");
  c_results_all->cd(++i);
  tg_ChX_tot_means->Draw("AP");
  
  tg_ChX_counts->SetLineColor(2);
  tg_ChX_counts->SetLineWidth(4);
  tg_ChX_counts->SetMarkerColor(4);
  tg_ChX_counts->SetMarkerStyle(21);
//   draw_and_save(tg_ChX_counts,"tg_Ch"+chan+"_counts",outdir,"AP");
  
  tg_ChX_efficiency->SetLineColor(2);
  tg_ChX_efficiency->SetLineWidth(4);
  tg_ChX_efficiency->SetMarkerColor(4);
  tg_ChX_efficiency->SetMarkerStyle(21);
  tg_ChX_efficiency->SetMarkerSize(1);
  tg_ChX_efficiency->GetYaxis()->SetRangeUser(-0.1,1.1);
//   draw_and_save(tg_ChX_efficiency,"tg_Ch"+chan+"_efficiency",outdir,"APL");
  c_results_all->cd(++i);
  tg_ChX_efficiency->Draw("APL");
  
  tg_intensity->SetLineColor(2);
  tg_intensity->SetLineWidth(4);
  tg_intensity->SetMarkerColor(4);
  tg_intensity->SetMarkerStyle(21);
//   draw_and_save(tg_intensity,"tg_intensity",outdir,"AP");
//   tg_intensity->GetYaxis()->SetRangeUser(0.0,30.0);
  
  
  
  
//   TSeqCollection * canvases = gROOT->GetListOfCanvases();
//   
//   for (Int_t i = 0; i < canvases->GetEntries(); i++) {
//     ((TCanvas*) canvases->At(i))->Modified();
//     ((TCanvas*) canvases->At(i))->Update();
//     
//     cout << ((TCanvas*) canvases->At(i))->GetName() << endl;
//   }

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
  tg_ChX_t1_gauss->Write();
  tg_ChX_counts->Write();
  tg_ChX_efficiency->Write();
  tg_ChX_tot_means->Write();
  c_t1_all->Write();
  
  f_out->Write();
  
}
