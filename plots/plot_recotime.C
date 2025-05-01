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

void plotAndFitHistograms() {

    gStyle->SetOptStat(0);      
    gStyle->SetOptFit(0);    
    
    // TFile *file = new TFile("HSonly_reconstruction_2GeV.root", "READ");
    TFile *file = new TFile("PUremoved_reconstruction_2GeV.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file event_time_reconstruction.root" << std::endl;
        return;
    }
    
    TH1 *eventTimeHist = (TH1*)file->Get("eventDeltaTime");
    TH1 *truthTimeHist = (TH1*)file->Get("embDeltaTime");
    // TH1 *eventTimeHist = (TH1*)file->Get("eventTime");
    // TH1 *truthTimeHist = (TH1*)file->Get("embTime");
    
    if (!eventTimeHist || !truthTimeHist) {
        std::cerr << "Error: Cannot find one or both histograms in the file" << std::endl;
        file->Close();
        return;
    }
    
    TH1 *eventTimeClone = (TH1*)eventTimeHist->Clone("eventTimeClone");
    TH1 *truthTimeClone = (TH1*)truthTimeHist->Clone("truthTimeClone");
    
    TCanvas *canvas = new TCanvas("canvas", "Delta t0", 900, 600);
    // TCanvas *canvas = new TCanvas("canvas", "Reconstruction Time", 900, 600);
    canvas->SetGrid();
    
    eventTimeClone->SetLineColor(kBlue);
    eventTimeClone->SetLineWidth(2);
    eventTimeClone->SetFillColor(kBlue-10);
    eventTimeClone->SetFillStyle(3004);
    
    truthTimeClone->SetLineColor(kRed);
    truthTimeClone->SetLineWidth(2);
    truthTimeClone->SetFillColor(kRed-10);
    truthTimeClone->SetFillStyle(3005);
    
    eventTimeClone->SetTitle("Delta t0;Time (ps);Entries");
    // eventTimeClone->SetTitle("Reconstruction Time;Time (ps);Entries");
    
    double eventTimeMax = eventTimeClone->GetMaximum();
    double truthTimeMax = truthTimeClone->GetMaximum();
    double maxY = std::max(eventTimeMax, truthTimeMax) * 1.1; 
    
    eventTimeClone->SetMaximum(maxY);
    
    double fitMin = -120;
    double fitMax = 120;
    
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
    
    fitInfo->Draw();
    
    TLegend *legend = new TLegend(0.65, 0.75, 0.89, 0.89);
    legend->AddEntry(eventTimeClone, "All layers Delta t0", "lf");
    legend->AddEntry(truthTimeClone, "EMB-only Delta t0", "lf");
    // legend->AddEntry(eventTimeClone, "All layers Reconstruction Time", "lf");
    // legend->AddEntry(truthTimeClone, "EMB-only Reconstruction Time", "lf");
    legend->AddEntry(eventTimeFit, "All layers Fit", "l");
    legend->AddEntry(truthTimeFit, "EMB-only Fit", "l");
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();
    
    canvas->Update();
    
    // canvas->SaveAs("embonly_reco_time.png");
    // canvas->SaveAs("embonly_Delta_t0.png");
    // canvas->SaveAs("embonly_reco_time_PUremoved.png");
    canvas->SaveAs("embonly_Delta_t0_PUremoved.png");
    
    std::cout << "Event Time Fit Results (range: -1000 to 500 ps):" << std::endl;
    std::cout << "  Mean = " << eventTimeFit->GetParameter(1) << " ± " << eventTimeFit->GetParError(1) << " ns" << std::endl;
    std::cout << "  Sigma = " << eventTimeFit->GetParameter(2) << " ± " << eventTimeFit->GetParError(2) << " ns" << std::endl;
    
    std::cout << "\nTruth Time Fit Results (range: -1000 to 500 ps):" << std::endl;
    std::cout << "  Mean = " << truthTimeFit->GetParameter(1) << " ± " << truthTimeFit->GetParError(1) << " ns" << std::endl;
    std::cout << "  Sigma = " << truthTimeFit->GetParameter(2) << " ± " << truthTimeFit->GetParError(2) << " ns" << std::endl;
    
    file->Close();
    delete file;
}

int main() {
    plotAndFitHistograms();
    return 0;
}
