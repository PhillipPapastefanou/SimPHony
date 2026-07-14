import os
import glob
import pandas as pd

# Paths provided
root_output_directory = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/"
scenario_name = "45-2_LHS_no_wcontvar_full"
scenario_name = "45-3_LHS_no_gstem_full"
scenario_name = "45_LHS_TLP"

input_path = os.path.join(root_output_directory, scenario_name, 'input')
output_path = os.path.join(root_output_directory, scenario_name, 'output')
post_path = os.path.join(root_output_directory, scenario_name, 'post')

vars_all = ['psi_leaf_50_close', 'd_50_close', 'stem_hydraulic_capacitance', 'k_xylem_sat', 'huber_value',
        'leaf_hydraulic_capacitance', 'psi50_xylem', 'psi88_xylem', 'g0', 'k_soil_sat0', 'psi_soil_sat0', 'jackson_root_beta',
        'pore_0', 'root_area_index', 'wcont_sigma_deviation', 'alpha', 'g_stem_res','gam_stem_x_H','k_xyl_x_H', 
        'g0_div_gam_stem_x_H', 'g_min_loss', 'p50-p88', 'g1', 'psi_tlp']

def combine_parameter_files():
    # 1. Create a search pattern for all files like 'parameters.csv0', 'parameters.csv1', etc.
    search_pattern = os.path.join(input_path, 'parameters.csv*')
    
    # glob finds all file paths matching the pattern
    file_paths = glob.glob(search_pattern)
    
    print(f"Found {len(file_paths)} files to combine.")
    
    if not file_paths:
        print("No files found. Please check your path.")
        return

    # 2. Loop through the files and read them into a list of DataFrames
    df_list = []
    i = 0
    for file in file_paths:
        try:
            # Note: If your CSVs don't have headers, you should use:
            # df = pd.read_csv(file, header=None, names=vars_all)
            df = pd.read_csv(file)
            df_list.append(df)
            print(i)
            i+=1
        except Exception as e:
            print(f"Could not read {file}: {e}")
            
    # 3. Concatenate all the individual DataFrames into one massive DataFrame
    print("Concatenating files...")
    combined_df = pd.concat(df_list, ignore_index=True)
    
    # Optional: If you want to force the dataframe to only keep the columns defined in `vars_all` 
    # (just in case some files have extra weird columns), you can uncomment the next line:
    # combined_df = combined_df[vars_all]
    
    # 4. Save the combined file
    # We will save it to the post_path (or you can change it to input_path)
    # Ensure the post directory exists
    os.makedirs(post_path, exist_ok=True)
    
    save_path = os.path.join(post_path, 'parameters_combined_all.csv')
    
    combined_df.to_csv(save_path, index=False)
    
    print(f"Success! Combined {len(combined_df)} total rows.")
    print(f"Saved to: {save_path}")

# Run the function
if __name__ == "__main__":
    combine_parameter_files()