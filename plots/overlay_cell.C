#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <algorithm>
#include <string>

// compareEventCellHistograms("HSonly_reconstruction_Eover1.0.root","HSonly_PUcells_removed_reco_Eover1.root", "eventCell", "HS-only", "HS-only&PU cells removed", "eventCell_comp.png", 0, 300)
// compareEventCellHistograms("HSonly_reconstruction_Eover1.0.root","HSonly_PUcells_removed_reco_Eover1.root", "embCell", "HS-only", "HS-only&PU cells removed", "embCell_comp.png", 0, 100)
void compareEventCellHistograms(
    const char* file1Name = "file1.root",
    const char* file2Name = "file2.root",
    const char* histName = "eventCell",
    const char* label1 = "File 1",
    const char* label2 = "File 2",
    const char* outputFileName = "eventCell_comparison.png",
    double xMin = 0,
    double xMax = 300
) {
    // Turn off statistics box
    gStyle->SetOptStat(0);
    
    // Open the first file
    TFile *file1 = new TFile(file1Name, "READ");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "Error: Cannot open file: " << file1Name << std::endl;
        return;
    }
    
    // Open the second file
    TFile *file2 = new TFile(file2Name, "READ");
    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error: Cannot open file: " << file2Name << std::endl;
        file1->Close();
        delete file1;
        return;
    }
    
    // Get histograms from files
    TH1 *hist1 = (TH1*)file1->Get(histName);
    if (!hist1) {
        std::cerr << "Error: Cannot find histogram '" << histName << "' in file: " << file1Name << std::endl;
        file1->Close();
        file2->Close();
        delete file1;
        delete file2;
        return;
    }
    
    TH1 *hist2 = (TH1*)file2->Get(histName);
    if (!hist2) {
        std::cerr << "Error: Cannot find histogram '" << histName << "' in file: " << file2Name << std::endl;
        file1->Close();
        file2->Close();
        delete file1;
        delete file2;
        return;
    }
    
    // Create clones of the histograms to avoid modifying the originals
    TH1 *hist1Clone = (TH1*)hist1->Clone("hist1Clone");
    TH1 *hist2Clone = (TH1*)hist2->Clone("hist2Clone");
    
    // Create a canvas
    TCanvas *canvas = new TCanvas("canvas", "Histogram Comparison", 900, 600);
    canvas->SetGrid();
    
    // Set different colors and styles for the histograms
    hist1Clone->SetLineColor(kBlue);
    hist1Clone->SetLineWidth(2);
    hist1Clone->SetFillColor(kBlue-10);
    hist1Clone->SetFillStyle(3004);
    
    hist2Clone->SetLineColor(kRed);
    hist2Clone->SetLineWidth(2);
    hist2Clone->SetFillColor(kRed-10);
    hist2Clone->SetFillStyle(3005);
    
    // Set the title
    hist1Clone->SetTitle(Form("%s Comparison;%s;Entries", histName, hist1Clone->GetXaxis()->GetTitle()));
    
    // Find the maximum y value for proper scaling
    double max1 = hist1Clone->GetMaximum();
    double max2 = hist2Clone->GetMaximum();
    double maxY = std::max(max1, max2) * 1.1; // Add 10% margin
    
    hist1Clone->SetMaximum(maxY);
    
    // Set custom X axis range if specified
    if (xMin != -999 && xMax != -999) {
        hist1Clone->GetXaxis()->SetRangeUser(xMin, xMax);
        hist2Clone->GetXaxis()->SetRangeUser(xMin, xMax);
        std::cout << "Setting X axis range: [" << xMin << ", " << xMax << "]" << std::endl;
    }
    
    // Draw the histograms
    hist1Clone->Draw();
    hist2Clone->Draw("SAME");
    
    // Create a legend
    TLegend *legend = new TLegend(0.7, 0.75, 0.89, 0.89);
    legend->AddEntry(hist1Clone, label1, "lf");
    legend->AddEntry(hist2Clone, label2, "lf");
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();
    
    // Add text showing mean and RMS for both histograms
    TPaveText *statsText = new TPaveText(0.65, 0.55, 0.89, 0.75, "NDC");
    statsText->SetBorderSize(0);
    statsText->SetFillColor(0);
    statsText->SetTextAlign(12);
    statsText->SetTextSize(0.03);
    
    // Add stats for first histogram
    TText *title1 = statsText->AddText(Form("%s statistics:", label1));
    title1->SetTextColor(kBlue);
    title1->SetTextFont(42);
    
    char mean1[100];
    sprintf(mean1, "Mean = %.2f", hist1Clone->GetMean());
    TText *mean1Text = statsText->AddText(mean1);
    mean1Text->SetTextColor(kBlue);
    
    char rms1[100];
    sprintf(rms1, "RMS = %.2f", hist1Clone->GetRMS());
    TText *rms1Text = statsText->AddText(rms1);
    rms1Text->SetTextColor(kBlue);
    
    // Add stats for second histogram
    TText *title2 = statsText->AddText(Form("%s statistics:", label2));
    title2->SetTextColor(kRed);
    title2->SetTextFont(42);
    
    char mean2[100];
    sprintf(mean2, "Mean = %.2f", hist2Clone->GetMean());
    TText *mean2Text = statsText->AddText(mean2);
    mean2Text->SetTextColor(kRed);
    
    char rms2[100];
    sprintf(rms2, "RMS = %.2f", hist2Clone->GetRMS());
    TText *rms2Text = statsText->AddText(rms2);
    rms2Text->SetTextColor(kRed);
    
    statsText->Draw();
    
    // Update and save the canvas
    canvas->Update();
    canvas->SaveAs(outputFileName);
    
    // Print some statistics
    std::cout << "Histogram statistics:" << std::endl;
    std::cout << label1 << ": Entries = " << hist1Clone->GetEntries() 
              << ", Mean = " << hist1Clone->GetMean() 
              << ", RMS = " << hist1Clone->GetRMS() << std::endl;
    
    std::cout << label2 << ": Entries = " << hist2Clone->GetEntries() 
              << ", Mean = " << hist2Clone->GetMean() 
              << ", RMS = " << hist2Clone->GetRMS() << std::endl;
    
    // Clean up
    file1->Close();
    file2->Close();
    delete file1;
    delete file2;
    
    std::cout << "Comparison completed. Output saved to " << outputFileName << std::endl;
}
