Thermal Benchmark
=================

.. sectionauthor:: Axel Huebl <a.huebl (at) hzdr.de>
.. moduleauthor:: Axel Huebl <a.huebl (at) hzdr.de>, Rémi Lehe <rlehe (at) lbl.gov>, Andrew Myers <atmyers@lbl.gov>

This example is just a little 3D3V thermal benchmark.
If implements a single (case 0) and double precision (case 1) warm electron-positron plasma.

* PCS (3rd, pc. cubic)
* e+/e- 16 particles per cell each
* dx = dy = dz = 0.3e-6 m
* dt = Courant limit
* n = 1.e25 m^{-3} (Density of each species)
* vx_th = vy_th = vz_th = 0.1 x c (Maxwellian distribution); PIConGPU: 51keV / k_B
* single & double precision
* Boris push
* Yee solver
* periodic for all boundaries
* random in-cell start
* 12.6e6 cells / GPU (256x192x256)
* 100 steps

The example is tuned for a single or four Nvidia P100 or V100 (16 GByte) GPU(s).


PIConGPU Version
----------------

0.4.3-781-67d1d9df977d0576531dd0c7a27ccd386b376b6f (02/2020)


Equivalent WarpX Input
----------------------

Version: 20.02
Input: `inputs_WarpX_20-02` (max-out at 4 particles per cell per species)


Included Cases
--------------

Compile them with ``pic-build -t <N>``.

* ``0``: 32bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``1``: 64bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``2``: 32bit, optimized, regular Esirkepov current deposition
* ``3``: 64bit, optimized, regular Esirkepov current deposition
