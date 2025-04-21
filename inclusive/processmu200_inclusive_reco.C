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

const double c_light = 299792458;

const float emb_x[7] = {1.25, 1.75, 2.5, 3.5, 4.5, 7.5, 55};

const float emb1_y[7] = {45.58, 38.24, 30.64, 24.90, 23.00, 16.19, 12.12};
const float emb1_ysigma[7] = {443.97, 318.89, 237.11, 181.55, 150.30, 142.56, 103.04};

const float emb2_y[7] = {-37.66, -5.42, 17.14, 27.26, 24.13, 26.50, 19.37};
const float emb2_ysigma[7] = {2169.56, 1507.85, 1061.49, 741.75, 569.08, 371.64, 177.16};

const float emb3_y[7] = {75.19, 91.66, 66.45, 59.93, 59.95, 44.59, 40.15};
const float emb3_ysigma[7] = {1335.87, 951.72, 720.61, 499.29, 442.37, 298.39, 221.61};

const float eme1_y[7] = {111.43, 92.91, 78.46, 60.97, 47.43, 31.16, 15.49};
const float eme1_ysigma[7] = {1069.59, 770.29, 579.83, 439.67, 354.02, 303.42, 333.59};

const float eme2_y[7] = {141.03, 136.79, 117.32, 100.06, 82.39, 67.07, 33.83};
const float eme2_ysigma[7] = {1754.53, 1289.32, 935.65, 681.08, 543.53, 383.86, 219.94};

const float eme3_y[7] = {122.03, 88.96, 73.61, 63.57, 46.46, 48.39, 32.22};
const float eme3_ysigma[7] = {1200.48, 887.45, 653.29, 471.23, 389.62, 286.83, 209.79};

TH1F *eventTimeHist;
TH1F *truthTimeHist;

void initialize_histograms() {
    eventTimeHist = new TH1F("eventTime", "Reconstructed Event Time", 300, -1500, 1500);
    eventTimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    eventTimeHist->GetYaxis()->SetTitle("Events");
    
    truthTimeHist = new TH1F("truthTime", "Truth Vertex Time", 300, -1500, 1500);
    truthTimeHist->GetXaxis()->SetTitle("Truth Time [ps]");
    truthTimeHist->GetYaxis()->SetTitle("Events");
}

float get_mean(bool is_barrel, int layer, int energy_bin) {
    if (is_barrel) {
        if (layer == 1) return emb1_y[energy_bin];
        else if (layer == 2) return emb2_y[energy_bin];
        else if (layer == 3) return emb3_y[energy_bin];
    } else { // is_endcap
        if (layer == 1) return eme1_y[energy_bin];
        else if (layer == 2) return eme2_y[energy_bin];
        else if (layer == 3) return eme3_y[energy_bin];
    }
    return 0.0;
}

float get_sigma(bool is_barrel, int layer, int energy_bin) {
    if (is_barrel) {
        if (layer == 1) return emb1_ysigma[energy_bin];
        else if (layer == 2) return emb2_ysigma[energy_bin];
        else if (layer == 3) return emb3_ysigma[energy_bin];
    } else { // is_endcap
        if (layer == 1) return eme1_ysigma[energy_bin];
        else if (layer == 2) return eme2_ysigma[energy_bin];
        else if (layer == 3) return eme3_ysigma[energy_bin];
    }
    return 1.0;
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
            double weighted_sum = 0.0;
            double weight_sum = 0.0;
            truthTimeHist->Fill(vtx_time);

            for (size_t j = 0; j < cellE->size(); ++j) {
                if (cellE->at(j) < 1.0) continue;
                if (cellSignificance->at(j) < 4.0) continue;

                float cell_time = cellTime->at(j);
                float cell_x = cellX->at(j);
                float cell_y = cellY->at(j);
                float cell_z = cellZ->at(j);

                float distance_to_origin = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z) / 1000.0;
                float distance_vtx_to_cell = std::sqrt((cell_x - vtx_x)*(cell_x - vtx_x)
                                                     + (cell_y - vtx_y)*(cell_y - vtx_y)
                                                     + (cell_z - vtx_z)*(cell_z - vtx_z)) / 1000.0;
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

                if (bin != -1 && (is_barrel || is_endcap) && (layer >= 1 && layer <= 3)) {
                    float mean = get_mean(is_barrel, layer, bin);
                    float sigma = get_sigma(is_barrel, layer, bin);
                    
                    float adjusted_time = corrected_time - mean;
                    
                    float weight = 1.0 / (sigma * sigma);
                    
                    weighted_sum += adjusted_time * weight;
                    weight_sum += weight;
                }
            }

            if (weight_sum > 0) {
                float event_time = weighted_sum / weight_sum;
                eventTimeHist->Fill(event_time);
            }
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void processmu200_inclusive_reco(int startIndex = 1, int endIndex = 46) {
    initialize_histograms();

    const std::string path = ".";
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

    TFile *outputFile = new TFile("event_time_reconstruction.root", "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    eventTimeHist->Write();
    truthTimeHist->Write();
    outputFile->Close();
    delete outputFile;
    delete eventTimeHist;
    delete truthTimeHist;

    std::cout << "Event time reconstruction completed. Results saved to event_time_reconstruction.root" << std::endl;
    
}
