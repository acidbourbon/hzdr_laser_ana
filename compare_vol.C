// this is the compare.C for the 

#define REFCHAN "08"
#define use_intersect false // use fit+ intersect methond, else use 50% of cumulant

#define t1_min -100
#define t1_max 200

#define t1_min -200
#define t1_max 200


// Bool_t file_exists(TString fname){
//   
//   fstream src_file(fname.Data());
//   return src_file.good();
// }

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
    TCanvas *c = new TCanvas("c_"+name,"c_"+name,200,10,1024,786);
    c->cd();
    hist->Draw(draw_options);
    c->Print(outdir+name+".png");
//   }
//   c->Print(outdir+name+".pdf");
    c->Print(outdir+name+".svg");
}




void get_toa_offset(TH1F* toa0, Float_t* intersect, Float_t* midpoint) {
    
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
  
//   cout << "x intersect " << x_intersect << endl;
  
//   draw_and_save(cum_toa0,"cum_toa0","./","");
  
//   return x_intersect;

  
}



void compare_vol(void) {
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
  TString scan_z=from_env("scan_z","false");
  TString scan_thr=from_env("scan_thr","false");
  
  TString pol_cent_x_str=from_env("pol_cent_x","4000");
  TString pol_cent_y_str=from_env("pol_cent_y","4000");
  TString pol_cent_z_str=from_env("pol_cent_z","4000");
  Double_t pol_cent_x = pol_cent_x_str.Atof();
  Double_t pol_cent_y = pol_cent_y_str.Atof();
  Double_t pol_cent_z = pol_cent_z_str.Atof();

  
  
  TGraph2D *tg_ChX_t1 = new TGraph2D();
  tg_ChX_t1->SetTitle("t1 Ch"+chan);
  tg_ChX_t1->SetName("tg_Ch"+chan+"_t1");
  tg_ChX_t1->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_t1->GetYaxis()->SetTitle("z-pos (um)");
  tg_ChX_t1->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_t1->GetZaxis()->SetTitle("t1 (ns)");
  tg_ChX_t1->GetZaxis()->SetTitleOffset(1.4);
  
  TGraph2D *tg_ChX_t1_unpolar = new TGraph2D();
  tg_ChX_t1_unpolar->SetTitle("t1 Ch"+chan+"_t1_unpolar");
  tg_ChX_t1_unpolar->SetName("tg_Ch"+chan+"_t1_unpolar");
  tg_ChX_t1_unpolar->GetXaxis()->SetTitle("Phi");
  tg_ChX_t1_unpolar->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_unpolar->GetYaxis()->SetTitle("radius (um)");
  tg_ChX_t1_unpolar->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_unpolar->GetZaxis()->SetTitle("t1 (ns)");
  tg_ChX_t1_unpolar->GetZaxis()->SetTitleOffset(1.4);
  
#define interpol_phisteps 72+1 // 5 degree steps
#define interpol_radsteps 100
#define interpol_max_rad  4000
#define interpol_max_y 3000
#define interpol_min_y -3000
#define interpol_max_z 2500
#define interpol_min_z -2500
  
  TH2F* t1_unpolar_interpol = new TH2F("t1_unpolar_interpol","t1_unpolar_interpol",interpol_phisteps,0,2*TMath::Pi(),interpol_radsteps,0,interpol_max_rad);
  
  
  TGraph2DErrors *tg_ChX_t1_means = new TGraph2DErrors();
  tg_ChX_t1_means->SetTitle("t1 Ch"+chan+" means");
  tg_ChX_t1_means->SetName("tg_Ch"+chan+"_t1_means");
  tg_ChX_t1_means->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1_means->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_means->GetYaxis()->SetTitle("z-pos (um)");
  tg_ChX_t1_means->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_means->GetZaxis()->SetTitle("t1 (ns)");
  tg_ChX_t1_means->GetZaxis()->SetTitleOffset(1.4);
  
  TGraph2DErrors *tg_ChX_tot_means = new TGraph2DErrors();
  tg_ChX_tot_means->SetTitle("tot Ch"+chan+" means");
  tg_ChX_tot_means->SetName("tg_Ch"+chan+"_tot_means");
  tg_ChX_tot_means->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_tot_means->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_tot_means->GetYaxis()->SetTitle("z-pos (um)");
  tg_ChX_tot_means->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_tot_means->GetZaxis()->SetTitle("tot (ns)");
  tg_ChX_tot_means->GetZaxis()->SetTitleOffset(1.4);

//   TGraphErrors *tg_ChX_tot_means = new TGraphErrors();
//   tg_ChX_tot_means->SetTitle("tot ChX means");
//   tg_ChX_tot_means->SetName("tg_ChX_tot_means");
//   tg_ChX_tot_means->GetXaxis()->SetTitle("y-pos (um)");
//   tg_ChX_tot_means->GetYaxis()->SetTitle("tot (ns)");
  
//   TGraph *tg_ChX_t1_std = new TGraph();
//   tg_ChX_t1_std->SetTitle("t1 ChX StdDevs");
//   tg_ChX_t1_std->SetName("tg_ChX_t1_std");
//   tg_ChX_t1_std->GetXaxis()->SetTitle("y-pos (um)");
//   tg_ChX_t1_std->GetYaxis()->SetTitle("t1 StdDev (ns)");
  
  TGraph2D *tg_ChX_t1_std = new TGraph2D();
  tg_ChX_t1_std->SetTitle("t1 Ch"+chan+" StdDevs");
  tg_ChX_t1_std->SetName("tg_Ch"+chan+"_t1_std");
  tg_ChX_t1_std->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_t1_std->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_std->GetYaxis()->SetTitle("z-pos (um)");
  tg_ChX_t1_std->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_std->GetZaxis()->SetTitle("t1 StdDev (ns)");
  tg_ChX_t1_std->GetZaxis()->SetTitleOffset(1.4);
  
  
  TGraph2D *tg_ChX_counts = new TGraph2D();
  tg_ChX_counts->SetTitle("Ch"+chan+" Counts");
  tg_ChX_counts->SetName("tg_Ch"+chan+"_counts");
  tg_ChX_counts->GetXaxis()->SetTitle("y-pos (um)");
  tg_ChX_counts->GetYaxis()->SetTitle("z-pos (um)");
  tg_ChX_counts->GetZaxis()->SetTitle("counts");
  
  TGraph2D *tg_intensity = new TGraph2D();
  tg_intensity->SetTitle("Laser Intensity");
  tg_intensity->SetName("tg_intensity");
  tg_intensity->GetXaxis()->SetTitle("y-pos (um)");
  tg_intensity->GetYaxis()->SetTitle("z-pos (um)");
  tg_intensity->GetZaxis()->SetTitle("intensity (nJ)");  
  
  TFile *f_out = new TFile(outdir+"/compare.root","RECREATE");
  
  
  
  TH1F* radiae = new TH1F(
  "radiae","radiae;distance to center (µm);measured points",
                                     2000,0,4000);  
  
  
  
  
  TTree* scan_data_tree = new TTree("scan_data_tree", "scan_data");
  
  Double_t t1;   
  Double_t t1_mean;   
  Double_t counts;    
  Double_t ref_counts;
  Double_t efficiency;
  Double_t t1_std;    
  Double_t tot_mean;  
  Double_t tot_std;  
  Double_t xpos;
  Double_t ypos;
  Double_t zpos;
  Double_t radius;
  Double_t phi;
  
  scan_data_tree->Branch("t1",&t1);
  scan_data_tree->Branch("t1_mean",&t1_mean);
  scan_data_tree->Branch("counts",&counts);
  scan_data_tree->Branch("ref_counts",&ref_counts);
  scan_data_tree->Branch("t1_std",&t1_std);
  scan_data_tree->Branch("tot_means",&tot_mean);
  scan_data_tree->Branch("tot_std",&tot_std);
  scan_data_tree->Branch("xpos",&xpos);
  scan_data_tree->Branch("ypos",&ypos);
  scan_data_tree->Branch("zpos",&zpos);
  scan_data_tree->Branch("efficiency",&efficiency);
  scan_data_tree->Branch("radius",&radius);
  scan_data_tree->Branch("phi",&phi);
  
  
  
  
  
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
  
  Int_t point_no = 0;
  Int_t unpolar_point_no = 0;
  
  for (Int_t i = 0; i< list.size(); i++){
    
    
//     TString fname   = ((class TObjString*) list[i].Tokenize(";")->At(0))->GetString();
//     TString comment = ((class TObjString*) list[i].Tokenize(";")->At(1))->GetString();
    TString fname   = list[i];
    TString comment = "blah";
    cout << "fname: " << fname << " comment: "<< comment << endl;
    
    
    cout << "opening file: "<< fname <<endl; 
     //~ fstream is not working !
    //~ if (not(file_exists(fname))){
      //~ cout << fname << " does not exist" << endl;
      //~ continue;
    //~ }
    
    
    TFile *f = new TFile(fname);
    
    f->cd();
    
    TH1F* CentA_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_t1"));  
    
    //  cutting ghots signals with long drift times t1 > 90 ns
		Int_t binNumber = CentA_t1->GetNbinsX() ;
		Float_t T1MaxCut (90);
		for(Int_t ibin =  CentA_t1->FindBin(T1MaxCut) ; ibin < binNumber ; ibin++){
			CentA_t1->SetBinContent(ibin,0);		
			}
		Float_t T1MinCut (-55);
		for(Int_t ibin = 0 ; ibin < CentA_t1->FindBin(T1MinCut)   ; ibin++){
			CentA_t1->SetBinContent(ibin,0);		
			}
    
    TH1F* CentA_tot = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_tot"));  
//     CentA_t1->Rebin(4);
    TH1F* RefChan_t1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+REFCHAN+"_t1"));  
    
    
    Float_t intersect = 0;
    Float_t midpoint = 0;
    get_toa_offset(CentA_t1, &intersect, &midpoint);
	CentA_t1->DrawCopy("hist e");
	CentA_t1->SaveAs("www.png");
    t1 = midpoint;
    if(use_intersect) {
      t1 = intersect;
    }
     t1_mean = CentA_t1->GetMean();
     counts = CentA_t1->GetEntries();
     ref_counts = RefChan_t1->GetEntries();
     t1_std  = CentA_t1->GetStdDev();
     tot_mean = CentA_tot->GetMean();
     tot_std  = CentA_tot->GetStdDev();
    
     efficiency = 0;
    if (ref_counts > 0) {
      efficiency = counts/ref_counts;
    }
    
