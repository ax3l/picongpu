#!/usr/bin/env python
#
import matplotlib as mpl
#mpl.use('Agg') # non-interactive backend
import matplotlib.pyplot as plt
from matplotlib.colors import colorConverter, LogNorm, Normalize
from picongpu.plugins.data import EnergyHistogramData
import h5py as h5
import numpy as np


dt = 0.8e-6 / 384. / ( 1.415 * 2.99792458e8 )


def getData(sim, species, sfilter, suffix=""):
    """MeV / A"""
    eneHist = EnergyHistogramData(sim)
    its = eneHist.get_iterations(species, sfilter)
    counts, bins = eneHist.get(species, sfilter, its)
    if species == 'D':
        bins = bins / 2.0
    if species == 'C':
        bins = bins / 6.0
    if species == 'N':
        bins = bins / 7.0
    return counts, bins, its

def plotAndSave(sim, species, sfilter, suffix=""):
    """..."""

    counts_dict, bins, its = getData(sim, species, sfilter, suffix)
    counts = np.array([counts_dict[i] for i in counts_dict.keys()])

    plt.figure()
    plt.imshow(
        counts.T,
        extent = [
            np.min(its) * dt * 1.e12,
            np.max(its) * dt * 1.e12,
            np.min(bins) * 1.e-3,
            np.max(bins) * 1.e-3
        ],
        interpolation = 'nearest',
        aspect = 'auto',
        origin='lower'
        ,norm = LogNorm()
        #,cmap=myCmaps[species]
        #,vmin=1.e-11
        #,vmax=1.e-6
    )

    # annotations
    cbar = plt.colorbar()
    cbar.set_label(r'count  [arb. u.]')

    ax = plt.gca()
    ax.set_xlabel(r'time  [ps]')
    ax.set_ylabel(r'Ene / A  [MeV]')

    # time steps
    ax2 = ax.twiny()
    ax2.set_xlim([np.min(its), np.max(its)])

    plt.title("{0} ({1})".format(species, sfilter))
    #plt.legend()

    #plt.show()
    plt.savefig("energyEvolution/" +
        sim + "_" + species + "_" + sfilter +
        "_energyEvolution.png", dpi=300)
    plt.close()

for case in range(6):
    for repetition in range(2):
        sim = str(case) + "0" + str(repetition)
        for species in ['H', 'C', 'N', 'e']:
            for sfilter in ['all']:
                plotAndSave(sim, species, sfilter)
