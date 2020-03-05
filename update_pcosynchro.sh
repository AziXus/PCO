#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root"
    exit 1
fi

install_pcosynchro() {
    PCOSYNCHRO_CLONE_DIR=/usr/src/pcosynchro
    rm -rf $PCOSYNCHRO_CLONE_DIR
    git clone https://gitlab.com/reds-public/pco-synchro.git $PCOSYNCHRO_CLONE_DIR
    cd $PCOSYNCHRO_CLONE_DIR/lib/pcosynchro/
    qmake
    make -j4
    make install
    #cp *.a /usr/local/lib/
    #ln -s $PCOSYNCHRO_CLONE_DIR/src /usr/local/include/pcosynchro
}

echo -e "\e[94m[INFO]\e[39m Compiling and installing pco-synchro library necessary for the labs and exercices"
install_pcosynchro

echo -e "\e[94m[INFO] \e[92mSuccess"
