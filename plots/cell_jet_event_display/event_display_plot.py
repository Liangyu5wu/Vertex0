import ROOT
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle
import matplotlib.patches as mpatches
from matplotlib.colors import LinearSegmentedColormap

def plot_jet_analysis(root_file_path, jet_number, radius=0.15, figsize=(20, 8)):
    file = ROOT.TFile(root_file_path, "READ")
    
    all_cells_hist = file.Get(f"all_cells_jets{jet_number}")
    jet_matched_hist = file.Get(f"jet_matched_jets{jet_number}")
    
    jets_eta = []
    jets_phi = []
    jets_pt = []
    
    tree = file.Get("jetInfo")
    if tree and tree.GetEntries() > 0:
        tree.GetEntry(0)
        
        pt_branch = getattr(tree, f"jets{jet_number}_pt")
        eta_branch = getattr(tree, f"jets{jet_number}_eta")
        phi_branch = getattr(tree, f"jets{jet_number}_phi")
        
        for i in range(pt_branch.size()):
            jets_pt.append(pt_branch[i])
            jets_eta.append(eta_branch[i])
            jets_phi.append(phi_branch[i])
    
    colors = ['white', '#440154', '#31688e', '#35b779', '#fde725']
    n_bins = 256
    cmap = LinearSegmentedColormap.from_list('custom', colors, N=n_bins)
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=figsize, dpi=300)
    
    def get_hist_data(hist):
        nx = hist.GetNbinsX()
        ny = hist.GetNbinsY()
        x_edges = np.array([hist.GetXaxis().GetBinLowEdge(i) for i in range(1, nx+2)])
        y_edges = np.array([hist.GetYaxis().GetBinLowEdge(i) for i in range(1, ny+2)])
        z = np.zeros((ny, nx))
        cell_count = 0
        max_val = 0
        for i in range(1, nx+1):
            for j in range(1, ny+1):
                content = hist.GetBinContent(i, j)
                z[j-1, i-1] = content
                if content > 0:
                    cell_count += 1
                    max_val = max(max_val, content)
        return x_edges, y_edges, z, cell_count, max_val
    
    x_edges1, y_edges1, z1, cell_count1, max_val1 = get_hist_data(all_cells_hist)
    x_edges2, y_edges2, z2, cell_count2, max_val2 = get_hist_data(jet_matched_hist)
    
    # Use unified color scale
    vmax = max(max_val1, max_val2)
    vmin = 0
    
    im1 = ax1.pcolormesh(x_edges1, y_edges1, z1, cmap=cmap, vmin=vmin, vmax=vmax)
    ax1.set_xlabel('η', fontsize=12)
    ax1.set_ylabel('φ', fontsize=12)
    ax1.set_title(f'All Cells - Jets{jet_number}\nTotal cells: {cell_count1}', fontsize=14, fontweight='bold')
    ax1.grid(True, alpha=0.3)
    plt.colorbar(im1, ax=ax1).set_label('Cell Energy [GeV]', fontsize=12)
    
    im2 = ax2.pcolormesh(x_edges2, y_edges2, z2, cmap=cmap, vmin=vmin, vmax=vmax)
    ax2.set_xlabel('η', fontsize=12)
    ax2.set_ylabel('φ', fontsize=12)
    ax2.set_title(f'Jet Matched Cells - Jets{jet_number}\nTotal cells: {cell_count2}', fontsize=14, fontweight='bold')
    ax2.grid(True, alpha=0.3)
    plt.colorbar(im2, ax=ax2).set_label('Cell Energy [GeV]', fontsize=12)
    
    for i, (eta, phi, pt) in enumerate(zip(jets_eta, jets_phi, jets_pt)):
        circle = Circle((eta, phi), radius, fill=False, edgecolor='red', linewidth=2)
        ax2.add_patch(circle)
        ax2.annotate(f'{pt:.1f} GeV', 
                    xy=(eta, phi), 
                    xytext=(eta + radius*1.5, phi + radius*1.5),
                    fontsize=10, 
                    fontweight='bold',
                    color='red',
                    bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8),
                    arrowprops=dict(arrowstyle='->', color='red', lw=1))
    
    if jets_eta:
        legend_elements = [mpatches.Circle((0, 0), 0.1, facecolor='none', edgecolor='red', linewidth=2, label='Leading pt jets')]
        ax2.legend(handles=legend_elements, loc='upper right')
    
    plt.tight_layout()
    
    output_filename = f"jet_analysis_jets{jet_number}_R{radius:.2f}.png"
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')
    plt.close()
    
    file.Close()
    
    print(f"Jet{jet_number} information:")
    for i, (eta, phi, pt) in enumerate(zip(jets_eta, jets_phi, jets_pt)):
        print(f"  Jet {i+1}: η={eta:.3f}, φ={phi:.3f}, pt={pt:.1f} GeV")
    print(f"All cells: {cell_count1}, Jet matched cells: {cell_count2}")
    print(f"Plot saved as: {output_filename}")

def main():
    root_file_path = "event_display_Eover1.0.root"
    
    while True:
        try:
            jet_num = int(input("Enter jet number (1-8): "))
            if 1 <= jet_num <= 8:
                break
            else:
                print("Please enter a number between 1-8")
        except ValueError:
            print("Please enter a valid number")
    
    while True:
        try:
            radius_input = input("Enter circle radius (default 0.15): ")
            if radius_input == "":
                radius = 0.15
                break
            else:
                radius = float(radius_input)
                break
        except ValueError:
            print("Please enter a valid number")
    
    plot_jet_analysis(root_file_path, jet_num, radius)

if __name__ == "__main__":
    main()
