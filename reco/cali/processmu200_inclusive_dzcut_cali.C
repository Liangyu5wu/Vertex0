#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

const double c_light = 0.299792458; // mm/ps

TH1F *embHist[3][7];
TH1F *emeHist[3][7];

void initialize_histograms() {
    const char* emb_layers[3] = {"EMB1", "EMB2", "EMB3"};
    const char* eme_layers[3] = {"EME1", "EME2", "EME3"};
    const char* energy_bins[7] = {"1-1.5", "1.5-2", "2-3", "3-4", "4-5", "5-10", "Above-10"};
    
    float hist_ranges[7][2] = {
        {-5000, 5000},  // 1-1.5
        {-4000, 4000},  // 1.5-2
        {-3000, 3000},  // 2-3
        {-3000, 3000},  // 3-4
        {-2000, 2000},  // 4-5
        {-2000, 2000},  // 5-10
        {-3000, 3000}   // 10-100
    };
    
    float bin_width = 10.0; 

    for (int layer = 0; layer < 3; ++layer) {
        for (int bin = 0; bin < 7; ++bin) {

            int nbins = static_cast<int>((hist_ranges[bin][1] - hist_ranges[bin][0]) / bin_width);

            std::string emb_name = std::string(emb_layers[layer]) + "_" + energy_bins[bin];
            std::string eme_name = std::string(eme_layers[layer]) + "_" + energy_bins[bin];
            
            embHist[layer][bin] = new TH1F(emb_name.c_str(), 
                                         (emb_name + " Corrected Time").c_str(), 
                                         nbins, 
                                         hist_ranges[bin][0], 
                                         hist_ranges[bin][1]); 
            
            emeHist[layer][bin] = new TH1F(eme_name.c_str(), 
                                         (eme_name + " Corrected Time").c_str(), 
                                         nbins, 
                                         hist_ranges[bin][0], 
                                         hist_ranges[bin][1]);
        }
    }
}

