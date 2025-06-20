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

TH1F *jetWidthHist;

int totalTruthVertices = 0;
int unmatchedVertices = 0;

void initialize_histograms() {
    jetWidthHist = new TH1F("jetWidth", "Selected Jets Width Distribution", 100, 0, 0.4);
    jetWidthHist->GetXaxis()->SetTitle("Jet Width");
    jetWidthHist->GetYaxis()->SetTitle("Jets");
}

void process_file(const std::string &filename, float jetPtThreshold = 30.0, int maxJets = -1) {
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
    tree->SetBranchAddress("AntiKt4EMTopoJets_pt", &TopoJetsPt);
    tree->SetBranchAddress("AntiKt4EMTopoJets_eta", &TopoJetsEta);
    tree->SetBranchAddress("AntiKt4EMTopoJets_phi", &TopoJetsPhi);
    tree->SetBranchAddress("AntiKt4EMTopoJets_width", &TopoJetsWidth);
    tree->SetBranchAddress("AntiKt4EMTopoJets_truthHSJet_idx", &TopoJets_TruthHSJetIdx);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        std::vector<float> selectedJetPt;
        std::vector<float> selectedJetEta;
        std::vector<float> selectedJetPhi;
        std::vector<float> selectedJetWidth;

        std::vector<std::tuple<float, float, float, float>> candidateJets;

        for (size_t j = 0; j < TopoJetsPt->size(); ++j) {
            bool isHighPt = (TopoJetsPt->at(j) > jetPtThreshold);
            bool hasMatch = (j < TopoJets_TruthHSJetIdx->size() && !TopoJets_TruthHSJetIdx->at(j).empty());
            
            if (isHighPt && hasMatch) {
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

            float vtx_x = truthVtxX->at(i);
            float vtx_y = truthVtxY->at(i);
            float vtx_z = truthVtxZ->at(i);

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

            // Fill jet width histogram for events that pass all cuts
            for (size_t k = 0; k < selectedJetWidth.size(); ++k) {
                jetWidthHist->Fill(selectedJetWidth[k]);
            }
            
            break; // Only process the first valid truth vertex per event
        }
    }

    file->Close();
    delete file;
    std::cout << "Processed file: " << filename << std::endl;
}

void processmu200_jetwidth_analysis(int startIndex = 1, int endIndex = 46, 
                                   float jetPtThreshold = 30.0, int maxJets = -1) {

    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
    gInterpreter->GenerateDictionary("vector<vector<int> >", "vector");
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
            process_file(filename.str(), jetPtThreshold, maxJets);
        } else {
            std::cerr << "File does not exist: " << filename.str() << std::endl;
        }
    }

    std::cout << "Statistical Summary:" << std::endl;
    std::cout << "Total Truth Vertices: " << totalTruthVertices << std::endl;
    std::cout << "Unmatched Vertices: " << unmatchedVertices << std::endl;
    std::cout << "Matching Rate: " << (100.0 * (totalTruthVertices - unmatchedVertices) / totalTruthVertices) << "%" << std::endl;

    std::ostringstream outputFilename;
    outputFilename << "jetwidth_analysis_jetPt" << jetPtThreshold;
    if (maxJets > 0) {
        outputFilename << "_maxJets" << maxJets;
    }
    outputFilename << ".root";

    TFile *outputFile = new TFile(outputFilename.str().c_str(), "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error creating output file" << std::endl;
        return;
    }

    jetWidthHist->Write();

    outputFile->Close();

    delete outputFile;
    delete jetWidthHist;

    std::cout << "Jet width analysis completed. Results saved to " << outputFilename.str() << std::endl;

    std::cout << "Parameters used: " << std::endl;
    std::cout << "  Jet pT threshold: " << jetPtThreshold << std::endl;
    std::cout << "  Max jets per event: " << (maxJets > 0 ? std::to_string(maxJets) : "all") << std::endl;
    
}
