import random


class PercentCalc(object):
    def __init__(self, percent):
        self.total = 1

        # Get rid of the decimal point
        while int(percent) < percent:
            self.total *= 10
            percent *= 10

        self.percent = percent

    def chance(self):
        """
        Roll the dice for given percent
        :return: True if percent succeeded, False otherwise
        """
        return random.randint(1, self.total) <= self.percent
