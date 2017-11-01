#
#include "Riostream.h"


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
//~ std::vector<float> file_to_float_array(TString fname) {
  
  //~ ifstream in;
  //~ in.open(fname);
  
  //~ std::vector<float> return_obj;
  
  //~ string line;
  
  //~ while (getline(in, line)) {
    //~ return_obj.push_back(atof(line));
  //~ }
  
  //~ in.close();
  //~ return return_obj;
//~ }
TH1F * get_histogram(TString fname ){
 
     TH1::AddDirectory(kFALSE);
 
    TFile *f = new TFile(fname);
     
      f->cd();
    	TString TDC = "0351";
    	TString chan = "07";
	 
       TH1F* h1 = ((TH1F*) f->Get("Histograms/Sec_"+TDC+"/Sec_"+TDC+"_Ch"+chan+"_t1"));  
     cout << fname << endl;
    f->Close();
    return h1;
}
TH1F * get_histogram_ascii(const char* fname){
	 in.open(fname); 
   Float_t x,y,z;
   Int_t nlines = 0;
   //~ TFile *f = new TFile("basic.root","RECREATE");
    nc->cd(ifile+1);
   TH1F *h1 = new TH1F("h1","drift time (ns)",200,-50,150); // for mdc ..
   //~ TNtuple *ntuple = new TNtuple("ntuple","data from ascii file","x:y:z");
	 cout << inputfile[ifile]<<endl;
   while (1) {
      in >> x ;
      if (!in.good()) break;
      if (nlines < 5) printf("x=%8f, y=%8f, z=%8f\n",x,y,z);
    
      h1->Fill(nlines*1.0-50.0, x);   //20 ns - 1000 bins --> 1 bin is 10 ps (0.02 ns)  
      
     
				
      //~ ntuple->Fill(x,y,z);
      nlines++;

   }
    return h1;
}


