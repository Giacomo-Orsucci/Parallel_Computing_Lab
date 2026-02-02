import os
import subprocess


#Script to execute parallel versions we want to test.

EXECUTABLES = {
    "Simd": "./cmake-build-benchmark/Simd",
}

cell_size = 2
threads_values = [8]#, 2, 4, 8] #
frames = 15000#
#I'm interested in quadratic sizes, but the code is written to handle also other cases.
screen_widths = [200]#400,800,1000,1200]#
screen_heights = [200]#400,800,1000,1200]
scan_size = 3 #3 is the standard for game of life
n_experiments = 1 #
gui = False

def run_benchmarks(exe, width, height, frames, n_threads, csv, cell_size, scan_size, gui):

    env = os.environ.copy()

    cmd = [
        exe,
        "--width", str(width),
        "--height", str(width),
        "--frames", str(frames),
        "--threads", str(n_threads),
        "--csv", str(csv),
        "--cell_size", str(cell_size),
        "--scan_size", str(scan_size),
        "--gui", str(gui)
    ]


    subprocess.run(cmd, env=env, capture_output=True, text=True)

def main():

        for layout, exe in EXECUTABLES.items():
            CSV_OUT = layout + ".csv"
            for width, height in zip(screen_widths, screen_heights):
                for n_threads in threads_values if layout != "Sequential" else [1]:
                    for run_id in range(n_experiments):
                        run_benchmarks(exe, width, height, frames, n_threads, CSV_OUT, cell_size, scan_size, gui)

if __name__ == "__main__":
    main()