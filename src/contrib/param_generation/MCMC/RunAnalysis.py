from AnalysisMCMC import MCMCAnalysis

paths = []

#paths.append('fix/previous_ks_down/')
#paths.append('fix/fixed_ksat_regulation/')
paths.append('')

for path in paths:
    MCMCAnalysis(input_path=path, output_path=path)