/*
void make_v_plot(const int nfiles,char* inputfile[],Float_t xposition[], const char* label,Int_t asciiOrroot ){
	 ifstream in;
Float_t mean[nfiles]; 
Float_t sigma[nfiles]; 
TCanvas * nc = new TCanvas(Form("nc-%s",label),"",1500,900);
nc->Divide(nfiles/3+1,3);
 
for(int ifile =0; ifile<nfiles; ifile++){ 
	TString TDC = "0351";
	int chan = 17;
	TH1F * h1;
	cout << inputfile[ifile] << endl;
	if(asciiOrroot) h1 = get_histogram_ascii(inputfile[ifile]);
	else h1 = get_histogram(TString(inputfile[ifile]), Form("Histograms/Sec_%s/Sec_%s_Ch%i_t1",TDC,TDC,chan));
  cout <<xposition[ifile]<< endl;

   // find peak position:
       TSpectrum *s = new TSpectrum();
		Int_t nfound = s->Search(h1,4,"",0.10);
			printf("Found %d candidate peaks to fit\n",nfound);
     //~ Double_t *xpeaks = s->GetPositionX();
     			//~ Int_t ihighestPeak(0);
			//~ Float_t highestPeak(0);
   //~ for (Int_t p=0;p<nfound;p++) {
      //~ Double_t xp = xpeaks[p];
      //~ Int_t bin = Int_t(xp );

      //~ Int_t bin = h1->GetXaxis()->FindBin(xp); 
      //~ Double_t yp = h1->GetBinContent(bin);
       //~ if(highestPeak<yp  ) ihighestPeak = p;
       //~ cout << xp<<endl;
	//~ } 
	         h1->Draw("hist");
	         h1->GetXaxis()->SetRangeUser(-20,80);
	h1->Fit("gaus","WW q","",5,55);
	float fit_mean =  h1->GetFunction("gaus")->GetParameter(1);
	float fit_sigma =  h1->GetFunction("gaus")->GetParameter(2);
	
	h1->Fit("gaus","WW q","",fit_mean-10,fit_mean+10);
	//~ h1->Fit("gaus","WW","",fit_mean-2*fit_sigma,fit_mean+2*fit_sigma);
	//~ h1->Fit("gaus","WW","",fitlow[ifile]+2,fithigh[ifile]-5);
      h1->DrawCopy(" p0 same");
   
	plotTopLegend(Form("x = %4.1f mm", xposition[ifile]), 0.1,1.01);
   in.close();

	 mean[ifile] = h1->GetFunction("gaus")->GetParameter(1);
	 sigma[ifile] = h1->GetFunction("gaus")->GetParameter(2);

   //~ f->Write();
	}
	nc->cd(nfiles+1);
	TGraph * gdrifttime = new TGraph(nfiles,xposition,mean);
	gdrifttime->Draw("a*");
	gdrifttime->Fit("pol1","ww q","",0,68.8);
	TF1 * linear1 = (TF1 *)gdrifttime->GetFunction("pol1")->Clone();
	Float_t vd1 = -1./(	gdrifttime->GetFunction("pol1")->GetParameter(1)/1000. ) ;
	gdrifttime ->SetTitle("drift times (ns) : x position (mm)");
	cout << " drift velocity left (mu/ns) = "<< vd1 << endl ;
	gdrifttime->Fit("pol1","ww q","",68.8,72);
		Float_t vd2 = 1./(	gdrifttime->GetFunction("pol1")->GetParameter(1)/1000. ) ;
	cout << " drift velocity right (mu/ns) = "<< vd2 << endl;
	linear1->Draw("same");
	
	plotTopLegend(Form("v_{drift} = %4.1f #mum/ns",vd1), 0.16,0.8);
	plotTopLegend(Form("v_{drift} = %4.1f #mum/ns",vd2), 0.5,0.7);
	nc->cd(nfiles+2);
	TGraph * gdrifttimeSigma = new TGraph(nfiles,xposition,sigma);
	gdrifttimeSigma->SetTitle("sigma of drift times (ns) : x position (mm)");
	gdrifttimeSigma->Draw("a*");
	 
}
*/
void make_v_plot_list(vector<TString> list,vector<TString> xlist , const char* label,Int_t asciiOrroot ){
 
 Float_t x_pos_anode_wire(69.2);
	const int nfiles=list.size();
Float_t mean[nfiles]; 
Float_t sigma[nfiles]; 
 TCanvas * nc = new TCanvas(Form("nc-%s",label),"",1500,900);
	nc->Divide(nfiles/3+1,3);
Float_t xposition[nfiles];
 for (Int_t ifile = 0; ifile< nfiles; ifile++){
	 xposition[ifile]= xlist[ifile].Atof();
    TString fname   = list[ifile];
	
	TH1F * h1;
	//~ if(asciiOrroot) h1 = get_histogram_ascii(inputfile[ifile]);
	if(asciiOrroot==-1)  h1 = get_histogram(fname );
  cout << xposition[ifile] << endl;
nc->cd(ifile+1);
   // find peak position:
       TSpectrum *s = new TSpectrum();
		Int_t nfound = s->Search(h1,4,"",0.10);
			printf("Found %d candidate peaks to fit\n",nfound);
     //~ Double_t *xpeaks = s->GetPositionX();
     			//~ Int_t ihighestPeak(0);
			//~ Float_t highestPeak(0);
   //~ for (Int_t p=0;p<nfound;p++) {
      //~ Double_t xp = xpeaks[p];
      //~ Int_t bin = Int_t(xp );

      //~ Int_t bin = h1->GetXaxis()->FindBin(xp); 
      //~ Double_t yp = h1->GetBinContent(bin);
       //~ if(highestPeak<yp  ) ihighestPeak = p;
       //~ cout << xp<<endl;
	//~ } 
	         h1->Draw("hist");
	         //~ h1->GetXaxis()->SetRangeUser(-20,80);
	h1->Fit("gaus","WW q","",-900,-600);
	float fit_mean =  h1->GetFunction("gaus")->GetParameter(1);
	float fit_sigma =  h1->GetFunction("gaus")->GetParameter(2);	
	h1->Fit("gaus","WW","",fit_mean-2*fit_sigma,fit_mean+2*fit_sigma);
	plotTopLegend(Form("x = %4.1f mm", xposition[ifile]), 0.1,1.01);
	
	
	//~ h1->Fit("gaus","WW","",fitlow[ifile]+2,fithigh[ifile]-5);
      h1->DrawCopy(" p0 same"); 
	
 

	 mean[ifile] = h1->GetFunction("gaus")->GetParameter(1);
	 sigma[ifile] = h1->GetFunction("gaus")->GetParameter(2);
 h1->GetXaxis()->SetRangeUser(fit_mean-10*fit_sigma,fit_mean+10*fit_sigma);
	}
	
	/// Draw results: 
		TCanvas * cresults = new TCanvas(Form("results-%s",label),"",900,800);
		cresults->Divide(2,1);
	cresults->cd(1);
	TGraph * gdrifttime = new TGraph(nfiles,xposition,mean);
	gdrifttime->Draw("a*");
	gdrifttime->Fit("pol1","ww q","",0,x_pos_anode_wire);
	TF1 * linear1 = (TF1 *)gdrifttime->GetFunction("pol1")->Clone();
	Float_t vd1 = -1./(	gdrifttime->GetFunction("pol1")->GetParameter(1)/1000. ) ;
	gdrifttime ->SetTitle("drift times (ns) : x position (mm)");
	cout << " drift velocity left (mu/ns) = "<< vd1 << endl ;
	gdrifttime->Fit("pol1","ww q","",x_pos_anode_wire,72);
		Float_t vd2 = 1./(	gdrifttime->GetFunction("pol1")->GetParameter(1)/1000. ) ;
	cout << " drift velocity right (mu/ns) = "<< vd2 << endl;
	linear1->Draw("same");
	
	plotTopLegend(Form("v_{drift} = %4.1f #mum/ns",vd1), 0.16,0.8);
	plotTopLegend(Form("v_{drift} = %4.1f #mum/ns",vd2), 0.5,0.7);
	cresults->cd(2);
	TGraph * gdrifttimeSigma = new TGraph(nfiles,xposition,sigma);
	gdrifttimeSigma->SetTitle("sigma of drift times (ns) : x position (mm)");
	gdrifttimeSigma->Draw("a*");
	 
}
void v_plot_scope_data(){
 gStyle->SetOptStat(0);
  
 ///PASTREC:
 {
		   const int nfiles1(10);
		  char* inputfile1[nfiles1]={
		"pasttrec-v-scan/Measurement_2017-10-30_0_031233_PT_670.Wfm.csv" ,
		"pasttrec-v-scan/Measurement_2017-10-30_1_031831_pt_675.Wfm.csv"  ,
		"pasttrec-v-scan/Measurement_2017-10-30_2_032435_pt_680.Wfm.csv"  ,
		"pasttrec-v-scan/Measurement_2017-10-30_10_040910_pt_683.Wfm.csv", 
		"pasttrec-v-scan/Measurement_2017-10-30_3_032933_pt_685.Wfm.csv"  ,
		"pasttrec-v-scan/Measurement_2017-10-30_9_040356_pt_687.Wfm.csv"  ,
		"pasttrec-v-scan/Measurement_2017-10-30_4_033603_pt_690.Wfm.csv"  ,
		"pasttrec-v-scan/Measurement_2017-10-30_5_034120_pt_695.Wfm.csv",
		"pasttrec-v-scan/Measurement_2017-10-30_6_034613_pt_700.Wfm.csv",
		"pasttrec-v-scan/Measurement_2017-10-30_8_035806_pt_710.Wfm.csv"   };  
		 //~ /// "Measurement_2017-10-30_7_035214_pt_750.Wfm.csv"  , };

		//~ Float_t fitlow[nfiles1] = 	{30,20,10,10, 7,7,7,12, 18,35   };
		//~ Float_t fithigh[nfiles1] = 	{50,40,40,35, 35,30,30,35, 40,55   };
		 Float_t xposition[nfiles1] = 	{ 67.0,67.5,68.0,68.3,68.5,68.7,69.0,69.5,70.0,71.0  };
	//~ make_v_plot( nfiles1,inputfile1,xposition,"PASTTREC");
	}

///ASD8:
{
  const int nfiles2(18);
  char* inputfile2[nfiles2];
  int ifile=0;
 Float_t xposition2[nfiles2];
 
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_10_013434_67_3.Wfm.csv" ; xposition2[ifile] = 67.3; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_11_014033_67_1.Wfm.csv" ; xposition2[ifile] = 67.1; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_12_014829_66_9.Wfm.csv" ; xposition2[ifile] = 66.9; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_13_015733_66_7.Wfm.csv" ; xposition2[ifile] =  66.7; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_14_021219_69_9.Wfm.csv" ; xposition2[ifile] =  66.9;  ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_15_021633_70_1.Wfm.csv" ; xposition2[ifile] =  70.1; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_16_022418_70_5.Wfm.csv" ; xposition2[ifile] =  70.5;   ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_17_023053_70_7.Wfm.csv" ; xposition2[ifile] =  70.7;  ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_18_025042_70_9.Wfm.csv" ; xposition2[ifile] =  70.9; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_19_020119_69_0.Wfm.csv" ; xposition2[ifile] =  69.0;  ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_20_020656_68_7.Wfm.csv" ; xposition2[ifile] =  68.7; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_3_004320_69_9.Wfm.csv"  ; xposition2[ifile] =  69.9; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_4_004917_69_7.Wfm.csv"  ; xposition2[ifile] =  69.7; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_5_005537_69_3.Wfm.csv"  ; xposition2[ifile] =  69.3;  ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_6_010749_68_9.Wfm.csv"  ; xposition2[ifile] =  68.9; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_7_011428_68_5.Wfm.csv"  ; xposition2[ifile] =  68.5,; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_8_012228_68_1.Wfm.csv"  ; xposition2[ifile] =  68.1; ifile++;
  inputfile2[ifile]="asd8-v-scan/Measurement_2017-10-29_9_012850_67_7.Wfm.csv"  ; xposition2[ifile] =  67.7;  ifile++;

//~ Float_t fitlow[nfiles] = 		{30,20,10,10, 7,7,7,12, 18,35 ,10, 7,7,7,12, 18,35 ,25  };
//~ Float_t fithigh[nfiles] = 		{50,40,40,35, 35,30,30,35, 40,55 , 35, 35,30,30,35, 40,55 ,55 };
//~ Float_t xposition[nfiles] = 	{ 67.3,67.1,66.9,66.7,66.9,70.1,70.5,70.7,70.9,69.0,68.7,69.9,69.7,69.3,68.9,68.5,68.1,67.7};
//~ Float_t xposition[nfiles] = 	{ 67.3,67.1,66.9,66.7,66.9,70.1,70.5,70.7,70.9,69.0,68.7,69.9,69.7,69.3,68.9,68.5,68.1,67.7};
//~ make_v_plot( nfiles2,inputfile2,xposition2,"ASD8");


}

///PASTTEC TDC:
	{
	  //~ ifstream in, in2;
	  
	  //~ int ifile=0;
	   
	 //~ Float_t xposition2[]; //xlist.txt 
	 //~ Float_t line;
	 //~ in.open("xlist.txt"); 
	 //~ int numfiles(0);
	 //~ while(1){
		  //~ in >> line;
		  		   //~ xposition2[ifile] =  line  ;
		   //~ if (!in.good()) break;
			  //~ numfiles++;
			  //~ cout <<xposition2[ifile] << endl;
			//~ }
	  //~ const int nfiles2(numfiles);
	 //~ char* inputfile2[];
	 //~ in2.open("root_files.txt"); 
	 //~ while(1){
		   //~ in2 >> inputfile2[ifile] ;
		   //~ if (!in2.good()) break;
			  //~ cout << inputfile2[ifile] << endl;
			//~ }
	  
	   std::vector<TString> list = file_to_str_array("root_files.txt");
       std::vector<TString> xlist = file_to_str_array("xlist.txt"); 
   
	 //~ make_v_plot( nfiles2,inputfile2,xposition2,"PASTTREC",0);
	 make_v_plot_list( list,xlist ,"PASTTREC",-1);


	}

}
