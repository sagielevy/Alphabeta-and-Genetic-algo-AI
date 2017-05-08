import threading
import subprocess
import time
from threading import Timer


class Fitness(object):
    def __init__(self, decoder_func, proc_path, static_args, num_processes, goal):
        self.decoder = decoder_func
        self.num_processes = num_processes
        self.proc_path = proc_path
        self.static_args = static_args
        self.goal = goal
        self.curr_processes = 0
        self.result = None

    def fitness_score(self, chrom_list):
        """
        Calculates distance of chromosome from goal
        :param chrom_list: Chromosome to calculate fitness score
        :return: Fitness score of given chromosome in values > 0.
        Return list of scores for all chroms
        """

        # Init result with relatively small but larger than 0 values (so that everyone has a chance of being picked)
        self.result = [0.01 for i in xrange(len(chrom_list))]

        # For each couple i, j where i != j, make a match. There shall be two matches for every such i,j
        # Once i as white and j as black and the other vice versa.
        # This is necessary because the player that goes first has an advantage.
        for i, chrom_a in enumerate(chrom_list):
            for j, chrom_b in enumerate(chrom_list):
                # Skip matches of same player (i == j)
                if i == j:
                    continue

                # Wait till a process is done when in full capacity
                while self.curr_processes == self.num_processes:
                    time.sleep(0.5)

                # Run a match
                self.curr_processes += 1
                self.popenAndCall(self._set_score, i, j,
                                  (self.proc_path,) + self.static_args + self.decoder(chrom_a) + self.decoder(chrom_b))

        return tuple(self.result)

    def _set_score(self, i, j, returncode):
        # Return code values: 1 if player i won, -1 if player j won, 0 if tie
        if returncode == 1:
            self.result[i] += 1
        elif returncode == -1:
            self.result[j] += 1

    def popenAndCall(self, onExit, i, j, popenArgs):
        """
        Runs the given args in a subprocess.Popen, and then calls the function
        onExit when the subprocess completes.
        onExit is a callable object, and popenArgs is a list/tuple of args that
        would give to subprocess.Popen.
        """

        def runInThread(onExit, popenArgs):
            # kill = lambda process: process.kill()
            proc = subprocess.Popen(args=" ".join([str(x) for x in popenArgs]))
            # timer = Timer(30, kill, [proc])

            # try:
            #     timer.start()
            proc.wait()
            onExit(i, j, proc.returncode)
            # finally:
            #     timer.cancel()
            self.curr_processes -= 1

            return

        thread = threading.Thread(target=runInThread, args=(onExit, popenArgs))
        thread.start()

        # returns immediately after the thread starts
        return thread
