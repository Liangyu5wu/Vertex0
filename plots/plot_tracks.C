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

//root -l -q "plot_tracks.C(\"histograms_varied_ranges_noupbound.root\", \"EMB3\")"

void plot_tracks(const char* file_path = "histograms.root", 
                const char* layer_name = "EMB3",
                double chi2_threshold = 1.0,
                int max_iterations = 5000) {
    std::map<std::string, std::vector<std::string>> layers = {
        {"EMB1", {"EMB1_1-1.5", "EMB1_1.5-2", "EMB1_2-3", "EMB1_3-4", "EMB1_4-5", "EMB1_5-10", "EMB1_Above-10"}},
        {"EMB2", {"EMB2_1-1.5", "EMB2_1.5-2", "EMB2_2-3", "EMB2_3-4", "EMB2_4-5", "EMB2_5-10", "EMB2_Above-10"}},
        {"EMB3", {"EMB3_1-1.5", "EMB3_1.5-2", "EMB3_2-3", "EMB3_3-4", "EMB3_4-5", "EMB3_5-10", "EMB3_Above-10"}},
        {"EME1", {"EME1_1-1.5", "EME1_1.5-2", "EME1_2-3", "EME1_3-4", "EME1_4-5", "EME1_5-10", "EME1_Above-10"}},
        {"EME2", {"EME2_1-1.5", "EME2_1.5-2", "EME2_2-3", "EME2_3-4", "EME2_4-5", "EME2_5-10", "EME2_Above-10"}},
        {"EME3", {"EME3_1-1.5", "EME3_1.5-2", "EME3_2-3", "EME3_3-4", "EME3_4-5", "EME3_5-10", "EME3_Above-10"}}
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
    
    TCanvas* canvas = new TCanvas("canvas", Form("%s Histograms (Iterative Gaussian Fit)", layer.c_str()), 1800, 2400);
    canvas->Divide(3, 7);
    
    std::vector<TObject*> objects_to_keep;

    std::vector<double> mu_HS, mu_PU, mu_other;
    std::vector<double> sigma_HS, sigma_PU, sigma_other;
    
    for (size_t i = 0; i < hist_names.size(); ++i) {
        for (const std::string& suffix : {"_matched_track_HS", "_matched_track_PU", "_other"}) {
            std::string hist_name = hist_names[i] + suffix;
            
            TPad* pad = (TPad*)canvas->cd(i * 3 + int((suffix=="_matched_track_HS") * 0 + (suffix=="_matched_track_PU") * 1 + (suffix=="_other") * 2) + 1);
            pad->SetMargin(0.1, 0.1, 0.2, 0.1);
            
            TH1* hist = (TH1*)root_file->Get(hist_name.c_str());
            if (!hist) {
                std::cerr << "Histogram " << hist_name << " not found in file!" << std::endl;
                continue;
            }
            
            TH1* hist_clone = (TH1*)hist->Clone();
            objects_to_keep.push_back(hist_clone);
            
            hist_clone->SetLineColor(kBlack);
            hist_clone->Draw();
            
            double mean = hist_clone->GetMean();
            double sigma = hist_clone->GetStdDev();
            
            double fit_min = hist_clone->GetXaxis()->GetXmin();
            double fit_max = hist_clone->GetXaxis()->GetXmax();
            double chi2_ndf = 999;
            int iteration = 0;
            bool good_fit = false;
            
            TF1* fit_function = new TF1(Form("gaus_fit_%s", hist_name.c_str()), "gaus", fit_min, fit_max);
            objects_to_keep.push_back(fit_function);
            
            std::vector<double> iteration_means;
            std::vector<double> iteration_sigmas;
            std::vector<double> iteration_chi2;
            
            while (!good_fit && iteration < max_iterations) {
                hist_clone->Fit(fit_function, "QS", "", fit_min, fit_max);
                
                mean = fit_function->GetParameter(1);
                sigma = fit_function->GetParameter(2);
                chi2_ndf = fit_function->GetChisquare() / fit_function->GetNDF();
                
                iteration_means.push_back(mean);
                iteration_sigmas.push_back(sigma);
                iteration_chi2.push_back(chi2_ndf);
                
                if (chi2_ndf < chi2_threshold) {
                    good_fit = true;
                } else {
                    fit_min = mean - 2.0 * sigma;
                    fit_max = mean + 2.0 * sigma;
                    if (iteration > 1000) {
                        fit_min = mean - 1.7 * sigma;
                        fit_max = mean + 1.7 * sigma;
                    }
                    if (iteration > 3000) {
                        fit_min = mean - 1.5 * sigma;
                        fit_max = mean + 1.5 * sigma;
                    }
                    iteration++;
                }
            }

            if (suffix == "_matched_track_HS") {
                mu_HS.push_back(mean);
                sigma_HS.push_back(sigma);
            } else if (suffix == "_matched_track_PU") {
                mu_PU.push_back(mean);
                sigma_PU.push_back(sigma);
            } else if (suffix == "_other") {
                mu_other.push_back(mean);
                sigma_other.push_back(sigma);
            }
            
            fit_function->SetLineColor(kRed);
            fit_function->SetLineWidth(2);
            
            TLine* line_min = new TLine(fit_min, 0, fit_min, hist_clone->GetMaximum() * 0.8);
            objects_to_keep.push_back(line_min);
            line_min->SetLineColor(kRed);
            line_min->SetLineStyle(2);
            line_min->Draw();
            
            TLine* line_max = new TLine(fit_max, 0, fit_max, hist_clone->GetMaximum() * 0.8);
            objects_to_keep.push_back(line_max);
            line_max->SetLineColor(kRed);
            line_max->SetLineStyle(2);
            line_max->Draw();
            
            TLatex* tex = new TLatex();
            objects_to_keep.push_back(tex);
            tex->SetNDC();
            tex->SetTextSize(0.05);
            tex->DrawLatex(0.15, 0.78, Form("#mu = %.2f", mean));
            tex->DrawLatex(0.15, 0.73, Form("#sigma = %.2f", sigma));
            tex->DrawLatex(0.15, 0.68, Form("#chi^{2}/ndf = %.2f", chi2_ndf));
            tex->DrawLatex(0.15, 0.63, Form("iterations = %d", iteration + 1));
            
            TLegend* legend = new TLegend(0.20, 0.35, 0.49, 0.49);
            objects_to_keep.push_back(legend);
            legend->AddEntry(hist_clone, "Histogram", "l");
            legend->AddEntry(fit_function, "Gaussian Fit", "l");
            legend->AddEntry(line_min, "Fit Range", "l");
            legend->SetBorderSize(0);
            legend->Draw();
            
            TLatex* hist_title = new TLatex();
            objects_to_keep.push_back(hist_title);
            hist_title->SetNDC();
            hist_title->SetTextSize(0.06);
            hist_title->SetTextFont(42);
        }
    }
    
    canvas->Update();
    canvas->SaveAs(Form("%s_Histograms_IterativeFit.png", layer.c_str()));
    std::cout << "Histograms saved with iterative fitting (chi2 threshold = " 
              << chi2_threshold << ", max iterations = " << max_iterations << ")" << std::endl;
    
    root_file->Close();
    delete canvas;

        std::cout << "mu_HS = [";
    for (size_t i = 0; i < mu_HS.size(); ++i) {
        std::cout << mu_HS[i] << (i < mu_HS.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    
    std::cout << "mu_PU = [";
    for (size_t i = 0; i < mu_PU.size(); ++i) {
        std::cout << mu_PU[i] << (i < mu_PU.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    
    std::cout << "mu_other = [";
    for (size_t i = 0; i < mu_other.size(); ++i) {
        std::cout << mu_other[i] << (i < mu_other.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;

    std::cout << "sigma_HS = [";
    for (size_t i = 0; i < sigma_HS.size(); ++i) {
        std::cout << sigma_HS[i] << (i < sigma_HS.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    
    std::cout << "sigma_PU = [";
    for (size_t i = 0; i < sigma_PU.size(); ++i) {
        std::cout << sigma_PU[i] << (i < sigma_PU.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    
    std::cout << "sigma_other = [";
    for (size_t i = 0; i < sigma_other.size(); ++i) {
        std::cout << sigma_other[i] << (i < sigma_other.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
    
    
    for (auto obj : objects_to_keep) {
        delete obj;
    }

}
