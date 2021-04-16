#! /usr/bin/python3

import os
import sys
import itertools

heuristics = ["basic", "a_basic", "board_aware"]
depth_range = range(1, 9)

def run_games(executable):
    try:
        os.mkdir("match_results")
    except FileExistsError:
        pass

    try:
        os.mkdir("results")
    except FileExistsError:
        pass

    if executable == None:
        if sys.platform.startswith("win"):
            executable = "powershell.exe ./bin/pszt_warcaby.exe"
        else:
            executable = "./bin/pszt_warcaby"
    
    base_params = " --wbot true --bbot true --gui false --log ./match_results/"
    
    games = get_test_games()
    
    for game in games:
        command = executable + base_params + game["name"] + " " + game["params"]
        os.system(command)
        print("played game " + game["name"])
        process_log("./match_results/" + game["name"], "results/" + game["name"])

    
    if sys.platform.startswith("win"):
        os.system("powershell.exe rm -r match_results")
    else:
        os.system("rm -rf match_results")


def get_test_games():
    games = []
    for i in depth_range:
        for h_permut in itertools.permutations(heuristics):
            h1 = h_permut[0]
            h2 = h_permut[1]

            games.append({
                "name": "%s-%d-vs-%s-%d.txt"%(h1,i,h2,i),
                "params": "--wheuristic %s --wdepth %d --bheuristic %s --bdepth %d"%(h1,i,h2,i)
            })

    return games


def process_log(in_path, out_path):
    inf = open(in_path, "r")
    log_lines = inf.read().split('\n')
    inf.close()

    param_white = log_lines[0]
    param_black = log_lines[1]
    end_line = log_lines[len(log_lines) - 2]

    sum_time_white = 0
    white_moves = 0
    sum_time_black = 0
    black_moves = 0

    for line in log_lines:
        elems = line.split(' ')
        if elems[0] == "white":
            white_moves += 1
            sum_time_white += int(elems[1])
        elif elems[0] == "black":
            black_moves += 1
            sum_time_black += int(elems[1])

    outf = open(out_path, "w")
    outf.write(param_white + '\n' + param_black + '\n')
    outf.write("white_average_move_time " + str(sum_time_white / white_moves) + ' µs\n')
    outf.write("black_average_move_time " + str(sum_time_black / black_moves) + ' µs\n')
    outf.write(end_line + '\n')
    outf.close()


if __name__ == "__main__":
    exec = None
    if (len(sys.argv) == 2):
        exec = sys.argv[1]

    run_games(exec)
