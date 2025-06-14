#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>

-- root -l
-- .L event_display.C
-- event_display_analysis(2.0, 1, 1)

const double c_light = 0.299792458; // mm/ps

std::vector<int> target_jet_counts = {1, 2, 3, 4, 5, 6, 7, 8};

// Dynamic histograms storage
std::map<int, TH2F*> all_cells_hist_map;
std::map<int, TH2F*> jet_matched_hist_map;
TH2F *all_cells_coverage_hist;

// Dynamic jet information storage
std::map<int, std::vector<float>> jets_pt_map;
std::map<int, std::vector<float>> jets_eta_map;
std::map<int, std::vector<float>> jets_phi_map;

std::map<int, bool> found_events;

void initialize_histograms() {
    const int bins_eta = 100;
    const int bins_phi = 100;
    const double eta_min = -5.0;
    const double eta_max = 5.0;
    const double phi_min = -4.0;
    const double phi_max = 4.0;

    const int bins_eta_all = 100;
    const int bins_phi_all = 100;
    const double eta_min_all = -10.0;
    const double eta_max_all = 10.0;
    const double phi_min_all = -10.0;
    const double phi_max_all = 10.0;
    
    // Create histograms dynamically for each jet count
    for (int jet_count : target_jet_counts) {
        std::string name_all = "all_cells_jets" + std::to_string(jet_count);
        std::string title_all = "All Cells (" + std::to_string(jet_count) + " Jet Event);#eta;#phi";
        all_cells_hist_map[jet_count] = new TH2F(name_all.c_str(), title_all.c_str(), 
                                                 bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);
        
        std::string name_jet = "jet_matched_jets" + std::to_string(jet_count);
        std::string title_jet = "Jet-matched Cells (" + std::to_string(jet_count) + " Jet Event);#eta;#phi";
        jet_matched_hist_map[jet_count] = new TH2F(name_jet.c_str(), title_jet.c_str(), 
                                                   bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);
        
        // Initialize jet info vectors
        jets_pt_map[jet_count] = std::vector<float>();
        jets_eta_map[jet_count] = std::vector<float>();
        jets_phi_map[jet_count] = std::vector<float>();
    }

    all_cells_coverage_hist = new TH2F("all_cells_coverage", "All Cells Coverage Check;#eta;#phi", 
                                       bins_eta_all, eta_min_all, eta_max_all, bins_phi_all, phi_min_all, phi_max_all);
}

void initialize_tracking() {
    for (int count : target_jet_counts) {
        found_events[count] = false;
    }
}

bool all_events_found() {
    for (const auto& pair : found_events) {
        if (!pair.second) return false;
    }
    return true;
}

