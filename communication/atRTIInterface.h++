
// ATLAS: Adaptable Tool Library for Advanced Simulation
//
// Copyright 2015 University of Central Florida
//
//
// This library provides many fundamental capabilities used in creating
// virtual environment simulations.  It includes elements such as vectors,
// matrices, quaternions, containers, communication schemes (UDP, TCP, DIS,
// HLA, Bluetooth), and XML processing.  It also includes some extensions
// to allow similar code to work in Linux and in Windows.  Note that support
// for iOS and Android development is also included.
//
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef AT_RTI_INTERFACE_H
#define AT_RTI_INTERFACE_H


#include "atNotifier.h++"
#include "atList.h++"
#include "atKeyedBufferHandler.h++"
#include "atOSDefs.h++"

#if defined(__AT_RTI_ENABLED__)
#define RTI_USES_STD_FSTREAM
#include "NullFederateAmbassador13.h"

#include "atRTIInterfaceAmbassador.h++"
#endif

#define AT_RTI_MAX_CLASS_HANDLES       500
#define AT_RTI_MAX_OBJECT_HANDLES      1000
#define AT_RTI_MAX_ATTRIBUTE_HANDLES   1000
#define AT_RTI_MAX_PARAMETER_HANDLES   1000

#define AT_RTI_MAX_MAP_ENTRIES         AT_RTI_MAX_CLASS_HANDLES * AT_RTI_MAX_PARAMETER_HANDLES

#define AT_RTI_ATTRIBUTE_TYPE     1
#define AT_RTI_INTERACTION_TYPE   2
#define AT_RTI_CONTROL_TYPE       3


typedef long   atClassID;
typedef long   atAttributeID;
typedef long   atInteractionID;
typedef long   atParameterID;
typedef long   atInstanceID;


typedef struct atInteractionParameterMapEntry
{
#if defined(__AT_RTI_ENABLED__)
   rti13::InteractionClassHandle  interaction;
   rti13::ParameterHandle         parameter;
#endif
};

typedef struct atObjectAttributeMapEntry
{
#if defined(__AT_RTI_ENABLED__)
   rti13::ObjectClassHandle  objClass;
   rti13::AttributeHandle    attribute;
#endif
};

typedef struct atObjectInstance
{
#if defined(__AT_RTI_ENABLED__)
   rti13::ObjectHandle        obj;
   rti13::ObjectClassHandle   objClass;
   char                       name[256];
#endif
};


class ATLAS_SYM atRTIInterface : public atNotifier
{
   protected:
      static atRTIInterface *          rti_interface_instance;

#if defined(__AT_RTI_ENABLED__)
      rti13::RTIambassador             rti_amb;
      atRTIInterfaceAmbassador         fed_amb;
#endif

#if defined(__AT_RTI_ENABLED__)
      rti13::FederateHandle            federate_id;
#endif
      char                             fed_exec_name[256];

      u_long                           num_class_handles;
#if defined(__AT_RTI_ENABLED__)
      rti13::ObjectClassHandle         class_handles[AT_RTI_MAX_CLASS_HANDLES];
#endif

      u_long                           num_attribute_handles[AT_RTI_MAX_CLASS_HANDLES];
#if defined(__AT_RTI_ENABLED__)
      rti13::AttributeHandle           attribute_handles[AT_RTI_MAX_CLASS_HANDLES][AT_RTI_MAX_ATTRIBUTE_HANDLES];
#endif

      u_long                           num_object_handles;
      atObjectInstance                 object_handles[AT_RTI_MAX_OBJECT_HANDLES];

      atObjectAttributeMapEntry        object_attribute_map[AT_RTI_MAX_MAP_ENTRIES];
      u_long                           num_object_attribute_map_entries;

      u_long                           num_interaction_class_handles;
#if defined(__AT_RTI_ENABLED__)
      rti13::InteractionClassHandle    interaction_class_handles[AT_RTI_MAX_CLASS_HANDLES];
#endif
      char                             interaction_names[AT_RTI_MAX_CLASS_HANDLES][256];

      u_long                           num_interaction_parameter_handles[AT_RTI_MAX_CLASS_HANDLES];
#if defined(__AT_RTI_ENABLED__)
      rti13::ParameterHandle           interaction_parameter_handles[AT_RTI_MAX_CLASS_HANDLES][AT_RTI_MAX_PARAMETER_HANDLES];
#endif

      atInteractionParameterMapEntry   interaction_parameter_map[AT_RTI_MAX_MAP_ENTRIES];
      u_long                           num_interaction_parameter_map_entries;

      atList *                         buf_list;

#if defined(__AT_RTI_ENABLED__)
   // HLA with its separate ambassador is evil (where is the object-oriented
   // design?)
   public:
      void   processAmbDiscover(rti13::ObjectHandle obj,
                rti13::ObjectClassHandle, const char *name);

      void   processAmbRemove(rti13::ObjectHandle obj);

      void   processAmbAttribute(rti13::ObjectHandle obj,
                const rti13::AttributeHandleValuePairSet& attrs);

      void   processAmbInteraction(rti13::InteractionClassHandle interaction,
                const rti13::ParameterHandleValuePairSet& parameters);
#endif

   public:
      atRTIInterface(char* fedExecName, char * fedFilename);
      virtual ~atRTIInterface();

      static atRTIInterface *   getInstance(char * fedExecName, 
                                            char * fedFilename);
      static atRTIInterface *   getInstance();

      atClassID         registerClass(char * classTypeStr);
      atAttributeID     registerAttribute(char * attrTypeStr, 
                                          atClassID classTypeID);
      atInteractionID   registerInteraction(char * interactionTypeStr);
      atParameterID     registerParameter(char * parameterTypeStr, 
                                          atInteractionID interactionTypeID);

      void   subscribeAttributes(atClassID classTypeID,
                                 u_long numAttributes,
                                 atAttributeID attrTypeIDs[]);
      void   publishAttributes(atClassID classTypeID,
                               u_long numAttributes,
                               atAttributeID attrTypeIDs[]);

      void   subscribeInteractions(u_long numInteractions,
                                   atInteractionID interactionTypeIDs[]);
      void   publishInteractions(u_long numInteractions,
                                 atInteractionID interactionTypeIDs[]);

      atInstanceID   createInstance(atClassID classTypeID);

      void   tick();

      atKeyedBufferHandler *   read();
};


#endif

