Thermal Benchmark
=================

.. sectionauthor:: Axel Huebl <axelhuebl@lbl.gov>
.. moduleauthor:: Axel Huebl <axelhuebl@lbl.gov>, Andrew Myers <atmyers@lbl.gov>

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

0.5.0 (07/2020 at commit 454a28efe6a6eb0fa1e0e97031ef2569a1ad313b)

* ``etc/picongpu/1.cfg``: same setup as 20-07, not occupying the GPU in full

  * other setups in this increase the number of cells per GPU while leaving the ppc per species constant
* note: supercell needs modification to e.g. ``8x4x4`` for 3D3V DP runs in ``include/picongpu/param/memory.param``


Equivalent WarpX Input
----------------------

WarpX Version: ``6b84b94dfc92ec0cd29b1e42f9ff0c45a45997a4`` (post ``21.06``)

AMReX Version: ``eb2fb8eb11d4df88d5b6c04a68f2971a6c3b97f0``

Picsar Version: ``c16b642e3dcf860480dd1dd21cefa3874f395773``

* Input: ``inputs_WarpX_21-06`` - this version leaves the GPU less than fully occupied, just derived from 20-05 before memory optimizations
* Input: ``inputs_full_single_WarpX_21-06`` - this tries to fully occupy the GPU using 4 ppc per species, single precision
* Input: ``inputs_full_double_WarpX_21-06`` - this tries to fully occupy the GPU using 4 ppc per species, double precision

Submission scripts:

* ``bsub_WarpX_21-06`` - simple 1 node run script (rename inputs file)
* ``bsub_6nodes_WarpX_21-06`` - weak-scaling to 3x3x4 devices for full 3D communication pattern

Included Cases
--------------

Compile them with ``pic-build -t <N>``.

* ``0``: 32bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``1``: 64bit, optimized trajectory Esirkepov current deposition ("EmZ", unpublished)
* ``2``: 32bit, optimized, regular Esirkepov current deposition
* ``3``: 64bit, optimized, regular Esirkepov current deposition