//     get_gauss_params(CentA_t1,&t1_gauss_mu,&t1_gauss_sigma);
    
    

    cout << "x pos: " << xlist[i] << endl;
    cout << "y pos: " << ylist[i] << endl;
    cout << "z pos: " << zlist[i] << endl;
    
    xpos= xlist[i].Atof();
    ypos= ylist[i].Atof();
    zpos= zlist[i].Atof();

//     if( t1 > 100 || t1 < -100) {
//         t1 = -100;
//     }
    
    Double_t graph_x;

    graph_x = ylist[i].Atoi();

    
    Double_t intensity = intensitylist[i].Atof();
    cout << "intensity" << intensitylist[i] << endl;
    cout << "counts  : " << counts << endl;

    Double_t graph_y = zlist[i].Atoi();
//     cout << "z: " << graph_y << endl;
    
    radius = TMath::Sqrt(TMath::Power(pol_cent_y-ypos,2) +  TMath::Power(pol_cent_z-zpos,2));
    
    Double_t polar_y = ypos- pol_cent_y;
    Double_t polar_z = zpos- pol_cent_z;
    
    phi = -1;
    
    Double_t pi = TMath::Pi();
    
    if (polar_y >= 0 && polar_z >= 0) {
      phi = TMath::ATan( polar_z/polar_y);
    } else if ( polar_y < 0 && polar_z >= 0) {
      phi = TMath::ATan( (-polar_y)/polar_z) + pi / 2.0;
    } else if ( polar_y < 0 && polar_z < 0) {
      phi = TMath::ATan( polar_z/polar_y) + pi;
    } else if ( polar_y >= 0 && polar_z < 0) {
      phi = TMath::ATan( polar_y/(-polar_z)) + pi * 3.0/2.0;
    }
    
    
    radiae->Fill(radius);
    
    scan_data_tree->Fill();
    
    if( t1 < t1_max && t1 > t1_min) {
//     if( xlist[i].Atoi() == 3900 ) {
//     if( xlist[i].Atoi() == 3400 || false) {
//     if( xlist[i].Atoi() == 3300 || false) {
//     if( xlist[i].Atoi() == 3200 || false) {
//     if( xlist[i].Atoi() == 5600 ) { // this scan did not finish!
      


        tg_ChX_t1->SetPoint(point_no,graph_x,graph_y,t1);
        tg_ChX_t1_unpolar->SetPoint(unpolar_point_no++,phi,radius,t1);
        if(phi>-0.01 && phi<0.01){ // phi is almost 0, duplicate points at 2 pi
          tg_ChX_t1_unpolar->SetPoint(unpolar_point_no++,2*pi,radius,t1);
        }
        tg_ChX_t1_unpolar->SetPoint(unpolar_point_no++,phi,radius,t1);
        tg_ChX_t1_means->SetPoint(point_no,graph_x,graph_y,t1_mean);
        tg_ChX_t1_means->SetPointError (point_no, 0, 0 ,t1_std);
        tg_ChX_tot_means->SetPoint(point_no,graph_x,graph_y,tot_mean);
        tg_ChX_tot_means->SetPointError (point_no, 0, 0, tot_std);
        tg_ChX_t1_std->SetPoint(point_no, graph_x, graph_y, t1_std);
        
//        tg_ChX_counts->SetPoint(point_no,graph_x,graph_y,counts);
        
        
//        tg_intensity->SetPoint(point_no,graph_x,graph_y,intensity);
        
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
   // CentA_t1->SetLineColor(color_contrast_array[i]);
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
  
  tg_ChX_t1->SetLineColor(2);
  tg_ChX_t1->SetLineWidth(1);
  tg_ChX_t1->SetMarkerColor(kBlack);
  tg_ChX_t1->SetMarkerStyle(6);
  draw_and_save(tg_ChX_t1,"tg_Ch"+chan+"_t1",outdir,"tri1 pcol");
		tg_ChX_t1->GetXaxis()->SetTitle("y-pos [#mum]");
	  tg_ChX_t1->GetXaxis()->SetTitleOffset(2.4);
	  tg_ChX_t1->GetYaxis()->SetTitle("z-pos [#mum]");
	  tg_ChX_t1->GetYaxis()->SetTitleOffset(2.4);
	  tg_ChX_t1->GetZaxis()->SetTitle("t1 [ns]");
	  tg_ChX_t1->GetZaxis()->SetTitleOffset(1.4);
//   tg_ChX_t1->Draw("AP");
      
  tg_ChX_t1_unpolar->SetLineColor(2);
  tg_ChX_t1_unpolar->SetLineWidth(1);
  tg_ChX_t1_unpolar->SetMarkerColor(kBlack);
  tg_ChX_t1_unpolar->SetMarkerStyle(6);
  draw_and_save(tg_ChX_t1_unpolar,"tg_Ch"+chan+"_t1_unpolar",outdir,"tri1 pcol");
  tg_ChX_t1_unpolar->GetXaxis()->SetTitle("phi");
  tg_ChX_t1_unpolar->GetXaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_unpolar->GetYaxis()->SetTitle("radius [#mum]");
  tg_ChX_t1_unpolar->GetYaxis()->SetTitleOffset(2.4);
  tg_ChX_t1_unpolar->GetZaxis()->SetTitle("t1 [ns]");
  tg_ChX_t1_unpolar->GetZaxis()->SetTitleOffset(1.4);
  
  tg_ChX_t1_means->SetLineColor(2);
  tg_ChX_t1_means->SetLineWidth(4);
  tg_ChX_t1_means->SetMarkerColor(4);
  tg_ChX_t1_means->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1_means,"tg_Ch"+chan+"_t1_means",outdir,"tri1 pcol");
  
  tg_ChX_tot_means->SetLineColor(2);
  tg_ChX_tot_means->SetLineWidth(4);
  tg_ChX_tot_means->SetMarkerColor(4);
  tg_ChX_tot_means->SetMarkerStyle(21);
  draw_and_save(tg_ChX_tot_means,"tg_Ch"+chan+"_tot_means",outdir,"tri1 pcol");
  
  tg_ChX_t1_std->SetLineColor(2);
  tg_ChX_t1_std->SetLineWidth(4);
  tg_ChX_t1_std->SetMarkerColor(4);
  tg_ChX_t1_std->SetMarkerStyle(21);
  draw_and_save(tg_ChX_t1_std,"tg_Ch"+chan+"_t1_std",outdir,"tri1 pcol");
  
  
  
  
