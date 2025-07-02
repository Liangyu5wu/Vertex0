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
#include <TH2F.h>
#include <TH1F.h>

// root -l
// .L event_display.C
// event_display_analysis_with_cuts(2, 2.0, 1, 46, 30, 100000, 0.3, 0, 1, 0.8, 0, 0, 10000.0, 0.0, 10000.0)

const double c_light = 0.299792458; // mm/ps

// Jet storage
std::vector<TH2F*> jet_cells_hists;
std::vector<float> all_jets_pt;
std::vector<float> all_jets_eta;
std::vector<float> all_jets_phi;
std::vector<float> all_jets_long_width;
std::vector<float> all_jets_long_width_sigma;

int jets_found = 0;
int jets_with_histograms = 0;
const int max_jets = 2000;
const int max_jet_histograms = 20; 

TH1F *all_jets_eta_hist;
TH1F *all_jets_phi_hist;
TH1F *all_jets_long_width_hist;
TH1F *all_jets_long_width_sigma_hist;

void initialize_summary_histograms() {
    all_jets_eta_hist = new TH1F("all_jets_eta", "All Selected Jets Eta Distribution;#eta;Number of Jets", 100, -5.0, 5.0);
    all_jets_phi_hist = new TH1F("all_jets_phi", "All Selected Jets Phi Distribution;#phi;Number of Jets", 100, -4.0, 4.0);
    all_jets_long_width_hist = new TH1F("all_jets_long_width", "All Selected Jets Longitudinal Width;Width [mm];Number of Jets", 500, 1000, 6000);
    all_jets_long_width_sigma_hist = new TH1F("all_jets_long_width_sigma", "All Selected Jets Longitudinal Width Sigma;Sigma [mm];Number of Jets", 1000, 0, 3000);
}

