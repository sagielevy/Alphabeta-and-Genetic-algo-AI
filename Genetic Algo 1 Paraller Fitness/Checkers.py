import Simulator
import logging


gene_pool = {"000": 0, "001": 1, "010": 2, "011": 3, "100": 4, "101": 5, "110": 6, "111": 7}
chrom_size = 4
goal = 10000  # Just a really high number, our goal here is to get the HIGHEST possible score, there's no perfect match
members = 20
values = []


def decode_fitness(chrom):
    """
    :return:
    Tuple of values decoded from chromosome
    """

    return gene_pool[chrom[0:3]], gene_pool[chrom[3:6]], gene_pool[chrom[6:9]], gene_pool[chrom[9:12]]


def decode_display(chrom):
    return "Protect own piece: {0}, protect own king: {1}, eat opponent piece: {2}, eat opponent king: {3}".\
        format(gene_pool[chrom[0:3]], gene_pool[chrom[3:6]], gene_pool[chrom[6:9]], gene_pool[chrom[9:12]])


if __name__ == "__main__":
    Simulator.Simulator(members, decode_fitness, goal, gene_pool, chrom_size, proc_path="Checkers.exe",
                        static_args=("false", 10), num_processes=5, generation_lim=100,
                        decoder_display=decode_display, log_level=logging.DEBUG, mutation_rate=0.01).simulate()
