import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

#data we want to plot
data_seq = "./new/Seq.csv"
data_compare = "./new/Simd.csv"

csv_header = ['SCREEN_WIDTH', 'SCREEN_HEIGHT', 'ROWS', 'COLS', 'SCAN_SIZE', 'FRAMES', 'THREADS', 'time_ms']

def load_data(filepath):
    # skipinitialspace=True to remove blank spaces in header
    df = pd.read_csv(filepath, comment='#', header=None, names=csv_header, skipinitialspace=True)

   #to handle correctly numerical values
    cols_to_fix = ['ROWS', 'THREADS', 'time_ms']
    for col in cols_to_fix:
        df[col] = pd.to_numeric(df[col], errors='coerce')

   #to drop nan
    df = df.dropna(subset=cols_to_fix)
    return df


df_seq = load_data(data_seq)
df_compare = load_data(data_compare)

# I did 6 run per experiment, discarding the first, but this function is built
# to discard the first run only if we have more than 1.
def get_means(df, group_cols):

    if df.groupby(group_cols)['time_ms'].count().min() > 1:
        df['run_idx'] = df.groupby(group_cols).cumcount()
        df = df[df['run_idx'] > 0]
    return df.groupby(group_cols)['time_ms'].mean()

mean_seq = get_means(df_seq, ['ROWS'])
mean_aos = get_means(df_compare, ['ROWS', 'THREADS'])

speedup_data = []

for (rows, threads), t_par in mean_aos.items():
    if rows in mean_seq.index:
        t_seq = mean_seq.loc[rows]
        speedup = t_seq / t_par if t_par > 0 else 0
        speedup_data.append({'ROWS': rows, 'THREADS': threads, 'speedup': speedup})

df_speedup = pd.DataFrame(speedup_data)



if not df_speedup.empty:
    plt.figure(figsize=(10, 6))

    for threads in sorted(df_speedup['THREADS'].unique()):

        sub = df_speedup[df_speedup['THREADS'] == threads].sort_values('ROWS')
        plt.plot(sub['ROWS'], sub['speedup'], marker='o', label=f"{int(threads)} threads")

    plt.grid(True, which="both", linestyle='--', alpha=0.6)
    plt.xlabel("Rows")
    plt.ylabel("Speedup")
    plt.title("Parallel_SIMD vs Sequential")
    plt.legend()
    plt.show()
