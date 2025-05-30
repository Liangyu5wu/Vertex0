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
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include "TROOT.h"

// root -l 
// .L plot_track_iterations.C
// create_fit_script("histograms_track_cali_dR0.025.root")
// .q
// ./run_all_fits.sh

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

std::string vectorToString(const std::string& name, const std::vector<double>& vec) {
    std::stringstream ss;
    ss << name << " = [";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i] << (i < vec.size() - 1 ? ", " : "");
    }
    ss << "]";
    return ss.str();
}

std::string vectorToString(const std::string& name, const std::vector<int>& vec) {
    std::stringstream ss;
    ss << name << " = [";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i] << (i < vec.size() - 1 ? ", " : "");
    }
    ss << "]";
    return ss.str();
}

void plot_inclusive_iterations(const char* file_path = "histograms.root", 
                     const char* layer_name = "EMB3",
                     double chi2_threshold = 1.0,
                     int max_iterations = 5000,
                     int fit_method = 2,
                     std::ofstream* output_file = nullptr) {
    gROOT->SetBatch(kTRUE);
    
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
        if (output_file && output_file->is_open()) {
            *output_file << "Invalid layer name: " << layer << std::endl;
        }
        return;
    }
    
    std::vector<std::string> hist_names = layers[layer];
    
    TFile* root_file = TFile::Open(file_path, "READ");
    if (!root_file || root_file->IsZombie()) {
        std::cerr << "Error: Could not open " << file_path << std::endl;
        if (output_file && output_file->is_open()) {
            *output_file << "Error: Could not open " << file_path << std::endl;
        }
        return;
    }
    
    std::vector<TCanvas*> canvases;
    std::vector<std::string> type_names = {"HS_Track", "PU_Track", "Other"};
    
    for (int i = 0; i < 3; i++) {
        TCanvas* canvas = new TCanvas(Form("canvas_%d", i), 
                                     Form("%s %s Histograms (Iterative Gaussian Fit)", 
                                         layer.c_str(), type_names[i].c_str()), 
                                     1600, 900);
        canvas->Divide(4, 2);
        canvases.push_back(canvas);
    }
    
    std::vector<std::vector<TObject*>> objects_to_keep(3);

    std::vector<double> mu_HS, mu_PU, mu_other;
    std::vector<double> sigma_HS, sigma_PU, sigma_other;
    std::vector<double> chi2_HS, chi2_PU, chi2_other;
    std::vector<int> iterations_HS, iterations_PU, iterations_other;
    
    std::vector<std::string> suffixes = {"_matched_track_HS", "_matched_track_PU", "_other"};
    
    for (size_t i = 0; i < hist_names.size(); ++i) {
        for (size_t suffix_idx = 0; suffix_idx < suffixes.size(); ++suffix_idx) {
            const std::string& suffix = suffixes[suffix_idx];
            std::string hist_name = hist_names[i] + suffix;
            
            int pad_idx = (i < 4) ? (i + 1) : (i + 5 - 4);
            canvases[suffix_idx]->cd(pad_idx);
            TPad* pad = (TPad*)canvases[suffix_idx]->GetPad(pad_idx);
            pad->SetMargin(0.12, 0.12, 0.15, 0.12);
            
            TH1* hist = (TH1*)root_file->Get(hist_name.c_str());
            if (!hist) {
                std::cerr << "Histogram " << hist_name << " not found in file!" << std::endl;
                if (output_file && output_file->is_open()) {
                    *output_file << "Histogram " << hist_name << " not found in file!" << std::endl;
                }
                
                if (suffix == "_matched_track_HS") {
                    mu_HS.push_back(std::numeric_limits<double>::quiet_NaN());
                    sigma_HS.push_back(std::numeric_limits<double>::quiet_NaN());
                    chi2_HS.push_back(std::numeric_limits<double>::quiet_NaN());
                    iterations_HS.push_back(-1);
                } else if (suffix == "_matched_track_PU") {
                    mu_PU.push_back(std::numeric_limits<double>::quiet_NaN());
                    sigma_PU.push_back(std::numeric_limits<double>::quiet_NaN());
                    chi2_PU.push_back(std::numeric_limits<double>::quiet_NaN());
                    iterations_PU.push_back(-1);
                } else if (suffix == "_other") {
                    mu_other.push_back(std::numeric_limits<double>::quiet_NaN());
                    sigma_other.push_back(std::numeric_limits<double>::quiet_NaN());
                    chi2_other.push_back(std::numeric_limits<double>::quiet_NaN());
                    iterations_other.push_back(-1);
                }
                
                continue;
            }
            
            TH1* hist_clone = (TH1*)hist->Clone();
            objects_to_keep[suffix_idx].push_back(hist_clone);
            
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
            objects_to_keep[suffix_idx].push_back(fit_function);
            
            if (fit_method == 1) {
                good_fit = iterativeFitMethod1(hist_clone, fit_function, mean, sigma, chi2_ndf, 
                                             iteration, fit_min, fit_max, chi2_threshold, max_iterations);
            } else {
                good_fit = iterativeFitMethod2(hist_clone, fit_function, mean, sigma, chi2_ndf, 
                                             iteration, fit_min, fit_max, chi2_threshold, max_iterations);
            }

            if (suffix == "_matched_track_HS") {
                mu_HS.push_back(mean);
                sigma_HS.push_back(sigma);
                chi2_HS.push_back(chi2_ndf);
                iterations_HS.push_back(iteration + 1);
            } else if (suffix == "_matched_track_PU") {
                mu_PU.push_back(mean);
                sigma_PU.push_back(sigma);
                chi2_PU.push_back(chi2_ndf);
                iterations_PU.push_back(iteration + 1);
            } else if (suffix == "_other") {
                mu_other.push_back(mean);
                sigma_other.push_back(sigma);
                chi2_other.push_back(chi2_ndf);
                iterations_other.push_back(iteration + 1);
            }
            
            fit_function->SetLineColor(kRed);
            fit_function->SetLineWidth(2);
            fit_function->Draw("same");
            
            TLine* line_min = new TLine(fit_min, 0, fit_min, hist_clone->GetMaximum() * 0.8);
            objects_to_keep[suffix_idx].push_back(line_min);
            line_min->SetLineColor(kRed);
            line_min->SetLineStyle(2);
            line_min->Draw();
            
            TLine* line_max = new TLine(fit_max, 0, fit_max, hist_clone->GetMaximum() * 0.8);
            objects_to_keep[suffix_idx].push_back(line_max);
            line_max->SetLineColor(kRed);
            line_max->SetLineStyle(2);
            line_max->Draw();
            
            TLatex* tex = new TLatex();
            objects_to_keep[suffix_idx].push_back(tex);
            tex->SetNDC();
            tex->SetTextSize(0.05);
            tex->DrawLatex(0.15, 0.78, Form("#mu = %.2f", mean));
            tex->DrawLatex(0.15, 0.73, Form("#sigma = %.2f", sigma));
            tex->DrawLatex(0.15, 0.68, Form("#chi^{2}/ndf = %.2f", chi2_ndf));
            tex->DrawLatex(0.15, 0.63, Form("iterations = %d", iteration + 1));
            
            TLegend* legend = new TLegend(0.15, 0.40, 0.49, 0.55);
            objects_to_keep[suffix_idx].push_back(legend);
            legend->AddEntry(hist_clone, "Histogram", "l");
            legend->AddEntry(fit_function, "Gaussian Fit", "l");
            legend->AddEntry(line_min, "Fit Range", "l");
            legend->SetBorderSize(0);
            legend->Draw();
            
            TLatex* hist_title = new TLatex();
            objects_to_keep[suffix_idx].push_back(hist_title);
            hist_title->SetNDC();
            hist_title->SetTextSize(0.06);
            hist_title->SetTextFont(42);
            
            std::string energy_bin = hist_names[i].substr(hist_names[i].find("_") + 1);
            hist_title->DrawLatex(0.15, 0.85, Form("%s", energy_bin.c_str()));
        }
    }
    
    for (int i = 0; i < 3; i++) {
        canvases[i]->Update();
        canvases[i]->SaveAs(Form("%s_Histograms_IterativeFit_%s_Method%d.png", layer.c_str(), type_names[i].c_str(), fit_method));
    }
    
    std::string output_message = "Histograms saved with iterative fitting (method = " + 
                                std::to_string(fit_method) + 
                                ", chi2 threshold = " + std::to_string(chi2_threshold) + 
                                ", max iterations = " + std::to_string(max_iterations) + ")";
    
    std::cout << output_message << std::endl;
    if (output_file && output_file->is_open()) {
        *output_file << output_message << std::endl;
    }

    std::string results_header = "\n=== Fit Results for " + layer + " ===\n";
    std::cout << results_header << std::endl;
    if (output_file && output_file->is_open()) {
        *output_file << results_header << std::endl;
    }
    
    std::cout << "--- HS Track Results ---" << std::endl;
    std::cout << vectorToString("mu_HS", mu_HS) << std::endl;
    std::cout << vectorToString("sigma_HS", sigma_HS) << std::endl;
    std::cout << vectorToString("chi2_HS", chi2_HS) << std::endl;
    
    std::cout << "\n--- PU Track Results ---" << std::endl;
    std::cout << vectorToString("mu_PU", mu_PU) << std::endl;
    std::cout << vectorToString("sigma_PU", sigma_PU) << std::endl;
    std::cout << vectorToString("chi2_PU", chi2_PU) << std::endl;
    
    std::cout << "\n--- Other Results ---" << std::endl;
    std::cout << vectorToString("mu_other", mu_other) << std::endl;
    std::cout << vectorToString("sigma_other", sigma_other) << std::endl;
    std::cout << vectorToString("chi2_other", chi2_other) << std::endl;
    
    if (output_file && output_file->is_open()) {
        *output_file << "--- HS Track Results ---" << std::endl;
        *output_file << vectorToString("mu_HS", mu_HS) << std::endl;
        *output_file << vectorToString("sigma_HS", sigma_HS) << std::endl;
        *output_file << vectorToString("chi2_HS", chi2_HS) << std::endl;
        *output_file << vectorToString("iterations_HS", iterations_HS) << std::endl;
        
        *output_file << "\n--- PU Track Results ---" << std::endl;
        *output_file << vectorToString("mu_PU", mu_PU) << std::endl;
        *output_file << vectorToString("sigma_PU", sigma_PU) << std::endl;
        *output_file << vectorToString("chi2_PU", chi2_PU) << std::endl;
        *output_file << vectorToString("iterations_PU", iterations_PU) << std::endl;
        
        *output_file << "\n--- Other Results ---" << std::endl;
        *output_file << vectorToString("mu_other", mu_other) << std::endl;
        *output_file << vectorToString("sigma_other", sigma_other) << std::endl;
        *output_file << vectorToString("chi2_other", chi2_other) << std::endl;
        *output_file << vectorToString("iterations_other", iterations_other) << std::endl;
    }
    
    root_file->Close();
    
    for (int i = 0; i < 3; i++) {
        for (auto obj : objects_to_keep[i]) {
            delete obj;
        }
        delete canvases[i];
    }
    
    objects_to_keep.clear();
    mu_HS.clear(); sigma_HS.clear(); chi2_HS.clear(); iterations_HS.clear();
    mu_PU.clear(); sigma_PU.clear(); chi2_PU.clear(); iterations_PU.clear();
    mu_other.clear(); sigma_other.clear(); chi2_other.clear(); iterations_other.clear();
}

