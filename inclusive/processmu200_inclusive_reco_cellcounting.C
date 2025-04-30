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


const float emb1_y[7] = {43.7295, 35.7214, 27.8916, 22.7914, 18.7999, 13.6416, 8.38867};
const float emb1_ysigma[7] = {429.494, 299.577, 210.759, 148.779, 117.356, 107.224, 58.2558};

const float emb2_y[7] = {-38.4946, -6.0501, 17.712, 27.7137, 22.916, 25.9662, 18.4498};
const float emb2_ysigma[7] = {2169.15, 1507.89, 1058.47, 736.912, 563.171, 361.537, 145.828};

const float emb3_y[7] = {77, 97.8707, 62.5646, 60.6678, 60.5333, 44.3077, 42.0982};
const float emb3_ysigma[7] = {1330.43, 937.263, 712.241, 491.431, 404.458, 283.305, 167.231};

const float eme1_y[7] = {110.762, 91.2157, 78.2725, 59.8991, 52.3744, 39.3877, 20.5152};
const float eme1_ysigma[7] = {1078.88, 768.96, 571.534, 406.757, 302.185, 207.768, 106.726};

const float eme2_y[7] = {139.8, 135.34, 118.115, 100.96, 86.2284, 71.7498, 36.9509};
const float eme2_ysigma[7] = {1759.41, 1296.14, 940.337, 681.157, 537.594, 354.015, 113.001};

const float eme3_y[7] = {123.66, 102.546, 87.1047, 66.8721, 55.2002, 56.1784, 38.5225};
const float eme3_ysigma[7] = {1196.9, 856.584, 625.558, 442.021, 356.891, 226.194, 100.985};

TH1F *eventTimeHist;
TH1F *truthTimeHist;
TH1F *eventDeltaTimeHist;

TH1F *emb1TimeHist;
TH1F *emb2TimeHist;
TH1F *emb3TimeHist;
TH1F *eme1TimeHist;
TH1F *eme2TimeHist;
TH1F *eme3TimeHist;

TH1F *emb1DeltaTimeHist;
TH1F *emb2DeltaTimeHist;
TH1F *emb3DeltaTimeHist;
TH1F *eme1DeltaTimeHist;
TH1F *eme2DeltaTimeHist;
TH1F *eme3DeltaTimeHist;

TH1F *embTimeHist;
TH1F *emeTimeHist;
TH1F *embDeltaTimeHist;
TH1F *emeDeltaTimeHist;

TH1F *eventCellHist;
TH1F *emeCellHist;
TH1F *embCellHist;

int totalTruthVertices = 0;
int unmatchedVertices = 0;

