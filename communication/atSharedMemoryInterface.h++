
#ifndef AT_SHARED_MEMORY_INTERFACE_H
#define AT_SHARED_MEMORY_INTERFACE_H


// INCLUDES
#include <sys/types.h>
#include "atIPCInterface.h++"
#include "atSharedQueue.h++"


class atSharedMemoryInterface : public atIPCInterface
{
   protected:
      atSharedQueue *   read_queue;
      atSharedQueue *   write_queue;

   public:
      atSharedMemoryInterface(key_t readKey, key_t writeKey);
      virtual ~atSharedMemoryInterface();

      int   read(u_char * buffer, u_long length);
      int   write(u_char * buffer, u_long length);
};

#endif
