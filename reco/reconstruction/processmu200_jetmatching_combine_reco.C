#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

const double c_light = 0.299792458; // mm/ps

const float emb1_y[7] = {44.0004, 36.1672, 27.7946, 22.8566, 18.6052, 13.5979, 8.63018};
const float emb1_ysigma[7] = {428.908, 299.433, 210.949, 149.052, 117.804, 107.251, 57.5941};

const float emb2_y[7] = {-37.6927, -4.76502, 18.4064, 28.5538, 23.8885, 26.6632, 18.3774};
const float emb2_ysigma[7] = {2167.48, 1507, 1056.7, 736.56, 562.19, 360.442, 145.808};

const float emb3_y[7] = {76.9224, 96.6823, 61.8036, 59.5018, 60.5852, 42.9381, 42.8869};
const float emb3_ysigma[7] = {1328.7, 937.662, 707.828, 488.065, 405.723, 280.358, 166.422};

const float eme1_y[7] = {110.994, 91.5183, 77.769, 60.8697, 52.896, 39.3309, 20.7378};
const float eme1_ysigma[7] = {1076.77, 768.73, 570.185, 404.887, 301.699, 207.33, 104.583};

const float eme2_y[7] = {140.013, 135.814, 118.308, 101.373, 87.0022, 71.9089, 37.0023};
const float eme2_ysigma[7] = {1759.24, 1295.54, 939.963, 680.803, 537.385, 353.075, 112.837};

const float eme3_y[7] = {125.077, 103.004, 88.0972, 68.0425, 54.7863, 56.1797, 38.8708};
const float eme3_ysigma[7] = {1197.23, 856.656, 625.128, 439.311, 356.805, 224.769, 101.193};

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

TH1F *selectedJetPtHist;
TH1F *selectedJetWidthHist;
TH1F *selectedJetCountHist;

TH1F *jetTimeHist;
TH1F *jetDeltaTimeHist;

TH1F *allMatchedJetPtHist;
TH1F *allMatchedJetWidthHist;
TH1F *allMatchedJetCountHist;

TH1F *jetEM1FractionHist;
TH1F *jetEM12FractionHist;

