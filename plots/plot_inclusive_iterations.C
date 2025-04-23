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

//root -l -q "plot_inclusive_iterations.C(\"histograms_inclusive_cali.root\", \"EMB3\", 1.0, 5000, 1)"
//root -l -q "plot_inclusive_iterations.C(\"histograms_inclusive_cali.root\", \"EME1\", 1.7, 5000, 2)"

bool iterativeFitMethod1(TH1* hist, TF1* fit_function, double& mean, double& sigma, 
                         double& chi2_ndf, int& iteration, double& fit_min, 
                         double& fit_max, double chi2_threshold, int max_iterations) {
    bool good_fit = false;
    while (!good_fit && iteration < max_iterations) {
        hist->Fit(fit_function, "QS", "", fit_min, fit_max);
        
        mean = fit_function->GetParameter(1);
        sigma = fit_function->GetParameter(2);
        chi2_ndf = fit_function->GetChisquare() / fit_function->GetNDF();
        
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
    return good_fit;
}

bool iterativeFitMethod2(TH1* hist, TF1* fit_function, double& mean, double& sigma, 
                         double& chi2_ndf, int& iteration, double& fit_min, 
                         double& fit_max, double chi2_threshold, int max_iterations) {
    bool good_fit = false;
    while (!good_fit && iteration < max_iterations) {
        hist->Fit(fit_function, "QS", "", fit_min, fit_max);
        
        mean = fit_function->GetParameter(1);
        sigma = fit_function->GetParameter(2);
        chi2_ndf = fit_function->GetChisquare() / fit_function->GetNDF();
        
        if (chi2_ndf < chi2_threshold) {
            good_fit = true;
        } else {
            fit_min = mean - 2.0 * sigma;
            fit_max = mean + 2.0 * sigma;
            if (iteration > 1000) {
                fit_min = mean - 1.7 * sigma;
                fit_max = mean + 1.7 * sigma;
            }
            if (iteration > 2000) {
                fit_min = mean - 1.5 * sigma;
                fit_max = mean + 1.5 * sigma;
            }
            if (iteration > 3000) {
                fit_min = mean - 1.0 * sigma;
                fit_max = mean + 1.0 * sigma;
            }
            if (iteration > 4000) {
                fit_min = mean - 0.7 * sigma;
                fit_max = mean + 0.7 * sigma;
            }
            iteration++;
        }
    }
    return good_fit;
}

void printVectorAsArray(const std::string& name, const std::vector<double>& vec) {
    std::cout << name << " = [";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << (i < vec.size() - 1 ? ", " : "");
    }
    std::cout << "]" << std::endl;
}

void plot_inclusive_iterations(const char* file_path = "histograms.root", 
                     const char* layer_name = "EMB3",
                     double chi2_threshold = 1.0,
                     int max_iterations = 5000,
                     int fit_method = 2) {
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
    
    TCanvas* canvas = new TCanvas("canvas", 
                                 Form("%s Histograms (Iterative Gaussian Fit)", layer.c_str()), 
                                 1200, 600);
    canvas->Divide(4, 2);
    
    std::vector<TObject*> objects_to_keep;

    std::vector<double> means;
    std::vector<double> sigmas;
    std::vector<double> chi2_values;
    std::vector<int> iterations;
    
    for (size_t i = 0; i < hist_names.size(); ++i) {
        TPad* pad = (TPad*)canvas->cd(i+1);
        pad->SetMargin(0.1, 0.1, 0.2, 0.1);
        
        TH1* hist = (TH1*)root_file->Get(hist_names[i].c_str());
        if (!hist) {
            std::cerr << "Histogram " << hist_names[i] << " not found in file!" << std::endl;
            means.push_back(std::numeric_limits<double>::quiet_NaN());
            sigmas.push_back(std::numeric_limits<double>::quiet_NaN());
            chi2_values.push_back(std::numeric_limits<double>::quiet_NaN());
            iterations.push_back(-1);
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
        
        TF1* fit_function = new TF1(Form("gaus_fit_%zu", i), "gaus", fit_min, fit_max);
        objects_to_keep.push_back(fit_function);
        
        if (fit_method == 1) {
            good_fit = iterativeFitMethod1(hist_clone, fit_function, mean, sigma, chi2_ndf, 
                                         iteration, fit_min, fit_max, chi2_threshold, max_iterations);
        } else {
            good_fit = iterativeFitMethod2(hist_clone, fit_function, mean, sigma, chi2_ndf, 
                                         iteration, fit_min, fit_max, chi2_threshold, max_iterations);
        }

        means.push_back(mean);
        sigmas.push_back(sigma);
        chi2_values.push_back(chi2_ndf);
        iterations.push_back(iteration + 1);
        
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
    
    canvas->Update();
    canvas->SaveAs(Form("%s_Histograms_IterativeFit_Method%d.png", layer.c_str(), fit_method));
    std::cout << "Histograms saved with iterative fitting (method = " << fit_method 
              << ", chi2 threshold = " << chi2_threshold 
              << ", max iterations = " << max_iterations << ")" << std::endl;

    std::cout << "\n=== Fit Results for " << layer << " ===\n" << std::endl;
    printVectorAsArray("means", means);
    printVectorAsArray("sigmas", sigmas);
    printVectorAsArray("chi2_values", chi2_values);
    
    root_file->Close();
    delete canvas;
    
    for (auto obj : objects_to_keep) {
        delete obj;
    }
}
