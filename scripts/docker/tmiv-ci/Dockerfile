FROM tmiv-checks:20220715

# Copy the test material for the integration test
COPY tmiv_integration_test /tmiv_integration_test

# Set-up ccache to speed up CI
RUN /usr/sbin/update-ccache-symlinks
ENV PATH=/usr/lib/ccache:$PATH
ENV CCACHE_DIR=/cache/.ccache

# Copy dependencies and build script
#
# Having built the dependencies locally, copy:
#   C:\Data\TM1\.deps\source -> source
#   C:\Data\TM1\scripts\build\build_dependencies.py -> build_dependencies.py
RUN mkdir -p /dependencies/build
COPY source /dependencies/source
COPY HM /dependencies/build/HM
COPY build_dependencies.json /dependencies/build/build_dependencies.json
COPY build_dependencies.py /dependencies/build/build_dependencies.py

# Build the dependencies in the Docker environment (Clang 14)
ENV CXX=clang++-14\ -w\ -stdlib=libc++
ENV CC=clang-14\ -w
RUN /dependencies/build/build_dependencies.py \
  --source-dir /dependencies/source \
  --build-dir /dependencies/build/clang-Debug \
  --install-dir /dependencies/install/clang-Debug \
  --build-type Debug -j 2

# Build the dependencies in the Docker environment (GCC 12)
ENV CXX=g++-12\ -w
ENV CC=gcc-12\ -w
RUN /dependencies/build/build_dependencies.py \
  --source-dir /dependencies/source \
  --build-dir /dependencies/build/gcc-Release \
  --install-dir /dependencies/install/gcc-Release \
  --build-type Release -j 2

# Remove the build directory but keep the source directory
RUN rm -r /dependencies/build
