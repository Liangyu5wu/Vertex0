#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TF1.h"
#include "TPad.h"
#include "TLine.h"
#include "TLegend.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>

//root -l "plot_histograms.C(\"histograms.root\", \"EMB3\", 3, 1.5)"

void plot_histograms(const char* file_path = "histograms.root", 
                     const char* layer_name = "EMB3", 
                     double cutoff_factor = 3.0, 
                     double fit_range_factor = 1.5) {
    std::map<std::string, std::vector<std::string>> layers = {
        {"EMB1", {"EMB1_1-1.5", "EMB1_1.5-2", "EMB1_2-3", "EMB1_3-4", "EMB1_4-5", "EMB1_5-10", "EMB1_10-100"}},
        {"EMB2", {"EMB2_1-1.5", "EMB2_1.5-2", "EMB2_2-3", "EMB2_3-4", "EMB2_4-5", "EMB2_5-10", "EMB2_10-100"}},
        {"EMB3", {"EMB3_1-1.5", "EMB3_1.5-2", "EMB3_2-3", "EMB3_3-4", "EMB3_4-5", "EMB3_5-10", "EMB3_10-100"}},
        {"EME1", {"EME1_1-1.5", "EME1_1.5-2", "EME1_2-3", "EME1_3-4", "EME1_4-5", "EME1_5-10", "EME1_10-100"}},
        {"EME2", {"EME2_1-1.5", "EME2_1.5-2", "EME2_2-3", "EME2_3-4", "EME2_4-5", "EME2_5-10", "EME2_10-100"}},
        {"EME3", {"EME3_1-1.5", "EME3_1.5-2", "EME3_2-3", "EME3_3-4", "EME3_4-5", "EME3_5-10", "EME3_10-100"}}
    };
    
    std::string layer(layer_name);
    
    if (layers.find(layer) == layers.end()) {
        std::cerr << "Invalid layer name: " << layer << std::endl;
        return;
    }
    
    std::vector<std::string> hist_names = layers[layer];
    
    TFile* root_file = TFile::Open(file_path, "READ");
    if (!root_file || root_file->IsZombie()) {
        std::cerr << "Error: Could not open " << file_path << std::endl;
        return;
    }
    
    TCanvas* canvas = new TCanvas("canvas", 
                                 Form("%s Histograms (Cutoff: %.1f σ, Fit: ±%.1f σ)", 
                                      layer.c_str(), cutoff_factor, fit_range_factor), 
                                 1200, 600);
    canvas->Divide(4, 2);
    
    std::vector<TObject*> objects_to_keep;
    
    for (size_t i = 0; i < hist_names.size(); ++i) {
        TPad* pad = (TPad*)canvas->cd(i+1);
        pad->SetMargin(0.1, 0.1, 0.2, 0.1);
        
        TH1* hist = (TH1*)root_file->Get(hist_names[i].c_str());
        if (!hist) {
            std::cerr << "Histogram " << hist_names[i] << " not found in file!" << std::endl;
            continue;
        }
        
        TH1* hist_clone = (TH1*)hist->Clone();
        objects_to_keep.push_back(hist_clone);
        
        double hist_mean = hist_clone->GetMean();
        double hist_std = hist_clone->GetStdDev();

        double cutoff = hist_mean - cutoff_factor * hist_std;
        
        TH1* trimmed_hist = (TH1*)hist_clone->Clone();
        objects_to_keep.push_back(trimmed_hist);
        
        for (int bin = 1; bin <= trimmed_hist->GetNbinsX(); bin++) {
            double bin_center = trimmed_hist->GetBinCenter(bin);
            if (bin_center < cutoff) {
                trimmed_hist->SetBinContent(bin, 0);
            }
        }
        
        hist_clone->SetLineColor(kBlack);
        hist_clone->Draw();
        
        trimmed_hist->SetLineColor(kRed);
        trimmed_hist->Draw("SAME");
        
        TLine* cutLine = new TLine(cutoff, 0, cutoff, hist_clone->GetMaximum() * 0.8);
        objects_to_keep.push_back(cutLine);
        cutLine->SetLineColor(kRed);
        cutLine->SetLineStyle(2);
        cutLine->Draw();
        
        double trimmed_mean = trimmed_hist->GetMean();
        double trimmed_std = trimmed_hist->GetStdDev();
        
        double fit_min = trimmed_mean - fit_range_factor * trimmed_std;
        double fit_max = trimmed_mean + fit_range_factor * trimmed_std;
        
        trimmed_hist->Fit("gaus", "QS", "", fit_min, fit_max);
        
        TF1* fit_function = trimmed_hist->GetFunction("gaus");
        if (fit_function) {
            double mu = fit_function->GetParameter(1);
            double sigma = fit_function->GetParameter(2);
            
            fit_function->SetLineColor(kBlue);
            fit_function->SetLineWidth(2);
            
            TLatex* tex = new TLatex();
            objects_to_keep.push_back(tex);
            tex->SetNDC();
            tex->SetTextSize(0.05);
            tex->DrawLatex(0.15, 0.85, Form("#mu = %.2f", mu));
            tex->DrawLatex(0.15, 0.80, Form("#sigma = %.2f", sigma));
            tex->DrawLatex(0.15, 0.75, Form("cutoff = %.2f", cutoff));
            
            TLegend* legend = new TLegend(0.60, 0.75, 0.89, 0.89);
            objects_to_keep.push_back(legend);
            legend->AddEntry(hist_clone, "Original", "l");
            legend->AddEntry(trimmed_hist, "Trimmed", "l");
            legend->AddEntry(fit_function, "Gaussian Fit", "l");
            legend->SetBorderSize(0);
            legend->Draw();
        }
    }
    
    canvas->Update();
    canvas->SaveAs(Form("%s_Histograms_Cut%.1f_Fit%.1f.png", 
                        layer.c_str(), cutoff_factor, fit_range_factor));
    std::cout << "Histograms saved with cutoff factor = " << cutoff_factor 
              << " and fit range factor = " << fit_range_factor << std::endl;
    
    root_file->Close();
    delete canvas;
    
    for (auto obj : objects_to_keep) {
        delete obj;
    }
}
