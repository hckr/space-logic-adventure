LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := space-logic-adventure

LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -fexceptions

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main

LOCAL_SHARED_LIBRARIES := sfml-system
LOCAL_SHARED_LIBRARIES += sfml-window
LOCAL_SHARED_LIBRARIES += sfml-graphics
LOCAL_SHARED_LIBRARIES += sfml-audio

include $(BUILD_SHARED_LIBRARY)

$(call import-module,sfml)
