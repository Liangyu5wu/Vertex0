#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TStyle.h>
#include <algorithm>
#include <TPaveText.h>
#include <TMath.h>
#include <TLatex.h>
#include <string>

void plotAndFitHistograms(
    int mode = 0,                                         // 0 = Delta t0, 1 = Reconstruction Time
    const char* inputFile = "HSonly_PUcells_removed_reco_Eover1.root", // Input ROOT file
    const char* outputBase = "HSonly_PUremoved",          // Base name for output file
    double fitMin = -120,                                 // Lower bound for fit range
    double fitMax = 120                                   // Upper bound for fit range
) {
    bool isDeltaT0 = (mode == 0);
    
    gStyle->SetOptStat(0);      
    gStyle->SetOptFit(0);    
    
    TFile *file = new TFile(inputFile, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file " << inputFile << std::endl;
        return;
    }
    
    const char* histName1 = isDeltaT0 ? "eventDeltaTime" : "eventTime";
    const char* histName2 = isDeltaT0 ? "embDeltaTime" : "embTime";
    
    TH1 *eventTimeHist = (TH1*)file->Get(histName1);
    TH1 *truthTimeHist = (TH1*)file->Get(histName2);
    
    if (!eventTimeHist || !truthTimeHist) {
        std::cerr << "Error: Cannot find one or both histograms in the file" << std::endl;
        file->Close();
        return;
    }
    
    TH1 *eventTimeClone = (TH1*)eventTimeHist->Clone("eventTimeClone");
    TH1 *truthTimeClone = (TH1*)truthTimeHist->Clone("truthTimeClone");
    
    const char* canvasTitle = isDeltaT0 ? "Delta t0" : "Reconstruction Time";
    TCanvas *canvas = new TCanvas("canvas", canvasTitle, 900, 600);
    canvas->SetGrid();
    
    eventTimeClone->SetLineColor(kBlue);
    eventTimeClone->SetLineWidth(2);
    eventTimeClone->SetFillColor(kBlue-10);
    eventTimeClone->SetFillStyle(3004);
    
    truthTimeClone->SetLineColor(kRed);
    truthTimeClone->SetLineWidth(2);
    truthTimeClone->SetFillColor(kRed-10);
    truthTimeClone->SetFillStyle(3005);
    
    const char* plotTitle = isDeltaT0 ? "Delta t0;Time (ps);Entries" : "Reconstruction Time;Time (ps);Entries";
    eventTimeClone->SetTitle(plotTitle);
    
    double eventTimeMax = eventTimeClone->GetMaximum();
    double truthTimeMax = truthTimeClone->GetMaximum();
    double maxY = std::max(eventTimeMax, truthTimeMax) * 1.1; 
    
    eventTimeClone->SetMaximum(maxY);
    
    TF1 *eventTimeFit = new TF1("eventTimeFit", "gaus", fitMin, fitMax);
    TF1 *truthTimeFit = new TF1("truthTimeFit", "gaus", fitMin, fitMax);
    
    eventTimeFit->SetLineColor(kBlue);
    eventTimeFit->SetLineWidth(2);
    eventTimeFit->SetLineStyle(2); 
    
    truthTimeFit->SetLineColor(kRed);
    truthTimeFit->SetLineWidth(2);
    truthTimeFit->SetLineStyle(2); 
    
    eventTimeClone->Draw();
    truthTimeClone->Draw("SAME");
    
    eventTimeClone->Fit(eventTimeFit, "RQ"); 
    truthTimeClone->Fit(truthTimeFit, "RQ+"); 
    
    eventTimeFit->Draw("SAME");
    truthTimeFit->Draw("SAME");

    TPaveText *fitInfo = new TPaveText(0.65, 0.55, 0.89, 0.75, "NDC");
    fitInfo->SetBorderSize(0);
    fitInfo->SetFillColor(0);
    fitInfo->SetTextAlign(12);
    fitInfo->SetTextSize(0.03);

    TText *emb1Title = fitInfo->AddText("All layers Fit:");
    emb1Title->SetTextColor(kBlue);
    emb1Title->SetTextFont(42);
    
    char emb1Mean[100];
    sprintf(emb1Mean, "Mean = %.2f #pm %.2f ps", eventTimeFit->GetParameter(1), eventTimeFit->GetParError(1));
    TText *emb1MeanText = fitInfo->AddText(emb1Mean);
    emb1MeanText->SetTextColor(kBlue);
    
    char emb1Sigma[100];
    sprintf(emb1Sigma, "Sigma = %.2f #pm %.2f ps", eventTimeFit->GetParameter(2), eventTimeFit->GetParError(2));
    TText *emb1SigmaText = fitInfo->AddText(emb1Sigma);
    emb1SigmaText->SetTextColor(kBlue);
    
    TText *eme1Title = fitInfo->AddText("EMB-only Fit:");
    eme1Title->SetTextColor(kRed);
    eme1Title->SetTextFont(42);
    
    char eme1Mean[100];
    sprintf(eme1Mean, "Mean = %.2f #pm %.2f ps", truthTimeFit->GetParameter(1), truthTimeFit->GetParError(1));
    TText *eme1MeanText = fitInfo->AddText(eme1Mean);
    eme1MeanText->SetTextColor(kRed);
    
    char eme1Sigma[100];
    sprintf(eme1Sigma, "Sigma = %.2f #pm %.2f ps", truthTimeFit->GetParameter(2), truthTimeFit->GetParError(2));
    TText *eme1SigmaText = fitInfo->AddText(eme1Sigma);
    eme1SigmaText->SetTextColor(kRed);
    
    char fitRangeText[100];
    sprintf(fitRangeText, "Fit Range: [%.0f, %.0f] ps", fitMin, fitMax);
    TText *fitRangeInfo = fitInfo->AddText(fitRangeText);
    fitRangeInfo->SetTextColor(kBlack);
    
    fitInfo->Draw();
    
    TLegend *legend = new TLegend(0.65, 0.75, 0.89, 0.89);
    
    if (isDeltaT0) {
        legend->AddEntry(eventTimeClone, "All layers Delta t0", "lf");
        legend->AddEntry(truthTimeClone, "EMB-only Delta t0", "lf");
    } else {
        legend->AddEntry(eventTimeClone, "All layers Reconstruction Time", "lf");
        legend->AddEntry(truthTimeClone, "EMB-only Reconstruction Time", "lf");
    }
    
    legend->AddEntry(eventTimeFit, "All layers Fit", "l");
    legend->AddEntry(truthTimeFit, "EMB-only Fit", "l");
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();
    
    canvas->Update();
    
    std::string outputFile;
    
    if (isDeltaT0) {
        outputFile = "embonly_Delta_t0_" + std::string(outputBase) + ".png";
    } else {
        outputFile = "embonly_reco_time_" + std::string(outputBase) + ".png";
    }
    
    canvas->SaveAs(outputFile.c_str());
    
    const char* resultType = isDeltaT0 ? "Delta t0" : "Reconstruction Time";
    std::cout << "Event " << resultType << " Fit Results (range: " << fitMin << " to " << fitMax << " ps):" << std::endl;
    std::cout << "  Mean = " << eventTimeFit->GetParameter(1) << " ± " << eventTimeFit->GetParError(1) << " ps" << std::endl;
    std::cout << "  Sigma = " << eventTimeFit->GetParameter(2) << " ± " << eventTimeFit->GetParError(2) << " ps" << std::endl;
    
    std::cout << "\nEMB-only " << resultType << " Fit Results (range: " << fitMin << " to " << fitMax << " ps):" << std::endl;
    std::cout << "  Mean = " << truthTimeFit->GetParameter(1) << " ± " << truthTimeFit->GetParError(1) << " ps" << std::endl;
    std::cout << "  Sigma = " << truthTimeFit->GetParameter(2) << " ± " << truthTimeFit->GetParError(2) << " ps" << std::endl;
    
    file->Close();
    delete file;
}
