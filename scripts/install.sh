dirs -c &&
mkdir -p stanford_project &&
pushd stanford_project &&

# install chai3d
if [ ! -d "chai3d" ]; then
	git clone https://github.com/manips-sai-org/chai3d.git || exit 1
fi
pushd chai3d &&
mkdir -p build &&
pushd build &&
cmake .. -DCMAKE_BUILD_TYPE=Release && make &&
popd && popd &&

# install sai2-urdfreader
if [ ! -d "sai2-urdfreader" ]; then
	git clone https://github.com/manips-sai-org/sai2-urdfreader.git || exit 1
fi
pushd sai2-urdfreader &&
mkdir -p build &&
pushd build &&
cmake .. -DCMAKE_BUILD_TYPE=Release && make &&
popd && popd &&

# install sai2-model
if [ ! -d "sai2-model" ]; then
	git clone https://github.com/manips-sai-org/sai2-model.git || exit 1
fi
pushd sai2-model &&
sh install.sh &&
popd &&

# install sai2-graphics
if [ ! -d "sai2-graphics" ]; then
	git clone https://github.com/manips-sai-org/sai2-graphics.git || exit 1
fi
pushd sai2-graphics &&
mkdir -p build &&
pushd build &&
cmake .. -DCMAKE_BUILD_TYPE=Release && make &&
popd && popd &&

# install sai2-common
if [ ! -d "sai2-common" ]; then
	git clone https://github.com/manips-sai-org/sai2-common.git || exit 1
fi
pushd sai2-common &&
mkdir -p build &&
pushd build &&
cmake .. -DCMAKE_BUILD_TYPE=Release && make &&
popd && popd &&

# copy sai2-simulation
popd &&
cp sai2-simulation.zip stanford_project/ &&
pushd stanford_project &&

# install sai2-simulation
if [ ! -d "sai2-simulation-master" ]; then
	unzip sai2-simulation.zip  || exit 1
fi
pushd sai2-simulation-master &&
mkdir -p build &&
pushd build &&
cmake .. -DCMAKE_BUILD_TYPE=Release && make &&
popd && popd &&

# install project code
if [ ! -d "short_project" ]; then
	git clone https://github.com/shameekganguly/short_project.git || exit 1
fi
pushd short_project &&
sh build.sh