TH1F *jetLongWidthHist;
TH1F *jetLongWidthSigmaHist;

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

    eventCellHist = new TH1F("eventCell", "Cells Used", 501, 0, 500);
    eventCellHist->GetXaxis()->SetTitle("Cells Used");
    eventCellHist->GetYaxis()->SetTitle("Events");

    emeCellHist = new TH1F("emeCell", "Cells Used (EME Only)", 501, 0, 500);
    emeCellHist->GetXaxis()->SetTitle("Cells Used");
    emeCellHist->GetYaxis()->SetTitle("Events");

    embCellHist = new TH1F("embCell", "Cells Used (EMB Only)", 501, 0, 500);
    embCellHist->GetXaxis()->SetTitle("Cells Used");
    embCellHist->GetYaxis()->SetTitle("Events");

    selectedJetPtHist = new TH1F("selectedJetPt", "Selected Jet PT Distribution", 2000, 0, 10000);
    selectedJetPtHist->GetXaxis()->SetTitle("Jet pT [GeV]");
    selectedJetPtHist->GetYaxis()->SetTitle("Jets");

    selectedJetWidthHist = new TH1F("selectedJetWidth", "Selected Jet Width Distribution", 100, 0, 0.4);
    selectedJetWidthHist->GetXaxis()->SetTitle("Jet Width");
    selectedJetWidthHist->GetYaxis()->SetTitle("Jets");

    selectedJetCountHist = new TH1F("selectedJetCount", "Number of Selected Jets per Event", 101, 0, 100);
    selectedJetCountHist->GetXaxis()->SetTitle("Number of Jets");
    selectedJetCountHist->GetYaxis()->SetTitle("Events");

    jetTimeHist = new TH1F("jetTime", "Reconstructed Jet Time", bins, min_range, max_range);
    jetTimeHist->GetXaxis()->SetTitle("Reconstructed Time [ps]");
    jetTimeHist->GetYaxis()->SetTitle("Jets");
    
    jetDeltaTimeHist = new TH1F("jetDeltaTime", "Delta t0 (Jet Level)", bins, min_range, max_range);
    jetDeltaTimeHist->GetXaxis()->SetTitle("Delta t0 [ps]");
    jetDeltaTimeHist->GetYaxis()->SetTitle("Jets");

    allMatchedJetCountHist = new TH1F("allMatchedJetCount", "Number of All Matched HS Jets per Event", 101, 0, 100);
    allMatchedJetCountHist->GetXaxis()->SetTitle("Number of Jets");
    allMatchedJetCountHist->GetYaxis()->SetTitle("Events");

    allMatchedJetPtHist = new TH1F("allMatchedJetPt", "PT of All Matched HS Jets", 2000, 0, 10000);
    allMatchedJetPtHist->GetXaxis()->SetTitle("Jet pT [GeV]");
    allMatchedJetPtHist->GetYaxis()->SetTitle("Jets");
    
    allMatchedJetWidthHist = new TH1F("allMatchedJetWidth", "Width of All Matched HS Jets", 100, 0, 0.4);
    allMatchedJetWidthHist->GetXaxis()->SetTitle("Jet Width");
    allMatchedJetWidthHist->GetYaxis()->SetTitle("Jets");

    jetEM1FractionHist = new TH1F("jetEM1Fraction", "EM1 Layer Energy Fraction of Selected Jets", 100, 0, 1.5);
    jetEM1FractionHist->GetXaxis()->SetTitle("EM1 Energy Fraction");
    jetEM1FractionHist->GetYaxis()->SetTitle("Jets");

    jetEM12FractionHist = new TH1F("jetEM12Fraction", "EM1+EM2 Layers Energy Fraction of Selected Jets", 100, 0, 1.5);
    jetEM12FractionHist->GetXaxis()->SetTitle("EM1+EM2 Energy Fraction");
    jetEM12FractionHist->GetYaxis()->SetTitle("Jets");

    jetLongWidthHist = new TH1F("jetLongWidth", "Longitudinal Width of Selected Jets", 600, 1000, 5700);
    jetLongWidthHist->GetXaxis()->SetTitle("Longitudinal Width [mm]");
    jetLongWidthHist->GetYaxis()->SetTitle("Jets");

    jetLongWidthSigmaHist = new TH1F("jetLongWidthSigma", "Sigma of Longitudinal Energy Distribution", 1000, 0, 3000);
    jetLongWidthSigmaHist->GetXaxis()->SetTitle("Longitudinal Width Sigma [mm]");
    jetLongWidthSigmaHist->GetYaxis()->SetTitle("Jets");
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

