import ROOT
import numpy as np
import argparse

# To run this plotting code:
# python plotmu200.py --file histograms.root --layer_type EMB --layer_name EMB1

def plot_histograms(file, layer_type, layer_name):
    layers = {
        "EMB1": ["EMB1_1-1.5", "EMB1_1.5-2", "EMB1_2-3", "EMB1_3-4", "EMB1_4-5", "EMB1_5-10", "EMB1_10-100"],
        "EMB2": ["EMB2_1-1.5", "EMB2_1.5-2", "EMB2_2-3", "EMB2_3-4", "EMB2_4-5", "EMB2_5-10", "EMB2_10-100"],
        "EMB3": ["EMB3_1-1.5", "EMB3_1.5-2", "EMB3_2-3", "EMB3_3-4", "EMB3_4-5", "EMB3_5-10", "EMB3_10-100"],
        "EME1": ["EME1_1-1.5", "EME1_1.5-2", "EME1_2-3", "EME1_3-4", "EME1_4-5", "EME1_5-10", "EME1_10-100"],
        "EME2": ["EME2_1-1.5", "EME2_1.5-2", "EME2_2-3", "EME2_3-4", "EME2_4-5", "EME2_5-10", "EME2_10-100"],
        "EME3": ["EME3_1-1.5", "EME3_1.5-2", "EME3_2-3", "EME3_3-4", "EME3_4-5", "EME3_5-10", "EME3_10-100"]
    }

    hist_names = layers.get(layer_name, None)
    if hist_names is None:
        print(f"Invalid layer name: {layer_name}")
        return
    
    canvas = ROOT.TCanvas("canvas", f"{layer_name} Histograms with Gaussian Fit", 1200, 600)
    canvas.Divide(4, 2)


    for i, hist_name in enumerate(hist_names, start=1):
        pad = canvas.cd(i)
        pad.SetMargin(0.1, 0.1, 0.2, 0.1)
    
        hist = file.Get(hist_name)
        if not hist:
            print(f"Histogram {hist_name} not found in file!")
            continue
    
        hist.Draw()
        fit_result = hist.Fit("gaus", "S")
        fit_function = hist.GetFunction("gaus")
        mu = fit_function.GetParameter(1)
        sigma = fit_function.GetParameter(2)
        fit_function.Draw("SAME")
    
        tex = ROOT.TLatex()
        tex.SetNDC()
        tex.SetTextSize(0.05)
        tex.DrawLatex(0.15, 0.85, f"#mu = {mu:.2f}")
        tex.DrawLatex(0.15, 0.80, f"#sigma = {sigma:.2f}")
    
        canvas.Update()
    
    canvas.SaveAs(f"{layer_name}_Histograms_Combined_HighRes.png")
    print(f"{layer_name} histograms saved to {layer_name}_Histograms_Combined_HighRes.png")

def main():
    parser = argparse.ArgumentParser(description="Plot and fit histograms from ROOT file.")
    parser.add_argument("--file", type=str, default="histograms.root", help="Path to the ROOT file.")
    parser.add_argument("--layer_type", type=str, choices=["EMB", "EME"], required=True, help="Select type of layer: EMB or EME")
    parser.add_argument("--layer_name", type=str, choices=["EMB1", "EMB2", "EMB3", "EME1", "EME2", "EME3"], required=True, help="Layer name.")
    
    args = parser.parse_args()

    root_file = ROOT.TFile.Open(args.file)
    if not root_file or root_file.IsZombie():
        print(f"Error: Could not open {args.file}")
        return

    plot_histograms(root_file, args.layer_type, args.layer_name)
    root_file.Close()

if __name__ == "__main__":
    main()
