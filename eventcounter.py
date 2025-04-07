import ROOT
import os
import glob

def count_events(directory, file_pattern):
    total_events = 0

    files = glob.glob(os.path.join(directory, file_pattern))
    
    for root_file in files:
        file = ROOT.TFile.Open(root_file)
        if not file or file.IsZombie():
            print(f"Error: Could not open {root_file}")
            continue
        
        tree = file.Get("ntuple")
        if not tree:
            print(f"Error: Could not get 'ntuple' tree from {root_file}")
            file.Close()
            continue
        
        n_entries = tree.GetEntries()
        print(f"File: {root_file}, Events: {n_entries}") 
        total_events += n_entries

        file.Close()

    return total_events

def main():
    directory = "."
    file_pattern = "user.scheong.43348828.Output._*.SuperNtuple.root"

    total_events = count_events(directory, file_pattern)
    print(f"Total events in all ROOT files: {total_events}")

if __name__ == "__main__":
    main()
