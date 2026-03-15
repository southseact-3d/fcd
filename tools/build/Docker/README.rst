==========================
Tungsten CAD build dependencies
==========================

Distributions
=============

The following commands are used to install the packages needed to build Tungsten CAD from source on the following distributions.


Arch Linux
----------

The following command is used to install the required packages used to compile Tungsten CAD on Arch Linux.

.. code-block:: console

    sudo sh tools/build/Docker/arch.sh


Debian
------

The following command is used to install the required packages used to compile Tungsten CAD on Debian.

.. code-block:: console

    sudo sh tools/build/Docker/debian.sh


Fedora
------

The following command is used to install the required packages used to compile Tungsten CAD on Fedora.

.. code-block:: console

    sudo sh tools/build/Docker/fedora.sh


Manjaro
-------

The following command is used to install the required packages used to compile Tungsten CAD on Manjaro Linux.

.. code-block:: console

    sudo sh tools/build/Docker/manjaro.sh


Ubuntu
------


The following command is used to install the required packages used to compile Tungsten CAD on Ubuntu Linux.

.. code-block:: console

    sudo sh tools/build/Docker/ubuntu.sh


Containers
==========

The following will create containers that have all the required dependencies
pre-installed that are needed to build Tungsten CAD from source.


Arch Linux
-----------

The following commands are used to create and run a Arch Linux build environment.

.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Arch --tag tungsten-cad-arch
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-arch


Debian
------

The following commands are used to create and run a Debian build environment.

.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Debian --tag tungsten-cad-debian
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-debian


Fedora
------

The following commands are used to create and run a Fedora build environment.

.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Fedora --tag tungsten-cad-fedora
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-fedora


Manjaro
-------

The following commands are used to create and run a Manjaro build environment.

.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Manjaro --tag tungsten-cad-manjaro
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-manjaro


Ubuntu
------

The following commands are used to create and run a Ubuntu build environment.


.. code-block:: console

    docker build --file tools/build/Docker/Dockerfile.Ubuntu --tag tungsten-cad-ubuntu
    docker run --rm --interactive --tty --volume $(pwd):/builds:z tungsten-cad-ubuntu


Build Code
==========

To build the Tungsten CAD code inside one of the running containers the following
commands should be used

.. code-block:: console

    mkdir tungsten-cad-build
    cd tungsten-cad-build
    cmake ../tungsten-cad-source
    make -j$(nproc --ignore=2)
