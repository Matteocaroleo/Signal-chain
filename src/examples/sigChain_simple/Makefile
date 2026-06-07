# Project Name
TARGET = sigChain_simple

# Dir classes path
CLASSES_DIR = ../../Classes
LIBS_DIR = ../../../libs

# Sources 
CPP_SOURCES = src/main.cpp \
			$(CLASSES_DIR)/effect/effectWrapper.cpp \
			$(CLASSES_DIR)/sigChain/sigChain_handler.cpp \
			$(CLASSES_DIR)/effect/effectInterface.cpp \
			$(CLASSES_DIR)/display/displayHandler.cpp \
				


# Library Locations
LIBDAISY_DIR = $(LIBS_DIR)/libDaisy
DAISYSP_DIR = $(LIBS_DIR)/DaisySP

# Include directories
C_INCLUDES += -I. \
		  -I$(CLASSES_DIR) \
		  -I$(CLASSES_DIR)/effect

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

include $(SYSTEM_FILES_DIR)/Makefile