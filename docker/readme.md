docker build ./ -f Dockerfile.deb -t sds_ver3
docker run -it --rm -v ${PWD}:/work sds_ver3 
