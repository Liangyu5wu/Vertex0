#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "TFile.h"
#include "TH1.h"

int entries_counting() {
    TFile* file = TFile::Open("histograms_varied_ranges_noupbound.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::vector<std::string> layers = {"EMB", "EME"};
    std::vector<std::string> layer_nums = {"1", "2", "3"};
    std::vector<std::string> energy_ranges = {"1-1.5", "1.5-2", "2-3", "3-4", "4-5", "5-10", "Above-10"};
    std::vector<std::string> categories = {"matched_track_HS", "matched_track_PU", "other"};
    std::map<std::string, std::map<std::string, std::map<std::string, std::vector<double>>>> counts;
    std::map<std::string, std::map<std::string, std::vector<double>>> totals;

    for (const auto& layer : layers) {
        for (const auto& num : layer_nums) {
            for (const auto& range : energy_ranges) {
                std::vector<double> category_counts;
                for (const auto& cat : categories) {
                    std::string hist_name = layer + num + "_" + range + "_" + cat;
                    TH1* hist = dynamic_cast<TH1*>(file->Get(hist_name.c_str()));
                    double entries = hist ? hist->GetEntries() : 0.0;
                    counts[layer][num][cat].push_back(entries);
                    category_counts.push_back(entries);
                }
                double sum = 0.0;
                for (const auto& count : category_counts) {
                    sum += count;
                }
                totals[layer][num].push_back(sum);
            }
        }
    }

    for (const auto& layer : layers) {
        for (const auto& num : layer_nums) {
            std::cout << "Layer: " << layer << ", Layer number: " << num << std::endl;
            
            for (const auto& cat : categories) {
                std::cout << "  " << cat << ": [";
                for (const auto& entry : counts[layer][num][cat]) {
                    std::cout << entry << ", ";
                }
                std::cout << "]" << std::endl;
            }
            
            std::cout << "  Totals: [";
            for (const auto& entry : totals[layer][num]) {
                std::cout << entry << ", ";
            }
            std::cout << "]" << std::endl;
            std::cout << std::endl;
        }
    }

    file->Close();
    delete file;

    return 0;
}