void process_file(const std::string &filename, int minCells, float energyThreshold = 1.0, 
                  float jetPtMin = 30.0, float jetPtMax = 1000.0, float deltaRThreshold = 0.3, 
                  float jetWidthMin = 0.17, float jetWidthMax = 0.4, float jetEtaCut = 2.0, 
                  float jetEM1FractionCut = 1.1, float jetEM12FractionCut = 1.1, 
                  float jetLongWidthCut = 10000.0, float jetLongWidthSigmaMin = 0.0, 
                  float jetLongWidthSigmaMax = 10000.0) {
    
    if (jets_found >= max_jets) return;
    
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
    for (Long64_t entry = 0; entry < nEntries && jets_found < max_jets; ++entry) {
        tree->GetEntry(entry);

        // Check for valid truth vertex
        bool hasValidTruthVertex = false;
        
        for (size_t i = 0; i < truthVtxTime->size(); ++i) {
            if (!truthVtxIsHS->at(i)) continue;
            
            float vtx_x = truthVtxX->at(i);
            float vtx_y = truthVtxY->at(i);
            float vtx_z = truthVtxZ->at(i);

            bool foundRecoVtx = false;
            for (size_t reco_i = 0; reco_i < recoVtxIsHS->size(); ++reco_i) {
                if (!recoVtxIsHS->at(reco_i)) continue;
                float reco_vtx_x = recoVtxX->at(reco_i);
                float reco_vtx_y = recoVtxY->at(reco_i);
                float reco_vtx_z = recoVtxZ->at(reco_i);
                
                float reco_dz_distance = std::sqrt((vtx_x - reco_vtx_x)*(vtx_x - reco_vtx_x)
                                                 + (vtx_y - reco_vtx_y)*(vtx_y - reco_vtx_y)
                                                 + (vtx_z - reco_vtx_z)*(vtx_z - reco_vtx_z));
                if (reco_dz_distance <= 2) {
                    foundRecoVtx = true;
                    break;
                }
            }
            
            if (foundRecoVtx) {
                hasValidTruthVertex = true;
                break;
            }
        }

        if (!hasValidTruthVertex) continue;

        // Select jets that pass basic cuts
        std::vector<int> candidateJetIndices;
        
        for (size_t j = 0; j < TopoJetsPt->size(); ++j) {
            bool isInPtRange = (TopoJetsPt->at(j) >= jetPtMin && TopoJetsPt->at(j) <= jetPtMax);
            bool hasMatch = (j < TopoJets_TruthHSJetIdx->size() && !TopoJets_TruthHSJetIdx->at(j).empty());
            bool isWidthInRange = (TopoJetsWidth->at(j) >= jetWidthMin && TopoJetsWidth->at(j) <= jetWidthMax);
            bool isInEtaRange = (std::fabs(TopoJetsEta->at(j)) <= jetEtaCut);
            
            if (isInPtRange && hasMatch && isWidthInRange && isInEtaRange) {
                candidateJetIndices.push_back(j);
            }
        }

        if (candidateJetIndices.empty()) continue;

        // Process each jet individually
        for (int jetIdx : candidateJetIndices) {
            if (jets_found >= max_jets) break;
            
            float jetEta = TopoJetsEta->at(jetIdx);
            float jetPhi = TopoJetsPhi->at(jetIdx);
            
            float jet_total_energy = 0.0;
            float jet_em1_energy = 0.0;
            float jet_em12_energy = 0.0;
            std::vector<std::pair<float, float>> jet_cell_r_e;
            
            // Calculate energy fractions and collect cells for width calculation
            for (size_t k = 0; k < cellE->size(); ++k) {
                if (cellE->at(k) < energyThreshold) continue;
                if (cellSignificance->at(k) < 4.0) continue;
                
                float cell_eta = cellEta->at(k);
                float cell_phi = cellPhi->at(k);
                float cell_x = cellX->at(k);
                float cell_y = cellY->at(k);
                float cell_z = cellZ->at(k);
                
                float dEta = jetEta - cell_eta;
                float dPhi = jetPhi - cell_phi;
                
                if (dPhi >= M_PI) {
                    dPhi -= 2 * M_PI;
                } else if (dPhi < -M_PI) {
                    dPhi += 2 * M_PI;
                }
                
                float DeltaR = std::sqrt(dEta * dEta + dPhi * dPhi);
                
                if (DeltaR < deltaRThreshold) {
                    bool is_barrel = cellIsEMBarrel->at(k);
                    bool is_endcap = cellIsEMEndCap->at(k);
                    bool isTile = cellIsTile->at(k);
                    if (!is_barrel && !is_endcap && !isTile) continue;
                    
                    int layer = cellLayer->at(k);
                    float energy = cellE->at(k);
                    float r_i = std::sqrt(cell_x*cell_x + cell_y*cell_y + cell_z*cell_z);
                    
                    jet_total_energy += energy;
                    if ((is_barrel || is_endcap) && layer == 1) {
                        jet_em1_energy += energy;
                        jet_em12_energy += energy;
                    }
                    if ((is_barrel || is_endcap) && layer == 2) {
                        jet_em12_energy += energy;
                    }
                    
                    jet_cell_r_e.push_back(std::make_pair(r_i, energy));
                }
            }
            
            // Check energy fraction cuts
            bool passEM1Cut = true;
            bool passEM12Cut = true;
            if (jet_total_energy > 0 && jetEM1FractionCut < 1.1 && jetEM12FractionCut < 1.1) {
                float em1_fraction = jet_em1_energy / jet_total_energy;
                float em12_fraction = jet_em12_energy / jet_total_energy;
                passEM1Cut = (em1_fraction > jetEM1FractionCut);
                passEM12Cut = (em12_fraction > jetEM12FractionCut);
            }
            
            // Calculate longitudinal width and sigma
            float longitudinal_width = 999999.9;
            float longitudinal_width_sigma = 999999.9;
            bool passLongWidthCut = true;
            bool passLongWidthSigmaCut = true;
            
            if (!jet_cell_r_e.empty()) {
                float weighted_r_sum = 0.0;
                float e_sum = 0.0;
                for (const auto& cell_info : jet_cell_r_e) {
                    weighted_r_sum += cell_info.first * cell_info.second;
                    e_sum += cell_info.second;
                }
                
                if (e_sum > 0) {
                    longitudinal_width = weighted_r_sum / e_sum;
                    passLongWidthCut = (longitudinal_width <= jetLongWidthCut);
                    
                    // Calculate sigma
                    float sigma = 0.0;
                    for (const auto& cell_info : jet_cell_r_e) {
                        float r_i = cell_info.first;
                        float e_i = cell_info.second;
                        sigma += e_i * (r_i - longitudinal_width) * (r_i - longitudinal_width);
                    }
                    longitudinal_width_sigma = std::sqrt(sigma / e_sum);
                    passLongWidthSigmaCut = (longitudinal_width_sigma >= jetLongWidthSigmaMin && 
                                           longitudinal_width_sigma <= jetLongWidthSigmaMax);
                }
            }
            
            if (!passEM1Cut || !passEM12Cut || !passLongWidthCut || !passLongWidthSigmaCut) continue;
            
            // Count cells that will be filled for this jet
            int cellCount = 0;
            for (size_t k = 0; k < cellE->size(); ++k) {
                if (cellE->at(k) < energyThreshold) continue;
                if (cellSignificance->at(k) < 4.0) continue;
                
                float cell_eta = cellEta->at(k);
                float cell_phi = cellPhi->at(k);
                
                float dEta = jetEta - cell_eta;
                float dPhi = jetPhi - cell_phi;
                
                if (dPhi >= M_PI) {
                    dPhi -= 2 * M_PI;
                } else if (dPhi < -M_PI) {
                    dPhi += 2 * M_PI;
                }
                
                float DeltaR = std::sqrt(dEta * dEta + dPhi * dPhi);
                
                if (DeltaR < deltaRThreshold) {
                    cellCount++;
                }
            }
            
            // Check minimum cell count
            if (cellCount < minCells) continue;
            
            all_jets_pt.push_back(TopoJetsPt->at(jetIdx));
            all_jets_eta.push_back(jetEta);
            all_jets_phi.push_back(jetPhi);
            all_jets_long_width.push_back(longitudinal_width);
            all_jets_long_width_sigma.push_back(longitudinal_width_sigma);
            
            // Fill summary histograms
            all_jets_eta_hist->Fill(jetEta);
            all_jets_phi_hist->Fill(jetPhi);
            all_jets_long_width_hist->Fill(longitudinal_width);
            all_jets_long_width_sigma_hist->Fill(longitudinal_width_sigma);
            
            // Create individual jet cell histogram only for first 20 jets
            if (jets_with_histograms < max_jet_histograms) {
                std::string jet_name = "jet_" + std::to_string(jets_with_histograms + 1) + "_cells";
                std::string jet_title = "Jet " + std::to_string(jets_with_histograms + 1) + 
                                       " Cell Distribution (pT=" + std::to_string(int(TopoJetsPt->at(jetIdx))) + 
                                       " GeV);#eta;#phi";
                TH2F* jet_hist = new TH2F(jet_name.c_str(), jet_title.c_str(), 100, -5.0, 5.0, 100, -4.0, 4.0);
                jet_hist->SetDirectory(0);
                
                // Fill the histogram with cells matched to this jet
                for (size_t k = 0; k < cellE->size(); ++k) {
                    if (cellE->at(k) < energyThreshold) continue;
                    if (cellSignificance->at(k) < 4.0) continue;
                    
                    float cell_eta = cellEta->at(k);
                    float cell_phi = cellPhi->at(k);
                    
                    float dEta = jetEta - cell_eta;
                    float dPhi = jetPhi - cell_phi;
                    
                    if (dPhi >= M_PI) {
                        dPhi -= 2 * M_PI;
                    } else if (dPhi < -M_PI) {
                        dPhi += 2 * M_PI;
                    }
                    
                    float DeltaR = std::sqrt(dEta * dEta + dPhi * dPhi);
                    
                    if (DeltaR < deltaRThreshold) {
                        jet_hist->Fill(cell_eta, cell_phi, cellE->at(k));
                    }
                }
                
                jet_cells_hists.push_back(jet_hist);
                jets_with_histograms++;
            }
            
            jets_found++;
            std::cout << "Found jet " << jets_found << " (pT=" << TopoJetsPt->at(jetIdx) 
                      << " GeV) with " << cellCount << " matched cells";
            if (jets_with_histograms <= max_jet_histograms) {
                std::cout << " - histogram created";
            }
            std::cout << std::endl;
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void event_display_analysis_with_cuts(int minCells = 5, float energyThreshold = 1.0, 
                                      int startIndex = 1, int endIndex = 46,
                                      float jetPtMin = 30.0, float jetPtMax = 1000.0, float deltaRThreshold = 0.3,
                                      float jetWidthMin = 0.17, float jetWidthMax = 0.4, float jetEtaCut = 2.0,
                                      float jetEM1FractionCut = 1.1, float jetEM12FractionCut = 1.1,
                                      float jetLongWidthCut = 10000.0, float jetLongWidthSigmaMin = 0.0,
                                      float jetLongWidthSigmaMax = 10000.0) {
    
    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");
    gInterpreter->GenerateDictionary("vector<pair<float,float> >", "vector;utility");
    
    // Reset global variables
    jets_found = 0;
    jet_cells_hists.clear();
    all_jets_pt.clear();
    all_jets_eta.clear();
    all_jets_phi.clear();
    all_jets_long_width.clear();
    all_jets_long_width_sigma.clear();
    
    // Initialize summary histograms
    initialize_summary_histograms();

    const std::string path = "./SuperNtuple_mu200";
    for (int i = startIndex; i <= endIndex && jets_found < max_jets; ++i) {
        std::ostringstream filename;
        filename << path << "/user.scheong.43348828.Output._" 
                 << std::setw(6) << std::setfill('0') << i 
                 << ".SuperNtuple.root";

        if (std::filesystem::exists(filename.str())) {
            process_file(filename.str(), minCells, energyThreshold, jetPtMin, jetPtMax, deltaRThreshold,
                        jetWidthMin, jetWidthMax, jetEtaCut, jetEM1FractionCut, jetEM12FractionCut,
                        jetLongWidthCut, jetLongWidthSigmaMin, jetLongWidthSigmaMax);
        } else {
            std::cerr << "File does not exist: " << filename.str() << std::endl;
        }
    }

    // Create output file
    std::ostringstream outputFilename;
    outputFilename << "jet_display_with_cuts_minCells" << minCells 
                  << "_Eover" << std::fixed << std::setprecision(1) << energyThreshold 
                  << "_jetPt" << std::setprecision(0) << jetPtMin << "to" << jetPtMax
                  << "_dR" << std::setprecision(1) << deltaRThreshold 
                  << "_" << std::setprecision(0) << jetLongWidthSigmaMin
                  << "_" << std::setprecision(0) << jetLongWidthSigmaMax << ".root";
    
    TFile *outputFile = new TFile(outputFilename.str().c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    // Write jet cell histograms
    for (size_t i = 0; i < jet_cells_hists.size(); ++i) {
        if (jet_cells_hists[i] != nullptr) {
            jet_cells_hists[i]->Write();
        }
    }
    
    // Write summary histograms
    all_jets_eta_hist->Write();
    all_jets_phi_hist->Write();
    all_jets_long_width_hist->Write();
    all_jets_long_width_sigma_hist->Write();

    // Create and write jet information tree
    TTree *jetInfoTree = new TTree("jetInfo", "Selected Jets Information");
    jetInfoTree->Branch("jets_pt", &all_jets_pt);
    jetInfoTree->Branch("jets_eta", &all_jets_eta);
    jetInfoTree->Branch("jets_phi", &all_jets_phi);
    jetInfoTree->Branch("jets_long_width", &all_jets_long_width);
    jetInfoTree->Branch("jets_long_width_sigma", &all_jets_long_width_sigma);
    
    jetInfoTree->Fill();
    jetInfoTree->Write();

    outputFile->Close();
    delete outputFile;

    // Clean up memory
    for (TH2F* hist : jet_cells_hists) {
        delete hist;
    }
    delete all_jets_eta_hist;
    delete all_jets_phi_hist;
    delete all_jets_long_width_hist;
    delete all_jets_long_width_sigma_hist;

    std::cout << "Jet display analysis with cuts completed. Results saved to " << outputFilename.str() << std::endl;
    std::cout << "Found " << jets_found << " jets passing all cuts." << std::endl;
}