void process_file(const std::string &filename, int file_index, float energyThreshold = 1.0, float significanceThreshold = 3.0) {
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
    std::vector<float> *cellEta = nullptr;
    std::vector<float> *cellPhi = nullptr;
    std::vector<float> *cellSignificance = nullptr;
    std::vector<float> *TopoJetsPt = nullptr;
    std::vector<float> *TopoJetsEta = nullptr;
    std::vector<float> *TopoJetsPhi = nullptr;
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
    tree->SetBranchAddress("Cell_eta", &cellEta);
    tree->SetBranchAddress("Cell_phi", &cellPhi);
    tree->SetBranchAddress("Cell_significance", &cellSignificance);
    tree->SetBranchAddress("AntiKt4EMTopoJets_pt", &TopoJetsPt);
    tree->SetBranchAddress("AntiKt4EMTopoJets_eta", &TopoJetsEta);
    tree->SetBranchAddress("AntiKt4EMTopoJets_phi", &TopoJetsPhi);
    tree->SetBranchAddress("AntiKt4EMTopoJets_truthHSJet_idx", &TopoJets_TruthHSJetIdx);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries && !all_events_found(); ++entry) {
        tree->GetEntry(entry);

        for (size_t j = 0; j < cellE->size(); ++j) {
            float cell_eta = cellEta->at(j);
            float cell_phi = cellPhi->at(j);
            all_cells_coverage_hist->Fill(cell_eta, cell_phi, cellE->at(j));
        }

        std::vector<float> selectedJetPt;
        std::vector<float> selectedJetEta;
        std::vector<float> selectedJetPhi;

        for (size_t j = 0; j < TopoJetsPt->size(); ++j) {
            bool isHighPt = (TopoJetsPt->at(j) > 30);
            bool hasMatch = (j < TopoJets_TruthHSJetIdx->size() && !TopoJets_TruthHSJetIdx->at(j).empty());
            
            if (isHighPt && hasMatch) {
                selectedJetPt.push_back(TopoJetsPt->at(j));
                selectedJetEta.push_back(TopoJetsEta->at(j));
                selectedJetPhi.push_back(TopoJetsPhi->at(j));
            }
        }

        int jet_count = selectedJetPt.size();
        
        if (std::find(target_jet_counts.begin(), target_jet_counts.end(), jet_count) != target_jet_counts.end() 
            && !found_events[jet_count]) {
            
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

            found_events[jet_count] = true;
            std::cout << "Found event with " << jet_count << " jets: File " << file_index 
                      << ", Event " << entry << std::endl;

            // Store jet information dynamically
            for (size_t k = 0; k < selectedJetPt.size(); ++k) {
                jets_pt_map[jet_count].push_back(selectedJetPt[k]);
                jets_eta_map[jet_count].push_back(selectedJetEta[k]);
                jets_phi_map[jet_count].push_back(selectedJetPhi[k]);
            }

            // Get corresponding histograms
            TH2F* hist_all = all_cells_hist_map[jet_count];
            TH2F* hist_jet = jet_matched_hist_map[jet_count];

            for (size_t j = 0; j < cellE->size(); ++j) {
                if (cellE->at(j) < energyThreshold) continue;
                if (cellSignificance->at(j) < significanceThreshold) continue;

                float cell_eta = cellEta->at(j);
                float cell_phi = cellPhi->at(j);
                
                if (hist_all) hist_all->Fill(cell_eta, cell_phi, cellE->at(j));
                
                bool isCloseToJet = false;
                for (size_t jetIdx = 0; jetIdx < selectedJetEta.size(); ++jetIdx) {
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
                    
                    if (DeltaR < 0.3) {
                        isCloseToJet = true;
                        break;
                    }
                }

                if (isCloseToJet && hist_jet) {
                    hist_jet->Fill(cell_eta, cell_phi, cellE->at(j));
                }
            }
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void event_display_analysis(float energyThreshold = 1.0, float significanceThreshold = 3.0, int startIndex = 1, int endIndex = 46) {
    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");
    
    initialize_tracking();
    initialize_histograms();

    const std::string path = "./SuperNtuple_mu200";
    for (int i = startIndex; i <= endIndex && !all_events_found(); ++i) {
        std::ostringstream filename;
        filename << path << "/user.scheong.43348828.Output._" 
                 << std::setw(6) << std::setfill('0') << i 
                 << ".SuperNtuple.root";

        if (std::filesystem::exists(filename.str())) {
            process_file(filename.str(), i, energyThreshold, significanceThreshold);
        } else {
            std::cerr << "File does not exist: " << filename.str() << std::endl;
        }
    }

    std::ostringstream outputFilename;
    outputFilename << "event_display_Eover" << std::fixed << std::setprecision(1) << energyThreshold << ".root";
    
    TFile *outputFile = new TFile(outputFilename.str().c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    // Write histograms dynamically
    for (const auto& pair : all_cells_hist_map) {
        pair.second->Write();
    }
    
    for (const auto& pair : jet_matched_hist_map) {
        pair.second->Write();
    }

    all_cells_coverage_hist->Write();

    // Create and write jet information tree dynamically
    TTree *jetInfoTree = new TTree("jetInfo", "Selected Jets Information");
    
    for (int jet_count : target_jet_counts) {
        std::string branch_name_pt = "jets" + std::to_string(jet_count) + "_pt";
        std::string branch_name_eta = "jets" + std::to_string(jet_count) + "_eta";
        std::string branch_name_phi = "jets" + std::to_string(jet_count) + "_phi";
        
        jetInfoTree->Branch(branch_name_pt.c_str(), &jets_pt_map[jet_count]);
        jetInfoTree->Branch(branch_name_eta.c_str(), &jets_eta_map[jet_count]);
        jetInfoTree->Branch(branch_name_phi.c_str(), &jets_phi_map[jet_count]);
    }
    
    jetInfoTree->Fill();
    jetInfoTree->Write();

    outputFile->Close();
    delete outputFile;

    // Clean up memory dynamically
    for (const auto& pair : all_cells_hist_map) {
        delete pair.second;
    }
    all_cells_hist_map.clear();
    
    for (const auto& pair : jet_matched_hist_map) {
        delete pair.second;
    }
    jet_matched_hist_map.clear();

    delete all_cells_coverage_hist;

    std::cout << "Event display analysis completed. Results saved to " << outputFilename.str() << std::endl;
    
    // Print summary
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Events found for each jet count:" << std::endl;
    for (int count : target_jet_counts) {
        std::cout << "  " << count << " jets: " << (found_events[count] ? "Found" : "Not found") << std::endl;
        if (found_events[count]) {
            std::cout << "    Total jets stored: " << jets_pt_map[count].size() << std::endl;
        }
    }
}