void process_file(const std::string &filename) {
    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    TTree *tree = nullptr;
    file->GetObject("ntuple", tree);
    if (!tree) {
        std::cerr << "Error getting TTree 'ntuple' from file: " << filename << std::endl;
        file->Close();
        return;
    }

    std::vector<float> *truthVtxTime = nullptr;
    std::vector<float> *truthVtxX = nullptr;
    std::vector<float> *truthVtxY = nullptr;
    std::vector<float> *truthVtxZ = nullptr;
    std::vector<bool> *truthVtxIsHS = nullptr;
    std::vector<float> *RecoVtxX = nullptr;
    std::vector<float> *RecoVtxY = nullptr;
    std::vector<float> *RecoVtxZ = nullptr;
    std::vector<float> *cellTime = nullptr;
    std::vector<float> *cellE = nullptr;
    std::vector<float> *cellX = nullptr;
    std::vector<float> *cellY = nullptr;
    std::vector<float> *cellZ = nullptr;
    std::vector<bool> *cellIsEMBarrel = nullptr;
    std::vector<bool> *cellIsEMEndCap = nullptr;
    std::vector<int> *cellLayer = nullptr;
    std::vector<float> *cellSignificance = nullptr;

    tree->SetBranchAddress("TruthVtx_time", &truthVtxTime);
    tree->SetBranchAddress("TruthVtx_x", &truthVtxX);
    tree->SetBranchAddress("TruthVtx_y", &truthVtxY);
    tree->SetBranchAddress("TruthVtx_z", &truthVtxZ);
    tree->SetBranchAddress("TruthVtx_isHS", &truthVtxIsHS);
    tree->SetBranchAddress("RecoVtx_x", &RecoVtxX);
    tree->SetBranchAddress("RecoVtx_y", &RecoVtxY);
    tree->SetBranchAddress("RecoVtx_z", &RecoVtxZ);
    tree->SetBranchAddress("Cell_time", &cellTime);
    tree->SetBranchAddress("Cell_e", &cellE);
    tree->SetBranchAddress("Cell_x", &cellX);
    tree->SetBranchAddress("Cell_y", &cellY);
    tree->SetBranchAddress("Cell_z", &cellZ);
    tree->SetBranchAddress("Cell_isEM_Barrel", &cellIsEMBarrel);
    tree->SetBranchAddress("Cell_isEM_EndCap", &cellIsEMEndCap);
    tree->SetBranchAddress("Cell_layer", &cellLayer);
    tree->SetBranchAddress("Cell_significance", &cellSignificance);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        for (size_t i = 0; i < truthVtxTime->size(); ++i) {
            if (!truthVtxIsHS->at(i)) continue;

            float vtx_time = truthVtxTime->at(i);
            float vtx_x = truthVtxX->at(i);
            float vtx_y = truthVtxY->at(i);
            float vtx_z = truthVtxZ->at(i);
            float reco_vtx_x = RecoVtxX->at(i);
            float reco_vtx_y = RecoVtxY->at(i);
            float reco_vtx_z = RecoVtxZ->at(i);

            float reco_dz_distance = std::sqrt((vtx_x - reco_vtx_x)*(vtx_x - reco_vtx_x)
                                             + (vtx_y - reco_vtx_y)*(vtx_y - reco_vtx_y)
                                             + (vtx_z - reco_vtx_z)*(vtx_z - reco_vtx_z));
            if (reco_dz_distance > 2) continue;

            for (size_t j = 0; j < cellE->size(); ++j) {
                if (cellE->at(j) < 1.0) continue;
                if (cellSignificance->at(j) < 4.0) continue;

                float cell_time = cellTime->at(j);
                float cell_x = cellX->at(j);
                float cell_y = cellY->at(j);
                float cell_z = cellZ->at(j);

                float distance_to_origin = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z);
                float distance_vtx_to_cell = std::sqrt((cell_x - vtx_x)*(cell_x - vtx_x)
                                                     + (cell_y - vtx_y)*(cell_y - vtx_y)
                                                     + (cell_z - vtx_z)*(cell_z - vtx_z));
                float corrected_time = cell_time
                                     + distance_to_origin / c_light
                                     - distance_vtx_to_cell / c_light
                                     - vtx_time;

                bool is_barrel = cellIsEMBarrel->at(j);
                bool is_endcap = cellIsEMEndCap->at(j);
                int layer = cellLayer->at(j);
                float energy = cellE->at(j);

                int bin = -1;
                if (energy > 1 && energy <= 1.5) bin = 0;
                else if (energy > 1.5 && energy <= 2) bin = 1;
                else if (energy > 2 && energy <= 3) bin = 2;
                else if (energy > 3 && energy <= 4) bin = 3;
                else if (energy > 4 && energy <= 5) bin = 4;
                else if (energy > 5 && energy <= 10) bin = 5;
                else if (energy > 10) bin = 6;

                if (bin != -1) {
                    if (is_barrel) {
                        if (layer == 1) embHist[0][bin]->Fill(corrected_time);
                        else if (layer == 2) embHist[1][bin]->Fill(corrected_time);
                        else if (layer == 3) embHist[2][bin]->Fill(corrected_time);
                    } else if (is_endcap) {
                        if (layer == 1) emeHist[0][bin]->Fill(corrected_time);
                        else if (layer == 2) emeHist[1][bin]->Fill(corrected_time);
                        else if (layer == 3) emeHist[2][bin]->Fill(corrected_time);
                    }
                }
            }
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void processmu200_cali(int startIndex = 1, int endIndex = 46) {
    initialize_histograms();

    const std::string path = "./SuperNtuple_mu200";
    for (int i = startIndex; i <= endIndex; ++i) {
        std::ostringstream filename;
        filename << path << "/user.scheong.43348828.Output._" 
                 << std::setw(6) << std::setfill('0') << i 
                 << ".SuperNtuple.root";

        if (std::filesystem::exists(filename.str())) {
            process_file(filename.str());
        } else {
            std::cerr << "File does not exist: " << filename.str() << std::endl;
        }
    }

    TFile *outputFile = new TFile("histograms_inclusive_cali.root", "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    for (int layer = 0; layer < 3; ++layer) {
        for (int bin = 0; bin < 7; ++bin) {
            embHist[layer][bin]->Write();
            emeHist[layer][bin]->Write();
        }
    }

    outputFile->Close();
    delete outputFile;

    for (int layer = 0; layer < 3; ++layer) {
        for (int bin = 0; bin < 7; ++bin) {
            delete embHist[layer][bin];
            delete emeHist[layer][bin];
        }
    }
    
}
