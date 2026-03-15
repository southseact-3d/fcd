========================
Tungsten CAD build with Conda
========================

Container
=========

Build and run the `tungsten-cad-conda` container with the following commands:

.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Conda --tag tungsten-cad-conda .
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-conda

Build Code
==========

Within the `tungsten-cad-conda` container, install the build dependencies and build
Tungsten CAD using the following commands:

.. code-block:: console

    conda/setup-environment.sh
    conda activate tungsten-cad
    cmake --preset conda-linux-debug
    cmake --build build/debug
