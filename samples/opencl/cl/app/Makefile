DEPTH = ../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

SUBDIRS = AESEncryptDecrypt\
	  BinarySearch     \
	  BinomialOption   \
          BitonicSort      \
          BlackScholes     \
	  BoxFilter	   \
	  BoxFilterGL	\
	  DCT   \
          DwtHaar1D        \
	  FastWalshTransform \
	  FloydWarshall    \
	  FluidSimulation2D \
          Histogram        \
	  KernelLaunch \
	  Mandelbrot \
	  MatrixMultiplication \
	  MatrixMulImage \
	  MatrixTranspose  \
	  MonteCarloAsian  \
	  MonteCarloAsianDP  \
	  NBody			   \
	  PrefixSum		   \
	  QuasiRandomSequence	\
          RadixSort        \
	  RecursiveGaussian	\
          Reduction		   \
	  SimpleConvolution \
	  SimpleGL \
	  SimpleImage	\
	  SimpleMultiDevice \
	  SobelFilter	\
	  ScanLargeArrays	\
	  Template	\
	  TemplateC	\
	  URNG	\
	  DeviceFission \
	  LUDecomposition \
	  BlackScholesDP \
	  BufferBandwidth \
	  ImageBandwidth \
	  TransferOverlap \
	  AtomicCounters \
	  BinomialOptionMultiGPU \
	  MonteCarloAsianMultiGPU \
	  ImageOverlap \
	  GaussianNoiseGL \
	  DeviceFission11Ext


ifneq ($(OS), lnx)
SUBDIRS += SimpleDX10 OpenCLService OpenEncode
endif

          
include $(DEPTH)/make/openclsdkrules.mk