void process_single_layer(const char* file_path, const char* layer_name, double chi2_threshold, int max_iterations, int fit_method, const char* output_file_path) {
    std::ofstream output_file(output_file_path, std::ios::app);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file_path << std::endl;
        return;
    }
    
    output_file << "---------- Processing " << layer_name << " ----------" << std::endl;
    std::cout << "Processing " << layer_name << "..." << std::endl;
    
    plot_inclusive_iterations(file_path, layer_name, chi2_threshold, max_iterations, fit_method, &output_file);
    
    output_file << std::endl;
    output_file.close();
    
    gSystem->Exit(0);
}

void fit_all_layers(const char* file_path = "histograms.root") {
    std::string file_str(file_path);
    size_t last_slash = file_str.find_last_of("/\\");
    size_t dot_pos = file_str.find_last_of(".");
    std::string file_name = file_str.substr(
        last_slash == std::string::npos ? 0 : last_slash + 1,
        dot_pos == std::string::npos ? file_str.length() : dot_pos - (last_slash == std::string::npos ? 0 : last_slash + 1)
    );
    
    std::string output_file_name = file_name + "_fit_results.txt";
    std::ofstream output_file(output_file_name);
    
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not create output file " << output_file_name << std::endl;
        return;
    }
    
    auto start_time = std::chrono::system_clock::now();
    std::time_t start_time_t = std::chrono::system_clock::to_time_t(start_time);
    output_file << "Fit started at: " << std::ctime(&start_time_t);
    output_file << "Input file: " << file_path << std::endl << std::endl;
    output_file.close();
    
    process_single_layer(file_path, "EMB1", 1.0, 5000, 1, output_file_name.c_str());
    
    std::cout << "First layer processed. To process remaining layers, run:" << std::endl;
    std::cout << "root -l -q 'process_single_layer(\"" << file_path << "\", \"EMB2\", 1.0, 5000, 1, \"" << output_file_name << "\")'" << std::endl;
    std::cout << "root -l -q 'process_single_layer(\"" << file_path << "\", \"EMB3\", 1.0, 5000, 1, \"" << output_file_name << "\")'" << std::endl;
    std::cout << "root -l -q 'process_single_layer(\"" << file_path << "\", \"EME1\", 1.5, 5000, 2, \"" << output_file_name << "\")'" << std::endl;
    std::cout << "root -l -q 'process_single_layer(\"" << file_path << "\", \"EME2\", 1.0, 5000, 1, \"" << output_file_name << "\")'" << std::endl;
    std::cout << "root -l -q 'process_single_layer(\"" << file_path << "\", \"EME3\", 1.0, 5000, 1, \"" << output_file_name << "\")'" << std::endl;
}