//   _       _                        _       _       
//  (_)     | |                      | |     | |      
//   _ _ __ | |_ ___ _ __ _ __   ___ | | __ _| |_ ___ 
//  | | '_ \| __/ _ \ '__| '_ \ / _ \| |/ _` | __/ _ \
//  | | | | | ||  __/ |  | |_) | (_) | | (_| | ||  __/
//  |_|_| |_|\__\___|_|  | .__/ \___/|_|\__,_|\__\___|
//                       | |                          
//                       |_|                          
//   _____ _____                 _      _____ ______  
//  |_   _|  __ \               | |    / __  \|  _  \ 
//    | | | |  \/_ __ __ _ _ __ | |__  `' / /'| | | | 
//    | | | | __| '__/ _` | '_ \| '_ \   / /  | | | | 
//    | | | |_\ \ | | (_| | |_) | | | |./ /___| |/ /  
//    \_/  \____/_|  \__,_| .__/|_| |_|\_____/|___/   
//                        | |                         
//                        |_|                         
//   _          _____ _   _  _____ ______             
//  | |        |_   _| | | |/ __  \|  ___|            
//  | |_ ___     | | | |_| |`' / /'| |_               
//  | __/ _ \    | | |  _  |  / /  |  _|              
//  | || (_) |   | | | | | |./ /___| |                
//   \__\___/    \_/ \_| |_/\_____/\_|                
//                                                    
//                                               
  
  
  
  
  for (Int_t phibin = 1; phibin < interpol_phisteps+1; phibin++){
    for (Int_t rbin = 1; rbin < interpol_radsteps+1; rbin++){
      Double_t my_phi = t1_unpolar_interpol->GetXaxis()->GetBinCenter(phibin);
      Double_t my_r = t1_unpolar_interpol->GetYaxis()->GetBinCenter(rbin);
      
      Double_t interpol_t1 = -30;
      
      Double_t my_y = my_r * TMath::Cos(my_phi);
      Double_t my_z = my_r * TMath::Sin(my_phi);
      if(my_y <= interpol_max_y && my_y >= interpol_min_y){
        if(my_z <= interpol_max_z && my_z >= interpol_min_z){
          interpol_t1 = tg_ChX_t1_unpolar->Interpolate(my_phi,my_r);
        }
      }
      
      t1_unpolar_interpol->SetBinContent(phibin,rbin,interpol_t1);
      
    }
  }
  
  draw_and_save(t1_unpolar_interpol,"t1_unpolar_interpol",outdir,"lego2");
  
  
  
  
  
  
  
  
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
 
  
//   TCanvas *c01 = new TCanvas("c_1","c_1",200,10,600,400);
//   
//   tg_ChX_t1->Draw("pcol");
  
  f_out->cd();
  tg_ChX_t1->Write();
  tg_ChX_t1_unpolar->Write();
  tg_ChX_t1_means->Write();
  tg_ChX_t1_std->Write();
  tg_ChX_tot_means->Write();
  t1_unpolar_interpol->Write();
  
  
  f_out->Write();
  
}
