import random
import Utils


class GeneManager(object):
    def __init__(self, gene_pool, chromosome_size, crossover_rate, mutation_rate):
        # Verify value validity
        if not (chromosome_size >= 1
                or gene_pool is dict
                or mutation_rate > 0
                or mutation_rate < 1
                or crossover_rate > 0
                or crossover_rate < 1):
            raise ValueError("Invalid parameters!")

        self.gene_pool = gene_pool
        self.chrome_size = chromosome_size
        self.mutation_chance_calc = Utils.PercentCalc(mutation_rate)
        self.crossover_chance_cate = Utils.PercentCalc(crossover_rate)

    def generate_chromosome(self):
        """
        Random chromosome generation
        :return: Randomly generated chromosome
        """

        chrom = ""

        for i in xrange(self.chrome_size):
            chrom += random.choice(self.gene_pool.keys())

        return chrom

    def attempt_crossover_chromosomes(self, first, second):
        # If crossover happens, swap from a certain point
        if self.crossover_chance_cate.chance():
            start_swap = random.randint(0, len(first))
            return first[:start_swap] + second[start_swap:], second[:start_swap] + first[start_swap:]
        return first, second

    def mutate_chromosome(self, chrom):
        """
        Randomly mutate bits across a chromosome
        :param chrom: Chromosome to mutate
        :return: Potentially mutated chromosome
        """

        result = list(chrom)

        for i in xrange(len(result)):
            # mutation_rate chance that a bit will be mutated
            if self.mutation_chance_calc.chance():
                result[i] = str(int(not int(result[i])))

        return "".join(result)
