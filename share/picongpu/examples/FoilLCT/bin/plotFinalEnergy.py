#!/usr/bin/env python
#
import matplotlib as mpl
#mpl.use('Agg') # non-interactive backend
import matplotlib.pyplot as plt
from matplotlib.colors import colorConverter, LogNorm, Normalize
from picongpu.plugins.data import EnergyHistogramData
import h5py as h5
import numpy as np


step = 60000


def getData(sim, species, sfilter, suffix=""):
    """MeV / A"""
    eneHist = EnergyHistogramData(sim)
    counts, bins = eneHist.get(species, sfilter, step)
    if species == 'D':
        bins = bins / 2.0
    if species == 'C':
        bins = bins / 6.0
    if species == 'N':
        bins = bins / 7.0
    return counts, bins

def plotAndSave(sim_list, species, sfilter, suffix=""):
    """..."""

    plt.figure()

    for sim in sim_list:
        counts, bins = getData(sim, species, sfilter, suffix)

        plt.plot(
            bins * 1.e-3,
            counts,
            label=sim
        )

    # annotations
    ax = plt.gca()
    ax.set_yscale('log')
    ax.set_xlabel(r'Ene / A  [MeV]')
    ax.set_ylabel(r'count  [arb. u.]')

    plt.title("{0} ({1})".format(species, sfilter))
    plt.legend()

    #plt.show()
    plt.savefig("finalEnergy/" +
        species + "_" + sfilter +
        "_" + str(step) + ".png", dpi=300)
    plt.close()

sim_list = []
for case in range(6):
    for repetition in range(2):
        sim = str(case) + "0" + str(repetition)
        sim_list.append(sim)

for species in ['H', 'C', 'N', 'e']:
    for sfilter in ['all']:
        plotAndSave(sim_list, species, sfilter)
