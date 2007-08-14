
#ifndef AT_TIMER_HPP
#define AT_TIMER_HPP


#include "atNotifier.h++"


class atTimer : public atNotifier
{
   private:

      double   mark_time;
      double   mark_interval;

   public:

      atTimer();
      virtual ~atTimer();

      virtual void   mark();
      virtual void   markAtInterval(double intervalTime);

      virtual double   getInterval();
      virtual double   getElapsed();
};

#endif