void initialize_histograms() {
    const int bins = 400;
    const double min_range = -2000;
    const double max_range = 2000;
    
    eventTimeHist = new TH1F("eventTime", "Reconstructed Event Time (All Layers)", bins, min_range, max_range);
    eventTimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    eventTimeHist->GetYaxis()->SetTitle("Events");
    
    truthTimeHist = new TH1F("truthTime", "Truth Vertex Time", bins, min_range, max_range);
    truthTimeHist->GetXaxis()->SetTitle("Truth Time [ps]");
    truthTimeHist->GetYaxis()->SetTitle("Events");

    eventDeltaTimeHist = new TH1F("eventDeltaTime", "Delta t0 (All Layers)", bins, min_range, max_range);
    eventDeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    eventDeltaTimeHist->GetYaxis()->SetTitle("Events");

    
    emb1TimeHist = new TH1F("emb1Time", "Reconstructed Event Time (EMB1 Only)", bins, min_range, max_range);
    emb1TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    emb1TimeHist->GetYaxis()->SetTitle("Events");
    
    emb2TimeHist = new TH1F("emb2Time", "Reconstructed Event Time (EMB2 Only)", bins, min_range, max_range);
    emb2TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    emb2TimeHist->GetYaxis()->SetTitle("Events");
    
    emb3TimeHist = new TH1F("emb3Time", "Reconstructed Event Time (EMB3 Only)", bins, min_range, max_range);
    emb3TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    emb3TimeHist->GetYaxis()->SetTitle("Events");
    
    eme1TimeHist = new TH1F("eme1Time", "Reconstructed Event Time (EME1 Only)", bins, min_range, max_range);
    eme1TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    eme1TimeHist->GetYaxis()->SetTitle("Events");
    
    eme2TimeHist = new TH1F("eme2Time", "Reconstructed Event Time (EME2 Only)", bins, min_range, max_range);
    eme2TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    eme2TimeHist->GetYaxis()->SetTitle("Events");
    
    eme3TimeHist = new TH1F("eme3Time", "Reconstructed Event Time (EME3 Only)", bins, min_range, max_range);
    eme3TimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    eme3TimeHist->GetYaxis()->SetTitle("Events");

    emb1DeltaTimeHist = new TH1F("emb1DeltaTime", "Delta t0 (EMB1 Only)", bins, min_range, max_range);
    emb1DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    emb1DeltaTimeHist->GetYaxis()->SetTitle("Events");

    emb2DeltaTimeHist = new TH1F("emb2DeltaTime", "Delta t0 (EMB2 Only)", bins, min_range, max_range);
    emb2DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    emb2DeltaTimeHist->GetYaxis()->SetTitle("Events");

    emb3DeltaTimeHist = new TH1F("emb3DeltaTime", "Delta t0 (EMB3 Only)", bins, min_range, max_range);
    emb3DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    emb3DeltaTimeHist->GetYaxis()->SetTitle("Events");

    eme1DeltaTimeHist = new TH1F("eme1DeltaTime", "Delta t0 (EME1 Only)", bins, min_range, max_range);
    eme1DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    eme1DeltaTimeHist->GetYaxis()->SetTitle("Events");

    eme2DeltaTimeHist = new TH1F("eme2DeltaTime", "Delta t0 (EME2 Only)", bins, min_range, max_range);
    eme2DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    eme2DeltaTimeHist->GetYaxis()->SetTitle("Events");

    eme3DeltaTimeHist = new TH1F("eme3DeltaTime", "Delta t0 (EME3 Only)", bins, min_range, max_range);
    eme3DeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    eme3DeltaTimeHist->GetYaxis()->SetTitle("Events");


    embDeltaTimeHist = new TH1F("embDeltaTime", "Delta t0 (EMB Only)", bins, min_range, max_range);
    embDeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    embDeltaTimeHist->GetYaxis()->SetTitle("Events");

    emeDeltaTimeHist = new TH1F("emeDeltaTime", "Delta t0 (EME Only)", bins, min_range, max_range);
    emeDeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    emeDeltaTimeHist->GetYaxis()->SetTitle("Events");

    embTimeHist = new TH1F("embTime", "Reconstructed Event Time (EMB Only)", bins, min_range, max_range);
    embTimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    embTimeHist->GetYaxis()->SetTitle("Events");

    emeTimeHist = new TH1F("emeTime", "Reconstructed Event Time (EME Only)", bins, min_range, max_range);
    emeTimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    emeTimeHist->GetYaxis()->SetTitle("Events");



    eventCellHist = new TH1F("eventCell", "Cells Used", 1000, 0, 8000);
    eventCellHist->GetXaxis()->SetTitle("Cells Used");
    eventCellHist->GetYaxis()->SetTitle("Events");

    emeCellHist = new TH1F("emeCell", "Cells Used (EME Only)", 1000, 0, 8000);
    emeCellHist->GetXaxis()->SetTitle("Cells Used");
    emeCellHist->GetYaxis()->SetTitle("Events");

    embCellHist = new TH1F("embCell", "Cells Used (EMB Only)", 1000, 0, 8000);
    embCellHist->GetXaxis()->SetTitle("Cells Used");
    embCellHist->GetYaxis()->SetTitle("Events");


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
    std::vector<float> *recoVtxX = nullptr;
    std::vector<float> *recoVtxY = nullptr;
    std::vector<float> *recoVtxZ = nullptr;
    std::vector<bool> *recoVtxIsHS = nullptr;
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
    tree->SetBranchAddress("RecoVtx_x", &recoVtxX);
    tree->SetBranchAddress("RecoVtx_y", &recoVtxY);
    tree->SetBranchAddress("RecoVtx_z", &recoVtxZ);
    tree->SetBranchAddress("RecoVtx_isHS", &recoVtxIsHS);
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

        int all_cell_used_counter = 0;
        int emb_cell_used_counter = 0;
        int eme_cell_used_counter = 0;

        for (size_t i = 0; i < truthVtxTime->size(); ++i) {
            if (!truthVtxIsHS->at(i)) continue;
            totalTruthVertices++;

            float vtx_time = truthVtxTime->at(i);
            float vtx_x = truthVtxX->at(i);
            float vtx_y = truthVtxY->at(i);
            float vtx_z = truthVtxZ->at(i);
            truthTimeHist->Fill(vtx_time);

            bool foundRecoVtx = false;
            float reco_vtx_x = 0.0;
            float reco_vtx_y = 0.0;
            float reco_vtx_z = 0.0;

            for (size_t reco_i = 0; reco_i < recoVtxIsHS->size(); ++reco_i) {
                if (!recoVtxIsHS->at(reco_i)) continue;
                reco_vtx_x = recoVtxX->at(reco_i);
                reco_vtx_y = recoVtxY->at(reco_i);
                reco_vtx_z = recoVtxZ->at(reco_i);
                foundRecoVtx = true;
                break;
            }
            
            if (!foundRecoVtx) {
                unmatchedVertices++;
                continue;
            }

            double weighted_sum = 0.0, weight_sum = 0.0;
            double weighted_sum_emb = 0.0, weight_sum_emb = 0.0;
            double weighted_sum_eme = 0.0, weight_sum_eme = 0.0;
            double weighted_sum_emb1 = 0.0, weight_sum_emb1 = 0.0;
            double weighted_sum_emb2 = 0.0, weight_sum_emb2 = 0.0;
            double weighted_sum_emb3 = 0.0, weight_sum_emb3 = 0.0;
            double weighted_sum_eme1 = 0.0, weight_sum_eme1 = 0.0;
            double weighted_sum_eme2 = 0.0, weight_sum_eme2 = 0.0;
            double weighted_sum_eme3 = 0.0, weight_sum_eme3 = 0.0;


            for (size_t j = 0; j < cellE->size(); ++j) {
                if (cellE->at(j) < 1.0) continue;
                if (cellSignificance->at(j) < 4.0) continue;

                float cell_time = cellTime->at(j);
                float cell_x = cellX->at(j);
                float cell_y = cellY->at(j);
                float cell_z = cellZ->at(j);

                float distance_to_origin = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z);
                float distance_vtx_to_cell = std::sqrt((cell_x - reco_vtx_x)*(cell_x - reco_vtx_x)
                                                     + (cell_y - reco_vtx_y)*(cell_y - reco_vtx_y)
                                                     + (cell_z - reco_vtx_z)*(cell_z - reco_vtx_z));
                float corrected_time = cell_time + distance_to_origin / c_light - distance_vtx_to_cell / c_light;

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
                    all_cell_used_counter++;

                    if (is_barrel) {
                        emb_cell_used_counter++;
                        weighted_sum_emb += adjusted_time * weight;
                        weight_sum_emb += weight;
                        if (layer == 1) {
                            weighted_sum_emb1 += adjusted_time * weight;
                            weight_sum_emb1 += weight;
                        } else if (layer == 2) {
                            weighted_sum_emb2 += adjusted_time * weight;
                            weight_sum_emb2 += weight;
                        } else if (layer == 3) {
                            weighted_sum_emb3 += adjusted_time * weight;
                            weight_sum_emb3 += weight;
                        }
                    } else if (is_endcap) {
                        eme_cell_used_counter++;
                        weighted_sum_eme += adjusted_time * weight;
                        weight_sum_eme += weight;
                        if (layer == 1) {
                            weighted_sum_eme1 += adjusted_time * weight;
                            weight_sum_eme1 += weight;
                        } else if (layer == 2) {
                            weighted_sum_eme2 += adjusted_time * weight;
                            weight_sum_eme2 += weight;
                        } else if (layer == 3) {
                            weighted_sum_eme3 += adjusted_time * weight;
                            weight_sum_eme3 += weight;
                        }
                    }
                }
            }

            eventCellHist->Fill(all_cell_used_counter);
            embCellHist->Fill(emb_cell_used_counter);
            emeCellHist->Fill(eme_cell_used_counter);


            if (weight_sum > 0) {
                float event_time = weighted_sum / weight_sum;
                float delta_event_time = event_time - vtx_time;
                eventDeltaTimeHist->Fill(delta_event_time);
                eventTimeHist->Fill(event_time);
            }

            if (weight_sum_emb > 0) {
                float event_time_emb = weighted_sum_emb / weight_sum_emb;
                float delta_event_time_emb = event_time_emb - vtx_time;
                embDeltaTimeHist->Fill(delta_event_time_emb);
                embTimeHist->Fill(event_time_emb);
            }

            if (weight_sum_eme > 0) {
                float event_time_eme = weighted_sum_eme / weight_sum_eme;
                float delta_event_time_eme = event_time_eme - vtx_time;
                emeDeltaTimeHist->Fill(delta_event_time_eme);
                emeTimeHist->Fill(event_time_eme);
            }

            if (weight_sum_emb1 > 0) {
                float event_time_emb1 = weighted_sum_emb1 / weight_sum_emb1;
                float delta_event_time_emb1 = event_time_emb1 - vtx_time;
                emb1DeltaTimeHist->Fill(delta_event_time_emb1);
                emb1TimeHist->Fill(event_time_emb1);
            }
            
            if (weight_sum_emb2 > 0) {
                float event_time_emb2 = weighted_sum_emb2 / weight_sum_emb2;
                float delta_event_time_emb2 = event_time_emb2 - vtx_time;
                emb2DeltaTimeHist->Fill(delta_event_time_emb2);
                emb2TimeHist->Fill(event_time_emb2);
            }
            
            if (weight_sum_emb3 > 0) {
                float event_time_emb3 = weighted_sum_emb3 / weight_sum_emb3;
                float delta_event_time_emb3 = event_time_emb3 - vtx_time;
                emb3DeltaTimeHist->Fill(delta_event_time_emb3);
                emb3TimeHist->Fill(event_time_emb3);
            }
            
            if (weight_sum_eme1 > 0) {
                float event_time_eme1 = weighted_sum_eme1 / weight_sum_eme1;
                float delta_event_time_eme1 = event_time_eme1 - vtx_time;
                eme1DeltaTimeHist->Fill(delta_event_time_eme1);
                eme1TimeHist->Fill(event_time_eme1);
            }
            
            if (weight_sum_eme2 > 0) {
                float event_time_eme2 = weighted_sum_eme2 / weight_sum_eme2;
                float delta_event_time_eme2 = event_time_eme2 - vtx_time;
                eme2DeltaTimeHist->Fill(delta_event_time_eme2);
                eme2TimeHist->Fill(event_time_eme2);
            }
            
            if (weight_sum_eme3 > 0) {
                float event_time_eme3 = weighted_sum_eme3 / weight_sum_eme3;
                float delta_event_time_eme3 = event_time_eme3 - vtx_time;
                eme3DeltaTimeHist->Fill(delta_event_time_eme3);
                eme3TimeHist->Fill(event_time_eme3);
            }
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void processmu200_inclusive_reco_cellcounting(int startIndex = 1, int endIndex = 46) {
    totalTruthVertices = 0;
    unmatchedVertices = 0;
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

    std::cout << "Statistical Summary:" << std::endl;
    std::cout << "Total Truth Vertices: " << totalTruthVertices << std::endl;
    std::cout << "Unmatched Vertices: " << unmatchedVertices << std::endl;
    std::cout << "Matching Rate: " << (100.0 * (totalTruthVertices - unmatchedVertices) / totalTruthVertices) << "%" << std::endl;

    TFile *outputFile = new TFile("inclusive_reconstruction.root", "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    eventTimeHist->Write();
    truthTimeHist->Write();
    eventDeltaTimeHist->Write();
    emb1TimeHist->Write();
    emb2TimeHist->Write();
    emb3TimeHist->Write();
    eme1TimeHist->Write();
    eme2TimeHist->Write();
    eme3TimeHist->Write();
    emb1DeltaTimeHist->Write();
    emb2DeltaTimeHist->Write();
    emb3DeltaTimeHist->Write();
    eme1DeltaTimeHist->Write();
    eme2DeltaTimeHist->Write();
    eme3DeltaTimeHist->Write();

    embDeltaTimeHist->Write();
    emeDeltaTimeHist->Write();
    embTimeHist->Write();
    emeTimeHist->Write();

    eventCellHist->Write();
    embCellHist->Write();
    emeCellHist->Write();


    outputFile->Close();

    delete outputFile;
    delete eventTimeHist;
    delete eventDeltaTimeHist;
    delete truthTimeHist;
    delete emb1TimeHist;
    delete emb2TimeHist;
    delete emb3TimeHist;
    delete eme1TimeHist;
    delete eme2TimeHist;
    delete eme3TimeHist;
    delete emb1DeltaTimeHist;
    delete emb2DeltaTimeHist;
    delete emb3DeltaTimeHist;
    delete eme1DeltaTimeHist;
    delete eme2DeltaTimeHist;
    delete eme3DeltaTimeHist;

    delete embDeltaTimeHist;
    delete emeDeltaTimeHist;
    delete embTimeHist;
    delete emeTimeHist;

    delete eventCellHist;
    delete embCellHist;
    delete emeCellHist;


    std::cout << "Event time reconstruction completed. Results saved to event_time_reconstruction.root" << std::endl;
    
}