void process_file(const std::string &filename, float energyThreshold = 1.0, float jetPtMin = 30.0, float jetPtMax = 1000.0,
                  float deltaRThreshold = 0.3, int maxJets = -1, float jetWidthMin = 0.0, float jetWidthMax = 1.0,
                  float jetEtaCut = 2.0, float jetEM1FractionCut = 1.1, float jetEM12FractionCut = 1.1, 
                  float jetLongWidthCut = 10000.0, float jetLongWidthSigmaCut = 10000.0) {
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
    std::vector<float> *cellEta = nullptr;
    std::vector<float> *cellPhi = nullptr;
    std::vector<bool> *cellIsEMBarrel = nullptr;
    std::vector<bool> *cellIsEMEndCap = nullptr;
    std::vector<bool> *cellIsTile = nullptr; 
    std::vector<int> *cellLayer = nullptr;
    std::vector<float> *cellSignificance = nullptr;
    std::vector<float> *TopoJetsPt = nullptr;
    std::vector<float> *TopoJetsEta = nullptr;
    std::vector<float> *TopoJetsPhi = nullptr;
    std::vector<float> *TopoJetsWidth = nullptr;
    std::vector<std::vector<int>> *TopoJets_TruthHSJetIdx = nullptr;

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
    tree->SetBranchAddress("Cell_eta", &cellEta);
    tree->SetBranchAddress("Cell_phi", &cellPhi);
    tree->SetBranchAddress("Cell_isEM_Barrel", &cellIsEMBarrel);
    tree->SetBranchAddress("Cell_isEM_EndCap", &cellIsEMEndCap);
    tree->SetBranchAddress("Cell_isTile", &cellIsTile);
    tree->SetBranchAddress("Cell_layer", &cellLayer);
    tree->SetBranchAddress("Cell_significance", &cellSignificance);
    tree->SetBranchAddress("AntiKt4EMTopoJets_pt", &TopoJetsPt);
    tree->SetBranchAddress("AntiKt4EMTopoJets_eta", &TopoJetsEta);
    tree->SetBranchAddress("AntiKt4EMTopoJets_phi", &TopoJetsPhi);
    tree->SetBranchAddress("AntiKt4EMTopoJets_width", &TopoJetsWidth);
    tree->SetBranchAddress("AntiKt4EMTopoJets_truthHSJet_idx", &TopoJets_TruthHSJetIdx);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        int all_cell_used_counter = 0;
        int emb_cell_used_counter = 0;
        int eme_cell_used_counter = 0;

        std::vector<float> selectedJetPt;
        std::vector<float> selectedJetEta;
        std::vector<float> selectedJetPhi;
        std::vector<float> selectedJetWidth;

        std::vector<std::tuple<float, float, float, float>> candidateJets;

        for (size_t j = 0; j < TopoJetsPt->size(); ++j) {
            bool isInPtRange = (TopoJetsPt->at(j) >= jetPtMin && TopoJetsPt->at(j) <= jetPtMax);
            bool hasMatch = (j < TopoJets_TruthHSJetIdx->size() && !TopoJets_TruthHSJetIdx->at(j).empty());
            bool isWidthInRange = (TopoJetsWidth->at(j) >= jetWidthMin && TopoJetsWidth->at(j) <= jetWidthMax);
            bool isInEtaRange = (std::fabs(TopoJetsEta->at(j)) <= jetEtaCut);
            
            if (isInPtRange && hasMatch && isWidthInRange && isInEtaRange) {
                candidateJets.push_back(std::make_tuple(TopoJetsPt->at(j), TopoJetsEta->at(j), 
                                                       TopoJetsPhi->at(j), TopoJetsWidth->at(j)));
            }
        }

        if (maxJets > 0 && candidateJets.size() > maxJets) {
            std::sort(candidateJets.begin(), candidateJets.end(), 
                     [](const std::tuple<float, float, float, float>& a, const std::tuple<float, float, float, float>& b) {
                         return std::get<0>(a) > std::get<0>(b);
                     });
            candidateJets.resize(maxJets);
        }

        for (const auto& jet : candidateJets) {
            selectedJetPt.push_back(std::get<0>(jet));
            selectedJetEta.push_back(std::get<1>(jet));
            selectedJetPhi.push_back(std::get<2>(jet));
            selectedJetWidth.push_back(std::get<3>(jet));
        }

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

            float reco_dz_distance = std::sqrt((vtx_x - reco_vtx_x)*(vtx_x - reco_vtx_x)
                                             + (vtx_y - reco_vtx_y)*(vtx_y - reco_vtx_y)
                                             + (vtx_z - reco_vtx_z)*(vtx_z - reco_vtx_z));
            if (reco_dz_distance > 2) continue;

            int allMatchedJetsCount = 0;
            for (size_t j = 0; j < TopoJetsPt->size(); ++j) {
                bool hasMatch = (j < TopoJets_TruthHSJetIdx->size() && !TopoJets_TruthHSJetIdx->at(j).empty());
                bool isInEnoughPt = (TopoJetsPt->at(j) >= 30);
                if (hasMatch && isInEnoughPt) {
                    allMatchedJetsCount++;
                    allMatchedJetPtHist->Fill(TopoJetsPt->at(j));
                    allMatchedJetWidthHist->Fill(TopoJetsWidth->at(j));
                }
            }

            allMatchedJetCountHist->Fill(allMatchedJetsCount);
            selectedJetCountHist->Fill(selectedJetPt.size());

            std::vector<double> jet_weighted_sum(selectedJetPt.size(), 0.0);
            std::vector<double> jet_weight_sum(selectedJetPt.size(), 0.0);
            std::vector<double> jet_total_energy(selectedJetPt.size(), 0.0);
            std::vector<double> jet_em1_energy(selectedJetPt.size(), 0.0);
            std::vector<double> jet_em12_energy(selectedJetPt.size(), 0.0);

            std::vector<double> jet_weighted_r_sum(selectedJetPt.size(), 0.0);
            std::vector<double> jet_e_sum_for_width(selectedJetPt.size(), 0.0);
            std::vector<double> jet_weighted_r_squared_sum(selectedJetPt.size(), 0.0);
            std::vector<std::vector<std::pair<float, float>>> jet_cell_r_e(selectedJetPt.size());

            //if (selectedJetPt.empty()) {
            //    continue;
            //}

            // Fill jet width histogram for events that pass all cuts
            for (size_t k = 0; k < selectedJetWidth.size(); ++k) {
                selectedJetWidthHist->Fill(selectedJetWidth[k]);
                selectedJetPtHist->Fill(selectedJetPt[k]);
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
                if (cellE->at(j) < energyThreshold) continue;
                if (cellSignificance->at(j) < 4.0) continue;

                float cell_eta = cellEta->at(j);
                float cell_phi = cellPhi->at(j);
                float cell_x = cellX->at(j);
                float cell_y = cellY->at(j);
                float cell_z = cellZ->at(j);

                bool is_barrel = cellIsEMBarrel->at(j);
                bool is_endcap = cellIsEMEndCap->at(j);
                bool isTile = cellIsTile->at(j);
                if (!is_barrel && !is_endcap && !isTile) continue;
                int layer = cellLayer->at(j);
                float energy = cellE->at(j);

                float r_i = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z);

                std::vector<int> matchedJetIndices;
                
                bool isCloseToJet = false;
                for (size_t jetIdx = 0; jetIdx < selectedJetPt.size(); ++jetIdx) {
                    float jetEta = selectedJetEta[jetIdx];
                    float jetPhi = selectedJetPhi[jetIdx];
                    
                    float dEta = jetEta - cell_eta;
                    float dPhi = jetPhi - cell_phi;
                    
                    if (dPhi >= M_PI) {
                        dPhi -= 2 * M_PI;
                    } else if (dPhi < -M_PI) {
                        dPhi += 2 * M_PI;
                    }
                    
                    float DeltaR = std::sqrt(dEta * dEta + dPhi * dPhi);
                    
                    if (DeltaR < deltaRThreshold) {
                        isCloseToJet = true;
                        matchedJetIndices.push_back(jetIdx);

                        jet_cell_r_e[jetIdx].push_back(std::make_pair(r_i, energy));

                        jet_e_sum_for_width[jetIdx] += energy;
                        jet_weighted_r_sum[jetIdx] += r_i * energy;
                        jet_weighted_r_squared_sum[jetIdx] += r_i * r_i * energy;

                        jet_total_energy[jetIdx] += energy;
                        if ((is_barrel || is_endcap) && layer == 1) {
                            jet_em1_energy[jetIdx] += energy;
                            jet_em12_energy[jetIdx] += energy;
                        }
                        if ((is_barrel || is_endcap) && layer == 2) {
                            jet_em12_energy[jetIdx] += energy;
                        }
                    }
                }

                if (!isCloseToJet) continue;

                float cell_time = cellTime->at(j);

                float distance_to_origin = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z);
                float distance_vtx_to_cell = std::sqrt((cell_x - reco_vtx_x)*(cell_x - reco_vtx_x)
                                                     + (cell_y - reco_vtx_y)*(cell_y - reco_vtx_y)
                                                     + (cell_z - reco_vtx_z)*(cell_z - reco_vtx_z));
                float corrected_time = cell_time + distance_to_origin / c_light - distance_vtx_to_cell / c_light;

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


                    for (int matchedJetIdx : matchedJetIndices) {
                        jet_weighted_sum[matchedJetIdx] += adjusted_time * weight;
                        jet_weight_sum[matchedJetIdx] += weight;
                    }

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

            for (size_t jetIdx = 0; jetIdx < selectedJetPt.size(); ++jetIdx) {

                float longitudinal_width = 999999.9;
                float saved_sigma = 999999.9;

                if (jet_e_sum_for_width[jetIdx] > 0) {

                    longitudinal_width = jet_weighted_r_sum[jetIdx] / jet_e_sum_for_width[jetIdx];

                    double sigma = 0.0;
                    double sum_weights = 0.0;
                    
                    for (const auto& cell_info : jet_cell_r_e[jetIdx]) {
                        float r_i = cell_info.first;
                        float e_i = cell_info.second;
                        
                        sigma += e_i * (r_i - longitudinal_width) * (r_i - longitudinal_width);
                        sum_weights += e_i;
                    }
                    
                    //if (sum_weights > 0 && jet_cell_r_e[jetIdx].size() > 1) {
                    if (sum_weights > 0) {
                        sigma = std::sqrt(sigma / sum_weights);
                        saved_sigma = sigma;
                    }
                }

                if (jet_total_energy[jetIdx] > 0) {
                    float em1_fraction = jet_em1_energy[jetIdx] / jet_total_energy[jetIdx];
                    float em12_fraction = jet_em12_energy[jetIdx] / jet_total_energy[jetIdx];

                    bool passEM1Cut = (em1_fraction >= jetEM1FractionCut);
                    bool passEM12Cut = (em12_fraction >= jetEM12FractionCut);
                    bool passLongWidthCut = (longitudinal_width <= jetLongWidthCut);
                    bool passLongWidthSigmaCut = (saved_sigma <= jetLongWidthSigmaCut);

                    if (passEM1Cut) {
                        jetEM1FractionHist->Fill(em1_fraction);
                    }
                    
                    if (passEM12Cut) {
                        jetEM12FractionHist->Fill(em12_fraction);
                    }

                    if (passEM1Cut && passEM12Cut && passLongWidthCut && passLongWidthSigmaCut && jet_weight_sum[jetIdx] > 0) {
                        float jet_time = jet_weighted_sum[jetIdx] / jet_weight_sum[jetIdx];
                        float delta_jet_time = jet_time - vtx_time;
                        
                        jetTimeHist->Fill(jet_time);
                        jetDeltaTimeHist->Fill(delta_jet_time);
                        jetLongWidthHist->Fill(longitudinal_width);
                        jetLongWidthSigmaHist->Fill(saved_sigma);
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
                float delta_event_time_emb3 = event_time_emb3 - vtx_time; emb3DeltaTimeHist->Fill(delta_event_time_emb3);
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
            
            break; // Only process the first valid truth vertex per event
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void processmu200_jetmatching_reco(float energyThreshold = 1.0, int startIndex = 1, int endIndex = 46, 
                                  float jetPtMin = 30.0, float jetPtMax = 1000.0, float deltaRThreshold = 0.3, int maxJets = -1,
                                  float jetWidthMin = 0.17, float jetWidthMax = 0.4, float jetEtaCut = 2.0, float jetEM1FractionCut = 1.1,
                                  float jetEM12FractionCut = 1.1, float jetLongWidthCut = 10000.0, float jetLongWidthSigmaCut = 10000.0) {

    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");
    gInterpreter->GenerateDictionary("vector<pair<float,float> >", "vector;utility");
    gInterpreter->GenerateDictionary("vector<vector<pair<float,float> > >", "vector;utility");
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
            process_file(filename.str(), energyThreshold, jetPtMin, jetPtMax, deltaRThreshold, maxJets, jetWidthMin, jetWidthMax, jetEtaCut, jetEM1FractionCut, jetEM12FractionCut,
                        jetLongWidthCut, jetLongWidthSigmaCut);
        } else {
            std::cerr << "File does not exist: " << filename.str() << std::endl;
        }
    }

    std::cout << "Statistical Summary:" << std::endl;
    std::cout << "Total Truth Vertices: " << totalTruthVertices << std::endl;
    std::cout << "Unmatched Vertices: " << unmatchedVertices << std::endl;
    std::cout << "Matching Rate: " << (100.0 * (totalTruthVertices - unmatchedVertices) / totalTruthVertices) << "%" << std::endl;

    std::ostringstream outputFilename;
    outputFilename << "jetmatching_reconstruction_Eover" << std::fixed << std::setprecision(1) 
                  << energyThreshold << "_jetPt" << std::setprecision(0) << jetPtMin << "to" << jetPtMax
                  << "_dR" << std::setprecision(1) << deltaRThreshold;
    if (maxJets > 0) {
        outputFilename << "_maxJets" << maxJets;
    }
    outputFilename << "_jetWidth" << std::setprecision(2) << jetWidthMin << "to" << jetWidthMax;
    outputFilename << "_jetEta" << std::setprecision(1) << jetEtaCut
                << "_EM1frac" << std::setprecision(2) << jetEM1FractionCut
                  << "_EM12frac" << std::setprecision(2) << jetEM12FractionCut;
    outputFilename << "_LW" << std::setprecision(0) << jetLongWidthCut;
    outputFilename << "_LWS" << std::setprecision(0) << jetLongWidthSigmaCut;
    outputFilename << ".root";

    TFile *outputFile = new TFile(outputFilename.str().c_str(), "RECREATE");
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

    selectedJetPtHist->Write();
    selectedJetWidthHist->Write();
    selectedJetCountHist->Write();
    jetTimeHist->Write();
    jetDeltaTimeHist->Write();

    allMatchedJetPtHist->Write();
    allMatchedJetWidthHist->Write();
    allMatchedJetCountHist->Write();
    jetEM1FractionHist->Write();
    jetEM12FractionHist->Write();
    jetLongWidthHist->Write();
    jetLongWidthSigmaHist->Write();

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

    delete selectedJetPtHist;
    delete selectedJetWidthHist;
    delete selectedJetCountHist;
    delete jetTimeHist;
    delete jetDeltaTimeHist;
    delete allMatchedJetCountHist;
    delete allMatchedJetPtHist;
    delete allMatchedJetWidthHist;
    delete jetEM1FractionHist;
    delete jetEM12FractionHist;
    delete jetLongWidthHist;
    delete jetLongWidthSigmaHist;

    std::cout << "Event time reconstruction completed. Results saved to " << outputFilename.str() << std::endl;

    std::cout << "Parameters used: " << std::endl;
    std::cout << "  Energy threshold: " << energyThreshold << std::endl;
    std::cout << "  Jet pT range: " << jetPtMin << " - " << jetPtMax << " GeV" << std::endl;
    std::cout << "  Delta R threshold: " << deltaRThreshold << std::endl;
    std::cout << "  Max jets per event: " << (maxJets > 0 ? std::to_string(maxJets) : "all") << std::endl;
    std::cout << "  Jet width range: " << jetWidthMin << " to " << jetWidthMax << std::endl;
    std::cout << "  Jet eta cut: |eta| <= " << jetEtaCut << std::endl;
    std::cout << "  Jet EM1 fraction cut: >= " << jetEM1FractionCut << std::endl;
    std::cout << "  Jet EM1+EM2 fraction cut: >= " << jetEM12FractionCut << std::endl;
    std::cout << "  Jet longitudinal width cut: <= " << jetLongWidthCut << " mm" << std::endl;
    std::cout << "  Jet longitudinal width sigma cut: <= " << jetLongWidthSigmaCut << " mm" << std::endl;
}
