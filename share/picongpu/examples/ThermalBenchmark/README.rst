Thermal Benchmark
=================

.. sectionauthor:: Axel Huebl <a.huebl (at) hzdr.de>
.. moduleauthor:: Axel Huebl <a.huebl (at) hzdr.de>, Andrew Myers <atmyers@lbl.gov>

This example is just a little 3D3V thermal benchmark.
If implements a single (case 0) and double precision (case 1) warm electron-positron plasma.

* PCS (3rd, pc. cubic)
* e+/e- 4 particles per cell each
* dx = dy = dz = 0.3e-6 m
* dt = Courant limit (0.999)
* n = 1.e25 m^{-3} (Density of each species)
* px_th = py_th = pz_th = 1.0 beta-gamma (Maxwellian distribution); PIConGPU: 161keV / k_B
* single & double precision
* Boris push
* Yee solver
* periodic for all boundaries
* random in-cell start
* 8.4e6 cells / GPU (128x256x256 - not maxed out)
* 100 steps

The example is tuned for a single or four Nvidia P100 or V100 (16 GByte) GPU(s).


PIConGPU Version
----------------

0.4.3-850-g0b72a28b8 (02/2020 from mainline/release-0.5.0-rc1)


Equivalent WarpX Input
----------------------

WarpX Version: 89b4801d778f58f04162b2c886c6c57ea4cae009
AMReX Version: d307624f8451a18da755a6cb1b5a3c31a42cdd48
Picsar Version: b78ba49a4299ddc8000b0ad4a244f291459fdb10

* Input: `inputs_WarpX_20-07` - this version leaves the GPU less than fully occupied
* Input: `inputs_full_single_WarpX_20-07` - this tries to fully occupy the GPU using 4 ppc per species, single precision
* Input: `inputs_full_double_WarpX_20-07` - this tries to fully occupy the GPU using 4 ppc per species, double precision


Included Cases
--------------

Compile them with ``pic-build -t <N>``.

* ``0``: 32bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``1``: 64bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``2``: 32bit, optimized, regular Esirkepov current deposition
* ``3``: 64bit, optimized, regular Esirkepov current deposition