void create_fit_script(const char* file_path = "histograms.root") {
    std::string file_str(file_path);
    size_t last_slash = file_str.find_last_of("/\\");
    size_t dot_pos = file_str.find_last_of(".");
    std::string file_name = file_str.substr(
        last_slash == std::string::npos ? 0 : last_slash + 1,
        dot_pos == std::string::npos ? file_str.length() : dot_pos - (last_slash == std::string::npos ? 0 : last_slash + 1)
    );
    
    std::string output_file_name = file_name + "_fit_results.txt";
    std::string script_name = "run_all_fits.sh";
    
    std::ofstream script(script_name);
    if (!script.is_open()) {
        std::cerr << "Error: Could not create script file " << script_name << std::endl;
        return;
    }
    
    script << "#!/bin/bash" << std::endl;
    script << "echo \"Starting fit process for " << file_path << "\"" << std::endl;
    script << std::endl;
    
    script << "cat > " << output_file_name << " << EOL" << std::endl;
    script << "Fit started at: $(date)" << std::endl;
    script << "Input file: " << file_path << std::endl << std::endl;
    script << "EOL" << std::endl << std::endl;
    
    std::vector<std::pair<std::string, std::pair<double, int>>> layers = {
        {"EMB1", {1.0, 1}},
        {"EMB2", {1.0, 1}},
        {"EMB3", {1.0, 1}},
        {"EME1", {1.5, 2}},
        {"EME2", {1.0, 1}},
        {"EME3", {1.0, 1}}
    };
    
    for (const auto& layer : layers) {
        script << "echo \"Processing " << layer.first << "...\"" << std::endl;
        script << "echo \"---------- Processing " << layer.first << " ----------\" >> " << output_file_name << std::endl;
        script << "root -l -b -q 'plot_inclusive_iterations.C(\"" << file_path << "\", \"" 
               << layer.first << "\", " << layer.second.first << ", 5000, " << layer.second.second 
               << ")' | tee -a " << output_file_name << std::endl;
        script << "echo \"\" >> " << output_file_name << std::endl;
        script << std::endl;
    }
    
    script << "echo \"Fit completed at: $(date)\" >> " << output_file_name << std::endl;
    script << "echo \"All fits completed. Results saved to " << output_file_name << "\"" << std::endl;
    
    script.close();
    
    system(("chmod +x " + script_name).c_str());
    
    std::cout << "Created script " << script_name << " to process all layers." << std::endl;
    std::cout << "Run it with: ./" << script_name << std::endl;
}
