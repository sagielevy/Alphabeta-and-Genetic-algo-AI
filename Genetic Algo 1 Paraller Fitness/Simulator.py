import GeneManager
import Fitness
import logging
import random
import math
import time


class Simulator(object):
    def __init__(self, members_size, decoder_fitness, goal, gene_pool, chromosome_size, proc_path, static_args=(),
                 num_processes=4, decoder_display=None, log_level=logging.DEBUG, crossover_rate=0.7,
                 mutation_rate=0.001, generation_lim=None, validate_chrom=None):
        self.members_size = members_size
        self.gene_manager = GeneManager.GeneManager(gene_pool, chromosome_size, crossover_rate, mutation_rate)
        self.fitness = Fitness.Fitness(decoder_fitness, proc_path, static_args, num_processes, goal)
        self.decoder_display = decoder_display
        self.validate_chrom = validate_chrom
        self.gen_lim = generation_lim
        self.gen_index = 0
        self.solution_found = False
        self.solution = None
        self.members = None

        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger()
        self.logger.setLevel(log_level)
        self.logger.addHandler(logging.FileHandler("Simulation.log"))

        self.logger.log(logging.INFO, "Beginning evolution at {0}! Goal is {1}".format(time.asctime(), str(goal)))

    def generate_initial_members(self):
        result = []

        for i in xrange(self.members_size):
            result.append(self.gene_manager.generate_chromosome())

            self.logger.log(logging.DEBUG, "Chromosome %d: %s", i, result[i])
            self.logger.log(logging.DEBUG, "Chromosome %d: %s", i, self.decoder_display(result[i]))
            # self.logger.log(logging.INFO, "Chromosome %d fitness: %f", i, self.fitness.fitness_score(result[i]))

        return result

    def roulette_wheel(self, scores, score_sum):
        """
        :return: Randomly select a member according to its fitness score
        """
        select = random.triangular(0, score_sum)

        # Search through the scores
        high = len(scores) - 1
        low = 0
        index = (high - low) / 2

        # Binary search for the range of selected
        while index > 0 and not (scores[index - 1] < select <= scores[index]):
            if select <= scores[index - 1]:
                high = index - 1
            elif select > scores[index]:
                low = index

            # Make sure we don't get stuck in a loop
            newI = low + int(math.ceil((high - low) / 2.0))

            if index == newI:
                index = newI - 1
            else:
                index = newI

        return self.members[index]

    # def handle_solution(self, chrom):
    #     self.solution_found = True
    #     self.solution = chrom

    def evolve_generation(self):
        """
        Evolve entirely new generation
        :return: New members
        """

        self.gen_index += 1
        result = []

        self.logger.log(logging.INFO, "Generation no. {0} at {1}".format(self.gen_index, time.asctime()))

        # Calculate fitness scores for everyone first
        scores = self.fitness.fitness_score(self.members)
        score_sum = sum(scores)
        acc_scores = [scores[0]]

        for i in xrange(1, len(scores)):
            acc_scores.append(acc_scores[i - 1] + scores[i])

        self.logger.log(logging.INFO, "Previous generation fitness score sum: %f", score_sum)

        # Print scores
        for i, score in enumerate(scores):
            self.logger.log(logging.DEBUG, "Chromosome %d: %s", i, self.members[i])
            self.logger.log(logging.DEBUG, "Chromosome %d: %s", i, self.decoder_display(self.members[i]))
            self.logger.log(logging.INFO, "Chromosome %d fitness: %f", i, score)

        # Play the roulette and populate new generation
        for i in xrange(0, self.members_size, 2):
            first = self.roulette_wheel(acc_scores, score_sum)
            second = self.roulette_wheel(acc_scores, score_sum)

            # Attempt to crossover chromosomes and make mutations
            first, second = self.gene_manager.attempt_crossover_chromosomes(first, second)
            first = self.gene_manager.mutate_chromosome(first)
            second = self.gene_manager.mutate_chromosome(second)

            result.append(first)
            result.append(second)

        # Finally set new generation into members
        self.members = result

    def simulate(self):
        """
        Simulate till solution has been found
        """

        self.members = self.generate_initial_members()

        # Evolve generations till a solution is found or generation limit has been reached
        while not self.solution_found and not (self.gen_lim is not None and self.gen_index >= self.gen_lim):
            self.evolve_generation()

        if self.solution_found:
            self.logger.log(logging.INFO, "Solution was found after %d generations.", self.gen_index)
            self.logger.log(logging.DEBUG, "Solution chromosome: %s", self.solution)

            if self.decoder_display:
                self.logger.log(logging.INFO, "Solution: %s", self.decoder_display(self.solution))
        else:
            self.logger.log(logging.INFO, "Solution was not found in %d generations.", self.gen_index)

        self.logger.log(logging.INFO, "Simulation ended at {0}".format(time.asctime()))
