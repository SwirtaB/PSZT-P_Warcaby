#!/bin/python

import os
import sys

def run_games():
    try:
        os.mkdir("match_results")
    except FileExistsError:
        pass

    try:
        os.mkdir("results")
    except FileExistsError:
        pass

    executable = ""
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
    for i in range(1, 5):
        games.append({"name": "basic-%d-vs-a_basic-%d.txt"%(i,i), "params": "--wheuristic basic --wdepth %d --bheuristic a_basic --bdepth %d"%(i,i) })
        games.append({"name": "a_basic-%d-vs-basic-%d.txt"%(i,i), "params": "--wheuristic a_basic --wdepth %d --bheuristic basic --bdepth %d"%(i,i) })

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
    run_games()